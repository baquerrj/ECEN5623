INCLUDE_DIRS =
LIB_DIRS =
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LL_NAMES= pthread \
			 rt
LDFLAGS= $(addprefix -l,$(LL_NAMES))


$(shell mkdir -p ./bin &> /dev/null)
$(shell mkdir -p ./build &> /dev/null)

.PHONY : all
all: ./bin/seqgen ./bin/seqgen2x

./bin/% : ./build/%.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)  

./build/%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS)


.PHONY : clean
clean:
	rm -rf ./bin ./build


