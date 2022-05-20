#!/bin/bash
OPATH=$PATH
export WORKSPACE=$PWD
. $WORKSPACE/edk2.sh
make -f $WORKSPACE/edk2-ampere-tools/Makefile clean
export PATH=$OPATH
