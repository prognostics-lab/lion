#pragma once

#include <lion/status.h>
#include <stddef.h>

typedef struct lion_app lion_app_t;

lion_status_t lion_init(lion_app_t *app);

#ifndef NDEBUG
lion_status_t lion_init_debug(lion_app_t *app);
#endif

#ifdef __cplusplus
}
#endif
