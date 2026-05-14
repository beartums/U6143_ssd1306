#ifndef CONFIG_H
#define CONFIG_H

#define TOP_LINE_IP       0
#define TOP_LINE_HOSTNAME 1
#define TOP_LINE_CUSTOM   2

typedef struct {
    int show_temperature;
    int show_memory;
    int show_disk;
    int temp_fahrenheit;  /* 1=fahrenheit, 0=celsius */
    int ip_source;        /* 0=eth0, 1=wlan0 */
    int top_line;         /* TOP_LINE_IP/HOSTNAME/CUSTOM */
    char custom_text[64];
} DisplayConfig;

extern DisplayConfig g_config;

void load_config(void);

#endif /* CONFIG_H */
