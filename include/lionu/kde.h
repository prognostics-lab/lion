#pragma once

#include <gsl/gsl_rng.h>
#include <lion/status.h>
#include <stddef.h>

/* This implementation is based on a simplification of scipy's implementation */

typedef enum lion_gaussian_kde_bwmethod {
  LION_GAUSSIAN_KDE_SCOTT,
  LION_GAUSSIAN_KDE_SILVERMAN,
} lion_gaussian_kde_bwmethod_t;

typedef struct lion_gaussian_kde {
  double  *data;
  size_t   len;
  double   variance;
  double   std;
  gsl_rng *rng;
} lion_gaussian_kde_t;

lion_status_t lion_gaussian_kde_init(double *data, size_t len, lion_gaussian_kde_bwmethod_t method, unsigned long seed, lion_gaussian_kde_t *out);
lion_status_t lion_gaussian_kde_cleanup(lion_gaussian_kde_t *kde);

double lion_gaussian_kde_sample(const lion_gaussian_kde_t *kde);
