#include "lion/params.h"
#include "lion/status.h"

#include <lion/lion.h>
#include <lion_math/current.h>
#include <lionu/kde.h>
#include <lionu/log.h>
#include <lionu/macros.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define OUTCSV_FILENAME     "simdata/lab_240716/data.csv"
#define OUTCURROPT_FILENAME "simdata/lab_240716/curropt.csv"
#define ETA_VALES_FILENAME  "examples/lab_240716/eta_values.csv"

FILE *csv_file;
#ifndef NDEBUG
FILE         *curropt_file = NULL;
lion_vector_t currs;
#endif

lion_status_t init_hook(lion_sim_t *sim) {
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

lion_status_t update_hook(lion_sim_t *sim) {
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

lion_status_t finished_hook(lion_sim_t *sim) {
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
  lion_sim_config_t conf = lion_sim_config_default();
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

  params.soh.model            = LION_SOH_MODEL_MASSERANO;
  params.soh.params.masserano = lion_params_default_soh_masserano();
  lion_gaussian_kde_t kde;
  lion_vector_t       eta_values;
  LION_CALL(lion_vector_from_csv(NULL, ETA_VALES_FILENAME, sizeof(double), "%lf", &eta_values), "Failed reading eta values for KDE");
  LION_CALL(
      lion_gaussian_kde_init(eta_values.data, eta_values.len, LION_GAUSSIAN_KDE_SCOTT, (unsigned long)time(NULL), &kde), "Failed setting up KDE"
  );
  params.soh.params.masserano.kde = &kde;

  log_info("Creating simulation");
  lion_sim_t sim;
  LION_CALL(lion_sim_new(&conf, &params, &sim), "Failed creating simulation");
  sim.init_hook     = &init_hook;
  sim.update_hook   = &update_hook;
  sim.finished_hook = &finished_hook;

  log_info("Configuring system inputs");
  lion_vector_t power;
  lion_vector_t amb_temp;
  LION_VCALL(
      lion_vector_from_csv(&sim, power_filename, sizeof(double), "%lf", &power), "Failed creating power profile from csv file '%s'", power_filename
  );
  LION_VCALL(
      lion_vector_from_csv(&sim, ambtemp_filename, sizeof(double), "%lf", &amb_temp),
      "Failed creating ambient temperature profile from csv file '%s'",
      ambtemp_filename
  );

  log_info("Running simulation");
  LION_CALL(lion_sim_run(&sim, &power, &amb_temp), "Failed running simulation");

  log_info("Cleaning up");
  LION_CALL(lion_vector_cleanup(&sim, &power), "Failed cleaning power vector");
  LION_CALL(lion_vector_cleanup(&sim, &amb_temp), "Failed cleaning ambient temperature vector");
#ifndef NDEBUG
  LION_CALL(lion_vector_cleanup(&sim, &currs), "Failed cleaning currents");
#endif
  LION_CALL(lion_sim_cleanup(&sim), "Failed cleaning simulation");
}
