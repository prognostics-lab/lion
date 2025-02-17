#pragma once

#include <lion/sim.h>
#include <lion/status.h>
#include <stddef.h>

#ifndef LION_PROGRESSBAR_WIDTH
  #define LION_PROGRESSBAR_WIDTH 100
#endif

lion_status_t lion_sim_show_state_info(lion_sim_t *sim);
lion_status_t lion_sim_show_state_debug(lion_sim_t *sim);
lion_status_t lion_sim_show_state_trace(lion_sim_t *sim);
lion_status_t lion_sim_simulate(lion_sim_t *sim, lion_vector_t *power, lion_vector_t *amb_temp);

#ifndef NDEBUG
lion_status_t lion_sim_init_debug(lion_sim_t *sim);
#endif

#ifdef __cplusplus
}
#endif
