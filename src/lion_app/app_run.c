#include <gsl/gsl_odeiv2.h>
#include <math.h>

#include <lion/lion.h>
#include <lion_utils/macros.h>
#include <lion_utils/vendor/log.h>

#include "app_run.h"

#define _SHOW_STATE(app, f)                                                    \
  f("Cell state");                                                             \
  f("|-> P                : %f W", app->state.power);                          \
  f("|-> T_amb            : %f K", app->state.ambient_temperature);            \
  f("|-> V_terminal       : %f V", app->state.voltage);                        \
  f("|-> I                : %f A", app->state.current);                        \
  f("|-> V_oc             : %f V", app->state.open_circuit_voltage);           \
  f("|-> R_int            : %f Ohm", app->state.internal_resistance);          \
  f("|-> EHC              : %f V/K", app->state.ehc);                          \
  f("|-> q_gen            : %f W", app->state.generated_heat);                 \
  f("|-> T_in             : %f K", app->state.internal_temperature);           \
  f("|-> T_s              : %f K", app->state.surface_temperature);            \
  f("|-> kappa            : %f", app->state.kappa);                            \
  f("|-> SoC_0            : %f", app->state.soc_nominal);                      \
  f("|-> SoC_use          : %f", app->state.soc_use);                          \
  f("|-> Q_0              : %f C", app->state.capacity_nominal);               \
  f("|-> Q_use            : %f C", app->state.capacity_use);

lion_status_t lion_app_show_state_info(lion_app_t *app) {
  _SHOW_STATE(app, logi_info);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_show_state_debug(lion_app_t *app) {
  _SHOW_STATE(app, logi_debug);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_show_state_trace(lion_app_t *app) {
  _SHOW_STATE(app, logi_trace);
  return LION_STATUS_SUCCESS;
}

void _template_progressbar(FILE *buf, int width) {
  int x = 0;
  for (x = 0; x < width; x++) {
    fprintf(buf, " ");
  }
  fprintf(buf, "]");
}

void _update_progressbar(FILE *buf, int i, int max, int width, int *c,
                         int *last_c) {
  double progress = i * 100.0 / max;
  *c = (int)progress;
  fprintf(buf, "\n\033[F");
  fprintf(buf, "%3d%%", *c);
  fprintf(buf, "\033[1C");
  fprintf(buf, "\033[%dC=", *last_c);
  int x = 0;
  for (x = *last_c; x < *c; x++) {
    fprintf(buf, "=");
  }
  if (x < width) {
    fprintf(buf, ">");
  }
  *last_c = *c;
}

void _finish_progressbar(FILE *buf) { fprintf(stderr, "\033[EDone\n"); }

lion_status_t lion_app_simulate(lion_app_t *app, lion_vector_t *power,
                                lion_vector_t *amb_temp) {

  uint64_t max_iters = fminl(power->len, amb_temp->len);
  logi_debug("Considering %d max iterations", max_iters);

  logi_debug("Starting iterations");
  _template_progressbar(stderr, LION_PROGRESSBAR_WIDTH);
  int c = 0;
  int last_c = 0;
  for (uint64_t i = 1; i < max_iters; i++) {
    _update_progressbar(stderr, i, max_iters, LION_PROGRESSBAR_WIDTH, &c,
                        &last_c);

    if (i == power->len || i == amb_temp->len) {
      logi_error("Ran out of inputs before reaching end of simulation");
      break;
    }
    LION_VCALL_I(lion_app_step(app, lion_vector_get_d(app, power, i),
                               lion_vector_get_d(app, amb_temp, i)),
                 "Failed at iteration %i", i);
  }
  _finish_progressbar(stderr);

  logi_debug("Finished iterations");
  if (app->finished_hook != NULL) {
    logi_debug("Found finished hook");
    LION_CALLDF_I(app->finished_hook(app), "Failed calling finished hook");
  }
  return LION_STATUS_SUCCESS;
}

#ifndef NDEBUG
lion_status_t lion_app_init_debug(lion_app_t *app) {
  app->_idebug_malloced_total = 0;
  _idebug_heap_info_t *head = heapinfo_new(app);
  if (head == NULL) {
    logi_error("Could not allocate memory for head node of heap info");
    return LION_STATUS_FAILURE;
  }
  app->_idebug_heap_head = head;

  return LION_STATUS_SUCCESS;
}
#endif
