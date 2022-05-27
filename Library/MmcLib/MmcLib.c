/** @file
  The instance of Post Code Library that layers on top of a Debug Library instance.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/PL011UartLib.h>
#include <Library/MmcLib.h>
#include <Library/BaseMemoryLib.h>

EFI_STATUS
MmcPostCode (
  IN UINT32  Value
  )
{
  UINTN   NumberOfBytes;
  UINT8   IpmiCmdBuf[] = {"[C0 00 80 11]\r\n"};
  UINTN   IpmiCmdBufSize = sizeof(IpmiCmdBuf);

  if (Value==0) return EFI_UNSUPPORTED;

  AsciiSPrint ((CHAR8 *)IpmiCmdBuf, sizeof(IpmiCmdBuf), "[C0 00 80 %2X]\r\n", (UINT8)Value);

  NumberOfBytes = PL011UartWrite ((UINTN)PcdGet64 (PcdSerialDbgRegisterBase), IpmiCmdBuf, IpmiCmdBufSize);

  if (NumberOfBytes == 0) {
    DEBUG ((DEBUG_ERROR, "%a Failed to Write MMC POST code data\n", __FUNCTION__));
    return EFI_NO_RESPONSE;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
MmcFirmwareVersion (
  IN CHAR8 *Buffer,
  IN UINTN BufferSize
  )
{
  UINTN   NumberOfBytes;
  UINT8   IpmiCmdBuf[] = {"[18 00 01]\r\n"};
  UINTN   IpmiCmdBufSize = sizeof(IpmiCmdBuf);
  UINT8   xBuffer[19*3+5+12];
  UINTN   xBufferSize=sizeof(xBuffer);

  NumberOfBytes = PL011UartWrite ((UINTN)PcdGet64 (PcdSerialDbgRegisterBase), IpmiCmdBuf, IpmiCmdBufSize);

  if (NumberOfBytes == 0) {
    DEBUG ((DEBUG_ERROR, "%a Failed to Get MMC Version\n", __FUNCTION__));
    return EFI_NO_RESPONSE;
  }

  NumberOfBytes = PL011UartRead ((UINTN)PcdGet64 (PcdSerialDbgRegisterBase), xBuffer, xBufferSize);

  if (NumberOfBytes == 0) {
    DEBUG ((DEBUG_ERROR, "%a Failed to Get MMC Version\n", __FUNCTION__));
    return EFI_NO_RESPONSE;
  }

  CopyMem (Buffer, xBuffer+34, BufferSize-1);
  Buffer[2]='.';

  return EFI_SUCCESS;
}
