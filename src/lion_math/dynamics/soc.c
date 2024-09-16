#include <lion/lion.h>
#include <lion_utils/vendor/log.h>

#include "soc.h"

double lion_soc_d(double current, double usable_capacity,
                  lion_params_t *params) {
  double diff = -current / usable_capacity;
  return diff;
}
