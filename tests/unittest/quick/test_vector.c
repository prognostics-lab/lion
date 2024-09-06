#include <lion/lion.h>
#include <lion_utils/test.h>
#include <lionu/log.h>
#include <lionu/macros.h>
#include <stddef.h>
#include <stdint.h>

lion_status_t test_creation_new(lion_app_t *app) {
  lion_vector_t vec;

  log_debug("Creating vector");
  LION_CALL(lion_vector_new(app, sizeof(uint32_t), &vec),
            "Failed creating vector");

  log_debug("Checking creation result");
  LION_ASSERT_EQI(vec.data, NULL);
  LION_ASSERT_EQI(vec.capacity, 0);
  LION_ASSERT_EQI(vec.len, 0);
  return LION_STATUS_SUCCESS;
}

lion_status_t test_creation_zero(lion_app_t *app) {
  lion_vector_t vec;
  size_t len = 10;

  log_info("Creating vector");
  LION_CALL(lion_vector_zero(app, len, sizeof(uint32_t), &vec),
            "Failed creating vector");

  log_debug("Checking creation result");
  for (uint32_t i = 0; i < len; i++) {
    LION_ASSERT_EQI(*(uint32_t *)((char *)vec.data + i * sizeof(uint32_t)), 0);
  }
  return LION_STATUS_SUCCESS;
}

lion_status_t test_creation_with_capacity(lion_app_t *app) {
  lion_vector_t vec;

  log_debug("Creating vector");
  LION_CALL(lion_vector_with_capacity(app, 6, sizeof(uint32_t), &vec),
            "Failed creating vector");

  log_debug("Checking creation result");
  LION_ASSERT_NEI(vec.data, NULL);
  LION_ASSERT_NEI(vec.capacity, 0);
  LION_ASSERT_EQI(vec.data_size, sizeof(uint32_t));
  LION_ASSERT_EQI(vec.len, 0);
  return LION_STATUS_SUCCESS;
}

lion_status_t test_creation_from_array(lion_app_t *app) {
  lion_vector_t vec;
  uint32_t data[] = {0, 1, 2, 3, 4, 5, 6, 7};
  size_t len = 8;

  log_info("Creating vector");
  LION_CALL(lion_vector_from_array(app, data, len, sizeof(uint32_t), &vec),
            "Failed creating vector");

  log_debug("Checking creation result");
  for (uint32_t i = 0; i < len; i++) {
    LION_ASSERT_EQI(*(uint32_t *)((char *)vec.data + i * sizeof(uint32_t)),
                    data[i]);
  }

  return LION_STATUS_SUCCESS;
}

lion_status_t test_methods_get(lion_app_t *app) {
  lion_vector_t vec;
  uint32_t data[] = {7, 6, 5, 4, 3, 2, 1, 0};
  size_t len = 8;

  log_debug("Creating vector");
  LION_CALL(lion_vector_from_array(app, data, len, sizeof(uint32_t), &vec),
            "Failed creating vector");

  log_debug("Checking all the elements");
  uint32_t val;
  for (uint32_t i = 0; i < len; i++) {
    LION_VCALL(lion_vector_get(app, &vec, i, &val), "Failed getting vec[%d]",
               i);
    LION_ASSERT_EQI(val, data[i]);
  }

  log_debug("Checking border scenarios");
  LION_ASSERT_FAILS(lion_vector_get(app, &vec, len, &val));

  // Check that the failed call to lion_vector_get did not accidentally
  // modify val
  LION_ASSERT_EQI(val, data[len - 1]);
  return LION_STATUS_SUCCESS;
}

