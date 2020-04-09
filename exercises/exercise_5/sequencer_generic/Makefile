INCLUDE_DIRS =
LIB_DIRS =
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(CDEFS)
LL_NAMES= pthread \
			 rt
LDFLAGS= $(addprefix -l,$(LL_NAMES))


$(shell mkdir -p ./bin &> /dev/null)
$(shell mkdir -p ./build &> /dev/null)

.PHONY : all
all: ./bin/seqgen ./bin/seqgen2x ./bin/seqgen100x

.PHONY : clean
clean:
	rm -rf ./bin ./build

./bin/% : ./build/%.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

./build/%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PRECIOUS : ./build/%.o
