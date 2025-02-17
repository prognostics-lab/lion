#pragma once

#include <lion/sim.h>
#include <lionpp/status.hpp>
#include <vector>

namespace lion {

enum SimRegime {
  ONLYSF  = LION_ONLYSF,
  ONLYAIR = LION_ONLYAIR,
  BOTH    = LION_BOTH,
};

class SimStepper {
public:
  enum Value {
    RK2     = LION_STEPPER_RK2,
    RK4     = LION_STEPPER_RK4,
    RKF45   = LION_STEPPER_RKF45,
    RKCK    = LION_STEPPER_RKCK,
    RK8PD   = LION_STEPPER_RK8PD,
    RK1IMP  = LION_STEPPER_RK1IMP,
    RK2IMP  = LION_STEPPER_RK2IMP,
    RK4IMP  = LION_STEPPER_RK4IMP,
    BSIMP   = LION_STEPPER_BSIMP,
    MSADAMS = LION_STEPPER_MSADAMS,
    MSBDF   = LION_STEPPER_MSBDF,
  };

  SimStepper() = default;
  SimStepper(Value val) : value(val) {}

  constexpr operator Value() const { return value; }
  constexpr operator lion_stepper_t() const { return static_cast<lion_stepper_t>(value); }

  explicit       operator bool() const = delete;
  constexpr bool operator==(SimStepper a) const { return value == a.value; }
  constexpr bool operator!=(SimStepper a) const { return value != a.value; }

private:
  Value value;
};

enum SimMinimizer {
  GOLDENSECTION = LION_MINIMIZER_GOLDENSECTION,
  BRENT         = LION_MINIMIZER_BRENT,
  QUADGOLDEN    = LION_MINIMIZER_QUADGOLDEN,
};

class SimConfig {
public:
  SimConfig();
  ~SimConfig();

  operator lion_sim_config_t();

  lion_sim_config_t *get_handle();

private:
  lion_sim_config_t handle;
};

class SimParams {
public:
  SimParams();
  ~SimParams();

  lion_params_init_t &init();
  lion_params_ehc_t  &ehc();
  lion_params_ocv_t  &ocv();
  lion_params_vft_t  &vft();
  lion_params_temp_t &temp();
  lion_params_rint_t &rint();

  operator lion_params_t();

  lion_params_t *get_handle();

private:
  lion_params_t handle;
};

class Sim {
public:
  Sim(SimConfig *conf, SimParams *params);
  Sim(Sim const &)            = delete;
  Sim &operator=(Sim const &) = delete;
  ~Sim();

  operator lion_sim_t *();

  Status   step(double power, double amb_temp);
  Status   run(std::vector<double> const &power, std::vector<double> const &amb_temp);
  bool     should_close() const;
  uint64_t max_iters() const;

private:
  lion_sim_t *handle;
};

} // namespace lion
