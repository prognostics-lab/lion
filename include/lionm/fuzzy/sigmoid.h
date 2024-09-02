#pragma once

typedef struct lion_mf_sigmoid_params {
  double a;
  double c;
} lion_mf_sigmoid_params_t;

double lion_mf_sigmoid(double x, lion_mf_sigmoid_params_t *params);
