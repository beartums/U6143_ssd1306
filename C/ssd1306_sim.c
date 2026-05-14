/* ssd1306_sim.c — terminal simulator for the SSD1306 128x32 OLED.
 *
 * Drop-in replacement for ssd1306_i2c.c: provides the low-level hardware
 * primitives (OLED_Set_Pos, OLED_WR_Byte, OLED_Clear, etc.) backed by an
 * in-memory framebuffer.  All high-level display logic lives in lcd_screens.c
 * and is shared with the real hardware build.
 *
 * Requires a UTF-8 terminal at least 130 columns wide.
 * Build with: make sim
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "ssd1306_i2c.h"
#include "config.h"

/* ------------------------------------------------------------------ */
/* Framebuffer state                                                    */
/* ------------------------------------------------------------------ */

static unsigned char framebuf[4][128];
static unsigned char cur_page = 0;
static unsigned char cur_col  = 0;

int i2cd = 1;    /* positive so display.c skips the "failed" branch */

/* ------------------------------------------------------------------ */
/* Hardware stubs                                                       */
/* ------------------------------------------------------------------ */

void ssd1306_begin(unsigned int vccstate, unsigned int i2caddr)
{
    (void)vccstate; (void)i2caddr;
}

void Write_IIC_Data(unsigned char d)    { (void)d; }
void Write_IIC_Command(unsigned char c) { (void)c; }

void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    cur_col  = x;
    cur_page = y & 0x03;
}

void OLED_WR_Byte(unsigned dat, unsigned cmd)
{
    if (cmd) { /* OLED_DATA = 1 */
        if (cur_page < 4 && cur_col < 128)
            framebuf[cur_page][cur_col++] = (unsigned char)dat;
    }
    /* command bytes are handled positionally via OLED_Set_Pos; ignore raw */
}

void OLED_Clear(void)
{
    memset(framebuf, 0, sizeof(framebuf));
}

void OLED_ClearLint(unsigned char x1, unsigned char x2)
{
    unsigned char i;
    for (i = x1; i < x2 && i < 4; i++)
        memset(framebuf[i], 0, 128);
}

/* ------------------------------------------------------------------ */
/* Terminal renderer                                                    */
/* ------------------------------------------------------------------ */

static int get_pixel(int row, int col)
{
    int page = row / 8;
    int bit  = row % 8;
    if (page >= 4 || col >= 128) return 0;
    return (framebuf[page][col] >> bit) & 1;
}

/* Returns the UTF-8 half-block character for a pair of vertical pixels. */
static const char *half_block(int top, int bot)
{
    if (!top && !bot) return " ";
    if ( top && !bot) return "\xe2\x96\x80"; /* ▀ U+2580 */
    if (!top &&  bot) return "\xe2\x96\x84"; /* ▄ U+2584 */
    return             "\xe2\x96\x88";       /* █ U+2588 */
}

void sim_render(void)
{
    printf("\033[2J\033[H");
    printf("SSD1306 Simulator  [Ctrl-C to quit]\n\n");

    /* top border */
    printf("\xe2\x94\x8c"); /* ┌ */
    for (int c = 0; c < 128; c++) printf("\xe2\x94\x80"); /* ─ */
    printf("\xe2\x94\x90\n"); /* ┐ */

    for (int tr = 0; tr < 16; tr++) {
        printf("\xe2\x94\x82"); /* │ */
        for (int tc = 0; tc < 128; tc++) {
            int top = get_pixel(tr * 2,     tc);
            int bot = get_pixel(tr * 2 + 1, tc);
            printf("%s", half_block(top, bot));
        }
        printf("\xe2\x94\x82\n"); /* │ */
    }

    /* bottom border */
    printf("\xe2\x94\x94"); /* └ */
    for (int c = 0; c < 128; c++) printf("\xe2\x94\x80"); /* ─ */
    printf("\xe2\x94\x98\n"); /* ┘ */

    fflush(stdout);
}

/* One-shot dump of the current framebuffer to f — no ANSI escape codes. */
void sim_dump(FILE *f)
{
    fprintf(f, "\xe2\x94\x8c"); /* ┌ */
    for (int c = 0; c < 128; c++) fprintf(f, "\xe2\x94\x80"); /* ─ */
    fprintf(f, "\xe2\x94\x90\n"); /* ┐ */

    for (int tr = 0; tr < 16; tr++) {
        fprintf(f, "\xe2\x94\x82"); /* │ */
        for (int tc = 0; tc < 128; tc++) {
            int top = get_pixel(tr * 2,     tc);
            int bot = get_pixel(tr * 2 + 1, tc);
            fprintf(f, "%s", half_block(top, bot));
        }
        fprintf(f, "\xe2\x94\x82\n"); /* │ */
    }

    fprintf(f, "\xe2\x94\x94"); /* └ */
    for (int c = 0; c < 128; c++) fprintf(f, "\xe2\x94\x80"); /* ─ */
    fprintf(f, "\xe2\x94\x98\n"); /* ┘ */
}
