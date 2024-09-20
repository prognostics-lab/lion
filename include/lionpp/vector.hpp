#pragma once

#include <vector>

#include <lion/vector.h>

namespace lion {

template <typename T> std::vector<T> vector_to_std(lion_vector_t *vec) {
  return std::vector<T>((T *)vec->data, (T *)vec->data + vec->len);
}

} // namespace lion
