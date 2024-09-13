#include "lion/status.h"
#include <lion/lion.h>
#include <lionu/log.h>
#include <lionu/macros.h>

#define POWER_FILENAME "data/240716_temp_profile_C4B1/processed/data_power.csv"
#define AMBTEMP_FILENAME                                                       \
  "data/240716_temp_profile_C4B1/processed/data_amb_pv_temp.csv"

lion_vector_t sf_temp;
lion_vector_t in_temp;

lion_status_t update_hook(lion_app_t *app) {
  log_trace("Iteration %d", app->state.step);
  LION_CALL(lion_vector_push(app, &sf_temp, &app->state.surface_temperature),
            "Failed pushing");
  LION_CALL(lion_vector_push(app, &in_temp, &app->state.internal_temperature),
            "Failed pushing");
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
  conf.update_hook = &update_hook;

  log_info("Setting up simulation parameters");
  lion_params_t params = lion_params_default();
  params.init.initial_soc = 0.1;
  params.init.initial_internal_temperature = 293.0;
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

  log_info("Configuring system outputs");
  LION_CALL(
      lion_vector_with_capacity(&app, power.len, sizeof(double), &sf_temp),
      "Failed creating surface temperature vector");
  LION_CALL(
      lion_vector_with_capacity(&app, power.len, sizeof(double), &in_temp),
      "Failed creating internal temperature vector");

  log_info("Running application");
  LION_CALL(lion_app_run(&app, &power, &amb_temp), "Failed running app");

  log_info("Printing stuff");
  for (int i = 0; i < sf_temp.len; i++) {
    double p = lion_vector_get_d(&app, &power, i);
    double sf = lion_vector_get_d(&app, &sf_temp, i);
    double in = lion_vector_get_d(&app, &in_temp, i);
    printf("%d, %f -> %f/%f\n", i, p, sf, in);
  }

  log_info("Cleaning up");
  LION_CALL(lion_vector_cleanup(&app, &power), "Failed cleaning power vector");
  LION_CALL(lion_vector_cleanup(&app, &amb_temp),
            "Failed cleaning ambient temperature vector");
  LION_CALL(lion_vector_cleanup(&app, &sf_temp),
            "Failed cleaning surface temperature vector");
  LION_CALL(lion_vector_cleanup(&app, &in_temp),
            "Failed cleaning internal temperature vector");
}
