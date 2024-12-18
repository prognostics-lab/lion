#include "lion/params.h"
#include "lion/status.h"

#include <lion/lion.h>
#include <lion_math/current.h>
#include <lionu/log.h>
#include <lionu/macros.h>
#include <stdlib.h>
#include <string.h>

#define OUTCSV_FILENAME     "simdata/lab_240716/data.csv"
#define OUTCURROPT_FILENAME "simdata/lab_240716/curropt.csv"

FILE *csv_file;
#ifndef NDEBUG
FILE         *curropt_file = NULL;
lion_vector_t currs;
#endif

lion_status_t init_hook(lion_app_t *app) {
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
  LION_CALL(lion_vector_linspace_d(app, -10.0, 10.0, 101, &currs), "Failed creating currents");
  for (int i = 0; i < currs.len - 1; i++) {
    fprintf(curropt_file, "%lf,", lion_vector_get_d(app, &currs, i));
  }
  fprintf(curropt_file, "%lf\n", lion_vector_get_d(app, &currs, currs.len - 1));
#endif
  return LION_STATUS_SUCCESS;
}

lion_status_t update_hook(lion_app_t *app) {
  // Store current state in csv file
  fprintf(
      csv_file,
      "%lf,%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
      app->state.time,
      app->state.step,
      app->state.power,
      app->state.ambient_temperature,
      app->state.voltage,
      app->state.current,
      app->state.open_circuit_voltage,
      app->state.internal_resistance,
      app->state.ehc,
      app->state.generated_heat,
      app->state.internal_temperature,
      app->state.surface_temperature,
      app->state.kappa,
      app->state.soc_nominal,
      app->state.capacity_nominal,
      app->state.soc_use,
      app->state.capacity_use
  );

#ifndef NDEBUG
  // Check the objective function of the current optimization process
  struct lion_optimization_iter_params params = {
      .power  = app->state.power,
      .voc    = app->state.open_circuit_voltage,
      .soc    = app->state.soc_use,
      .params = app->params,
  };

  double c, target;
  for (int i = 0; i < currs.len - 1; i++) {
    c      = lion_vector_get_d(app, &currs, i); // current
    target = lion_current_optimize_targetfn(c, &params);
    fprintf(curropt_file, "%lf,", target);
  }
  c      = lion_vector_get_d(app, &currs, currs.len - 1); // current
  target = lion_current_optimize_targetfn(c, &params);
  fprintf(curropt_file, "%lf\n", target);
#endif
  return LION_STATUS_SUCCESS;
}

lion_status_t finished_hook(lion_app_t *app) {
  fclose(csv_file);
#ifndef NDEBUG
  fclose(curropt_file);
#endif
  return LION_STATUS_SUCCESS;
}

lion_status_t setup_paths(int argc, char *argv[], char *out_power, char *out_amb) {
  if (argc != 1) {
    // We assume the paths are passed as arguments
    const char *p;
    int         power_set   = 0;
    int         ambtemp_set = 0;
    for (int i = 1; i < argc; i++) {
      p = argv[i];
      if (strncmp(p, "power=", 6) == 0) {
        power_set = 1;
        strncpy(out_power, p + 6, FILENAME_MAX);
      } else if (strncmp(p, "amb=", 4) == 0) {
        ambtemp_set = 1;
        strncpy(out_amb, p + 4, FILENAME_MAX);
      }
    }

    if (!power_set || !ambtemp_set) {
      log_fatal("Both paths must be provided");
      return LION_STATUS_FAILURE;
    }
  } else {
    // If they are not passed as arguments, they are assumed to
    // be environment variables
    log_warn("Getting paths from environment variables, to avoid use args");
    const char *power = getenv("LION_POWER_FILENAME");
    const char *amb   = getenv("LION_AMBTEMP_FILENAME");
    if (power == NULL || amb == NULL) {
      log_fatal("Filenames could not be found");
      return LION_STATUS_FAILURE;
    }

    strncpy(out_power, power, FILENAME_MAX);
    strncpy(out_amb, amb, FILENAME_MAX);
  }
  return LION_STATUS_SUCCESS;
}

int main(int argc, char *argv[]) {
  log_info("Setting up paths");
  char power_filename[FILENAME_MAX];
  char ambtemp_filename[FILENAME_MAX];
  LION_CALL(setup_paths(argc, argv, power_filename, ambtemp_filename), "Failed setting up paths");

  log_info("Power path: '%s'", power_filename);
  log_info("Ambient temperature path: '%s'", ambtemp_filename);

  log_info("Setting up configuration");
  lion_app_config_t conf = lion_app_config_default();
  // Metadata
  conf.log_dir           = "logs";
  conf.log_stdlvl        = LOG_DEBUG;
  // Simulation parameters
  conf.sim_stepper       = LION_STEPPER_RK8PD;
  conf.sim_time_seconds  = 7500.0;
  conf.sim_step_seconds  = 1.0;
  conf.sim_epsabs        = 1e-1;
  conf.sim_epsrel        = 1e-1;
  conf.sim_min_maxiter   = 10000;

  // log_info("Setting up internal resistance parameters");
  // lion_params_rint_polarization_t rint = lion_params_default_polarization();

  log_info("Setting up simulation parameters");
  lion_params_t params            = lion_params_default();
  params.init.soc                 = 0.1;
  params.init.temp_in             = 296.0;
  params.init.soh                 = 1.0;
  params.init.capacity            = 14400.0;
  params.init.current_guess       = 10.0;
  params.rint.model               = LION_RINT_MODEL_POLARIZATION;
  params.rint.params.polarization = lion_params_default_rint_polarization();

  log_info("Creating application");
  lion_app_t app;
  LION_CALL(lion_app_new(&conf, &params, &app), "Failed creating application");
  app.init_hook     = &init_hook;
  app.update_hook   = &update_hook;
  app.finished_hook = &finished_hook;

  log_info("Configuring system inputs");
  lion_vector_t power;
  lion_vector_t amb_temp;
  LION_VCALL(
      lion_vector_from_csv(&app, power_filename, sizeof(double), "%lf", &power), "Failed creating power profile from csv file '%s'", power_filename
  );
  LION_VCALL(
      lion_vector_from_csv(&app, ambtemp_filename, sizeof(double), "%lf", &amb_temp),
      "Failed creating ambient temperature profile from csv file '%s'",
      ambtemp_filename
  );

  log_info("Running application");
  LION_CALL(lion_app_run(&app, &power, &amb_temp), "Failed running app");

  log_info("Cleaning up");
  LION_CALL(lion_vector_cleanup(&app, &power), "Failed cleaning power vector");
  LION_CALL(lion_vector_cleanup(&app, &amb_temp), "Failed cleaning ambient temperature vector");
#ifndef NDEBUG
  LION_CALL(lion_vector_cleanup(&app, &currs), "Failed cleaning currents");
#endif
  LION_CALL(lion_app_cleanup(&app), "Failed cleaning app");
}
