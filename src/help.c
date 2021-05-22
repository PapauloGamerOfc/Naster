#include "commands.h"

void help(struct discord *client, const struct discord_message *msg, const language *language, const char *prefix, const char *command)
{
    size_t i;
    size_t j;
    size_t k;
    struct discord_create_message_params params = {};
    struct discord_embed *embed = NULL;
    char *buf = NULL;

    if(command)
    {
        for(i = 0; languages[i]; i++)
        {
            for(j = 0; j < languages[i]->n_commands; j++)
            {
                if(!strcasecmp(languages[i]->commands[j].name, command))
                {
                    embed = discord_embed_alloc();
                    snprintf(embed->title, sizeof(embed->title), "%s - %s", language->constants.help, language->commands[j].name);
                    buf = embed->description;
                    buf += snprintf(buf, sizeof(embed->description) - (buf - embed->description), "%s\n\n", language->commands[j].description);
                    buf += snprintf(buf, sizeof(embed->description) - (buf - embed->description), "**%s**\n%s%s%s%s\n\n", language->constants.signature, prefix, language->commands[j].name, language->commands[j].signature[0] == '\0' ? "" : " ", language->commands[j].signature);
                    buf += snprintf(buf, sizeof(embed->description) - (buf - embed->description), "**%s**\n\n", language->commands[j].n_examples == 1 ? language->constants.example : language->constants.examples);
                    for(k = 0; k < language->commands[j].n_examples; k++)
                    {
                        buf += snprintf(buf, sizeof(embed->description) - (buf - embed->description), "**%s**\n %s%s%s%s", language->commands[j].examples[k].description, prefix, language->commands[j].name, language->commands[j].examples[k].value[0] == '\0' ? "" : " ", language->commands[j].examples[k].value);
                        if(k + 1 < language->commands[j].n_examples)
                        {
                            buf += snprintf(buf, sizeof(embed->description) - (buf - embed->description), "\n\n");
                        }
                    }
                    params.embed = embed;
                    discord_create_message(client, msg->channel_id, &params, NULL);
                    discord_embed_free(embed);
                    return;
                }
            }
        }
    }

    embed = discord_embed_alloc();
    strncpy(embed->title, language->constants.help, sizeof(embed->title));
    buf = embed->description;
    for(j = 0; j < language->n_commands; j++)
    {
        buf += snprintf(buf, sizeof(embed->description) - (buf - embed->description), "**%s%s%s**\n%s", language->commands[j].name, language->commands[j].signature[0] == '\0' ? "" : " ", language->commands[j].signature, language->commands[j].description);
        if(j < language->n_commands)
        {
            buf += snprintf(buf, sizeof(embed->description) - (buf - embed->description), "\n\n");
        }
    }
    params.embed = embed;
    discord_create_message(client, msg->channel_id, &params, NULL);
    discord_embed_free(embed);
    return;
}