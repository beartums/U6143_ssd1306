CC     = gcc
CFLAGS = -Wall -g

REAL_SRC = display.c ssd1306_i2c.c lcd_screens.c config.c
SIM_SRC  = display_sim.c ssd1306_sim.c lcd_screens.c config.c

.PHONY: all sim test clean

all: display

display: $(REAL_SRC)
	$(CC) $(CFLAGS) -o $@ $^

sim: $(SIM_SRC)
	$(CC) $(CFLAGS) -o display_sim $^

test:
	$(CC) -I. tests/test_config.c config.c -o tests/test_config -g
	cd tests && ./test_config

clean:
	rm -f display display_sim
	rm -f tests/test_config
