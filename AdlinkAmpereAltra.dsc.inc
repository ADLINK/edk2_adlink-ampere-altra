## @file
#
# Copyright (c) 2022, ADLink. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##


################################################################################
#
# Specific Platform Library
#
################################################################################
[LibraryClasses]
  #
  # Pcie Board
  #
  BoardPcieLib|Library/BoardPcieLib/BoardPcieLib.inf

  #
  # Library for POST Code converting
  #
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf

  MmcLib|Library/MmcLib/MmcLib.inf
  NVLib|Library/NVLib/NVLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  #
  # RTC Library: Common RTC
  #
  RealTimeClockLib|Library/PCF8563RealTimeClockLib/PCF8563RealTimeClockLib.inf

[PcdsFixedAtBuild.common]
  # set baudrate to match with MMC
  gArmPlatformTokenSpaceGuid.PcdSerialDbgUartBaudRate|57600

  # Enable POST Code 
  gEfiMdePkgTokenSpaceGuid.PcdPostCodePropertyMask|0x00000008

################################################################################
#
# Specific Platform Component
#
################################################################################
[Components.common]

  #
  # USB 3.0 Renesas μPD70220x
  #
  Drivers/Xhci/RenesasFirmwarePD720202/RenesasFirmwarePD720202.inf {
    <LibraryClasses>
      DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  } 

  Drivers/MmcSetPowerOffType/Dxe/MmcSetPowerOffTypeDxe.inf
  Drivers/MmcSetPowerOffType/Pei/MmcSetPowerOffTypePei.inf

  #
  # POST code thru MMC and utilize Intel POST code map
  #
  MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf {
    <LibraryClasses>
      PostCodeLib|Library/PostCodeLibMmc/PostCodeLibMmc.inf
      PostCodeMapLib|PostCodeDebugFeaturePkg/Library/PostCodeMapLib/PostCodeMapLib.inf
  }

  MdeModulePkg/Universal/StatusCodeHandler/Pei/StatusCodeHandlerPei.inf {
    <LibraryClasses>
      PostCodeLib|Library/PostCodeLibMmc/PostCodeLibMmc.inf
      PostCodeMapLib|PostCodeDebugFeaturePkg/Library/PostCodeMapLib/PostCodeMapLib.inf
  }