# Standard things
.SUFFIXES :
.SUFFIXES : .cpp .o .so

.PHONY : default
default : targets

dir = defaultScheduler
include $(dir)/defaultScheduler.mk

dir = fifoScheduler
include $(dir)/fifoScheduler.mk

dir = serialized
include $(dir)/serialized.mk


# General directory independent rules
%.o : %.cpp
	$(COMP)

% : %.o
	$(LINK)

.PHONY : targets
targets : $(TGT_BIN) $(TGT_SBIN) $(TGT_ETC) $(TGT_LIB)

.PHONY : clean
clean : clean_fifoScheduler clean_defaultScheduler clean_serialized

.PHONY : clean_obj
clean_obj : clean_obj_clean
