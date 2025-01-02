#include "lion/params.h"

#include <lion/lion.h>

#define LION_PARAMS_DEFAULT_INIT                                                                                                                     \
  {                                                                                                                                                  \
    .soc           = 0.1,                                                                                                                            \
    .temp_in       = 298.0,                                                                                                                          \
    .soh           = 1.0,                                                                                                                            \
    .capacity      = 14400.0,                                                                                                                        \
    .current_guess = 10.0,                                                                                                                           \
  }

#define LION_PARAMS_DEFAULT_EHC                                                                                                                      \
  {                                                                                                                                                  \
    .a     = 4e-5,                                                                                                                                   \
    .b     = 5e-5,                                                                                                                                   \
    .mu    = 0.4,                                                                                                                                    \
    .kappa = 3,                                                                                                                                      \
    .sigma = 0.05,                                                                                                                                   \
    .l     = 7,                                                                                                                                      \
  }

#define LION_PARAMS_DEFAULT_OCV                                                                                                                      \
  {                                                                                                                                                  \
    .alpha = 0.15,                                                                                                                                   \
    .beta  = 17,                                                                                                                                     \
    .gamma = 10.5,                                                                                                                                   \
    .v0    = 4.14,                                                                                                                                   \
    .vl    = 3.977,                                                                                                                                  \
  }

#define LION_PARAMS_DEFAULT_VFT                                                                                                                      \
  {                                                                                                                                                  \
    .k1   = -5.738,                                                                                                                                  \
    .k2   = 209.9,                                                                                                                                   \
    .tref = 298,                                                                                                                                     \
  }

#define LION_PARAMS_DEFAULT_TEMP                                                                                                                     \
  {                                                                                                                                                  \
    .cp   = 100.0,                                                                                                                                   \
    .rin  = 3.0,                                                                                                                                     \
    .rout = 9.0,                                                                                                                                     \
  }

#define LION_PARAMS_DEFAULT_RINT_FIXED {.internal_resistance = 0.12}

#define LION_PARAMS_DEFAULT_RINT_POLARIZATION                                                                                                        \
  {                                                                                                                                                  \
    .c40 =                                                                                                                                           \
        {                                                                                                                                            \
              .a = -19.9748,                                                                                                                             \
              .c = -26.5422,                                                                                                                             \
              },                                                                                                                                           \
    .c20 =                                                                                                                                           \
        {                                                                                                                                            \
              .mean  = -20.0,                                                                                                                            \
              .sigma = 3.0,                                                                                                                              \
              },                                                                                                                                           \
    .c10 =                                                                                                                                           \
        {                                                                                                                                            \
              .mean  = -10.0,                                                                                                                            \
              .sigma = 2.3875,                                                                                                                           \
              },                                                                                                                                           \
    .c4 =                                                                                                                                            \
        {                                                                                                                                            \
              .mean  = -4.0,                                                                                                                             \
              .sigma = 2.1623,                                                                                                                           \
              },                                                                                                                                           \
    .d5 =                                                                                                                                            \
        {                                                                                                                                            \
              .mean  = 5.0,                                                                                                                              \
              .sigma = 2.0,                                                                                                                              \
              },                                                                                                                                           \
    .d10 =                                                                                                                                           \
        {                                                                                                                                            \
              .mean  = 10.0,                                                                                                                             \
              .sigma = 3.1631,                                                                                                                           \
              },                                                                                                                                           \
    .d15 =                                                                                                                                           \
        {                                                                                                                                            \
              .mean  = 15.0,                                                                                                                             \
              .sigma = 2.0,                                                                                                                              \
              },                                                                                                                                           \
    .d30 =                                                                                                                                           \
        {                                                                                                                                            \
              .a = 15.9494,                                                                                                                              \
              .c = 17.3438,                                                                                                                              \
              },                                                                                                                                           \
    .poly =                                                                                                                                          \
        {                                                                                                                                            \
              {0.04172, 0.001688, -0.01526, 0.04006},                                                                                                    \
              {0.04385, 0.01758, -0.04159, 0.05488},                                                                                                     \
              {0.05166, 0.02408, -0.05132, 0.06101},                                                                                                     \
              {0.07004, 0.03910, -0.05345, 0.05015},                                                                                                     \
              {0.1317, -0.05083, -0.2579, 0.3084},                                                                                                       \
              {0.0958, -0.05706, -0.07709, 0.1141},                                                                                                      \
              {0.07868, -0.05782, -0.008633, 0.04612},                                                                                                   \
              {0.07218, -0.07066, 0.04202, 0.0061},                                                                                                      \
              },                                                                                                                                           \
}

#define LION_PARAMS_DEFAULT_RINT                                                                                                                     \
  {                                                                                                                                                  \
    .model        = LION_RINT_MODEL_FIXED,                                                                                                           \
    .params.fixed = LION_PARAMS_DEFAULT_RINT_FIXED,                                                                                                  \
  }

#define LION_PARAMS_DEFAULT                                                                                                                          \
  {                                                                                                                                                  \
    .init = LION_PARAMS_DEFAULT_INIT,                                                                                                                \
    .ehc  = LION_PARAMS_DEFAULT_EHC,                                                                                                                 \
    .ocv  = LION_PARAMS_DEFAULT_OCV,                                                                                                                 \
    .vft  = LION_PARAMS_DEFAULT_VFT,                                                                                                                 \
    .temp = LION_PARAMS_DEFAULT_TEMP,                                                                                                                \
    .rint = LION_PARAMS_DEFAULT_RINT,                                                                                                                \
  }

lion_params_init_t lion_params_default_init(void) {
  lion_params_init_t out = LION_PARAMS_DEFAULT_INIT;
  return out;
}
lion_params_ehc_t lion_params_default_ehc(void) {
  lion_params_ehc_t out = LION_PARAMS_DEFAULT_EHC;
  return out;
}
lion_params_ocv_t lion_params_default_ocv(void) {
  lion_params_ocv_t out = LION_PARAMS_DEFAULT_OCV;
  return out;
}
lion_params_vft_t lion_params_default_vft(void) {
  lion_params_vft_t out = LION_PARAMS_DEFAULT_VFT;
  return out;
}
lion_params_temp_t lion_params_default_temp(void) {
  lion_params_temp_t out = LION_PARAMS_DEFAULT_TEMP;
  return out;
}
lion_params_rint_fixed_t lion_params_default_rint_fixed(void) {
  lion_params_rint_fixed_t out = LION_PARAMS_DEFAULT_RINT_FIXED;
  return out;
}
lion_params_rint_polarization_t lion_params_default_rint_polarization(void) {
  lion_params_rint_polarization_t out = LION_PARAMS_DEFAULT_RINT_POLARIZATION;
  return out;
}
lion_params_rint_t lion_params_default_rint(void) {
  lion_params_rint_t out = LION_PARAMS_DEFAULT_RINT;
  return out;
}
lion_params_t lion_params_default(void) {
  lion_params_t out = LION_PARAMS_DEFAULT;
  return out;
}
