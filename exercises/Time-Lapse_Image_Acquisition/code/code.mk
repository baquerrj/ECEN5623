sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


INC_$(d) := $(d)/inc
BIN_$(d) := $(d)/bin
LIB_$(d) := $(d)/lib
DEP_$(d) := $(d)/build
OBJ_$(d) := $(d)/build

SRC_$(d) := $(d)/src/client
include $(d)/client.mk
SRC_$(d) := $(d)/src/server
include $(d)/server.mk
SRC_$(d) := $(d)/src/utils
include $(d)/utils.mk

$(shell mkdir -p $(BIN_$(d)) >/dev/null)
$(shell mkdir -p $(LIB_$(d)) >/dev/null)
$(shell mkdir -p $(OBJ_$(d)) >/dev/null)

BIN_TGTS_$(d) := $(BIN_$(d))/client $(BIN_$(d))/server
#LIB_TGTS_$(d) := $(LIB_$(d))/liblogging.so

$(BIN_$(d))/client_OBJECTS := $(CLIENT_OBJS_$(d)) $(UTILS_OBJS_$(d))
$(BIN_$(d))/server_OBJECTS := $(SERVER_OBJS_$(d)) $(UTILS_OBJS_$(d))
#$(LIB_$(d))/liblogging.so_OBJECTS := $(LOGGING_OBJS_$(d))

TGT_BIN := $(TGT_BIN) $(BIN_TGTS_$(d))
TGT_LIB := $(TGT_LIB) $(LIB_TGTS_$(d))

.SECONDEXPANSION:
#$(BIN_TGTS_$(d)) : LF_TGT := -L$(LIB_$(d)) -llogging $(LL_FLAG)
$(BIN_TGTS_$(d)) : LF_TGT := $(LL_FLAG)
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
