#include <lion/lion.h>
#include <lion_utils/test.h>
#include <lionu/log.h>
#include <lionu/macros.h>
#include <stddef.h>
#include <stdint.h>

lion_status_t test_modification_push1(lion_app_t *app) {
  lion_vector_t vec;
  uint32_t      data[] = {0, 1, 2, 3, 4, 5, 6, 7};
  size_t        len    = sizeof(data) / sizeof(uint32_t);

  log_debug("Creating vector");
  LION_CALL(lion_vector_from_array(app, data, len, sizeof(uint32_t), &vec), "Failed creating vector");
  LION_ASSERT_EQI(vec.capacity, 8);

  log_debug("Pushing element");
  uint32_t val = 20;
  LION_CALL(lion_vector_push(app, &vec, &val), "Failed pushing element");

  log_debug("Checking that the dimensions changed properly");
  LION_ASSERT_EQI(vec.len, 9);
  LION_ASSERT_EQI(vec.capacity, 16);

  log_debug("Checking border scenario");
  LION_ASSERT_FAILS(lion_vector_push(app, &vec, NULL));
  log_debug("Checking that the failed push does not modify the vector");
  LION_ASSERT_EQI(vec.len, 9);
  LION_ASSERT_EQI(vec.capacity, 16);

  log_debug("Checking that the push happened fine");
  uint32_t contained_val;
  LION_CALL(lion_vector_get(app, &vec, 8, &contained_val), "Failed getting vec[8]");
  LION_ASSERT_EQI(contained_val, val);

  LION_CALL(lion_vector_cleanup(app, &vec), "Failed to clean up");
  return LION_STATUS_SUCCESS;
}

lion_status_t test_modification_push2(lion_app_t *app) {
  lion_vector_t vec;
  size_t        capacity = 8;

  log_debug("Creating vector");
  LION_CALL(lion_vector_with_capacity(app, capacity, sizeof(uint32_t), &vec), "Failed creating vector");

  log_debug("Pushing element");
  uint32_t val = 20;
  LION_CALL(lion_vector_push(app, &vec, &val), "Failed pushing element");

  log_debug("Checking that the dimensions changed properly");
  LION_ASSERT_EQI(vec.len, 1);
  LION_ASSERT_EQI(vec.capacity, capacity);

  log_debug("Checking that the push happened fine");
  uint32_t contained_val;
  LION_CALL(lion_vector_get(app, &vec, 0, &contained_val), "Failed getting vec[0]");
  LION_ASSERT_EQI(contained_val, val);

  LION_CALL(lion_vector_cleanup(app, &vec), "Failed to clean up");
  return LION_STATUS_SUCCESS;
}

lion_status_t test_modification_extend_array(lion_app_t *app) {
  lion_vector_t vec;
  uint32_t      data[] = {0, 1, 2, 3, 4, 5, 6, 7};
  size_t        len    = 8;

  log_debug("Creating vector");
  LION_CALL(lion_vector_from_array(app, data, len, sizeof(uint32_t), &vec), "Failed creating vector");

  log_debug("Extending vector");
  uint32_t values[]   = {20, 19, 15};
  size_t   values_len = 3;
  LION_CALL(lion_vector_extend_array(app, &vec, values, values_len), "Failed extending array");

  log_debug("Checking that the dimensions changed properly");
  LION_ASSERT_EQI(vec.len, 11);
  LION_ASSERT_EQI(vec.capacity, 11);

  log_debug("Checking border scenario");
  LION_ASSERT_FAILS(lion_vector_extend_array(app, &vec, NULL, 10010));
  log_debug("Checking that the failed extend does not modify the vector");
  LION_ASSERT_EQI(vec.len, 11);
  LION_ASSERT_EQI(vec.capacity, 11);

  log_debug("Checking that the extend happened fine");
  uint32_t contained_val;
  for (uint32_t i = 0; i < len; i++) {
    LION_VCALL(lion_vector_get(app, &vec, i, &contained_val), "Failed getting vec[%d]", i);
    logi_debug("Found %u", contained_val);
    LION_ASSERT_EQI(contained_val, data[i]);
  }
  for (uint32_t i = 0; i < values_len; i++) {
    LION_VCALL(lion_vector_get(app, &vec, i + len, &contained_val), "Failed getting vec[%d]", i + len);
    logi_debug("Found %u", contained_val);
    LION_ASSERT_EQI(contained_val, values[i]);
  }
  log_debug("Found no issues");

  LION_CALL(lion_vector_cleanup(app, &vec), "Failed to clean up");
  return LION_STATUS_SUCCESS;
}

int main(void) {
  LION_CALL_TEST(NULL, test_modification_push1);
  LION_CALL_TEST(NULL, test_modification_push2);
  LION_CALL_TEST(NULL, test_modification_extend_array);
  return TEST_PASS;
}
