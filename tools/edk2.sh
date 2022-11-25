#!/bin/bash
export PACKAGES_PATH=$WORKSPACE/edk2-platforms:$WORKSPACE/edk2-platforms/Features/Intel:$WORKSPACE/edk2
export PYTHON_COMMAND=/usr/bin/python3.6
# export CROSS_COMPILE=aarch64-ampere-linux-gnu-
. $WORKSPACE/edk2/edksetup.sh
