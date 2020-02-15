INCLUDE_DIRS =
LIB_DIRS =
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS=

HFILES=
CFILES= modified_feasibility_tests.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	modified_feasibility_tests

clean:
	-rm -f *.o *.d
	-rm -f modified_feasibility_tests

modified_feasibility_tests: modified_feasibility_tests.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o -lm

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<