CTYPEDEF = """
#define LION_FUZZY_SETS_COUNT 8
#define LION_FUZZY_SETS_DEGREE 4
#define LION_SOH_TABLE_COUNT   11

typedef struct lion_params_init {
  double soc;
  double temp_in;
  double soh;
  double capacity;
  double current_guess;
} lion_params_init_t;

typedef struct lion_params_ehc {
  double a;
  double b;
  double mu;
  double kappa;
  double sigma;
  double l;
} lion_params_ehc_t;

typedef struct lion_params_vft {
  double k1;
  double k2;
  double tref;
} lion_params_vft_t;

typedef struct lion_params_temp {
  double cp;
  double rin;
  double rout;
} lion_params_temp_t;

typedef struct lion_params_ocv {
  double alpha;
  double beta;
  double gamma;
  double v0;
  double vl;
} lion_params_ocv_t;

typedef enum lion_rint_model {
  LION_RINT_MODEL_FIXED,
  LION_RINT_MODEL_POLARIZATION,
} lion_rint_model_t;

typedef struct lion_params_rint_fixed {
  double internal_resistance;
} lion_params_rint_fixed_t;

typedef struct lion_params_rint_polarization {
  lion_mf_sigmoid_params_t  c40;
  lion_mf_gaussian_params_t c20;
  lion_mf_gaussian_params_t c10;
  lion_mf_gaussian_params_t c4;

  lion_mf_gaussian_params_t d5;
  lion_mf_gaussian_params_t d10;
  lion_mf_gaussian_params_t d15;
  lion_mf_sigmoid_params_t  d30;

  double poly[LION_FUZZY_SETS_COUNT][LION_FUZZY_SETS_DEGREE];
} lion_params_rint_polarization_t;

typedef struct lion_params_rint {
  lion_rint_model_t model;
  union {
    lion_params_rint_fixed_t        fixed;
    lion_params_rint_polarization_t polarization;
  } params;
} lion_params_rint_t;

typedef struct lion_params_degradation_element {
  double max;
  double min;
  double coeff;
} lion_params_degradation_element_t;

typedef struct lion_params_soh_vendor {
  uint64_t total_cycles;
  double   final_soh;
} lion_params_soh_vendor_t;

typedef struct lion_params_soh_masserano {
  uint64_t                          total_cycles;
  double                            final_soh;
  lion_params_degradation_element_t table[LION_SOH_TABLE_COUNT];
} lion_params_soh_masserano_t;

typedef struct lion_params_soh {
  lion_soh_model_t model;
  union {
    lion_params_soh_vendor_t    vendor;
    lion_params_soh_masserano_t masserano;
  } params;
} lion_params_soh_t;

typedef struct lion_params {
  lion_params_init_t init;
  lion_params_ehc_t ehc;
  lion_params_ocv_t ocv;
  lion_params_vft_t vft;
  lion_params_temp_t temp;
  lion_params_rint_t rint;
} lion_params_t;
"""


CDEF = """
lion_params_init_t              lion_params_default_init(void);
lion_params_ehc_t               lion_params_default_ehc(void);
lion_params_ocv_t               lion_params_default_ocv(void);
lion_params_vft_t               lion_params_default_vft(void);
lion_params_temp_t              lion_params_default_temp(void);
lion_params_rint_fixed_t        lion_params_default_rint_fixed(void);
lion_params_rint_polarization_t lion_params_default_rint_polarization(void);
lion_params_rint_t              lion_params_default_rint(void);
lion_params_soh_t               lion_params_default_soh(void);
lion_params_t                   lion_params_default(void);
"""
