#include "soh.h"

#include <math.h>

double lion_soh_next(double soh, double soc_mean, double soc_max, double soc_min, lion_params_t *params) {
  double rate = exp(log(params->soh.final_soh) / params->soh.total_cycles);
  return rate * soh;
}
