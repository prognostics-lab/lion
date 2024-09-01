#pragma once

#include <lion/app.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG

#define lionu_malloc(app, x) malloc(x)
#define lionu_realloc(app, x, y) realloc(x, y)
#define lionu_calloc(app, x, y) calloc(x, y)
#define lionu_free(app, x) free(x)

#else

void *_lionu_malloc(lion_app_t *app, size_t size, const char *filename,
                    int line);
void *_lionu_realloc(lion_app_t *app, void *ptr, size_t new_size,
                     const char *filename, int line);
void *_lionu_calloc(lion_app_t *app, size_t num, size_t size,
                    const char *filename, int line);
void _lionu_free(lion_app_t *app, void *ptr, const char *filename, int line);

#define lionu_malloc(app, x) _lionu_malloc(app, x, __FILENAME__, __LINE__)
#define lionu_realloc(app, x, y)                                               \
  _lionu_realloc(app, x, y, __FILENAME__, __LINE__)
#define lionu_calloc(app, x, y) _lionu_calloc(app, x, y, __FILENAME__, __LINE__)
#define lionu_free(app, x) _lionu_free(app, x, __FILENAME__, __LINE__)

#endif

#ifdef __cplusplus
}
#endif
