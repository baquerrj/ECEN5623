INCLUDE_DIRS =
LIB_DIRS =
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS=

HFILES=
CFILES= seqgen.c seqgen2x.c

SRCS= ${HFILES} ${CFILES}


$(shell mkdir -p ./bin &> /dev/null)
$(shell mkdir -p ./build &> /dev/null)

all: ./bin/seqgen ./bin/seqgen2x

clean:
	rm -rf ./bin ./build

./bin/% : ./build/%.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ -lpthread -lrt

#./bin/seqgen2: ./build/seqgen2.o
#	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ -lpthread -lrt
#
#./bin/seqgen: ./build/seqgen.o
#	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ -lpthread -lrt
#

./build/%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS)

depend:
