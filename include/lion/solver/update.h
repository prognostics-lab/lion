/// @file
#pragma once

#include <lion/app.h>
#include <lion/status.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup functions
/// @{

/// @brief Update the state of the system.
///
/// Updates all the variables in the state of the system. This function assumes that
/// `app->state.internal_temperature` and `app->state.soc_nominal` have both been set
/// to the current state.
/// @param[inout] app Pointer to the `lion_app_t` instance that is to be updated.
lion_status_t lion_slv_update(lion_app_t *app);

/// @}

#ifdef __cplusplus
}
#endif
