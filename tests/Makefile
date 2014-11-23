PROG=tests
CFLAGS=--std=c1x --pedantic -Wall -Werror -ggdb -I../src
LFLAGS=-lm
CFILES=$(wildcard *.c)
SRCFILES=$(wildcard ../src/*.c)
OFILES=$(CFILES:.c=.o) $(SRCFILES:.c=.o)

$(PROG): $(OFILES)
	make ../src
	$(CC) -o $(PROG) $(OFILES) $(LFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(PROG) *.o
