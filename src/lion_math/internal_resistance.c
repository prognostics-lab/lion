#include "internal_resistance.h"

#include <lion/lion.h>
#include <lion_utils/vendor/log.h>
#include <lionu/fuzzy.h>
#include <lionu/math.h>

// TODO: Consider a version of this code with a fixed internal resistance

double lion_resistance_fixed(double soc, double current, lion_params_t *params) {
  lion_params_rint_fixed_t *p = params->rint.params;
  return p->internal_resistance;
}

double lion_resistance_polarization(double soc, double current, lion_params_t *params) {
  lion_params_rint_polarization_t *p = params->rint.params;

  // Evaluate memberships
  double c40 = lion_mf_sigmoid(current, &p->c40);
  double c20 = lion_mf_gaussian(current, &p->c20);
  double c10 = lion_mf_gaussian(current, &p->c10);
  double c4  = lion_mf_gaussian(current, &p->c4);
  double d5  = lion_mf_gaussian(current, &p->d5);
  double d10 = lion_mf_gaussian(current, &p->d10);
  double d15 = lion_mf_gaussian(current, &p->d15);
  double d30 = lion_mf_sigmoid(current, &p->d30);

  double memberships[LION_FUZZY_SETS_COUNT] = {c40, c20, c10, c4, d5, d10, d15, d30};
  double memberships_sum                    = lion_sum_d(memberships, LION_FUZZY_SETS_COUNT);

  // Calculate resulting internal resistance
  double num = 0;
  for (int i = 0; i < LION_FUZZY_SETS_COUNT; i++) {
    num += memberships[i] * lion_polyval_d(soc, p->poly[i], LION_FUZZY_SETS_DEGREE);
  }
  return num / memberships_sum;
}

double lion_resistance(double soc, double current, lion_params_t *params) {
  switch (params->rint.model) {
  case LION_RINT_MODEL_FIXED:
    return lion_resistance_fixed(soc, current, params);
  case LION_RINT_MODEL_POLARIZATION:
    return lion_resistance_polarization(soc, current, params);
  default:
    logi_error("Internal resistance model not valid");
    return -1.0;
  }
}
