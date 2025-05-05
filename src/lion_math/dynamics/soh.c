#include "soh.h"

#include <math.h>

// TODO: Expand the model to include the different effects

double degradation_factor(double soc_mean, double soc_max, double soc_min, lion_params_degradation_element_t *table) {
  // TODO: Implement kNN to estimate degradation factor
  // TODO: Evaluate implementing kNN through an outside library
  return 1.0;
}

double lion_soh_next(double soh, double soc_mean, double soc_max, double soc_min, lion_params_t *params) {
  double correction   = degradation_factor(soc_mean, soc_max, soc_min, params->soh.table);
  double total_cycles = correction * (double)params->soh.total_cycles;

  // TODO: Evaluate implementing temperature correction

  double rate = exp(log(params->soh.final_soh) / total_cycles);

  // TODO: Implement KDE to introduce noise into the rate
  return rate * soh;
}
