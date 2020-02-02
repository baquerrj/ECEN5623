# Question 4 Makefile

ifeq ($(ARCH), aarch64)
CC := aarch64-linux-gnu-gcc
else
CC := gcc
endif

CFLAGS	:= -Wall -g
BINDIR	:= ./bin
OBJDIR 	:= ./obj
PROG 	:= $(BINDIR)/q4

HFILES	:= $(wildcard *.h)
CFILES 	:= $(wildcard *.c)
SRCS   	:= ${HFILES} ${CFILES}
OBJFILES:= $(patsubst %.c, $(OBJDIR)/%.o, $(CFILES))

.PHONY: all 
all: directories $(PROG) 

.PHONY: clean
clean:
	rm -rf $(BINDIR) $(OBJDIR) *.o *.d

.PHONY: directories
directories: $(OBJDIR) $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PROG): $(OBJFILES)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@ -lpthread -pthread
