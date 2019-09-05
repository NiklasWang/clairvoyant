#!/bin/sh

BASH=`pwd`
LIBPATH=${BASH}:${BASH}/release:./
export LD_LIBRARY_PATH=${LIBPATH}:${LD_LIBRARY_PATH}

