#include <lion/lion.h>
#include <lion_utils/test.h>
#include <lionu/log.h>
#include <lionu/macros.h>
#include <stddef.h>
#include <stdint.h>

// TODO: Add tests for all calculations

lion_status_t test_current(lion_sim_t *sim) {
  // TODO: Add calculation of current
  return TEST_PASS;
}

lion_status_t setup_test(lion_sim_t *sim) {
  lion_sim_config_t conf = lion_sim_config_default();
  conf.log_dir           = "logs";
  conf.log_stdlvl        = LOG_DEBUG;

  lion_params_t params = lion_params_default();

  LION_CALL(lion_sim_new(&conf, &params, sim), "Failed creating sim for test");
  return LION_STATUS_SUCCESS;
}

int main() {
  lion_sim_t sim;
  LION_CALL(setup_test(&sim), "Failed setting up tests");

  LION_CALL_TEST(&sim, test_current);

  return TEST_PASS;
}
