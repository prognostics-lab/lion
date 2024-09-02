#pragma once

typedef struct lionm_params {
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
} lionm_params_t;
