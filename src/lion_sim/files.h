#pragma once

#include <lion/sim.h>
#include <lion/status.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int           lion_count_lines(FILE *file);
lion_status_t lion_readline(lion_sim_t *sim, FILE *file, char *buffer, char **out);

#ifdef __cplusplus
}
#endif
