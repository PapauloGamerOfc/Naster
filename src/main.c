#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "blake2b.h"
#include <inttypes.h>
#include "commands.h"
#include "discord-voice-connections.h"
#include "naster-utils.h"
#include "orka-utils.h"

#ifndef LIL_ENDIAN
#define LIL_ENDIAN  1234
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN  4321
#endif

#ifndef BYTEORDER
#ifdef __linux__
#include <endian.h>
#define BYTEORDER  __BYTE_ORDER
#else /* __linux__ */
#if defined(__hppa__) || \
    defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
    (defined(__MIPS__) && defined(__MISPEB__)) || \
    defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
    defined(__sparc__)
#define BYTEORDER   BIG_ENDIAN
#else
#define BYTEORDER   LIL_ENDIAN
#endif
#endif /* __linux__ */
#endif /* !BYTEORDER */

#define prefix "na."

#define next_arg strtok(NULL, " ")

language *english    = NULL;
language *portuguese = NULL;
language *default_language = NULL;
language **languages = NULL;

void on_ready(struct discord *client, const struct discord_user *me)
{
    struct discord_guild **guilds = NULL;
    size_t nguilds;
    size_t i;

    printf("Logged in as %s#%s.\n", me->username, me->discriminator);

    discord_get_current_user_guilds(client, &guilds);
    nguilds = ntl_length((ntl_t) guilds);
    printf("I am on %zu guilds.\n", nguilds);

    for(i = 0; i < nguilds; i++)
    {
        printf("%zu: %s\n", i + 1, guilds[i]->name);
    }

    discord_join_vc(client, 790619435202248725, 790619436985745431, false, true);
}

void on_voice_ready(struct discord_voice *vc)
{
    printf("ready!\n");
    discord_send_speaking(vc, DISCORD_VOICE_MICROPHONE, 0);
}

language *region_to_language(const char *region);

bool command_equals(const char *str, const char *command)
{
    size_t i;
    size_t j;

    for(i = 0; languages[i]; i++)
    {
        for(j = 0; j < languages[i]->n_commands; j++)
        {
            if(!strcmp(languages[i]->commands[j].identifier, command))
            {
                if(!strcasecmp(languages[i]->commands[j].name, str))
                {
                    return true;
                }
                break;
            }
        }
    }
    return false;
}

void on_message_create(
    struct discord *client,
    const struct discord_user *me,
    const struct discord_message *msg)
{
    const char *command = NULL;
    struct discord_guild *guild = NULL;
    language *language = NULL;

    if(msg->author->id == me->id) return;

    guild = discord_guild_alloc();
    discord_get_guild(client, msg->guild_id, guild);
    language = region_to_language(guild->region);

    if(strcasestartswith(msg->content, prefix))
    {
        command = strtok(msg->content + strlen(prefix), " ");
        if(command_equals(command, "help"))
        {
            help(client, msg, language, prefix, next_arg);
        }
        else if(command_equals(command, "clear"))
        {
            clear(client, msg, language, prefix, next_arg);
        }
        else if(command_equals(command, "ship"))
        {
            ship(client, msg, language, prefix, next_arg, next_arg);
        }
    }
}

language **language_ntl(language *first, ...)
{
    language **ntl = NULL;
    language *cur = NULL;
    size_t len;
    size_t i;
    va_list args;
    va_start(args, first);
    for(cur = first, len = 1; cur; cur = va_arg(args, language*), len++);
    va_end(args);
    ntl = malloc(len * sizeof(language*));
    if(!ntl)
    {
        printf("Error: out of memory.\n");
        exit(-1);
        return NULL;
    }
    va_start(args, first);
    for(cur = first, i = 0; i < len; cur = va_arg(args, language*), i++)
    {
        ntl[i] = cur;
    }
    va_end(args);
    return ntl;
}

language *region_to_language(const char *region)
{
    if(!strcmp(region, "brazil"))
    {
        return portuguese;
    }
    else if(strstartswith(region, "us"))
    {
        return english;
    }
    return default_language;
}

int main()
{
    struct discord *client = NULL;
    struct discord_voice_cbs cbs = {};

    english    = language_new("english");
    portuguese = language_new("portuguese");
    default_language = english;
    languages  = language_ntl(english, portuguese, NULL);

    discord_global_init();

    client = discord_config_init("bot.config");
    assert(NULL != client);

    discord_set_on_ready(client, &on_ready);
    discord_set_on_message_create(client, &on_message_create);

    cbs.on_ready = on_voice_ready;
    discord_set_voice_cbs(client, &cbs);

    discord_run(client);

    discord_cleanup(client);
    discord_global_cleanup();

    return 0;
}