#!/bin/bash
#
# Arg1=board name
# Arg2=board stepping
# example to make AvaAp1 board A1 stepping: 
#   . make_adlink AvaAp1 A1
#
OPATH=$PATH
OEM_PLATFORM_DIR=$WORKSPACE/adlink-platforms
OEM_CHIP_DIR=$WORKSPACE/edk2_adlink-ampere-altra
OEM_CHIPTOOL_DIR=$OEM_CHIP_DIR/tools/
. $OEM_CHIPTOOL_DIR/edk2.sh
PATH=$OEM_CHIPTOOL_DIR:$PATH
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
VER=2.04
fi
if [ -z ${BUILD} ]; then
BUILD=100.06
fi
if  [ "${BOARD_STEPPING}" == "A1" ]; then
    BUILD=$BUILD.A1
    FAILSAFE_WORKAROUND=1
    BOARD_SETTING=$OEM_PLATFORM_DIR/Platform/Ampere/"$BOARD_NAME"Pkg/"$BOARD_NAME"BoardSettingVRWA.cfg
else   
    FAILSAFE_WORKAROUND=0
    BOARD_SETTING=$OEM_PLATFORM_DIR/Platform/Ampere/"$BOARD_NAME"Pkg/"$BOARD_NAME"BoardSetting.cfg
fi    
if  [ "${DEVELMENT_MODE}" == "1" ]; then
    make -f $WORKSPACE/edk2-ampere-tools/Makefile \
        PROGRAMMER_TOOL=#$OEM_CHIPTOOL_DIR/dpcmd \
        POWER_SCRIPT=$OEM_CHIPTOOL_DIR/target_power.sh \
        EDK2_PLATFORMS_PKG_DIR=$EDK2_PLATFORMS_PKG_DIR \
        BOARD_NAME=$BOARD_NAME \
        VM_SHARED_DIR=$HOME/AmpereR \
        CHECKSUM_TOOL=$OEM_CHIPTOOL_DIR/checksum \
        PACKAGES_PATH=$OEM_PLATFORM_DIR:$OEM_CHIP_DIR:$WORKSPACE/edk2-platforms/Features/Intel/Debugging:$WORKSPACE/OpenPlatformPkg:"${PACKAGES_PATH}" \
        ATF_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/atf/altra_atf_signed_2.06.20220308.slim \
        SCP_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/scp/altra_scp_signed_2.06.20220308.slim \
        FAILSAFE_WORKAROUND=$FAILSAFE_WORKAROUND \
        BOARD_SETTING=$BOARD_SETTING \
        LINUXBOOT_BIN=$OEM_CHIPTOOL_DIR/flashkernel \
        DEBUG=0 \
        VER=$VER BUILD=$BUILD \
        tianocore_img # linuxboot_img # all # tianocore_capsule # 
else
    make -f $WORKSPACE/edk2-ampere-tools/Makefile \
        POWER_SCRIPT=$OEM_CHIPTOOL_DIR/target_power.sh \
        EDK2_PLATFORMS_PKG_DIR=$EDK2_PLATFORMS_PKG_DIR \
        BOARD_NAME=$BOARD_NAME \
        VM_SHARED_DIR=$HOME/AmpereR \
        CHECKSUM_TOOL=./edk2_adlink-ampere-altra/tools/checksum \
        PACKAGES_PATH=$OEM_PLATFORM_DIR:$OEM_CHIP_DIR:$WORKSPACE/edk2-platforms/Features/Intel/Debugging:$WORKSPACE/OpenPlatformPkg:"${PACKAGES_PATH}" \
        ATF_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/atf/altra_atf_signed_2.06.20220308.slim \
        SCP_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/scp/altra_scp_signed_2.06.20220308.slim \
        FAILSAFE_WORKAROUND=$FAILSAFE_WORKAROUND \
        BOARD_SETTING=$BOARD_SETTING \
        LINUXBOOT_BIN=$OEM_CHIPTOOL_DIR/flashkernel \
        SPI_SIZE_MB=32 \
        DEBUG=0 \
        VER=$VER BUILD=$BUILD \
        tianocore_capsule
    if [ $? -eq 0 ]; then
        make -f $WORKSPACE/edk2-ampere-tools/Makefile \
            EDK2_PLATFORMS_PKG_DIR=$EDK2_PLATFORMS_PKG_DIR \
            BOARD_NAME=$BOARD_NAME \
            VM_SHARED_DIR=$HOME/AmpereR \
            CHECKSUM_TOOL=./edk2_adlink-ampere-altra/tools/checksum \
            ATF_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/atf/altra_atf_signed_2.06.20220308.slim \
            SPI_SIZE_MB=32 \
            VER=$VER BUILD=$BUILD \
            history
    fi
fi
export PATH=$OPATH
