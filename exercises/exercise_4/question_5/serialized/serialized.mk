sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


INC_$(d) := $(d)/inc
BIN_$(d) := $(d)/bin
LIB_$(d) := $(d)/lib
DEP_$(d) := $(d)/build
OBJ_$(d) := $(d)/build

SRC_$(d) := $(d)/src
SRCS_$(d) :=	canny.cpp \
					hough.cpp \
					houghElliptical.cpp \
					q5.cpp \
					logging.cpp

SRC_OBJS_$(d) := $(patsubst %,$(OBJ_$(d))/%.o,$(basename $(SRCS_$(d))))

$(OBJ_$(d))/%.o : CF_TGT := -fPIC -I$(INC_$(d))
$(OBJ_$(d))/%.o : DEPDIR := $(DEP_$(d))
$(OBJ_$(d))/%.o : $(SRC_$(d))/%.cpp
	$(COMP)
	@mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

include $(wildcard $(patsubst %,$(DEP_$(d))/%.d,$(basename $(SRCS_$(d)))))


$(shell mkdir -p $(BIN_$(d)) >/dev/null)
$(shell mkdir -p $(LIB_$(d)) >/dev/null)
$(shell mkdir -p $(OBJ_$(d)) >/dev/null)

BIN_TGTS_$(d) := $(BIN_$(d))/$(d)
LIB_TGTS_$(d) := $(LIB_$(d))/lib$(d).so

$(BIN_$(d))/$(d)_OBJECTS := $(SRC_OBJS_$(d))
$(LIB_$(d))/lib$(d).so_OBJECTS := $(SRC_OBJS_$(d))

TGT_BIN := $(TGT_BIN) $(BIN_TGTS_$(d))
TGT_LIB := $(TGT_LIB) $(LIB_TGTS_$(d))

.SECONDEXPANSION:
$(BIN_TGTS_$(d)) : LF_TGT := $(LL_FLAG)
$(BIN_TGTS_$(d)) : FLAG_DEFS = -DSHOW_WINDOWS
$(BIN_TGTS_$(d)) : $$($$@_OBJECTS)
	@echo "BIN_TGTS_$(d) = $(BIN_TGTS_$(d))"
	$(LINK)

.SECONDEXPANSION:
$(LIB_TGTS_$(d)) : LF_TGT := -shared $(LL_FLAG)
$(LIB_TGTS_$(d)) : $$($$@_OBJECTS)
	$(LINK)

.PHONY: clean_$(d)
clean_$(d) : CLEAN := $(BIN_$(d))/ $(LIB_$(d))/ $(OBJ_$(d))/
clean_$(d) :
	rm -rf $(CLEAN)

.PHONY: clean_obj_$(d)
clean_obj_$(d) : CLEAN := $(BIN_$(d))/* $(LIB_$(d))/*.so $(OBJ_$(d))/*.o
clean_obj_$(d) :
	rm -rf $(CLEAN)

$(DEP_$(d))/%.d: ;
.PRECIOUS: $(DEP_$(d))/%.d
.PRECIOUS: $(OBJ_$(d))/%.o

d  := $(dirstack_$(sp))
sp := $(basename $(sp))
