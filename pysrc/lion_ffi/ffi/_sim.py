CTYPEDEF = """
typedef struct lion_sim lion_sim_t;

typedef enum lion_regime {
  LION_ONLYSF,
  LION_ONLYAIR,
  LION_BOTH,
} lion_regime_t;

typedef enum lion_stepper {
  LION_STEPPER_RK2,
  LION_STEPPER_RK4,
  LION_STEPPER_RKF45,
  LION_STEPPER_RKCK,
  LION_STEPPER_RK8PD,
  LION_STEPPER_RK1IMP,
  LION_STEPPER_RK2IMP,
  LION_STEPPER_RK4IMP,
  LION_STEPPER_BSIMP,
  LION_STEPPER_MSADAMS,
  LION_STEPPER_MSBDF,
} lion_stepper_t;

typedef enum lion_minimizer {
  LION_MINIMIZER_GOLDENSECTION,
  LION_MINIMIZER_BRENT,
  LION_MINIMIZER_QUADGOLDEN,
} lion_minimizer_t;

typedef enum lion_jacobian_method {
  LION_JACOBIAN_ANALYTICAL,
  LION_JACOBIAN_2POINT,
} lion_jacobian_method_t;

extern "Python" lion_status_t init_pythoncb(lion_sim_t *);
extern "Python" lion_status_t update_pythoncb(lion_sim_t *);
extern "Python" lion_status_t finished_pythoncb(lion_sim_t *);

typedef struct lion_sim_config {
  const char *sim_name;

  lion_regime_t      sim_regime;
  lion_stepper_t     sim_stepper;
  lion_minimizer_t   sim_minimizer;
  lion_jacobian_method_t sim_jacobian;
  double                 sim_time_seconds;
  double                 sim_step_seconds;
  double                 sim_epsabs;
  double                 sim_epsrel;
  uint64_t               sim_min_maxiter;

  const char *log_dir;
  int         log_stdlvl;
  int         log_filelvl;
} lion_sim_config_t;

typedef struct lion_sim_state {
  double   time;
  uint64_t step;

  double power;
  double ambient_temperature;

  double voltage;
  double current;
  double ref_open_circuit_voltage;
  double open_circuit_voltage;
  double internal_resistance;

  uint64_t cycle;
  double   soh;

  double ehc;
  double generated_heat;
  double internal_temperature;
  double surface_temperature;

  double kappa;
  double soc_nominal;
  double capacity_nominal;
  double soc_use;
  double capacity_use;

  ...;
} lion_sim_state_t;

typedef struct lion_slv_inputs {
  lion_sim_state_t *sys_inputs;
  lion_params_t    *sys_params;
} lion_slv_inputs_t;

typedef struct lion_sim {
  lion_sim_config_t *conf;
  lion_params_t     *params;
  lion_sim_state_t   state;
  lion_slv_inputs_t  inputs;
  lion_status_t (*init_hook)(lion_sim_t *sim);
  lion_status_t (*update_hook)(lion_sim_t *sim);
  lion_status_t (*finished_hook)(lion_sim_t *sim);
  ...;
} lion_sim_t;
"""


CDEF = """
lion_status_t lion_sim_config_new(lion_sim_config_t *out);
lion_sim_config_t lion_sim_config_default(void);

lion_status_t lion_sim_new(lion_sim_config_t *conf, lion_params_t *params,
                           lion_sim_t *out);
lion_status_t lion_sim_init(lion_sim_t *sim);
lion_status_t lion_sim_reset(lion_sim_t *sim);
lion_status_t lion_sim_step(lion_sim_t *sim, double power,
                            double ambient_temperature);
lion_status_t lion_sim_run(lion_sim_t *sim, lion_vector_t *power,
                           lion_vector_t *ambient_temperature);

int lion_sim_should_close(lion_sim_t *sim);
uint64_t lion_sim_max_iters(lion_sim_t *sim);

lion_status_t lion_sim_cleanup(lion_sim_t *sim);
"""
