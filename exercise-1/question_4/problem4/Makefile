INCLUDE_DIRS =
LIB_DIRS =
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS=

HFILES=
CFILES= prob4.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:    pthread

clean:
	-rm -f *.o *.d
	-rm -f pthread

pthread: prob4.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ prob4.o -pthread

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<
