# Question 4 Makefile

ifeq ($(ARCH), aarch64)
CC := aarch64-linux-gnu-gcc
else
CC := gcc
endif

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

CFLAGS	:= -Wall -g -I./ -I./inc
BINDIR	:= ./bin
OBJDIR 	:= ./obj
INCDIR	:= ./inc
SRCDIR	:= ./src

PROG 	:= $(BINDIR)/$(current_dir)


HFILES	:= $(wildcard $(INCDIR)/*.h)
CFILES 	:= $(wildcard $(SRCDIR)/*.c)
SRCS   	:= ${HFILES} ${CFILES}
OBJFILES:= $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(CFILES))

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

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(PROG): $(OBJFILES)
	$(CC) $(CFLAGS) $^ -o $@ -lm -lrt -pthread
