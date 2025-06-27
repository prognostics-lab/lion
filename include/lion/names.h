/// @file
/// @brief Names for different lion enums and values.
#pragma once

#include "sim.h"

/// @addtogroup functions
/// @{

/// Get the name of the regime.
const char *lion_regime_name(lion_regime_t regime);

/// Get the name of the stepper.
const char *lion_stepper_name(lion_stepper_t stepper);

/// Get the name of the minimizer.
const char *lion_minimizer_name(lion_minimizer_t minimizer);

/// Get the name of the GSL error number.
const char *lion_gsl_errno_name(const int num);

/// Get the name of a jacobian calculation method.
const char *lion_jacobian_name(lion_jacobian_method_t jacobian);

/// Get the name of the internal resistance model.
const char *lion_params_rint_get_name(lion_rint_model_t model);

/// Get the name of the degradation model.
const char *lion_params_soh_get_name(lion_soh_model_t model);

/// @}
