INCLUDE_DIRS =
LIB_DIRS =
CC=g++

BINDIR	:= ./bin
OBJDIR 	:= ./obj
INCDIR	:= ./inc
SRCDIR	:= ./src

CDEFS=
CFLAGS= -O0 -pg $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lrt
CPPLIBS= -L/usr/lib -lopencv_core -lopencv_flann -lopencv_video

HFILES=
CFILES=
CPPFILES= $(wildcard $(SRCDIR)/*.cpp)

SRCS= ${HFILES} ${CFILES}
CPPOBJS= $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(CPPFILES))

.PHONY: all
all:	directories $(BINDIR)/capture

.PHONY: clean
clean:
	-rm -rf $(BINDIR) $(OBJDIR)

.PHONY: directories
directories: $(OBJDIR) $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/capture: $(OBJDIR)/capture.o
	$(CC) $(LDFLAGS) $(CFLAGS) $^ -o $@ `pkg-config --libs opencv` $(CPPLIBS)