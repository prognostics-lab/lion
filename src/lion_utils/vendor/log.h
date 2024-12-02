/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_INTERNAL_H
#define LOG_INTERNAL_H

#include <lionu/log.h>

#define logi_trace(...) log_log_internal(LOG_TRACE, __FILENAME__, __LINE__, __VA_ARGS__)
#define logi_debug(...) log_log_internal(LOG_DEBUG, __FILENAME__, __LINE__, __VA_ARGS__)
#define logi_info(...)  log_log_internal(LOG_INFO, __FILENAME__, __LINE__, __VA_ARGS__)
#define logi_warn(...)  log_log_internal(LOG_WARN, __FILENAME__, __LINE__, __VA_ARGS__)
#define logi_error(...) log_log_internal(LOG_ERROR, __FILENAME__, __LINE__, __VA_ARGS__)
#define logi_fatal(...) log_log_internal(LOG_FATAL, __FILENAME__, __LINE__, __VA_ARGS__)

int log_add_fp_internal(FILE *fp, int level);

void log_log_internal(int level, const char *file, int line, const char *fmt, ...);

#endif
