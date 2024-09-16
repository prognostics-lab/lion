#include "lion/status.h"
#include <lion/lion.h>
#include <lionu/log.h>
#include <lionu/macros.h>

#define POWER_FILENAME "data/240716_temp_profile_C4B1/processed/data_power.csv"
#define AMBTEMP_FILENAME                                                       \
  "data/240716_temp_profile_C4B1/processed/data_amb_pv_temp.csv"
#define OUTCSV_FILENAME "simdata/lab_240716/data.csv"

FILE *f;

lion_status_t init_hook(lion_app_t *app) {
  f = fopen(OUTCSV_FILENAME, "w+");
  if (f == NULL) {
    log_error("Failed to open output file");
    return LION_STATUS_FAILURE;
  }
  fprintf(f, "time,step,power,ambient_temperature,voltage,current,"
             "open_circuit_voltage,internal_resistance,ehc,generated_heat,"
             "internal_temperature,surface_temperature,kappa,soc_nominal,"
             "capacity_nominal,soc_use,capacity_use\n");
  return LION_STATUS_SUCCESS;
}

lion_status_t update_hook(lion_app_t *app) {
  fprintf(
      f,
      "%lf,%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
      app->state.time, app->state.step, app->state.power,
      app->state.ambient_temperature, app->state.voltage, app->state.current,
      app->state.open_circuit_voltage, app->state.internal_resistance,
      app->state.ehc, app->state.generated_heat,
      app->state.internal_temperature, app->state.surface_temperature,
      app->state.kappa, app->state.soc_nominal, app->state.capacity_nominal,
      app->state.soc_use, app->state.capacity_use);
  return LION_STATUS_SUCCESS;
}

lion_status_t finished_hook(lion_app_t *app) {
  fclose(f);
  return LION_STATUS_SUCCESS;
}

int main(void) {
  log_info("Setting up configuration");
  lion_app_config_t conf = lion_app_config_default();
  // Metadata
  conf.log_dir = "logs";
  conf.log_stdlvl = LOG_DEBUG;
  // Simulation parameters
  conf.sim_stepper = LION_STEPPER_RK4;
  conf.sim_time_seconds = 7500.0;
  conf.sim_step_seconds = 1.0;
  conf.sim_epsabs = 1e-1;
  conf.sim_epsrel = 1e-1;
  conf.sim_min_max_iter = 10000;
  // Hooks
  conf.init_hook = &init_hook;
  conf.update_hook = &update_hook;
  conf.finished_hook = &finished_hook;

  log_info("Setting up simulation parameters");
  lion_params_t params = lion_params_default();
  params.init.initial_soc = 0.1;
  params.init.initial_internal_temperature = 296.0;
  params.init.initial_soh = 1.0;
  params.init.initial_capacity = 14400.0;
  params.init.initial_current_guess = 10.0;

  log_info("Creating application");
  lion_app_t app;
  LION_CALL(lion_app_new(&conf, &params, &app), "Failed creating application");

  log_info("Configuring system inputs");
  lion_vector_t power;
  lion_vector_t amb_temp;
  LION_VCALL(
      lion_vector_from_csv(&app, POWER_FILENAME, sizeof(double), "%lf", &power),
      "Failed creating power profile from csv file '%s'", POWER_FILENAME);
  LION_VCALL(lion_vector_from_csv(&app, AMBTEMP_FILENAME, sizeof(double), "%lf",
                                  &amb_temp),
             "Failed creating ambient temperature profile from csv file '%s'",
             AMBTEMP_FILENAME);

  log_info("Running application");
  LION_CALL(lion_app_run(&app, &power, &amb_temp), "Failed running app");

  log_info("Cleaning up");
  LION_CALL(lion_vector_cleanup(&app, &power), "Failed cleaning power vector");
  LION_CALL(lion_vector_cleanup(&app, &amb_temp),
            "Failed cleaning ambient temperature vector");
  LION_CALL(lion_app_cleanup(&app), "Failed cleaning app");
}