lion_status_t test_methods_set(lion_app_t *app) {
  lion_vector_t vec;
  uint32_t data[] = {7, 6, 5, 4, 3, 2, 1, 0};
  size_t len = 8;

  log_debug("Creating vector");
  LION_CALL(lion_vector_from_array(app, data, len, sizeof(uint32_t), &vec),
            "Failed creating vector");

  log_debug("Checking all the elements before setting");
  uint32_t val;
  for (uint32_t i = 0; i < len; i++) {
    LION_VCALL(lion_vector_get(app, &vec, i, &val), "Failed getting vec[%d]",
               i);
    LION_ASSERT_EQI(val, data[i]);
  }

  log_debug("Changing two elements");
  size_t i0 = 2;
  size_t i1 = 6;
  uint32_t new0 = 20;
  uint32_t new1 = 1881;
  LION_CALL(lion_vector_set(app, &vec, i0, &new0), "Failed setting elements");
  LION_CALL(lion_vector_set(app, &vec, i1, &new1), "Failed setting elements");

  log_debug("Checking elements after setting");
  for (uint32_t i = 0; i < len; i++) {
    LION_VCALL(lion_vector_get(app, &vec, i, &val), "Failed getting vec[%d]",
               i);
    if (i == i0) {
      LION_ASSERT_EQI(val, new0);
    } else if (i == i1) {
      LION_ASSERT_EQI(val, new1);
    } else {
      LION_ASSERT_EQI(val, data[i]);
    }
  }

  log_debug("Checking border scenarios");
  LION_ASSERT_FAILS(lion_vector_set(app, &vec, len - 1, NULL));

  // Check that the failed call to lion_vector_get did not accidentally
  // modify val
  LION_ASSERT_EQI(val, data[len - 1]);
  return LION_STATUS_SUCCESS;
}

lion_status_t test_modification_push1(lion_app_t *app) {
  lion_vector_t vec;
  uint32_t data[] = {0, 1, 2, 3, 4, 5, 6, 7};
  size_t len = sizeof(data) / sizeof(uint32_t);

  log_debug("Creating vector");
  LION_CALL(lion_vector_from_array(app, data, len, sizeof(uint32_t), &vec),
            "Failed creating vector");
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
  LION_CALL(lion_vector_get(app, &vec, 8, &contained_val),
            "Failed getting vec[8]");
  LION_ASSERT_EQI(contained_val, val);
  return LION_STATUS_SUCCESS;
}

lion_status_t test_modification_push2(lion_app_t *app) {
  lion_vector_t vec;
  size_t capacity = 8;

  log_debug("Creating vector");
  LION_CALL(lion_vector_with_capacity(app, capacity, sizeof(uint32_t), &vec),
            "Failed creating vector");

  log_debug("Pushing element");
  uint32_t val = 20;
  LION_CALL(lion_vector_push(app, &vec, &val), "Failed pushing element");

  log_debug("Checking that the dimensions changed properly");
  LION_ASSERT_EQI(vec.len, 1);
  LION_ASSERT_EQI(vec.capacity, capacity);

  log_debug("Checking that the push happened fine");
  uint32_t contained_val;
  LION_CALL(lion_vector_get(app, &vec, 0, &contained_val),
            "Failed getting vec[0]");
  LION_ASSERT_EQI(contained_val, val);
  return LION_STATUS_SUCCESS;
}

lion_status_t test_modification_extend_array(lion_app_t *app) {
  lion_vector_t vec;
  uint32_t data[] = {0, 1, 2, 3, 4, 5, 6, 7};
  size_t len = 8;

  log_debug("Creating vector");
  LION_CALL(lion_vector_from_array(app, data, len, sizeof(uint32_t), &vec),
            "Failed creating vector");

  log_debug("Extending vector");
  uint32_t values[] = {20, 19, 15};
  size_t values_len = 3;
  LION_CALL(lion_vector_extend_array(app, &vec, values, values_len),
            "Failed extending array");

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
    LION_VCALL(lion_vector_get(app, &vec, i, &contained_val),
               "Failed getting vec[%d]", i);
    logi_debug("Found %u", contained_val);
    LION_ASSERT_EQI(contained_val, data[i]);
  }
  for (uint32_t i = 0; i < values_len; i++) {
    LION_VCALL(lion_vector_get(app, &vec, i + len, &contained_val),
               "Failed getting vec[%d]", i + len);
    logi_debug("Found %u", contained_val);
    LION_ASSERT_EQI(contained_val, values[i]);
  }
  log_debug("Found no issues");
  return LION_STATUS_SUCCESS;
}

int main(void) {
  LION_CALL_TEST(NULL, test_creation_new);
  LION_CALL_TEST(NULL, test_creation_zero);
  LION_CALL_TEST(NULL, test_creation_with_capacity);
  LION_CALL_TEST(NULL, test_creation_from_array);

  LION_CALL_TEST(NULL, test_methods_get);
  LION_CALL_TEST(NULL, test_methods_set);

  LION_CALL_TEST(NULL, test_modification_push1);
  LION_CALL_TEST(NULL, test_modification_push2);
  LION_CALL_TEST(NULL, test_modification_extend_array);
  return TEST_PASS;
}
