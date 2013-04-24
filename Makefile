CC = gcc
CFLAGS = -Wall -O2 -g
CPOSTFLAGS = -lm

all: example

cfrax.o: cfrax.c cfrax.h
	makeheaders cfrax.c
	$(CC) $(CFLAGS) -c cfrax.c $(CPOSTFLAGS)

example.o: example.c example.h
	makeheaders example.c
	$(CC) $(CFLAGS) -c example.c $(CPOSTFLAGS)

example: cfrax.o example.o
	$(CC) $(CFLAGS) cfrax.o example.o -o example $(CPOSTFLAGS)

.PHONY: clean
clean:
	/bin/rm -f *.o example
