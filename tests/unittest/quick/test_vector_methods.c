#include <lion/lion.h>
#include <lion_utils/test.h>
#include <lionu/log.h>
#include <lionu/macros.h>
#include <stddef.h>
#include <stdint.h>

lion_status_t test_methods_get(lion_sim_t *sim) {
  lion_vector_t vec;
  uint32_t      data[] = {7, 6, 5, 4, 3, 2, 1, 0};
  size_t        len    = 8;

  log_debug("Creating vector");
  LION_CALL(lion_vector_from_array(sim, data, len, sizeof(uint32_t), &vec), "Failed creating vector");

  log_debug("Checking all the elements");
  uint32_t val;
  for (uint32_t i = 0; i < len; i++) {
    LION_VCALL(lion_vector_get(sim, &vec, i, &val), "Failed getting vec[%d]", i);
    LION_ASSERT_EQI(val, data[i]);
  }

  log_debug("Checking border scenarios");
  LION_ASSERT_FAILS(lion_vector_get(sim, &vec, len, &val));

  // Check that the failed call to lion_vector_get did not accidentally
  // modify val
  LION_ASSERT_EQI(val, data[len - 1]);

  LION_CALL(lion_vector_cleanup(sim, &vec), "Failed to clean up");
  return LION_STATUS_SUCCESS;
}

lion_status_t test_methods_set(lion_sim_t *sim) {
  lion_vector_t vec;
  uint32_t      data[] = {7, 6, 5, 4, 3, 2, 1, 0};
  size_t        len    = 8;

  log_debug("Creating vector");
  LION_CALL(lion_vector_from_array(sim, data, len, sizeof(uint32_t), &vec), "Failed creating vector");

  log_debug("Checking all the elements before setting");
  uint32_t val;
  for (uint32_t i = 0; i < len; i++) {
    LION_VCALL(lion_vector_get(sim, &vec, i, &val), "Failed getting vec[%d]", i);
    LION_ASSERT_EQI(val, data[i]);
  }

  log_debug("Changing two elements");
  size_t   i0   = 2;
  size_t   i1   = 6;
  uint32_t new0 = 20;
  uint32_t new1 = 1881;
  LION_CALL(lion_vector_set(sim, &vec, i0, &new0), "Failed setting elements");
  LION_CALL(lion_vector_set(sim, &vec, i1, &new1), "Failed setting elements");

  log_debug("Checking elements after setting");
  for (uint32_t i = 0; i < len; i++) {
    LION_VCALL(lion_vector_get(sim, &vec, i, &val), "Failed getting vec[%d]", i);
    if (i == i0) {
      LION_ASSERT_EQI(val, new0);
    } else if (i == i1) {
      LION_ASSERT_EQI(val, new1);
    } else {
      LION_ASSERT_EQI(val, data[i]);
    }
  }

  log_debug("Checking border scenarios");
  LION_ASSERT_FAILS(lion_vector_set(sim, &vec, len - 1, NULL));

  // Check that the failed call to lion_vector_get did not accidentally
  // modify val
  LION_ASSERT_EQI(val, data[len - 1]);

  LION_CALL(lion_vector_cleanup(sim, &vec), "Failed to clean up");
  return LION_STATUS_SUCCESS;
}

int main(void) {
  LION_CALL_TEST(NULL, test_methods_get);
  LION_CALL_TEST(NULL, test_methods_set);
  return TEST_PASS;
}
