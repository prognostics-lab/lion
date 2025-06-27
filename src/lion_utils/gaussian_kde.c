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

void lion_gaussian_kde_init(double *data, size_t len, lion_gaussian_kde_bwmethod_t method, lion_gaussian_kde_t *out) {

  // Calculate the covariance factor
  double factor;
  switch (method) {
  case LION_GAUSSIAN_KDE_SCOTT:
    factor = scott_factor(len);
  case LION_GAUSSIAN_KDE_SILVERMAN:
    factor = silverman_factor(len);
  }

  double              variance = factor * factor * gsl_stats_variance(data, 1, len);
  double              std      = sqrt(variance);
  gsl_rng            *rng      = gsl_rng_alloc(gsl_rng_taus);
  lion_gaussian_kde_t result   = {.data = data, .len = len, .variance = variance, .std = std, .rng = rng};

  *out = result;
}

void lion_gaussian_kde_cleanup(lion_gaussian_kde_t *kde) { gsl_rng_free(kde->rng); }

double lion_gaussian_kde_sample(const lion_gaussian_kde_t *kde) {
  double            sample = gsl_ran_gaussian(kde->rng, kde->std);
  unsigned long int idx    = gsl_rng_uniform_int(kde->rng, kde->len);
  double            mean   = kde->data[idx];
  return mean + sample;
}
