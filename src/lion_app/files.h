#pragma once

#include <lion/app.h>
#include <lion/status.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int lion_count_lines(FILE *file);
lion_status_t lion_readline(lion_app_t *app, FILE *file, char *buffer,
                            char **out);

#ifdef __cplusplus
}
#endif
