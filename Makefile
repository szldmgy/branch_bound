CC = clang
CFLAGS = -Wall -Wextra -pedantic `pkg-config --cflags glib-2.0`
LDFLAGS = `pkg-config --libs glib-2.0`
PROGNAME = bb

ifeq ($(CC),clang)
	OPTFLAGS = -O4
else
	OPTFLAGS = -O3
endif

all: $(PROGNAME)

debug: CFLAGS += -g
debug: LDFLAGS += -g
debug: OPTFLAGS = -O0
debug: $(PROGNAME)

$(PROGNAME): *.o
	$(CC) $(LDFLAGS) $(OPTFLAGS) -o $(PROGNAME) $^

*.o: *.c
	$(CC) $(CFLAGS) $(OPTFLAGS) -c $<

clean:
	rm -f $(PROGNAME) *.o *~ .*~ *.dSYM

.PHONY: clean debug all
