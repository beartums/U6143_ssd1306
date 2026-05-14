/* display_sim.c — main() for the terminal simulator.
 *
 * Identical logic to display.c but calls sim_render() after each screen
 * update to paint the framebuffer to the terminal.
 *
 *   make sim
 *   ./display_sim
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ssd1306_i2c.h"
#include "config.h"

void sim_render(void);
void sim_dump(FILE *f);

static const char *screen_name(unsigned char id)
{
    switch (id) {
        case 0: return "Temperature";
        case 1: return "Memory";
        case 2: return "Disk";
        case 3: return "Clock";
        case 4: return "Uptime";
        case 5: return "CPU Frequency";
        case 6: return "GPU Temperature";
        case 7: return "Network";
        case 8: return "WiFi";
        case 9:  return "Docker";
        case 10: return "Hostname";
        case 11: return "IP Address";
        default: return "Unknown";
    }
}

int main(int argc, char **argv)
{
    int dump_mode = (argc > 1 && strcmp(argv[1], "--dump") == 0);

    ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);

    load_config();
    FirstGetIpAddress();

    unsigned char screens[12];
    int nscreens = 0;
    if (g_config.show_temperature) screens[nscreens++] = 0;
    if (g_config.show_memory)      screens[nscreens++] = 1;
    if (g_config.show_disk)        screens[nscreens++] = 2;
    if (g_config.show_clock)       screens[nscreens++] = 3;
    if (g_config.show_uptime)      screens[nscreens++] = 4;
    if (g_config.show_cpu_freq)    screens[nscreens++] = 5;
    if (g_config.show_gpu_temp)    screens[nscreens++] = 6;
    if (g_config.show_network)     screens[nscreens++] = 7;
    if (g_config.show_wifi)        screens[nscreens++] = 8;
    if (g_config.show_docker)      screens[nscreens++] = 9;
    if (g_config.show_hostname)    screens[nscreens++] = 10;
    if (g_config.show_ip)          screens[nscreens++] = 11;
    if (nscreens == 0) { screens[0] = 0; nscreens = 1; }

    if (dump_mode) {
        for (int i = 0; i < nscreens; i++) {
            LCD_Display(screens[i]);
            printf("[ %s ]\n", screen_name(screens[i]));
            sim_dump(stdout);
            printf("\n");
        }
        return 0;
    }

    int idx = 0;
    while (1) {
        LCD_Display(screens[idx]);
        sim_render();
        printf("  Screen %d/%d: %s  (%ds per screen)\n",
               idx + 1, nscreens, screen_name(screens[idx]), g_config.screen_time);
        fflush(stdout);
        sleep((unsigned int)g_config.screen_time);
        idx = (idx + 1) % nscreens;
    }
    return 0;
}
