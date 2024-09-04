#pragma once

#include <stddef.h>

#include <lion/app.h>
#include <lion/status.h>

lion_status_t lion_app_init(lion_app_t *app);
lion_status_t lion_app_simulate(lion_app_t *app);

#ifndef NDEBUG
lion_status_t lion_app_init_debug(lion_app_t *app);
#endif

#ifdef __cplusplus
}
#endif
