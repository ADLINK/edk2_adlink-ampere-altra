/** @file
  MMC Library.

  Copyright (c) 2022, ADLink. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/PL011UartLib.h>
#include <Library/MmcLib.h>

EFI_STATUS
MmcPostCode (
  IN UINT32  Value
  )
{
  UINTN   numofbytes;
  UINT8   IpmiCmdBuf[] = {"[C0 00 80 11]\r\n"};
  UINTN   IpmiCmdBufSize = sizeof(IpmiCmdBuf);

  AsciiSPrint ((CHAR8 *)IpmiCmdBuf, sizeof(IpmiCmdBuf), "[C0 00 80 %2X]\r\n", (UINT8)Value);

  numofbytes = PL011UartWrite ((UINTN)PcdGet64 (PcdSerialDbgRegisterBase), IpmiCmdBuf, IpmiCmdBufSize);

  if (numofbytes == 0) {
    DEBUG ((DEBUG_ERROR, "%a Failed to Write MMC POST code data\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
MmcSetPowerOffType (
  IN UINT8  Value
  )
{
  UINTN   numofbytes;
  UINT8   IpmiCmdBuf[] = {"[C0 00 15 01]\r\n"};
  UINTN   IpmiCmdBufSize = sizeof(IpmiCmdBuf);

  AsciiSPrint ((CHAR8 *)IpmiCmdBuf, sizeof(IpmiCmdBuf), "[C0 00 80 %02X]\r\n", Value);

  DEBUG ((DEBUG_INFO, "%a Write MMC Power off type %d\n", __FUNCTION__, Value));
  numofbytes = PL011UartWrite ((UINTN)PcdGet64 (PcdSerialDbgRegisterBase), IpmiCmdBuf, IpmiCmdBufSize);

  if (numofbytes == 0) {
    DEBUG ((DEBUG_ERROR, "%a Failed to Write MMC Power off type\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

