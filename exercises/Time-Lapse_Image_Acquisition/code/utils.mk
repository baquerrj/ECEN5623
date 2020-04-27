SRCS_$(d) :=	logging.cpp \
					thread.cpp \
					thread_utils.cpp \
					SocketBase.cpp \
					SocketServer.cpp \
					SocketClient.cpp \
					V4l2.cpp \
					RingBuffer.cpp

UTILS_OBJS_$(d) := $(patsubst %,$(OBJ_$(d))/%.o,$(basename $(SRCS_$(d))))

$(OBJ_$(d))/%.o : CF_TGT := -fPIC -I$(INC_$(d))
$(OBJ_$(d))/%.o : DEPDIR := $(DEP_$(d))
$(OBJ_$(d))/%.o : $(SRC_$(d))/%.cpp
	$(COMP)
	@mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

include $(wildcard $(patsubst %,$(DEP_$(d))/%.d,$(basename $(SRCS_$(d)))))