#include <lion/lion.h>
#include <lionu/log.h>

int main(void) {
  log_info("Setting up configuration");
  lion_app_config_t conf = lion_app_config_default();
  conf.log_dir = "logs";
  conf.log_stdlvl = LOG_DEBUG;

  log_info("Setting up simulation parameters");
  lion_params_t params = lion_params_default();

  lion_app_t app;
  lion_app_new(&conf) return lion_app_run(&app);
}
