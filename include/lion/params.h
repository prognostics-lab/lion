/// @file
#pragma once

#include <lionu/fuzzy.h>
#include <stdint.h>

#define LION_FUZZY_SETS_COUNT  8
#define LION_FUZZY_SETS_DEGREE 4

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup types
/// @{

/// @brief Initialization parameters.
typedef struct lion_params_init {
  double soc;           ///< Initial state of charge.
  double temp_in;       ///< Initial internal temperature.
  double soh;           ///< Initial state of health.
  double capacity;      ///< Initial nominal capacity.
  double current_guess; ///< Initial guess for the current, default is 0.
} lion_params_init_t;

/// @brief Entropic heat coefficient model parameters.
typedef struct lion_params_ehc {
  double a;
  double b;
  double mu;
  double kappa;
  double sigma;
  double l;
} lion_params_ehc_t;

/// @brief Vogel-Fulcher-Tammann model for temperature-dependence of capacity.
typedef struct lion_params_vft {
  double k1;
  double k2;
  double tref; ///< Reference temperature.
} lion_params_vft_t;

/// @brief Temperature model parameters.
typedef struct lion_params_temp {
  double cp;   ///< Heat capacity of the cell.
  double rin;  ///< Interior-surface thermal resistivity of the cell.
  double rout; ///< Surface-ambient thermal resistivity of the cell.
} lion_params_temp_t;

/// @brief Open-circuit voltage model parameters.
typedef struct lion_params_ocv {
  double alpha;
  double beta;
  double gamma;
  double v0;
  double vl;
} lion_params_ocv_t;

/// @brief Internal resistance models.
typedef enum lion_rint_model {
  LION_RINT_MODEL_FIXED,        ///< Fixed internal resistance model.
  LION_RINT_MODEL_POLARIZATION, ///< Current and state of charge dependent internal resistance model.
} lion_rint_model_t;

/// @brief Fixed internal resistance model.
typedef struct lion_params_rint_fixed {
  double internal_resistance; ///< Internal resistance.
} lion_params_rint_fixed_t;

/// @brief Current and state of charge dependent internal resistance model.
typedef struct lion_params_rint_polarization {
  lion_mf_sigmoid_params_t  c40;                              ///< 40A charge fuzzy parameters.
  lion_mf_gaussian_params_t c20;                              ///< 20A charge fuzzy parameters.
  lion_mf_gaussian_params_t c10;                              ///< 10A charge fuzzy parameters.
  lion_mf_gaussian_params_t c4;                               ///< 4A charge fuzzy parameters.

  lion_mf_gaussian_params_t d5;                               ///< 5A discharge fuzzy parameters.
  lion_mf_gaussian_params_t d10;                              ///< 10A discharge fuzzy parameters.
  lion_mf_gaussian_params_t d15;                              ///< 15A discharge fuzzy parameters.
  lion_mf_sigmoid_params_t  d30;                              ///< 30A discharge fuzzy parameters.

  double poly[LION_FUZZY_SETS_COUNT][LION_FUZZY_SETS_DEGREE]; ///< Polynomial coefficients.
} lion_params_rint_polarization_t;

/// @brief Container for the internal resistance model.
typedef struct lion_params_rint {
  lion_rint_model_t model; ///< Model to use.
  union {
    lion_params_rint_fixed_t        fixed;
    lion_params_rint_polarization_t polarization;
  } params; ///< Model parameters.
} lion_params_rint_t;

/// @brief Parameters for the degradation model.
typedef struct lion_params_soh {
  uint64_t total_cycles; ///< Nominal number of cycles the cell has.
  double   final_soh;    ///< Nominal state of health after `total_cycles` (end of life)
} lion_params_soh_t;

/// @brief Parameters of the system.
typedef struct lion_params {
  lion_params_init_t init; ///< Initial conditions.
  lion_params_ehc_t  ehc;  ///< Entropic heat coefficient.
  lion_params_ocv_t  ocv;  ///< Open-circuit voltage.
  lion_params_vft_t  vft;  ///< Temperature-dependence of capacity.
  lion_params_temp_t temp; ///< Temperature model.
  lion_params_rint_t rint; ///< Internal resistance model.
  lion_params_soh_t  soh;  ///< Degradation model.
} lion_params_t;

/// @}

/// @addtogroup functions
/// @{

/// Get default initial conditions parameters.
lion_params_init_t lion_params_default_init(void);

/// Get default entropic heat coefficient parameters.
lion_params_ehc_t lion_params_default_ehc(void);

/// Get default open-circuit voltage parameters.
lion_params_ocv_t lion_params_default_ocv(void);

/// Get default capacity parameters.
lion_params_vft_t lion_params_default_vft(void);

/// Get default temperature parameters.
lion_params_temp_t lion_params_default_temp(void);

/// Get default fixed internal resistance parameters.
lion_params_rint_fixed_t lion_params_default_rint_fixed(void);

/// Get default polarization internal resistance parameters.
lion_params_rint_polarization_t lion_params_default_rint_polarization(void);

/// Get default internal resistance parameters.
lion_params_rint_t lion_params_default_rint(void);

/// Get default degradation model parameters.
lion_params_soh_t lion_params_default_soh(void);

/// Get default system parameters.
lion_params_t lion_params_default(void);

const char *lion_params_rint_get_name(lion_rint_model_t model);

/// @}

#ifdef __cplusplus
}
#endif
