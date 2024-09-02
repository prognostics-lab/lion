#pragma once

#include <lion/app.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG

#define lion_malloc(app, x) malloc(x)
#define lion_realloc(app, x, y) realloc(x, y)
#define lion_calloc(app, x, y) calloc(x, y)
#define lion_free(app, x) free(x)

#else

void *_lion_malloc(lion_app_t *app, size_t size, const char *filename,
                   int line);
void *_lion_realloc(lion_app_t *app, void *ptr, size_t new_size,
                    const char *filename, int line);
void *_lion_calloc(lion_app_t *app, size_t num, size_t size,
                   const char *filename, int line);
void _lion_free(lion_app_t *app, void *ptr, const char *filename, int line);

#define lion_malloc(app, x) _lion_malloc(app, x, __FILENAME__, __LINE__)
#define lion_realloc(app, x, y) _lion_realloc(app, x, y, __FILENAME__, __LINE__)
#define lion_calloc(app, x, y) _lion_calloc(app, x, y, __FILENAME__, __LINE__)
#define lion_free(app, x) _lion_free(app, x, __FILENAME__, __LINE__)

#endif

#ifdef __cplusplus
}
#endif
