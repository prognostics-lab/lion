#include "files.h"

#include "mem.h"

#include <lion/sim.h>
#include <lion_utils/vendor/log.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 65536

int lion_count_lines(FILE *file) {
  char buf[BUF_SIZE];
  int  counter = 0;
  for (;;) {
    size_t res = fread(buf, 1, BUF_SIZE, file);
    if (ferror(file))
      return -1;

    int i;
    for (i = 0; i < res; i++)
      if (buf[i] == '\n')
        counter++;

    if (feof(file))
      break;
  }

  return counter;
}

lion_status_t lion_readline(lion_sim_t *sim, FILE *file, char *buffer, char **out) {
  int    alloced    = 0;
  size_t max_length = 128;
  if (buffer == NULL) {
    alloced = 1;
    logi_info("No preallocated buffer was passed, allocating one");
    buffer = lion_malloc(sim, sizeof(char) * max_length);
  }

  if (buffer == NULL) {
    logi_error("Could not allocate memory for line buffer");
    return LION_STATUS_FAILURE;
  }

  char   ch    = (char)getc(file);
  size_t count = 0;

  while ((ch != '\n') && (ch != EOF)) {
    if (count == max_length) {
      if (alloced) {
        logi_debug("Encountered max length, reallocating");
        max_length += 128;
        buffer      = lion_realloc(sim, buffer, max_length);
        if (buffer == NULL) {
          logi_error("Error reallocating space for line buffer");
          return LION_STATUS_FAILURE;
        }
      } else {
        logi_error("Encountered max length, failing");
        return LION_STATUS_FAILURE;
      }
    }
    buffer[count] = ch;
    count++;

    ch = (char)getc(file);
  }

  buffer[count] = '\0';
  // char line[count + 1];
  char *line    = lion_malloc(sim, (count + 1) * sizeof(char));
  strncpy(line, buffer, count + 1);
  *out = line;
  lion_free(sim, line);
  if (alloced)
    lion_free(sim, buffer);
  return LION_STATUS_SUCCESS;
}
