#ifndef LANGUAGES_H
#define LANGUAGES_H

#include <stddef.h>

typedef struct
{
    const char *name;
    struct
    {
        char *english;
        char *portuguese;
    } languages;
    struct
    {
        char *help;
        char *signature;
        char *example;
        char *examples;
        char *all;
    } constants;
    struct
    {
        char *identifier;
        char *name;
        char *description;
        char *signature;
        struct
        {
            char *description;
            char *value;
        } *examples;
        size_t n_examples;
    } *commands;
    size_t n_commands;
} language;


language *language_new(const char *name);

void language_free(language *language);

#endif