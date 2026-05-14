/******
Demo for ssd1306 i2c driver for  Raspberry Pi
******/
#include <stdio.h>
#include "ssd1306_i2c.h"
#include "config.h"
#include "time.h"
#include <unistd.h>



int main(void)
{
    ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
    if(i2cd<0)
    {
        printf("I2C device failed to open\r\n");
        return 0;
    }
    usleep(150*1000);

    load_config();
    FirstGetIpAddress();

    /* Build list of enabled screens in rotation order */
    unsigned char screens[3];
    int nscreens = 0;
    if (g_config.show_temperature) screens[nscreens++] = 0;
    if (g_config.show_memory)      screens[nscreens++] = 1;
    if (g_config.show_disk)        screens[nscreens++] = 2;
    if (nscreens == 0) { screens[0] = 0; nscreens = 1; }

    int idx = 0;
    while(1)
    {
        LCD_Display(screens[idx]);
        sleep(3);
        idx = (idx + 1) % nscreens;
    }
    return 0;
}
