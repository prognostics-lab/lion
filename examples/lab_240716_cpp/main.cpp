#include <cstdio>
#include <lion/vector.h>
#include <lion_math/current.h>
#include <lionpp/lion.hpp>
#include <lionpp/vector.hpp>
#include <lionu/log.h>
#include <lionu/macros.h>
#include <string>

#define OUTCSV_FILENAME     "simdata/lab_240716_cpp/data.csv"
#define OUTCURROPT_FILENAME "simdata/lab_240716_cpp/curropt.csv"

FILE *csv_file;
#ifndef NDEBUG
FILE         *curropt_file = NULL;
lion_vector_t currs;
#endif

extern "C" lion_status_t init_hook(lion_sim_t *sim) {
  // data csv file
  csv_file = fopen(OUTCSV_FILENAME, "w+");
  if (csv_file == NULL) {
    log_error("Failed to open output file");
    return LION_STATUS_FAILURE;
  }
  fprintf(
      csv_file,
      "time,step,power,ambient_temperature,voltage,current,"
      "open_circuit_voltage,internal_resistance,ehc,generated_heat,"
      "internal_temperature,surface_temperature,kappa,soc_nominal,"
      "capacity_nominal,soc_use,capacity_use\n"
  );

#ifndef NDEBUG
  // curropt file
  curropt_file = fopen(OUTCURROPT_FILENAME, "w+");
  if (curropt_file == NULL) {
    log_error("Failed to open curropt output file");
    return LION_STATUS_FAILURE;
  }
  LION_CALL(lion_vector_linspace_d(sim, -10.0, 10.0, 101, &currs), "Failed creating currents");
  for (int i = 0; i < currs.len - 1; i++) {
    fprintf(curropt_file, "%lf,", lion_vector_get_d(sim, &currs, i));
  }
  fprintf(curropt_file, "%lf\n", lion_vector_get_d(sim, &currs, currs.len - 1));
#endif
  return LION_STATUS_SUCCESS;
}

extern "C" lion_status_t update_hook(lion_sim_t *sim) {
  // Store current state in csv file
  fprintf(
      csv_file,
      "%lf,%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
      sim->state.time,
      sim->state.step,
      sim->state.power,
      sim->state.ambient_temperature,
      sim->state.voltage,
      sim->state.current,
      sim->state.open_circuit_voltage,
      sim->state.internal_resistance,
      sim->state.ehc,
      sim->state.generated_heat,
      sim->state.internal_temperature,
      sim->state.surface_temperature,
      sim->state.kappa,
      sim->state.soc_nominal,
      sim->state.capacity_nominal,
      sim->state.soc_use,
      sim->state.capacity_use
  );

#ifndef NDEBUG
  // Check the objective function of the current optimization process
  struct lion_optimization_iter_params params = {
    .power  = sim->state.power,
    .voc    = sim->state.open_circuit_voltage,
    .soc    = sim->state.soc_use,
    .params = sim->params,
  };

  double c, target;
  for (int i = 0; i < currs.len - 1; i++) {
    c      = lion_vector_get_d(sim, &currs, i); // current
    target = lion_current_optimize_targetfn(c, &params);
    fprintf(curropt_file, "%lf,", target);
  }
  c      = lion_vector_get_d(sim, &currs, currs.len - 1); // current
  target = lion_current_optimize_targetfn(c, &params);
  fprintf(curropt_file, "%lf\n", target);
#endif
  return LION_STATUS_SUCCESS;
}

extern "C" lion_status_t finished_hook(lion_sim_t *sim) {
  fclose(csv_file);
#ifndef NDEBUG
  fclose(curropt_file);
#endif
  return LION_STATUS_SUCCESS;
}

lion::Status setup_paths(int argc, char *argv[], std::string *out_power, std::string *out_amb) {
  if (argc != 1) {
    // We assume the paths are passed as arguments
    std::string p;
    bool        power_set   = false;
    bool        ambtemp_set = false;
    for (int i = 1; i < argc; i++) {
      p = argv[i];
      if (p.starts_with("power=")) {
        power_set  = true;
        *out_power = p.substr(6);
      }
      if (p.starts_with("amb=")) {
        ambtemp_set = true;
        *out_amb    = p.substr(4);
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
    const char *amb   = getenv("LION_AMBTEMP_FILENAME");
    if (power == NULL || amb == NULL) {
      log_fatal("Filenames could not be found");
      return lion::Status::FAILURE;
    }

    *out_power = std::string(power);
    *out_amb   = std::string(amb);
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
  lion::SimConfig  conf;
  lion::SimStepper stepper            = lion::SimStepper::RK8PD;
  conf.get_handle()->log_dir          = "logs";
  conf.get_handle()->log_stdlvl       = LOG_DEBUG;
  conf.get_handle()->sim_stepper      = stepper;
  conf.get_handle()->sim_time_seconds = 7500.0;
  conf.get_handle()->sim_step_seconds = 1.0;
  conf.get_handle()->sim_epsabs       = 1e-1;
  conf.get_handle()->sim_epsrel       = 1e-1;
  conf.get_handle()->sim_min_maxiter  = 10000;

  log_info("Setting up simulation parameters");
  lion::SimParams params;
  params.init().soc           = 0.1;
  params.init().temp_in       = 296.0;
  params.init().soh           = 1.0;
  params.init().capacity      = 14400.0;
  params.init().current_guess = 10.0;

  log_info("Creating simulation");
  lion::Sim sim(&conf, &params);
  // TODO: Implement hooks in the C++ wrsimer

  log_info("Configuring system inputs");
  lion_vector_t _power;
  lion_vector_t _amb_temp;
  LION_VCALL(
      lion_vector_from_csv(sim, power_filename.c_str(), sizeof(double), "%lf", &_power),
      "Failed creating power profile from csv file '%s'",
      power_filename.c_str()
  );
  LION_VCALL(
      lion_vector_from_csv(sim, ambtemp_filename.c_str(), sizeof(double), "%lf", &_amb_temp),
      "Failed creating ambient temperature profile from csv file '%s'",
      ambtemp_filename.c_str()
  );
  std::vector<double> power    = lion::vector_to_std<double>(&_power);
  std::vector<double> amb_temp = lion::vector_to_std<double>(&_amb_temp);

  log_info("Running simulation");
  sim.run(power, amb_temp);

  log_info("Cleaning up");
  LION_CALL(lion_vector_cleanup(sim, &_power), "Failed cleaning power vector");
  LION_CALL(lion_vector_cleanup(sim, &_amb_temp), "Failed cleaning ambient temperature vector");
#ifndef NDEBUG
  LION_CALL(lion_vector_cleanup(sim, &currs), "Failed cleaning currents");
#endif
}
