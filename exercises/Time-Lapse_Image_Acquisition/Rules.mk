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
clean : clean_code clean_docs
	rm -rf $(BIN)
	rm -f *log *ppm

.PHONY : clean_obj
clean_obj : clean_obj_clean

.PHONY : remote
remote :
	zip -r remote.zip *
	ssh -tt jetsonnano  "mkdir -p ~/Time-Lapse_Image_Acquisition"
	scp remote.zip jetsonnano:~/
	ssh -tt jetsonnano "unzip remote.zip -d remote/"

.PHONY : docs
docs :
	/usr/bin/doxygen docs/Doxyfile

.PHONY : clean_docs
clean_docs :
	rm -rf docs/html docs/latex