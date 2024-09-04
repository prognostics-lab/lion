#pragma once

#include <lion/app.h>
#include <lion/status.h>

lion_status_t lion_slv_update(lion_app_t *app, double soc,
                              double internal_temperature);
