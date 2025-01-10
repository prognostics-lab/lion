#include "jacobian.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <lion/app.h>
#include <lion/params.h>
#include <lion/solver/inputs.h>
#include <lion/solver/sys.h>
#include <lion_math/capacity.h>
#include <lion_math/current.h>
#include <lion_math/dynamics/soc.h>
#include <lion_math/dynamics/temperature.h>
#include <lion_math/internal_resistance.h>
#include <lion_math/open_circuit.h>
#include <lion_utils/vendor/log.h>

const char *lion_jacobian_name(lion_jacobian_method_t jacobian) {
  switch (jacobian) {
  case LION_JACOBIAN_ANALYTICAL:
    return "LION_JACOBIAN_ANALYTICAL";
  case LION_JACOBIAN_2POINT:
    return "LION_JACOBIAN_2POINT";
  default:
    return "Unknown";
  }
  return "Unexpected return";
}

int lion_slv_system(double t, const double state[], double out[], void *inputs) {
  /*
     state[0] -> state of charge
     state[1] -> internal temperature

     inputs[0] -> *lion_app_state_t
     inputs[1] -> *lion_params_t
   */
  lion_slv_inputs_t *p          = inputs;
  lion_app_state_t  *sys_inputs = p->sys_inputs;
  lion_params_t     *sys_params = p->sys_params;

  (void)t;
  out[0] = lion_soc_d(sys_inputs->current, sys_inputs->capacity_use, sys_params);
  out[1] = lion_internal_temperature_d(state[1], sys_inputs->generated_heat, sys_inputs->ambient_temperature, sys_params);
  return GSL_SUCCESS;
}

// TODO: For some reason this approach is failing ????? :))))

int lion_slv_jac_analytical(double t, const double state[], double *dfdy, double dfdt[], void *inputs) {
  lion_slv_inputs_t *p          = inputs;
  lion_app_state_t  *sys_state  = p->sys_inputs;
  lion_params_t     *sys_params = p->sys_params;

  (void)t;
  gsl_matrix_view dfdy_mat = gsl_matrix_view_array(dfdy, 2, 2);
  gsl_matrix     *m        = &dfdy_mat.matrix;

  double jac00 = jac_0_0_analytical(sys_state, sys_params);
  double jac01 = jac_0_1_analytical(sys_state, sys_params);
  double jac10 = jac_1_0_analytical(sys_state, sys_params);
  double jac11 = jac_1_1_analytical(sys_state, sys_params);
  double jac0t = jac_0_t_analytical(sys_state, sys_params);
  double jac1t = jac_1_t_analytical(sys_state, sys_params);

  gsl_matrix_set(m, 0, 0, jac00);
  gsl_matrix_set(m, 0, 1, jac01);
  gsl_matrix_set(m, 1, 0, jac10);
  gsl_matrix_set(m, 1, 1, jac11);
  dfdt[0] = jac0t;
  dfdt[1] = jac1t;
  logi_trace("jac_analytical={{%f, %f}, {%f, %f}}", jac00, jac01, jac10, jac11);
  return GSL_SUCCESS;
}

int lion_slv_jac_2point(double t, const double state[], double *dfdy, double dfdt[], void *inputs) {
  lion_slv_inputs_t *p          = inputs;
  lion_app_state_t  *sys_state  = p->sys_inputs;
  lion_params_t     *sys_params = p->sys_params;

  (void)t;
  gsl_matrix_view dfdy_mat = gsl_matrix_view_array(dfdy, 2, 2);
  gsl_matrix     *m        = &dfdy_mat.matrix;

  double jac00 = jac_0_0_2point(sys_state, sys_params);
  double jac01 = jac_0_1_2point(sys_state, sys_params);
  double jac10 = jac_1_0_2point(sys_state, sys_params);
  double jac11 = jac_1_1_2point(sys_state, sys_params);
  double jac0t = jac_0_t_2point(sys_state, sys_params);
  double jac1t = jac_1_t_2point(sys_state, sys_params);

  gsl_matrix_set(m, 0, 0, jac00);
  gsl_matrix_set(m, 0, 1, jac01);
  gsl_matrix_set(m, 1, 0, jac10);
  gsl_matrix_set(m, 1, 1, jac11);
  dfdt[0] = jac0t;
  dfdt[1] = jac1t;
  logi_trace("jac_2point={{%f, %f}, {%f, %f}}", jac00, jac01, jac10, jac11);
  return GSL_SUCCESS;
}
