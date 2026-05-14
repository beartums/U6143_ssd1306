Target		= display
CXX         	= gcc
ODIR		= obj
SRC		=$(wildcard *.c)
DIR		=$(notdir $(SRC))
OBJ 		= $(patsubst %.c, %.o, $(DIR))
$(Target): $(OBJ)
	$(CXX) $^ -o $@ -g -rdynamic 
.PHONY: clean test
test:
	$(CXX) -I. tests/test_config.c config.c -o tests/test_config -g
	cd tests && ./test_config

clean:
	rm -rf *.o
	rm -rf $(Target)
	rm -f tests/test_config
