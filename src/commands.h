#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "language.h"
#include "naster-utils.h"
#include <discord.h>

extern language **languages;

void help(struct discord *client, const struct discord_message *msg, const language *language, const char *prefix, const char *command);

void clear(struct discord *client, const struct discord_message *msg, const language *language, const char *prefix, const char *arg);

void ship(struct discord *client, const struct discord_message *msg, const language *language, const char *prefix, char *a, char *b);

#endif // COMMANDS_H