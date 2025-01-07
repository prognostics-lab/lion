#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lion_mf_gaussian_params {
  double mean;
  double sigma;
} lion_mf_gaussian_params_t;

double lion_mf_gaussian(double x, lion_mf_gaussian_params_t *params);

#ifdef __cplusplus
}
#endif
