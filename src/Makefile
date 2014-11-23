CFLAGS=--std=c1x --pedantic -Wall -Werror -ggdb
LFLAGS=-lm
CFILES=$(wildcard *.c)
OFILES=$(CFILES:.c=.o)

all: $(OFILES)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
