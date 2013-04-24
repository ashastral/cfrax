CC = gcc
CFLAGS = -Wall -O2

all: example

cfrax.o: cfrax.c cfrax.h
	makeheaders cfrax.c
	$(CC) $(CFLAGS) -c cfrax.c

example.o: example.c example.h
	makeheaders example.c
	$(CC) $(CFLAGS) -c example.c

example: cfrax.o example.o
	$(CC) $(CFLAGS) cfrax.o example.o -o example

.PHONY: clean
clean:
	/bin/rm -f *.o example
