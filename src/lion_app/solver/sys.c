#include <gsl/gsl_errno.h>

#include <lion/solver/sys.h>
#include <lion_math/dynamics/soc.h>
#include <lion_math/dynamics/temperature.h>

int lion_slv_system(double t, double state[], double out[], void *inputs) {
  /*
     state[0] -> state of charge
     state[1] -> internal temperature

     inputs[0] -> *electrical values
        [voltage, current, heat, capacity, amb_temp]
     inputs[1] -> *system parameters
   */

  void **p = (void **)inputs;
  void *sys_inputs_p = p[0];
  void *sys_params_p = p[1];
  double *sys_inputs = (double *)sys_inputs_p;
  lion_params_t *sys_params = (lion_params_t *)sys_params_p;

  (void)t;
  out[0] = lion_soc_d(sys_inputs[1], sys_inputs[3], sys_params);
  out[1] = lion_internal_temperature_d(state[1], sys_inputs[2], sys_inputs[4],
                                       sys_params);
  return GSL_SUCCESS;
}

// TODO: Implement calculating the Jacobian

