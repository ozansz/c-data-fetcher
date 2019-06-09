INCDIR = ./src
CC = gcc
CFLAGS = -Wall -ansi -pedantic-errors -g
INCLUDES = -I$(INCDIR)

all: test

test:
	$(CC) $(CFLAGS) $(INCLUDES) -o test test.c $(INCDIR)/*.c
	rm -f *.o

.PHONY: clean

clean:
	rm -f test *.o
