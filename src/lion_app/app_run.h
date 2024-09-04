#pragma once

#include <stddef.h>

#include <lion/app.h>
#include <lion/status.h>

lion_status_t lion_app_init(lion_app_t *app, double initial_power,
                            double initial_amb_temp);
lion_status_t lion_app_simulate(lion_app_t *app, lion_vector_t *power,
                                lion_vector_t *amb_temp);

#ifndef NDEBUG
lion_status_t lion_app_init_debug(lion_app_t *app);
#endif

#ifdef __cplusplus
}
#endif
