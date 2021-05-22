#ifndef NASTER_UTILS_H
#define NASTER_UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool strcasestartswith(const char *str, const char *substr);

bool strcaseendswith(const char *str, const char *substr);

bool strstartswith(const char *str, const char *substr);

bool strendswith(const char *str, const char *substr);

void str_tolower(char *str);

bool is_number(const char *str, size_t len);

int parse_uint64(const char *str, size_t len, uint64_t *result);

int parse_user_id(const char *str, uint64_t *id);

#ifdef __cplusplus
}
#endif

#endif // NASTER_UTILS_H