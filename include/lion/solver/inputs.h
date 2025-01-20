/// @file
#pragma once

#include <lion/params.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lion_app_state lion_app_state_t;

/// @addtogroup types
/// @{

/// @brief Inputs for the solver.
///
/// Both the current state and the parameters of the system are passed at each iteration of the solver,
/// to be used for the update function as well as the Jacobian calculation.
typedef struct lion_slv_inputs {
  lion_app_state_t *sys_inputs; ///< System state.
  lion_params_t    *sys_params; ///< System parameters.
} lion_slv_inputs_t;

/// @}

#ifdef __cplusplus
}
#endif
