#!/bin/bash
#
# Arg1=board name
# Arg2=board stepping
# example to make AvaAp1 board A1 stepping: 
#   . make_adlink AvaAp1 A1
#
OPATH=$PATH
OEM_PLATFORM_DIR=$WORKSPACE/adlink-platforms
OEM_COMMON_DIR=$WORKSPACE/edk2_adlink-ampere-altra
. $OEM_COMMON_DIR/tools/edk2.sh
PATH=$OEM_COMMON_DIR/tools:$PATH
#
# BOARD info from arguments
#
BOARD_NAME=$1
BOARD_STEPPING=$2

EDK2_PLATFORMS_PKG_DIR=$OEM_PLATFORM_DIR/Platform/Ampere/"$BOARD_NAME"Pkg

#
# Firmware Version
#
if [ -z ${VER} ]; then
VER=2.08
fi
if [ -z ${BUILD} ]; then
BUILD=100.00
fi
if [ -z ${DEVEL_MODE} ]; then
DEVEL_MODE=0
fi
if [ -z ${DEBUG} ]; then
DEBUG=0
fi
if [ -z ${ATF_SCP_VER} ]; then
    ATF_SCP_VER=2.10.20221028
fi
ATF_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/atf/altra_atf_signed_$ATF_SCP_VER.slim
SCP_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/scp/altra_scp_signed_$ATF_SCP_VER.slim
if  [ "${BOARD_STEPPING}" == "A1" ]; then
    BUILD=$BUILD.A1
    FAILSAFE_WORKAROUND=1
    BOARD_SETTING=$OEM_PLATFORM_DIR/Platform/Ampere/"$BOARD_NAME"Pkg/"$BOARD_NAME"BoardSettingVRWA.cfg
else   
    FAILSAFE_WORKAROUND=0
    BOARD_SETTING=$OEM_PLATFORM_DIR/Platform/Ampere/"$BOARD_NAME"Pkg/"$BOARD_NAME"BoardSetting.cfg
    git log --pretty=format:"%s" $(git describe --tags --abbrev=0 @^)..@ > $EDK2_PLATFORMS_PKG_DIR/taglog.txt
fi

if  [ "${DEVEL_MODE}" == "1" ]; then
    make -f $WORKSPACE/edk2-ampere-tools/Makefile \
        DEVEL_MODE=TRUE \
        OEM_COMMON_DIR=$OEM_COMMON_DIR \
        EDK2_PLATFORMS_PKG_DIR=$EDK2_PLATFORMS_PKG_DIR \
        BOARD_NAME=$BOARD_NAME \
        RELEASE_DIR=$HOME/AmpereR \
        PACKAGES_PATH=$OEM_PLATFORM_DIR:$OEM_COMMON_DIR:$WORKSPACE/edk2-platforms/Features/Intel/Debugging:$WORKSPACE/OpenPlatformPkg:"${PACKAGES_PATH}" \
        ATF_SLIM=$ATF_SLIM \
        SCP_SLIM=$SCP_SLIM \
        FAILSAFE_WORKAROUND=$FAILSAFE_WORKAROUND \
        BOARD_SETTING=$BOARD_SETTING \
        DEBUG=$DEBUG \
        VER=$VER BUILD=$BUILD \
        CROSS_COMPILE=aarch64-linux-gnu- \
        tianocore_img # linuxboot_img # all # tianocore_capsule # 
else
    make -f $WORKSPACE/edk2-ampere-tools/Makefile \
        OEM_COMMON_DIR=$OEM_COMMON_DIR \
        EDK2_PLATFORMS_PKG_DIR=$EDK2_PLATFORMS_PKG_DIR \
        BOARD_NAME=$BOARD_NAME \
        RELEASE_DIR=$HOME/AmpereR \
        PACKAGES_PATH=$OEM_PLATFORM_DIR:$OEM_COMMON_DIR:$WORKSPACE/edk2-platforms/Features/Intel/Debugging:$WORKSPACE/OpenPlatformPkg:"${PACKAGES_PATH}" \
        ATF_SLIM=$ATF_SLIM \
        SCP_SLIM=$SCP_SLIM \
        FAILSAFE_WORKAROUND=$FAILSAFE_WORKAROUND \
        BOARD_SETTING=$BOARD_SETTING \
        DEBUG=$DEBUG \
        VER=$VER BUILD=$BUILD \
        CROSS_COMPILE=aarch64-linux-gnu- \
        CHECKSUM_TOOL=$OEM_COMMON_DIR/tools/checksum \
        SPI_SIZE_MB=32 \
        tianocore_capsule
    if [ $? -eq 0 ]; then
        make -f $WORKSPACE/edk2-ampere-tools/Makefile \
            OEM_COMMON_DIR=$OEM_COMMON_DIR \
            EDK2_PLATFORMS_PKG_DIR=$EDK2_PLATFORMS_PKG_DIR \
            BOARD_NAME=$BOARD_NAME \
            RELEASE_DIR=$HOME/AmpereR \
            CHECKSUM_TOOL=$OEM_COMMON_DIR/tools/checksum \
            ATF_SLIM=$ATF_SLIM \
            SPI_SIZE_MB=32 \
            DEBUG=$DEBUG \
            VER=$VER BUILD=$BUILD \
            Release
    fi
fi
export PATH=$OPATH
