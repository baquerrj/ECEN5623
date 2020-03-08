#------------------------------------------------------------------------------
# Makefile
# @author	: Puneet Bansal
# @date		: 2nd March 2020 
#------------------------------------------------------------------------------


all: heap_mq1 posix_mq

posix_mq: posix_mq.c
	gcc posix_mq.c -o posix_mq -lpthread -lrt

heap_mq1: heap_mq1.c
	gcc heap_mq1.c -o heap_mq1 -lpthread -lrt
clean:
	rm -f heap_mq1 heap_mqA posix_mq
