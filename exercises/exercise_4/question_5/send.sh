#!/bin/bash

SRC=`realpath $1`
DST=${SRC}

COMMAND="scp ${SRC}/src/* jetsonnano:${DST}/src/"
eval $COMMAND

COMMAND="scp ${SRC}/inc/* jetsonnano:${DST}/inc/."
eval $COMMAND

COMMAND="scp ${SRC}/Makefile jetsonnano:${DST}/Makefile"
eval $COMMAND
