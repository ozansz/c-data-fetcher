INCDIR = ./src
CC = gcc
CFLAGS = -Wall -ansi -pedantic-errors
INCLUDES = -I$(INCDIR)

test: test.o
	$(CC) $(CFLAGS) $(INCLUDES) -o test test.o

test.o: test.c
	$(CC) $(CFLAGS) $(INCLUDES) -c test.c

clean:
	rm -f test *.o
