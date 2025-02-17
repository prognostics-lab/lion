#include <lion/lion.h>
#include <lion_utils/test.h>
#include <lionu/log.h>
#include <lionu/macros.h>
#include <stddef.h>
#include <stdint.h>

lion_status_t test_creation_new(lion_sim_t *sim) {
  lion_vector_t vec;

  log_debug("Creating vector");
  LION_CALL(lion_vector_new(sim, sizeof(uint32_t), &vec), "Failed creating vector");

  log_debug("Checking creation result");
  LION_ASSERT_EQI(vec.data, NULL);
  LION_ASSERT_EQI(vec.capacity, 0);
  LION_ASSERT_EQI(vec.len, 0);

  LION_CALL(lion_vector_cleanup(sim, &vec), "Failed to clean up");
  return LION_STATUS_SUCCESS;
}

lion_status_t test_creation_zero(lion_sim_t *sim) {
  lion_vector_t vec;
  size_t        len = 10;

  log_info("Creating vector");
  LION_CALL(lion_vector_zero(sim, len, sizeof(uint32_t), &vec), "Failed creating vector");

  log_debug("Checking creation result");
  for (uint32_t i = 0; i < len; i++) {
    LION_ASSERT_EQI(*(uint32_t *)((char *)vec.data + i * sizeof(uint32_t)), 0);
  }

  LION_CALL(lion_vector_cleanup(sim, &vec), "Failed to clean up");
  return LION_STATUS_SUCCESS;
}

lion_status_t test_creation_with_capacity(lion_sim_t *sim) {
  lion_vector_t vec;

  log_debug("Creating vector");
  LION_CALL(lion_vector_with_capacity(sim, 6, sizeof(uint32_t), &vec), "Failed creating vector");

  log_debug("Checking creation result");
  LION_ASSERT_NEI(vec.data, NULL);
  LION_ASSERT_NEI(vec.capacity, 0);
  LION_ASSERT_EQI(vec.data_size, sizeof(uint32_t));
  LION_ASSERT_EQI(vec.len, 0);

  LION_CALL(lion_vector_cleanup(sim, &vec), "Failed to clean up");
  return LION_STATUS_SUCCESS;
}

lion_status_t test_creation_from_array(lion_sim_t *sim) {
  lion_vector_t vec;
  uint32_t      data[] = {0, 1, 2, 3, 4, 5, 6, 7};
  size_t        len    = 8;

  log_info("Creating vector");
  LION_CALL(lion_vector_from_array(sim, data, len, sizeof(uint32_t), &vec), "Failed creating vector");

  log_debug("Checking creation result");
  for (uint32_t i = 0; i < len; i++) {
    LION_ASSERT_EQI(*(uint32_t *)((char *)vec.data + i * sizeof(uint32_t)), data[i]);
  }

  LION_CALL(lion_vector_cleanup(sim, &vec), "Failed to clean up");
  return LION_STATUS_SUCCESS;
}

lion_status_t test_creation_from_csv(lion_sim_t *sim) {
  const char   *FILENAME = LION_PROJECT_ROOT_DIR "tests/unittest/quick/resources/vector_create1.csv";
  lion_vector_t vec;
  double        expected[] = {1.1, 2.2, 3.3, 4.4, 5.5};
  size_t        len        = 5;

  log_info("Creating vector");
  LION_CALL(lion_vector_from_csv(sim, FILENAME, sizeof(double), "%lf", &vec), "Failed creating vector from csv");

  log_debug("Checking creation result");
  LION_ASSERT_EQI(vec.len, len);
  LION_ASSERT_EQI(vec.capacity, len);
  for (uint32_t i = 0; i < len; i++) {
    LION_ASSERT_EQF(((double *)vec.data)[i], expected[i]);
    LION_ASSERT_EQF(lion_vector_get_d(sim, &vec, i), expected[i]);
  }

  LION_CALL(lion_vector_cleanup(sim, &vec), "Failed to clean up");
  return LION_STATUS_SUCCESS;
}

int main(void) {
  LION_CALL_TEST(NULL, test_creation_new);
  LION_CALL_TEST(NULL, test_creation_zero);
  LION_CALL_TEST(NULL, test_creation_with_capacity);
  LION_CALL_TEST(NULL, test_creation_from_array);
  LION_CALL_TEST(NULL, test_creation_from_csv);
  return TEST_PASS;
}
