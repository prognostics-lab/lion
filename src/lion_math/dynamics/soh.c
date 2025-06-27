#include "soh.h"

#include <lion_utils/vendor/log.h>
#include <math.h>

double lion_soh_next_vendor(double soh, double soc_mean, double soc_max, double soc_min, double internal_temperature, lion_params_t *params) {
  lion_params_soh_vendor_t *p = &params->soh.params.vendor;

  double rate = exp(log(p->final_soh) / (double)p->total_cycles);
  return rate * soh;
}

double degradation_factor(double soc_mean, double soc_max, double soc_min, lion_params_degradation_element_t *table) {
  // TODO: Implement kNN to estimate degradation factor
  // TODO: Evaluate implementing kNN through an outside library
  return 1.0;
}

double temperature_factor(double temperature) {
  // TODO: Implement temperature consideration model
  return 1.0;
}

double lion_soh_next_masserano(double soh, double soc_mean, double soc_max, double soc_min, double internal_temperature, lion_params_t *params) {
  lion_params_soh_masserano_t *p            = &params->soh.params.masserano;
  double                       total_cycles = (double)p->total_cycles;

  // Correct from SoC changes
  double soc_factor = degradation_factor(soc_mean, soc_max, soc_min, p->table);
  total_cycles      = soc_factor * total_cycles;

  // Correct from temperature
  double temp_factor = temperature_factor(internal_temperature);
  total_cycles       = temp_factor * total_cycles;

  double rate = exp(log(p->final_soh) / total_cycles);

  // TODO: Implement KDE to introduce noise into the rate
  return rate * soh;
}

double lion_soh_next(double soh, double soc_mean, double soc_max, double soc_min, double internal_temperature, lion_params_t *params) {
  switch (params->soh.model) {
  case LION_SOH_MODEL_VENDOR:
    return lion_soh_next_vendor(soh, soc_mean, soc_max, soc_min, internal_temperature, params);
  case LION_SOH_MODEL_MASSERANO:
    return lion_soh_next_masserano(soh, soc_mean, soc_max, soc_min, internal_temperature, params);
  default:
    logi_error("Degradation model not valid");
    return -1.0;
  }
}
