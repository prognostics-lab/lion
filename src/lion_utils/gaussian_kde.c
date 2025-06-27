#include "vendor/log.h"

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_statistics.h>
#include <lionu/kde.h>
#include <math.h>

double scott_factor(size_t len) { return pow((double)len, -0.2); }

double silverman_factor(size_t len) {
  // TODO: Properly implement Silverman's factor
  return pow((double)len, -0.2);
}

lion_status_t lion_gaussian_kde_init(double *data, size_t len, lion_gaussian_kde_bwmethod_t method, unsigned long seed, lion_gaussian_kde_t *out) {
  // Calculate the covariance factor
  double factor;
  switch (method) {
  case LION_GAUSSIAN_KDE_SCOTT:
    factor = scott_factor(len);
    break;
  case LION_GAUSSIAN_KDE_SILVERMAN:
    factor = silverman_factor(len);
    break;
  default:
    logi_error("Could not read KDE bandwidth method [%i]", method);
    return LION_STATUS_FAILURE;
  }

  double variance = factor * factor * gsl_stats_variance(data, 1, len);
  double std      = sqrt(variance);

  gsl_rng_env_setup();
  const gsl_rng_type *T;
  T = gsl_rng_default;
  logi_info("Initializing rng, type '%s', seed %i", T->name, seed);
  gsl_rng *rng = gsl_rng_alloc(T);

  lion_gaussian_kde_t result = {.data = data, .len = len, .variance = variance, .std = std, .rng = rng};

  *out = result;
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_gaussian_kde_cleanup(lion_gaussian_kde_t *kde) {
  gsl_rng_free(kde->rng);
  return LION_STATUS_SUCCESS;
}

double lion_gaussian_kde_sample(const lion_gaussian_kde_t *kde) {
  double            sample = gsl_ran_gaussian(kde->rng, kde->std);
  unsigned long int idx    = gsl_rng_uniform_int(kde->rng, kde->len);
  double            mean   = kde->data[idx];
  return mean + sample;
}
