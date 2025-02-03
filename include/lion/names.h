/// @file
/// @brief Names for different lion enums and values.
#pragma once

#include "app.h"

/// @addtogroup functions
/// @{

/// Get the name of the regime.
const char *lion_app_regime_name(lion_app_regime_t regime);

/// Get the name of the stepper.
const char *lion_app_stepper_name(lion_app_stepper_t stepper);

/// Get the name of the minimizer.
const char *lion_app_minimizer_name(lion_app_minimizer_t minimizer);

/// Get the name of the GSL error number.
const char *lion_app_gsl_errno_name(const int num);

/// Get the name of a jacobian calculation method.
const char *lion_jacobian_name(lion_jacobian_method_t jacobian);

/// Get the name of the internal resistance model.
const char *lion_params_rint_get_name(lion_rint_model_t model);

/// @}
