#include "commands.h"
#include <orka-utils.h>

void clear(struct discord *client, const struct discord_message *msg, const language *language, const char *prefix, const char *arg)
{
    size_t i;
    bool all = false;
    uint64_t amount;
    struct discord_get_channel_messages_params params = {};
    struct discord_message **messages = NULL;
    NTL_T(u64_snowflake_t) list = NULL;
    u64_unix_ms_t now;
    bool found_new;
    size_t new;
    size_t len = 0;

    if(!arg)
    {
        help(client, msg, language, prefix, "clear");
        return;
    }

    for(i = 0; languages[i]; i++)
    {
        if(!strcasecmp(arg, languages[i]->constants.all))
        {
            all = true;
            break;
        }
    }

    if(!all)
    {
        if(parse_uint64(arg, strlen(arg), &amount) || amount == 0)
        {
            help(client, msg, language, prefix, "clear");
            return;
        }
        amount++;
    }

    while((all || amount) && params.limit == len)
    {
        params.limit = all ? 100 : amount;
        if(messages) params.before = messages[len - 1]->id;
        discord_get_channel_messages(client, msg->channel_id, &params, &messages);
        now = orka_timestamp_ms();
        found_new = false;
        for(i = 0; messages[i]; i++)
        {
            if(!found_new && now > messages[i]->timestamp && now - messages[i]->timestamp > 1206000000)
            {
                new = i;
                found_new = true;
            }
        }
        len = i;
        if(!found_new) new = len;

        if(new == 1) new = 0;
        else if(new)
        {
            list = (NTL_T(u64_snowflake_t)) ntl_malloc(new, sizeof(u64_snowflake_t));
            if(!list)
            {
                printf("Error: out of memory.\n");
                exit(-1);
                return;
            }
            for(i = 0; i < new; i++)
            {
                *list[i] = messages[i]->id;
            }
            discord_bulk_delete_messages(client, msg->channel_id, list);
            ntl_free((ntl_t) list, NULL);
        }

        for(i = new; messages[i]; i++)
        {
            discord_delete_message(client, msg->channel_id, messages[i]->id);
        }

        if(!all)
        {
            amount -= len;
        }
    }
}