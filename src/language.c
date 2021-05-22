#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "language.h"
#include <json-actor.h>

#define FREE_IF_OBJ(obj, free) if(obj) free(obj)

static void free_objs(void *start, const void *end)
{
    void **s = start;
    const void **e = (const void**) end;
    while(start < end)
    {
        if(*s)
        {
            free(*s);
            *s = NULL;
        }
        start++;
    }
}

language *language_new(const char *name)
{
    char filename[FILENAME_MAX + 1];
    language *language = NULL;
    FILE *f = NULL;
    size_t len;
    size_t res;
    char *buf = NULL;
    json_item_t *obj = NULL;
    json_item_t *languages = NULL;
    json_item_t *constants = NULL;
    json_item_t *commands = NULL;
    json_item_t *command = NULL;
    json_item_t *examples = NULL;
    json_item_t *example = NULL;
    size_t i, j;

    language = malloc(sizeof(*language));
    if(!language)
    {
        printf("Error: out of memory.\n");
        goto err;
    }
    memset(language, 0, sizeof(*language));

    language->name = name;

    if(snprintf(filename, sizeof(filename), "languages/%s.json", name) > sizeof(filename) > FILENAME_MAX)
    {
        printf("Error: language name exceeds max file name length.\n");
        goto err;
    }

    if((f = fopen(filename, "r")) == NULL)
    {
        printf("Error: cannot open language file.\n");
        goto err;
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    buf = malloc(len + 1);
    if(!buf)
    {
        printf("Error: out of memory.\n");
        goto err;
    }

    if((res = fread(buf, 1, len, f)) == 0 && len > 0)
    {
        printf("Error reading file.\n");
        goto err;
    }
    len = res;

    fclose(f);
    f = NULL;

    obj = json_parse(buf, len);
    if(!obj)
    {
        printf("Error parsing JSON.\n");
        goto err;
    }

    free(buf);
    buf = NULL;

    languages = json_get_child(obj, "languages");
    constants = json_get_child(obj, "constants");
    commands  = json_get_child(obj, "commands");

    language->languages.english    = json_strdup(json_get_child(languages, "english"));
    language->languages.portuguese = json_strdup(json_get_child(languages, "portuguese"));

    language->constants.help        = json_strdup(json_get_child(constants, "help"));
    language->constants.signature   = json_strdup(json_get_child(constants, "signature"));
    language->constants.example     = json_strdup(json_get_child(constants, "example"));
    language->constants.examples    = json_strdup(json_get_child(constants, "examples"));
    language->constants.all         = json_strdup(json_get_child(constants, "all"));

    language->n_commands = json_size(commands);
    language->commands = malloc(language->n_commands * sizeof(*language->commands));
    if(!language->commands)
    {
        printf("Error: out of memory.\n");
        goto err;
    }

    for(i = 0; i < language->n_commands; i++)
    {
        command = json_get_byindex(commands, i);
        language->commands[i].identifier  = strdup(json_get_key(command));
        language->commands[i].name        = json_strdup(json_get_child(command, "name"));
        language->commands[i].description = json_strdup(json_get_child(command, "description"));
        language->commands[i].signature   = json_strdup(json_get_child(command, "signature"));
        examples = json_get_child(command, "examples");
        language->commands[i].n_examples = json_size(examples);
        language->commands[i].examples = malloc(language->commands[i].n_examples * sizeof(*language->commands[i].examples));
        if(!language->commands[i].examples)
        {
            printf("Error: out of memory.\n");
            goto err;
        }
        for(j = 0; j < language->commands[i].n_examples; j++)
        {
            example = json_get_byindex(examples, j);
            language->commands[i].examples[j].description = json_strdup(json_get_child(example, "description"));
            language->commands[i].examples[j].value       = json_strdup(json_get_child(example, "value"));
        }
    }

    json_cleanup(obj);

    return language;

    err:
    FREE_IF_OBJ(language, language_free);
    FREE_IF_OBJ(f, fclose);
    FREE_IF_OBJ(buf, free);
    FREE_IF_OBJ(obj, json_cleanup);
    exit(-1);
    return NULL;
}

void language_free(language *language)
{
    size_t i, j;
    free_objs(&language->languages, &language->commands);
    for(i = 0; i < language->n_commands; i++)
    {
        free_objs(&language->commands[i].description, &language->commands[i].examples);
        for(j = 0; j < language->commands[i].n_examples; j++)
        {
            free_objs(language->commands[i].examples + j, language->commands[i].examples + j + 1);
        }
        if(language->commands[i].examples)
        {
            free(language->commands[i].examples);
            language->commands[i].examples = NULL;
        }
    }
    if(language->commands) 
    {
        free(language->commands);
        language->commands = NULL;
    }

    free(language);
}