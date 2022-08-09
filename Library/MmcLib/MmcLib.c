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
#include <Library/BaseMemoryLib.h>
#include <Library/MmcLib.h>
#include <Library/NVLib.h>

EFI_STATUS
MmcPostCode(
    IN UINT32 Value)
{
  UINTN NumberOfBytes;
  UINT8 IpmiCmdBuf[] = {"[C0 00 80 11]\r\n"};
  UINTN IpmiCmdBufSize = sizeof(IpmiCmdBuf);

  AsciiSPrint((CHAR8 *)IpmiCmdBuf, sizeof(IpmiCmdBuf), "[C0 00 80 %2X]\r\n", (UINT8)Value);

  NumberOfBytes = PL011UartWrite((UINTN)PcdGet64(PcdSerialDbgRegisterBase), IpmiCmdBuf, IpmiCmdBufSize);

  if (NumberOfBytes == 0)
  {
    DEBUG((DEBUG_ERROR, "%a Failed to Write MMC POST code data\n", __FUNCTION__));
    return EFI_NO_RESPONSE;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
MmcSetPowerOffType(
    IN UINT8 Value)
{
  UINTN numofbytes;
  UINT8 IpmiCmdBuf[] = {"[C0 00 15 01]\r\n"};
  UINTN IpmiCmdBufSize = sizeof(IpmiCmdBuf);

  AsciiSPrint((CHAR8 *)IpmiCmdBuf, sizeof(IpmiCmdBuf), "[C0 00 80 %02X]\r\n", Value);

  DEBUG((DEBUG_INFO, "%a Write MMC Power off type %d\n", __FUNCTION__, Value));
  numofbytes = PL011UartWrite((UINTN)PcdGet64(PcdSerialDbgRegisterBase), IpmiCmdBuf, IpmiCmdBufSize);

  if (numofbytes == 0)
  {
    DEBUG((DEBUG_ERROR, "%a Failed to Write MMC Power off type\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
MmcFirmwareVersion(
    IN UINT8 *Buffer,
    IN UINTN BufferSize)
{
  UINTN NumberOfBytes;
  UINT8 IpmiCmdBuf[] = {"[18 00 01]\r\n"};
  UINTN IpmiCmdBufSize = sizeof(IpmiCmdBuf);
  UINT8 xBuffer[19 * 3 + 5 + 12];
  UINTN xBufferSize = sizeof(xBuffer);

  if (GetFirmwareMajorVersion()==0xA1) {
    DEBUG((DEBUG_INFO, "%a A1 is not supported\n", __FUNCTION__));
    return EFI_UNSUPPORTED;
  }

  NumberOfBytes = PL011UartWrite((UINTN)PcdGet64(PcdSerialDbgRegisterBase), IpmiCmdBuf, IpmiCmdBufSize);

  if (NumberOfBytes == 0)
  {
    DEBUG((DEBUG_ERROR, "%a Failed to Get MMC Version\n", __FUNCTION__));
    return EFI_NO_RESPONSE;
  }

  NumberOfBytes = PL011UartRead((UINTN)PcdGet64(PcdSerialDbgRegisterBase), xBuffer, xBufferSize);

  if (NumberOfBytes == 0)
  {

    DEBUG((DEBUG_ERROR, "%a Failed to Get MMC Version\n", __FUNCTION__));

    return EFI_NO_RESPONSE;
  }

  DEBUG((DEBUG_INFO, "%a [18 00 01]--->", __FUNCTION__));
  for (int i=0; i< xBufferSize; i++) {
    DEBUG((DEBUG_INFO, "%02d:%02x ", i, xBuffer[i]));
  }
  DEBUG((DEBUG_INFO, "\n"));

  DEBUG((DEBUG_INFO, "%a [18 00 01]--->", __FUNCTION__));
  for (int i=0; i< xBufferSize; i++) {
    DEBUG((DEBUG_INFO, "%c", xBuffer[i]));
  }
  DEBUG((DEBUG_INFO, "\n"));

  CopyMem(Buffer, xBuffer + 66, BufferSize - 1);

  Buffer[2] = '.';

  return EFI_SUCCESS;
}