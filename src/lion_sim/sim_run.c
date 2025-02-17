#include "sim_run.h"

#include <gsl/gsl_odeiv2.h>
#include <lion/lion.h>
#include <lion_utils/macros.h>
#include <lion_utils/vendor/log.h>
#include <math.h>

#define _SHOW_STATE(sim, f)                                                                                                                          \
  f("Cell state");                                                                                                                                   \
  f("|-> P                : %f W", sim->state.power);                                                                                                \
  f("|-> T_amb            : %f K", sim->state.ambient_temperature);                                                                                  \
  f("|-> V_terminal       : %f V", sim->state.voltage);                                                                                              \
  f("|-> I                : %f A", sim->state.current);                                                                                              \
  f("|-> V_oc             : %f V", sim->state.open_circuit_voltage);                                                                                 \
  f("|-> R_int            : %f Ohm", sim->state.internal_resistance);                                                                                \
  f("|-> EHC              : %f V/K", sim->state.ehc);                                                                                                \
  f("|-> q_gen            : %f W", sim->state.generated_heat);                                                                                       \
  f("|-> T_in             : %f K", sim->state.internal_temperature);                                                                                 \
  f("|-> T_s              : %f K", sim->state.surface_temperature);                                                                                  \
  f("|-> kappa            : %f", sim->state.kappa);                                                                                                  \
  f("|-> SoC_0            : %f", sim->state.soc_nominal);                                                                                            \
  f("|-> SoC_use          : %f", sim->state.soc_use);                                                                                                \
  f("|-> Q_0              : %f C", sim->state.capacity_nominal);                                                                                     \
  f("|-> Q_use            : %f C", sim->state.capacity_use);

lion_status_t lion_sim_show_state_info(lion_sim_t *sim) {
  _SHOW_STATE(sim, logi_info);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_sim_show_state_debug(lion_sim_t *sim) {
  _SHOW_STATE(sim, logi_debug);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_sim_show_state_trace(lion_sim_t *sim) {
  _SHOW_STATE(sim, logi_trace);
  return LION_STATUS_SUCCESS;
}

void _template_progressbar(FILE *buf, int width) {
  int x = 0;
  for (x = 0; x < width; x++) {
    fprintf(buf, " ");
  }
  fprintf(buf, "]");
}

void _update_progressbar(FILE *buf, int i, int max, int width, int *c, int *last_c) {
  double progress = i * 100.0 / max;
  *c              = (int)progress;
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

lion_status_t lion_sim_simulate(lion_sim_t *sim, lion_vector_t *power, lion_vector_t *amb_temp) {

  uint64_t max_iters = fminl(power->len, amb_temp->len);
  logi_debug("Considering %d max iterations", max_iters);

  logi_debug("Starting iterations");
  _template_progressbar(stderr, LION_PROGRESSBAR_WIDTH);
  int c      = 0;
  int last_c = 0;
  for (uint64_t i = 1; i < max_iters; i++) {
    _update_progressbar(stderr, i, max_iters, LION_PROGRESSBAR_WIDTH, &c, &last_c);

    if (i == power->len || i == amb_temp->len) {
      logi_error("Ran out of inputs before reaching end of simulation");
      break;
    }
    LION_VCALL_I(lion_sim_step(sim, lion_vector_get_d(sim, power, i), lion_vector_get_d(sim, amb_temp, i)), "Failed at iteration %i", i);
  }
  _finish_progressbar(stderr);

  logi_debug("Finished iterations");
  if (sim->finished_hook != NULL) {
    logi_debug("Found finished hook");
    LION_CALLDF_I(sim->finished_hook(sim), "Failed calling finished hook");
  }
  return LION_STATUS_SUCCESS;
}

#ifndef NDEBUG
lion_status_t lion_sim_init_debug(lion_sim_t *sim) {
  sim->_idebug_malloced_total = 0;
  _idebug_heap_info_t *head   = heapinfo_new(sim);
  if (head == NULL) {
    logi_error("Could not allocate memory for head node of heap info");
    return LION_STATUS_FAILURE;
  }
  sim->_idebug_heap_head = head;

  return LION_STATUS_SUCCESS;
}
#endif
