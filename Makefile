INCDIR = ./src
CC = gcc
CFLAGS = -Wall -ansi -pedantic-errors -g
INCLUDES = -I$(INCDIR)

all: test cleanobj

test:
	$(CC) $(CFLAGS) $(INCLUDES) -o test test.c $(INCDIR)/*.c

.PHONY: clean
.PHONY: cleanobj

cleanobj:
	rm -f *.o

clean:
	rm -f test test.exe *.o
