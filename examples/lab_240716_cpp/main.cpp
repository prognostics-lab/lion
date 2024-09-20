#include <cstdio>
#include <string>

#include <lion/vector.h>
#include <lion_math/current.h>
#include <lionpp/lion.hpp>
#include <lionpp/vector.hpp>
#include <lionu/log.h>
#include <lionu/macros.h>

#define OUTCSV_FILENAME "simdata/lab_240716_cpp/data.csv"
#define OUTCURROPT_FILENAME "simdata/lab_240716_cpp/curropt.csv"

FILE *csv_file;
#ifndef NDEBUG
FILE *curropt_file = NULL;
lion_vector_t currs;
#endif

extern "C" lion_status_t init_hook(lion_app_t *app) {
  // data csv file
  csv_file = fopen(OUTCSV_FILENAME, "w+");
  if (csv_file == NULL) {
    log_error("Failed to open output file");
    return LION_STATUS_FAILURE;
  }
  fprintf(csv_file,
          "time,step,power,ambient_temperature,voltage,current,"
          "open_circuit_voltage,internal_resistance,ehc,generated_heat,"
          "internal_temperature,surface_temperature,kappa,soc_nominal,"
          "capacity_nominal,soc_use,capacity_use\n");

#ifndef NDEBUG
  // curropt file
  curropt_file = fopen(OUTCURROPT_FILENAME, "w+");
  if (curropt_file == NULL) {
    log_error("Failed to open curropt output file");
    return LION_STATUS_FAILURE;
  }
  LION_CALL(lion_vector_linspace_d(app, -10.0, 10.0, 101, &currs),
            "Failed creating currents");
  for (int i = 0; i < currs.len - 1; i++) {
    fprintf(curropt_file, "%lf,", lion_vector_get_d(app, &currs, i));
  }
  fprintf(curropt_file, "%lf\n", lion_vector_get_d(app, &currs, currs.len - 1));
#endif
  return LION_STATUS_SUCCESS;
}

extern "C" lion_status_t update_hook(lion_app_t *app) {
  // Store current state in csv file
  fprintf(
      csv_file,
      "%lf,%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
      app->state.time, app->state.step, app->state.power,
      app->state.ambient_temperature, app->state.voltage, app->state.current,
      app->state.open_circuit_voltage, app->state.internal_resistance,
      app->state.ehc, app->state.generated_heat,
      app->state.internal_temperature, app->state.surface_temperature,
      app->state.kappa, app->state.soc_nominal, app->state.capacity_nominal,
      app->state.soc_use, app->state.capacity_use);

#ifndef NDEBUG
  // Check the objective function of the current optimization process
  struct lion_optimization_iter_params params = {
      .power = app->state.power,
      .voc = app->state.open_circuit_voltage,
      .soc = app->state.soc_use,
      .params = app->params,
  };

  double c, target;
  for (int i = 0; i < currs.len - 1; i++) {
    c = lion_vector_get_d(app, &currs, i); // current
    target = lion_current_optimize_targetfn(c, &params);
    fprintf(curropt_file, "%lf,", target);
  }
  c = lion_vector_get_d(app, &currs, currs.len - 1); // current
  target = lion_current_optimize_targetfn(c, &params);
  fprintf(curropt_file, "%lf\n", target);
#endif
  return LION_STATUS_SUCCESS;
}

extern "C" lion_status_t finished_hook(lion_app_t *app) {
  fclose(csv_file);
#ifndef NDEBUG
  fclose(curropt_file);
#endif
  return LION_STATUS_SUCCESS;
}

lion::Status setup_paths(int argc, char *argv[], std::string *out_power,
                         std::string *out_amb) {
  if (argc != 1) {
    // We assume the paths are passed as arguments
    std::string p;
    bool power_set = false;
    bool ambtemp_set = false;
    for (int i = 1; i < argc; i++) {
      p = argv[i];
      if (p.starts_with("power=")) {
        power_set = true;
        *out_power = p.substr(6);
      }
      if (p.starts_with("amb=")) {
        ambtemp_set = true;
        *out_amb = p.substr(4);
      }
    }

    if (!power_set || !ambtemp_set) {
      log_fatal("Both paths must be provided");
      return lion::Status::FAILURE;
    }
  } else {
    // If they are not passed as arguments, they are assumed to
    // be environment variables
    log_warn("Getting paths from environment variables, to avoid use args");
    const char *power = getenv("LION_POWER_FILENAME");
    const char *amb = getenv("LION_AMBTEMP_FILENAME");
    if (power == NULL || amb == NULL) {
      log_fatal("Filenames could not be found");
      return lion::Status::FAILURE;
    }

    *out_power = std::string(power);
    *out_amb = std::string(amb);
  }
  return lion::Status::SUCCESS;
}

int main(int argc, char *argv[]) {
  log_info("Setting up paths");
  std::string power_filename;
  std::string ambtemp_filename;
  setup_paths(argc, argv, &power_filename, &ambtemp_filename);

  log_info("Power path: '%s'", power_filename.c_str());
  log_info("Ambient temperature path: '%s'", ambtemp_filename.c_str());

  log_info("Setting up configuration");
  lion::AppConfig conf;
  lion::AppStepper stepper = lion::AppStepper::RK8PD;
  conf.get_handle()->log_dir = "logs";
  conf.get_handle()->log_stdlvl = LOG_DEBUG;
  conf.get_handle()->sim_stepper = stepper;
  conf.get_handle()->sim_time_seconds = 7500.0;
  conf.get_handle()->sim_step_seconds = 1.0;
  conf.get_handle()->sim_epsabs = 1e-1;
  conf.get_handle()->sim_epsrel = 1e-1;
  conf.get_handle()->sim_min_max_iter = 10000;
  conf.get_handle()->init_hook = &init_hook;
  conf.get_handle()->update_hook = &update_hook;
  conf.get_handle()->finished_hook = &finished_hook;

  log_info("Setting up simulation parameters");
  lion::AppParams params;
  params.init().initial_soc = 0.1;
  params.init().initial_internal_temperature = 296.0;
  params.init().initial_soh = 1.0;
  params.init().initial_capacity = 14400.0;
  params.init().initial_current_guess = 10.0;

  log_info("Creating application");
  lion::App app(&conf, &params);

  log_info("Configuring system inputs");
  lion_vector_t _power;
  lion_vector_t _amb_temp;
  LION_VCALL(lion_vector_from_csv(app, power_filename.c_str(), sizeof(double),
                                  "%lf", &_power),
             "Failed creating power profile from csv file '%s'",
             power_filename.c_str());
  LION_VCALL(lion_vector_from_csv(app, ambtemp_filename.c_str(), sizeof(double),
                                  "%lf", &_amb_temp),
             "Failed creating ambient temperature profile from csv file '%s'",
             ambtemp_filename.c_str());
  std::vector<double> power = lion::vector_to_std<double>(&_power);
  std::vector<double> amb_temp = lion::vector_to_std<double>(&_amb_temp);

  log_info("Running application");
  app.run(power, amb_temp);

  log_info("Cleaning up");
  LION_CALL(lion_vector_cleanup(app, &_power), "Failed cleaning power vector");
  LION_CALL(lion_vector_cleanup(app, &_amb_temp),
            "Failed cleaning ambient temperature vector");
#ifndef NDEBUG
  LION_CALL(lion_vector_cleanup(app, &currs), "Failed cleaning currents");
#endif
}
