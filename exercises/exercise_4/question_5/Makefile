CDEFS		:=
CFLAGS	:= -O0 -pg -g $(CDEFS)
LL_NAMES = 	pthread \
				rt \
				opencv_core \
				opencv_flann \
				opencv_video
LL_FLAG = $(addprefix -l,$(LL_NAMES))

INCLUDE_DIRS := ./inc
LIB_DIRS 	 :=
CC				 := g++

LIB_DIR  := ./lib
BIN_DIR	:= ./bin
OBJ_DIR 	:= ./obj
INC_DIR	:= ./inc
SRC_DIR	:= ./src

$(shell mkdir -p $(BIN_DIR) &> /dev/null)
$(shell mkdir -p $(OBJ_DIR) &> /dev/null)
$(shell mkdir -p $(LIB_DIR) &> /dev/null)

BIN_TGTS	:= $(BIN_DIR)/capture
LIB_TGTS := $(LIB_DIR)/libCapture.so

CPPFILES= $(wildcard $(SRC_DIR)/*.cpp)

CPPOBJS= $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPPFILES))

SRCS	:=	canny.cpp \
			hough.cpp \
			houghElliptical.cpp \
			q5.cpp

SRC_OBJS := $(patsubst %, $(OBJ_DIR)/%.o, $(basename $(SRCS)))

$(BIN_DIR)/capture_OBJECTS := $(SRC_OBJS)
$(LIB_DIR)/libCapture.so_OBJECTS := $(SRC_OBJS)

TGT_BIN := $(TGT_LIB) $(BIN_TGTS)
TGT_LIB := $(TGT_LIB) $(LIB_TGTS)

COMP = 	$(CC) -o $@ -c $< $(CFLAGS) $(CF_TGT)
LINK = 	$(CC) -o $@ $^ $(LF_TGT) $(LL_TGT) `pkg-config --libs opencv` $(LL_ALL)

.PHONY: all
all: $(TGT_BIN) $(TGT_LIB)

.PHONY: clean
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR) $(LIB_DIR)

$(OBJ_DIR)/%.o : CF_TGT := -I$(INCLUDE_DIRS) -fPIC
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(COMP)

$(BIN_TGTS) : LF_TGT := $(LL_FLAG)
$(BIN_TGTS) : $(SRC_OBJS)
	$(LINK)

$(LIB_TGTS) : LF_TGT := -shared
$(LIB_TGTS) : $(SRC_OBJS)
	$(LINK)

test:
	@echo "SRCS = $(SRCS)"
	@echo "SRC_OBJS = $(SRC_OBJS)"
	@echo "BIN_TGTS = $(BIN_TGTS)"
	@echo "LIB_TGTS = $(LIB_TGTS)"