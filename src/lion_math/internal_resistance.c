#include "internal_resistance.h"

#include <lion/lion.h>
#include <lionu/fuzzy.h>
#include <lionu/math.h>

double lion_resistance(double soc, double current, lion_params_t *params) {
  // Evaluate memberships
  double c40 = lion_mf_sigmoid(current, &params->rint.c40);
  double c20 = lion_mf_gaussian(current, &params->rint.c20);
  double c10 = lion_mf_gaussian(current, &params->rint.c10);
  double c4  = lion_mf_gaussian(current, &params->rint.c4);
  double d5  = lion_mf_gaussian(current, &params->rint.d5);
  double d10 = lion_mf_gaussian(current, &params->rint.d10);
  double d15 = lion_mf_gaussian(current, &params->rint.d15);
  double d30 = lion_mf_sigmoid(current, &params->rint.d30);

  double memberships[LION_FUZZY_SETS_COUNT] = {c40, c20, c10, c4, d5, d10, d15, d30};
  double memberships_sum                    = lion_sum_d(memberships, LION_FUZZY_SETS_COUNT);

  // Calculate resulting internal resistance
  double num = 0;
  for (int i = 0; i < LION_FUZZY_SETS_COUNT; i++) {
    num += memberships[i] * lion_polyval_d(soc, params->rint.poly[i], LION_FUZZY_SETS_DEGREE);
  }
  return num / memberships_sum;
}
