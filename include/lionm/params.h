#pragma once

#include "fuzzy/gaussian.h"
#include "fuzzy/sigmoid.h"

typedef struct lion_params {
  // Entropic heat coefficient
  struct {
    double a;
    double b;
    double kappa;
    double mu;
    double sigma;
    double lambda;
  } ehc;

  // Open circuit voltage
  struct {
    double alpha;
    double beta;
    double gamma;
    double v0;
    double vl;
  } ocv;

  // Capacity - temperature dependence
  struct {
    double k1;
    double k2;
    double tref;
  } vft;

  // Temperature model
  struct {
    double cp;
    double rin;
    double rout;
  } t;

  // Internal resistance fuzzy model
  struct {
    lion_mf_sigmoid_params_t c40;
    lion_mf_gaussian_params_t c20;
    lion_mf_gaussian_params_t c10;
    lion_mf_gaussian_params_t c4;

    lion_mf_gaussian_params_t d5;
    lion_mf_gaussian_params_t d10;
    lion_mf_gaussian_params_t d15;
    lion_mf_sigmoid_params_t d30;
  } rint;
} lion_params_t;
