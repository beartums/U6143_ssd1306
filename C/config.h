#ifndef CONFIG_H
#define CONFIG_H

#define TOP_LINE_IP       0
#define TOP_LINE_HOSTNAME 1
#define TOP_LINE_CUSTOM   2
#define TOP_LINE_NONE     3  /* top line disabled */

typedef struct {
    int show_temperature;
    int show_memory;
    int show_disk;
    int show_clock;
    int show_uptime;
    int show_cpu_freq;
    int show_gpu_temp;
    int show_network;
    int show_wifi;
    int show_docker;
    int show_hostname;
    int show_ip;
    int temp_fahrenheit;       /* 1=fahrenheit, 0=celsius */
    int load_cores;            /* 0=percent (default), 1=raw cores */
    int top_line;              /* TOP_LINE_IP/HOSTNAME/CUSTOM/NONE */
    char custom_text[64];
    char i2c_bus[32];
    char network_interfaces[64]; /* comma-delimited list, e.g. "eth0,wlan0" */
    int screen_time;             /* seconds per screen */
} DisplayConfig;

extern DisplayConfig g_config;

void load_config(void);

#endif /* CONFIG_H */
