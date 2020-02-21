# Question 4 Makefile

ifeq ($(ARCH), aarch64)
CC := aarch64-linux-gnu-gcc
else
CC := gcc
endif

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

CFLAGS	:= -Wall -g
BINDIR	:= ./bin
OBJDIR 	:= ./obj
PROG 	:= $(BINDIR)/$(current_dir)

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
	$(CC) $(CFLAGS) $^ -o $@ -lm
