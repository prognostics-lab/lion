#pragma once

#include <stddef.h>

#include <lion/app.h>
#include <lion/status.h>

#ifndef LION_PROGRESSBAR_WIDTH
#define LION_PROGRESSBAR_WIDTH 100
#endif

lion_status_t lion_app_show_state_info(lion_app_t *app);
lion_status_t lion_app_show_state_debug(lion_app_t *app);
lion_status_t lion_app_show_state_trace(lion_app_t *app);
lion_status_t lion_app_simulate(lion_app_t *app, lion_vector_t *power,
                                lion_vector_t *amb_temp);

#ifndef NDEBUG
lion_status_t lion_app_init_debug(lion_app_t *app);
#endif

#ifdef __cplusplus
}
#endif
