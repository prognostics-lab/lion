#include <lion/lion.h>
#include <lion_utils/test.h>
#include <lionu/log.h>
#include <lionu/macros.h>
#include <stddef.h>
#include <stdint.h>

// TODO: Add tests for all calculations

lion_status_t test_current(lion_app_t *app) {
  // TODO: Add calculation of current
  return TEST_PASS;
}

lion_status_t setup_test(lion_app_t *app) {
  lion_app_config_t conf = lion_app_config_default();
  conf.log_dir           = "logs";
  conf.log_stdlvl        = LOG_DEBUG;

  lion_params_t params = lion_params_default();

  LION_CALL(lion_app_new(&conf, &params, app), "Failed creating app for test");
  return LION_STATUS_SUCCESS;
}

int main() {
  lion_app_t app;
  LION_CALL(setup_test(&app), "Failed setting up tests");

  LION_CALL_TEST(&app, test_current);

  return TEST_PASS;
}
