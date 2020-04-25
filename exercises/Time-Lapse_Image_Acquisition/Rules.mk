# Standard things
.SUFFIXES :
.SUFFIXES : .cpp .o .so

.PHONY : default
default : targets

dir := code
include $(dir)/code.mk

BIN := ./bin/
$(shell mkdir -p $(BIN) / &> /dev/null)
# General directory independent rules
%.o : %.cpp
	$(COMP)

% : %.o
	$(LINK)

.PHONY : targets
targets :  $(TGT_LIB) $(TGT_BIN) $(TGT_SBIN) $(TGT_ETC)
	@ln -f -t $(BIN) -s ../code/bin/server
	@ln -f -t $(BIN) -s ../code/bin/client

.PHONY : clean
clean : clean_code
	rm -rf $(BIN)
	rm -f *log *ppm

.PHONY : clean_obj
clean_obj : clean_obj_clean

.PHONY : update
update :
	ssh -tt jetsonnano  "mkdir -p ~/Time-Lapse_Image_Acquisition"
	scp -r code/  jetsonnano:~/Time-Lapse_Image_Acquisition/
	scp Makefile  jetsonnano:~/Time-Lapse_Image_Acquisition/
	scp Rules.mk  jetsonnano:~/Time-Lapse_Image_Acquisition/
