#include "naster-utils.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

bool strcasestartswith(const char *str, const char *substr)
{
    for(; toupper(*str) == toupper(*substr) && *str; str++, substr++);
    return !(*substr);
}

bool strcaseendswith(const char *str, const char *substr)
{
    size_t str_len;
    size_t substr_len;
    size_t i;
    str_len = strlen(str);
    substr_len = strlen(substr);
    if(substr_len > str_len) return false;
    for(; *substr; substr++);
    substr--;
    for(i = str_len - 1; toupper(str[i]) == toupper(*substr) && i < str_len; i--, substr--);
    return i == (str_len - substr_len) - 1;
}

bool strstartswith(const char *str, const char *substr)
{
    for(; *str == *substr && *str; str++, substr++);
    return !(*substr);
}

bool strendswith(const char *str, const char *substr)
{
    size_t str_len;
    size_t substr_len;
    size_t i;
    str_len = strlen(str);
    substr_len = strlen(substr);
    if(substr_len > str_len) return false;
    for(; *substr; substr++);
    substr--;
    for(i = str_len - 1; str[i] == *substr && i < str_len; i--, substr--);
    return i == (str_len - substr_len) - 1;
}

void str_tolower(char *str)
{
    for(; *str; str++)
    {
        *str = tolower(*str);
    }
}

bool is_number(const char *str, size_t len)
{
    size_t i;
    if(len == 0) return false;
    if(str[0] == '-')
    {
        str++;
        len--;
        if(len == 0) return false;
    }
    for(i = 0; i < len; i++)
    {
        if(str[i] < '0' || str[i] > '9') return false;
    }
    return true;
}

int parse_uint64(const char *str, size_t len, uint64_t *result)
{
    char *buf = NULL;
    if(!is_number(str, len)) return -1;
    buf = malloc(len + 1);
    memcpy(buf, str, len);
    buf[len] = '\0';
    uint64_t res = strtoull(buf, NULL, 10);
    free(buf);
    if(res == 0 && !(str[0] == '0' && str[1] != '\0')) return -1;
    if(res == UINT64_MAX && strcmp(str, "18446744073709551615")) return -1;
    *result = res;
    return 0;
}

int parse_user_id(const char *str, uint64_t *id)
{
    if(!parse_uint64(str, strlen(str), id)) return 0;
    if(strendswith(str, ">"))
    {
        if(strstartswith(str, "<@!"))
        {
            return parse_uint64(str + 3, strlen(str + 3) - 1, id);
        }
        if(strstartswith(str, "<@"))
        {
            return parse_uint64(str + 2, strlen(str + 2) - 1, id);
        }
    }
    return -1;
}