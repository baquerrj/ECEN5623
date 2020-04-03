LL_NAMES = 	pthread \
				rt \
				opencv_core \
				opencv_flann \
				opencv_video

LL_FLAG = $(addprefix -l,$(LL_NAMES))

CF_ALL   = -std=c++0x -O3 -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
LF_ALL   =
LL_ALL   =

CXX             = g++-7
COMP            = $(CXX) -o $@ -c $< $(FLAG_DEFS) $(CF_ALL) $(CF_TGT)
LINK            = $(CXX) -o $@ $^ $(LF_ALL) $(LF_TGT) $(LL_TGT) $(LL_ALL) `pkg-config --libs opencv`

include Rules.mk
