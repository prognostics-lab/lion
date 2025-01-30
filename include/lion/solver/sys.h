/// @file
#pragma once

#include <lion/params.h>

// TODO: Add logic to change dimension depending on simtype
#define LION_SLV_DIMENSION 2

#ifdef __cplusplus
extern "C" {
#endif

// TODO: Give a second thought to how these are organized

/// @addtogroup types
/// @{

/// @brief Jacobian calculation method.
///
/// The following methods for jacobian calculation are currently supported:
/// - LION_JACOBIAN_ANALYTICAL : uses the analytical equations to calculate the jacobian.
/// - LION_JACOBIAN_2POINT     : uses central differences to numerically calculate the jacobian.
typedef enum lion_jacobian_method {
  LION_JACOBIAN_ANALYTICAL, ///< Analytical method.
  LION_JACOBIAN_2POINT,     ///< Central differences method.
} lion_jacobian_method_t;

/// @}

/// @addtogroup functions
/// @{

/// @brief Get the name of a jacobian calculation method.
const char *lion_jacobian_name(lion_jacobian_method_t jacobian);

/// @brief Derivative of the system.
///
/// Assuming the system is modelled through some equation dx/dt = f(x; t, p),
/// where p are some parameters, then this function would correspond to f.
/// @param[in]  t       Current time.
/// @param[in]  state   Array with the current state.
/// @param[out] out     Array for the next state, same size as `state`.
/// @param[in]  inputs  Pointer to a `lion_slv_inputs_t` struct with the system state and parameters.
int lion_slv_system_continuous(double t, const double state[], double out[], void *inputs);

/// @brief Analytical jacobian.
///
/// Calculates the analytical jacobian.
/// @param[in]  t       Current time.
/// @param[in]  state   Array with the current state.
/// @param[out] dfdy    Matrix with the components of the jacobian.
/// @param[out] dfdt    Array with the time derivates of each update function.
/// @param[in]  inputs  Pointer to a `lion_slv_inputs_t` struct with the system state and parameters.
int lion_slv_jac_analytical(double t, const double state[], double *dfdy, double dfdt[], void *inputs);

/// @brief Two-point jacobian.
///
/// Calculates the numerical jacobian using central differences.
/// @param[in]  t       Current time.
/// @param[in]  state   Array with the current state.
/// @param[out] dfdy    Matrix with the components of the jacobian.
/// @param[out] dfdt    Array with the time derivates of each update function.
/// @param[in]  inputs  Pointer to a `lion_slv_inputs_t` struct with the system state and parameters.
int lion_slv_jac_2point(double t, const double state[], double *dfdy, double dfdt[], void *inputs);

/// @}

#ifdef __cplusplus
}
#endif
