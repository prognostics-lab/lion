#include "soh.h"

#include "lion/vector.h"

#include <lion_utils/vendor/log.h>
#include <lionu/math.h>
#include <math.h>

double lion_soh_next_vendor(
    lion_sim_t *sim, double soh, double soc_mean, double soc_max, double soc_min, double internal_temperature, lion_params_t *params
) {
  lion_params_soh_vendor_t *p = &params->soh.params.vendor;

  double rate = exp(log(p->final_soh) / (double)p->total_cycles);
  return rate * soh;
}

double degradation_factor(lion_sim_t *sim, double soc_mean, double soc_max, double soc_min, double eq_final_soh, lion_knn_regressor_t *knn) {
  lion_vector_t input;
  double        data[3] = {soc_mean, soc_max - soc_min, eq_final_soh};
  lion_vector_from_array(sim, data, 3, sizeof(double), &input);
  return lion_knn_regressor_predict(sim, knn, &input);
}

double temperature_factor(double temperature, double *poly_coeffs, uint32_t count) { return lion_polyval_d(temperature - 273.0, poly_coeffs, count); }

double lion_soh_next_masserano(
    lion_sim_t *sim, double soh, double soc_mean, double soc_max, double soc_min, double internal_temperature, lion_params_t *params
) {
  lion_params_soh_masserano_t *p            = &params->soh.params.masserano;
  double                       total_cycles = (double)p->eq_cycles;

  // Correct from SoC changes
  double soc_factor = degradation_factor(sim, soc_mean, soc_max, soc_min, p->eq_final_soh, &p->knn);
  logi_debug("kNN factor : %lf", soc_factor);
  total_cycles = soc_factor * total_cycles;

  // Correct from temperature
  double temp_factor = temperature_factor(internal_temperature, p->temp_poly, LION_SOH_TEMP_POLYCOUNT);
  temp_factor        = lion_clip_d(temp_factor, 0.0, 1.0);
  logi_debug("Temperature factor : %lf", temp_factor);
  total_cycles = temp_factor * total_cycles;

  double noise = 0.0;
  double BIAS  = 0.999161393145505;
  if (p->kde.is_trained)
    noise = lion_gaussian_kde_sample(&p->kde) - BIAS;
  double base_rate = exp(log(p->eq_final_soh) / total_cycles);
  double rate      = lion_clip_d(base_rate + noise, 0.0, 1.0);

  logi_debug("Final SoH: %lf", p->nominal_final_soh);
  logi_debug("Total cycles: %lf", total_cycles);
  logi_debug("Base rate: %lf", base_rate);
  logi_debug("Rate: %lf", rate);
  logi_debug("Noise: %lf", noise);

  return rate * soh;
}

double lion_soh_next(
    lion_sim_t *sim, double soh, double soc_mean, double soc_max, double soc_min, double internal_temperature, lion_params_t *params
) {
  switch (params->soh.model) {
  case LION_SOH_MODEL_VENDOR:
    return lion_soh_next_vendor(sim, soh, soc_mean, soc_max, soc_min, internal_temperature, params);
  case LION_SOH_MODEL_MASSERANO:
    if (!params->soh.params.masserano.kde.is_trained) {
      logi_warn("Default Masserano model requires manually setting up the KDE");
      logi_warn("Masserano model will skip sampling noise model");
    }
    return lion_soh_next_masserano(sim, soh, soc_mean, soc_max, soc_min, internal_temperature, params);
  default:
    logi_error("Degradation model not valid");
    return -1.0;
  }
}
