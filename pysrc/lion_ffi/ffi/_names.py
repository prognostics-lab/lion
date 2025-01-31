CTYPEDEF = """
"""


CDEF = """
const char *lion_app_regime_name(lion_app_regime_t regime);
const char *lion_app_stepper_name(lion_app_stepper_t stepper);
const char *lion_app_minimizer_name(lion_app_minimizer_t minimizer);
const char *lion_app_gsl_errno_name(const int num);
const char *lion_jacobian_name(lion_jacobian_method_t jacobian);
const char *lion_params_rint_get_name(lion_rint_model_t model);
"""
