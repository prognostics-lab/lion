#include <lion/lion.h>
#include <lionu/log.h>
#include <lionu/macros.h>

#define POWER_FILENAME "data/240716_temp_prfile_C4B1/TestData.csv"
#define AMBTEMP_FILENAME "data/240716_temp_prfile_C4B1/chamber.csv"

int main(void) {
  log_info("Setting up configuration");
  lion_app_config_t conf = lion_app_config_default();
  conf.log_dir = "logs";
  conf.log_stdlvl = LOG_DEBUG;

  log_info("Setting up simulation parameters");
  lion_params_t params = lion_params_default();

  log_info("Creating application");
  lion_app_t app;
  LION_CALL(lion_app_new(&conf, &params, &app), "Failed creating application");

  log_info("Configuring system inputs");
  lion_vector_t power;
  lion_vector_t amb_temp;
  LION_VCALL(lion_vector_from_csv(&app, POWER_FILENAME, sizeof(double), &power),
             "Failed creating power profile from csv file '%s'",
             POWER_FILENAME);
  LION_VCALL(
      lion_vector_from_csv(&app, AMBTEMP_FILENAME, sizeof(double), &amb_temp),
      "Failed creating ambient temperature profile from csv file '%s'",
      AMBTEMP_FILENAME);

  log_info("Running application");
  return lion_app_run(&app, &power, &amb_temp);
}
