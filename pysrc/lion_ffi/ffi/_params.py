CTYPEDEF = """
#define LION_FUZZY_SETS_COUNT 8
#define LION_FUZZY_SETS_DEGREE 4

typedef struct lion_params {
  struct {
    double initial_soc;
    double initial_internal_temperature;
    double initial_soh;
    double initial_capacity;
    double initial_current_guess;
  } init;

  struct {
    double a;
    double b;
    double kappa;
    double mu;
    double sigma;
    double lambda;
  } ehc;

  struct {
    double alpha;
    double beta;
    double gamma;
    double v0;
    double vl;
  } ocv;

  struct {
    double k1;
    double k2;
    double tref;
  } vft;

  struct {
    double cp;
    double rin;
    double rout;
  } t;

  struct {
    lion_mf_sigmoid_params_t c40;
    lion_mf_gaussian_params_t c20;
    lion_mf_gaussian_params_t c10;
    lion_mf_gaussian_params_t c4;

    lion_mf_gaussian_params_t d5;
    lion_mf_gaussian_params_t d10;
    lion_mf_gaussian_params_t d15;
    lion_mf_sigmoid_params_t d30;

    double poly[LION_FUZZY_SETS_COUNT][LION_FUZZY_SETS_DEGREE];
  } rint;
} lion_params_t;
"""


CDEF = """
lion_params_t lion_params_default(void);
"""
