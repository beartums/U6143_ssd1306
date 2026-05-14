#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

DisplayConfig g_config = {
    .show_temperature = 1,
    .show_memory      = 1,
    .show_disk        = 1,
    .temp_fahrenheit  = 1,
    .ip_source        = 0,
    .top_line         = TOP_LINE_IP,
    .custom_text      = "UCTRONICS",
};

static void trim(char *s)
{
    char *end = s + strlen(s) - 1;
    while (end >= s && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n'))
        *end-- = '\0';
    char *start = s;
    while (*start == ' ' || *start == '\t') start++;
    if (start != s)
        memmove(s, start, strlen(start) + 1);
}

void load_config(void)
{
    FILE *f = NULL;
    const char *env = getenv("SSD1306_CONF");
    if (env)
        f = fopen(env, "r");
    if (!f)
        f = fopen("./ssd1306.conf", "r");
    if (!f)
        f = fopen("/etc/ssd1306.conf", "r");
    if (!f)
        return;

    char line[128];
    while (fgets(line, sizeof(line), f)) {
        char *eq = strchr(line, '=');
        if (!eq)
            continue;
        *eq = '\0';
        char *key = line;
        char *val = eq + 1;
        trim(key);
        trim(val);
        if (key[0] == '#' || key[0] == '\0')
            continue;

        if (strcmp(key, "show_temperature") == 0)
            g_config.show_temperature = atoi(val);
        else if (strcmp(key, "show_memory") == 0)
            g_config.show_memory = atoi(val);
        else if (strcmp(key, "show_disk") == 0)
            g_config.show_disk = atoi(val);
        else if (strcmp(key, "temp_unit") == 0)
            g_config.temp_fahrenheit = (strcmp(val, "fahrenheit") == 0);
        else if (strcmp(key, "ip_source") == 0)
            g_config.ip_source = (strcmp(val, "wlan0") == 0) ? 1 : 0;
        else if (strcmp(key, "top_line") == 0) {
            if (strcmp(val, "hostname") == 0)
                g_config.top_line = TOP_LINE_HOSTNAME;
            else if (strcmp(val, "custom") == 0)
                g_config.top_line = TOP_LINE_CUSTOM;
            else
                g_config.top_line = TOP_LINE_IP;
        }
        else if (strcmp(key, "custom_text") == 0)
            strncpy(g_config.custom_text, val, sizeof(g_config.custom_text) - 1);
    }
    fclose(f);
}
