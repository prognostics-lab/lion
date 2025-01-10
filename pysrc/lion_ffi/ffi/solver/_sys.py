CTYPEDEF = """
typedef enum lion_jacobian_method {
  LION_JACOBIAN_ANALYTICAL = 0,
  LION_JACOBIAN_2POINT     = 1,
} lion_jacobian_method_t;
"""


CDEF = """
const char *lion_jacobian_name(lion_jacobian_method_t jacobian);

int lion_slv_system(double t, const double state[], double out[], void *inputs);
int lion_slv_jac_analytical(double t, const double state[], double *dfdy, double dfdt[], void *inputs);
int lion_slv_jac_2point(double t, const double state[], double *dfdy, double dfdt[], void *inputs);
"""
