#include <gsl/gsl_errno.h>
#include <lion/names.h>

const char *lion_regime_name(lion_regime_t regime) {
  switch (regime) {
  case LION_ONLYSF:
    return "LION_ONLYSF";
  case LION_ONLYAIR:
    return "LION_ONLYAIR";
  case LION_BOTH:
    return "LION_BOTH";
  default:
    return "N/A";
  }
  return "Unexpected return";
}

const char *lion_stepper_name(lion_stepper_t stepper) {
  switch (stepper) {
  case LION_STEPPER_RK2:
    return "LION_STEPPER_RK2";
  case LION_STEPPER_RK4:
    return "LION_STEPPER_RK4";
  case LION_STEPPER_RKF45:
    return "LION_STEPPER_RKF45";
  case LION_STEPPER_RKCK:
    return "LION_STEPPER_RKCK";
  case LION_STEPPER_RK8PD:
    return "LION_STEPPER_RK8PD";
  case LION_STEPPER_RK1IMP:
    return "LION_STEPPER_RK1IMP";
  case LION_STEPPER_RK2IMP:
    return "LION_STEPPER_RK2IMP";
  case LION_STEPPER_RK4IMP:
    return "LION_STEPPER_RK4IMP";
  case LION_STEPPER_BSIMP:
    return "LION_STEPPER_BSIMP";
  case LION_STEPPER_MSADAMS:
    return "LION_STEPPER_MSADAMS";
  case LION_STEPPER_MSBDF:
    return "LION_STEPPER_MSBDF";
  default:
    return "N/A";
  }
  return "Unexpected return";
}

const char *lion_minimizer_name(lion_minimizer_t minimizer) {
  switch (minimizer) {
  case LION_MINIMIZER_GOLDENSECTION:
    return "LION_MINIMIZER_GOLDENSECTION";
  case LION_MINIMIZER_BRENT:
    return "LION_MINIMIZER_BRENT";
  case LION_MINIMIZER_QUADGOLDEN:
    return "LION_MINIMIZER_QUADGOLDEN";
  default:
    return "N/A";
  }
  return "Unexpected return";
}

const char *lion_gsl_errno_name(const int num) { return gsl_strerror(num); }

const char *lion_jacobian_name(lion_jacobian_method_t jacobian) {
  switch (jacobian) {
  case LION_JACOBIAN_ANALYTICAL:
    return "LION_JACOBIAN_ANALYTICAL";
  case LION_JACOBIAN_2POINT:
    return "LION_JACOBIAN_2POINT";
  default:
    return "N/A";
  }
  return "Unexpected return";
}
