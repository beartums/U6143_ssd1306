/******
Demo for ssd1306 i2c driver for  Raspberry Pi 
******/
#include <stdio.h>
#include "ssd1306_i2c.h"
#include "time.h"
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char* argv[])
{
    bool debug = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            debug = true;
        }
    }
    struct DisplayConfig config = {1,1,1,1};
    int display_order[4];
    int display_count = 0;
    if (!load_display_config("display.cfg", &config, debug)) {
        printf("Warning: Could not load display_config.yaml, using defaults.\n");
    }
    if (config.show_temperature) display_order[display_count++] = 0;
    if (config.show_cpu_memory) display_order[display_count++] = 1;
    if (config.show_sd_memory) display_order[display_count++] = 2;
    if (config.show_hostname) display_order[display_count++] = 3;
    unsigned char symbol=0;
    ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
    if(i2cd<0)
    {
        printf("I2C device failed to open\r\n");
        return 0;
    }
    usleep(150*1000);
    FirstGetIpAddress();
    if (config.show_hostname) {
        LCD_DisplayHostname();
        sleep(3);
    }
    int idx = 0;
    while(display_count > 0)
    {
        LCD_Display(display_order[idx]);
        sleep(1);
        sleep(1);
        sleep(1);
        idx = (idx + 1) % display_count;
    }
    return 0;
}
