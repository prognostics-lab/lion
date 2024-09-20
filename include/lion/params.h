#pragma once

#include <lionu/fuzzy.h>

#define LION_FUZZY_SETS_COUNT 8
#define LION_FUZZY_SETS_DEGREE 4

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lion_params_init {
  double initial_soc;
  double initial_internal_temperature;
  double initial_soh;
  double initial_capacity;
  double initial_current_guess;
} lion_params_init_t;

typedef struct lion_params_ehc {
  double a;
  double b;
  double mu;
  double kappa;
  double sigma;
  double lambda;
} lion_params_ehc_t;

typedef struct lion_params_vft {
  double k1;
  double k2;
  double tref;
} lion_params_vft_t;

typedef struct lion_params_temp {
  double cp;
  double rin;
  double rout;
} lion_params_temp_t;

typedef struct lion_params_ocv {
  double alpha;
  double beta;
  double gamma;
  double v0;
  double vl;
} lion_params_ocv_t;

typedef struct lion_params_rint {
  lion_mf_sigmoid_params_t c40;
  lion_mf_gaussian_params_t c20;
  lion_mf_gaussian_params_t c10;
  lion_mf_gaussian_params_t c4;

  lion_mf_gaussian_params_t d5;
  lion_mf_gaussian_params_t d10;
  lion_mf_gaussian_params_t d15;
  lion_mf_sigmoid_params_t d30;

  double poly[LION_FUZZY_SETS_COUNT][LION_FUZZY_SETS_DEGREE];
} lion_params_rint_t;

typedef struct lion_params {
  lion_params_init_t init;
  lion_params_ehc_t ehc;
  lion_params_ocv_t ocv;
  lion_params_vft_t vft;
  lion_params_temp_t t;
  lion_params_rint_t rint;
} lion_params_t;

lion_params_t lion_params_default(void);

#ifdef __cplusplus
}
#endif
