#include <lionm/lionm.h>

double lion_soc_d(double current, double usable_capacity,
                  lion_params_t *params) {
  return -current / usable_capacity;
}
