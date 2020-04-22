LL_NAMES = 	pthread \
				rt

LL_FLAG = $(addprefix -l,$(LL_NAMES))

CF_ALL   = -std=c++0x -O3 -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
LF_ALL   =
LL_ALL   = -Wl,-rpath,./code/lib


INC_ALL = -I./code/inc
CXX             = g++-7
COMP            = $(CXX) -o $@ -c $< $(FLAG_DEFS) $(CF_ALL) $(CF_TGT) $(INC_ALL)
LINK            = $(CXX) -o $@ $^ $(LF_ALL) $(LF_TGT) $(LL_TGT) $(LL_ALL)

include Rules.mk
