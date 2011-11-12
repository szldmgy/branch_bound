CC = clang
CFLAGS = -Wall -Wextra -pedantic
LDFLAGS =
PROGNAME = bb

ifeq ($(CC),clang)
	OPTFLAGS = -O4 -flto -march=native
else
	OPTFLAGS = -O3 -flto -march=native
endif

all: $(PROGNAME)

debug: CFLAGS += -g
debug: LDFLAGS += -g
debug: OPTFLAGS = -O0
debug: $(PROGNAME)

$(PROGNAME): *.o
	$(CC) -pipe $(LDFLAGS) $(OPTFLAGS) -o $(PROGNAME) $^

*.o: *.c
	$(CC) -pipe $(CFLAGS) $(OPTFLAGS) -c $<

clean:
	rm -f $(PROGNAME) *.o *~ .*~ *.dSYM


.PHONY: clean debug all

