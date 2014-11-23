CFLAGS=--std=c1x --pedantic -Wall -Werror -ggdb -I../src
LFLAGS=-lm
CFILES=$(wildcard *.c)
SRCFILES=$(wildcard ../src/*.c)
OFILES=$(CFILES:.c=.o)
SRCOFILES=$(SRCFILES:.c=.o)
EXECFILES=$(CFILES:.c=)

$(EXECFILES): %: %.o $(SRCOFILES)
	$(CC) -o $@ $< $(SRCOFILES) $(LFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(PROG) *.o
