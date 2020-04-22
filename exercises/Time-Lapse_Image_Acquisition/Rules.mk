# Standard things
.SUFFIXES :
.SUFFIXES : .cpp .o .so

.PHONY : default
default : targets

dir := code
include $(dir)/code.mk

# General directory independent rules
%.o : %.cpp
	$(COMP)

% : %.o
	$(LINK)

.PHONY : targets
targets :  $(TGT_LIB) $(TGT_BIN) $(TGT_SBIN) $(TGT_ETC)
	ln -sf code/bin ./bin

.PHONY : clean
clean : clean_code
	rm ./bin

.PHONY : clean_obj
clean_obj : clean_obj_clean

.PHONY : update
update :
	ssh -tt jetsonnano  "mkdir -p ~/Time-Lapse_Image_Acquisition"
	scp -r *  jetsonnano:~/Time-Lapse_Image_Acquisition/
