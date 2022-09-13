/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>

#include <Guid/PlatformInfoHob.h>
#include <Guid/SmBios.h>
#include <Library/AmpereCpuLib.h>
#include <Library/ArmLib.h>
#include <Library/ArmLib/ArmLibPrivate.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Smbios.h>

#define CPU_CACHE_LEVEL_COUNT 3

#define MHZ_SCALE_FACTOR 1000000

#define CACHE_SIZE(x, y)            (UINT16) (0x8000 | ((x >> 16) * GetNumberOfActiveCoresPerSocket (y)))
#define CACHE_SIZE_2(x, y)          (0x80000000 | ((x >> 16) * GetNumberOfActiveCoresPerSocket (y)))
#define PROCESSOR_VERSION_ALTRA     "Ampere(R) Altra(R) Processor\0"
#define PROCESSOR_VERSION_ALTRA_MAX "Ampere(R) Altra(R) Max Processor\0"

#define TYPE4_ADDITIONAL_STRINGS                                       \
  "SOCKET 0\0"                            /* socket type */            \
  "Ampere(R)\0"                           /* manufacturer */           \
  PROCESSOR_VERSION_ALTRA_MAX             /* processor description */  \
  "NotSet\0"                              /* part number */            \
  "Not Specified                     \0"  /* processor serial number */

#define TYPE7_ADDITIONAL_STRINGS                                       \
  "L1 Instruction Cache\0" /* L1 Instruction Cache  */

//
// Type definition and contents of the default SMBIOS table.
// This table covers only the minimum structures required by
// the SMBIOS specification (section 6.2, version 3.0)
//
#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE4 Base;
  CHAR8              Strings[sizeof (TYPE4_ADDITIONAL_STRINGS)];
} ARM_TYPE4;

typedef struct {
  SMBIOS_TABLE_TYPE7 Base;
  CHAR8              Strings[sizeof (TYPE7_ADDITIONAL_STRINGS)];
} ARM_TYPE7;

#pragma pack()
//-------------------------------------
//        SMBIOS Platform Common
//-------------------------------------
enum {
  ADDITIONAL_STR_INDEX_1 = 1,
  ADDITIONAL_STR_INDEX_2,
  ADDITIONAL_STR_INDEX_3,
  ADDITIONAL_STR_INDEX_4,
  ADDITIONAL_STR_INDEX_5,
  ADDITIONAL_STR_INDEX_6,
  ADDITIONAL_STR_INDEX_7,
  ADDITIONAL_STR_INDEX_8,
  ADDITIONAL_STR_INDEX_9,
  ADDITIONAL_STR_INDEX_MAX
};

// Type 4 Processor Socket 0
STATIC ARM_TYPE4 mArmDefaultType4Sk0 = {
  {
    {                                        // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE4),           // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,          // socket type
    3,                               // processor type CPU
    ProcessorFamilyIndicatorFamily2, // processor family, acquire from field2
    ADDITIONAL_STR_INDEX_2,          // manufacturer
    {{0,},{0.}},                     // processor id
    ADDITIONAL_STR_INDEX_3,          // version
    {0,0,0,0,0,1},                   // voltage
    0,                               // external clock
    3000,                            // max speed
    3000,                            // current speed
    0x41,                            // status
    ProcessorUpgradeOther,
    0xFFFF,                 // l1 cache handle
    0xFFFF,                 // l2 cache handle
    0xFFFF,                 // l3 cache handle
    ADDITIONAL_STR_INDEX_5, // serial not set
    0,                      // asset not set
    ADDITIONAL_STR_INDEX_4, // part number
    80,                     // core count in socket
    80,                     // enabled core count in socket
    0,                      // threads per socket
    0xEC,                   // processor characteristics
    ProcessorFamilyARMv8,   // ARM core
  },
  TYPE4_ADDITIONAL_STRINGS
};

// Type 4 Processor Socket 1
STATIC ARM_TYPE4 mArmDefaultType4Sk1 = {
  {
    {                                        // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE4),           // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,          // socket type
    3,                               // processor type CPU
    ProcessorFamilyIndicatorFamily2, // processor family, acquire from field2
    ADDITIONAL_STR_INDEX_2,          // manufacturer
    {{0,},{0.}},                     // processor id
    ADDITIONAL_STR_INDEX_3,          // version
    {0,0,0,0,0,1},                   // voltage
    0,                               // external clock
    3000,                            // max speed
    3000,                            // current speed
    0x41,                            // status
    ProcessorUpgradeOther,
    0xFFFF,                 // l1 cache handle
    0xFFFF,                 // l2 cache handle
    0xFFFF,                 // l3 cache handle
    ADDITIONAL_STR_INDEX_5, // serial not set
    0,                      // asset not set
    ADDITIONAL_STR_INDEX_4, // part number
    80,                     // core count in socket
    80,                     // enabled core count in socket
    0,                      // threads per socket
    0xEC,                   // processor characteristics
    ProcessorFamilyARMv8,   // ARM core
  },
  TYPE4_ADDITIONAL_STRINGS
};

// Type 7 Cache Information
STATIC ARM_TYPE7 mArmDefaultType7Sk0L1I = {
  {
    {                                    // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_CACHE_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE7),       // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    0x180,                  // L1 enabled, Write Back
    0x8001,                 // 64k i cache max
    0x8001,                 // 64k installed
    {0, 0, 0, 0, 0, 1},     // SRAM type
    {0, 0, 0, 0, 0, 1},     // SRAM type
    0,                      // unkown speed
    CacheErrorParity,       // parity checking
    CacheTypeInstruction,   // Instruction cache
    CacheAssociativity4Way, // 4-way
  },
  "L1 Instruction Cache\0"
};

// Type 7 Cache Information
STATIC ARM_TYPE7 mArmDefaultType7Sk0L1D = {
  {
    {                                    // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_CACHE_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE7),       // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    0x180,                  // L1 enabled, Write Back
    0x8001,                 // 64k i cache max
    0x8001,                 // 64k installed
    {0, 0, 0, 0, 0, 1},     // SRAM type
    {0, 0, 0, 0, 0, 1},     // SRAM type
    0,                      // unkown speed
    CacheErrorParity,       // parity checking
    CacheTypeData,          // Data cache
    CacheAssociativity4Way, // 4-way
  },
  "L1 Data Cache\0"
};
// Type 7 Cache Information
STATIC ARM_TYPE7 mArmDefaultType7Sk0L2 = {
  {
    {                                    // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_CACHE_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE7),       // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    0x181,                  // L2 enabled, Write Back
    0x8010,                 // 1M cache max
    0x8010,                 // 1M installed
    {0, 0, 0, 0, 0, 1},     // SRAM type
    {0, 0, 0, 0, 0, 1},     // SRAM type
    0,                      // unkown speed
    CacheErrorSingleBit,    // single bit ECC
    CacheTypeUnified,       // Unified cache
    CacheAssociativity8Way, // 8-way
  },
  "L2 Cache\0"
};

// Type 7 Cache Information
STATIC ARM_TYPE7 mArmDefaultType7Sk1L1I = {
  {
    {                                    // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_CACHE_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE7),       // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    0x180,                  // L1 enabled, Write Back
    0x8001,                 // 64k i cache max
    0x8001,                 // 64k installed
    {0, 0, 0, 0, 0, 1},     // SRAM type
    {0, 0, 0, 0, 0, 1},     // SRAM type
    0,                      // unkown speed
    CacheErrorParity,       // parity checking
    CacheTypeInstruction,   // Instruction cache
    CacheAssociativity4Way, // 4-way
  },
  "L1 Instruction Cache\0"
};

// Type 7 Cache Information
STATIC ARM_TYPE7 mArmDefaultType7Sk1L1D = {
  {
    {                                    // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_CACHE_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE7),       // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    0x180,                  // L1 enabled, Write Back
    0x8001,                 // 64k i cache max
    0x8001,                 // 64k installed
    {0, 0, 0, 0, 0, 1},     // SRAM type
    {0, 0, 0, 0, 0, 1},     // SRAM type
    0,                      // unkown speed
    CacheErrorParity,       // parity checking
    CacheTypeData,          // Data cache
    CacheAssociativity4Way, // 4-way
  },
  "L1 Data Cache\0"
};
// Type 7 Cache Information
STATIC ARM_TYPE7 mArmDefaultType7Sk1L2 = {
  {
    {                                    // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_CACHE_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE7),       // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    0x181,                  // L2 enabled, Write Back
    0x8010,                 // 1M cache max
    0x8010,                 // 1M installed
    {0, 0, 0, 0, 0, 1},     // SRAM type
    {0, 0, 0, 0, 0, 1},     // SRAM type
    0,                      // unkown speed
    CacheErrorSingleBit,    // single bit ECC
    CacheTypeUnified,       // Unified cache
    CacheAssociativity8Way, // 8-way
  },
  "L2 Cache\0"
};

STATIC CONST VOID *DefaultCommonTables[] =
{
  &mArmDefaultType4Sk0,
  &mArmDefaultType4Sk1,
  NULL
};

STATIC CONST VOID *DefaultType7Sk0Tables[] =
{
  &mArmDefaultType7Sk0L1I,
  &mArmDefaultType7Sk0L1D,
  &mArmDefaultType7Sk0L2,
  NULL
};

STATIC CONST VOID *DefaultType7Sk1Tables[] =
{
  &mArmDefaultType7Sk1L1I,
  &mArmDefaultType7Sk1L1D,
  &mArmDefaultType7Sk1L2,
  NULL
};

STATIC
UINT32
GetCacheConfig (
  UINT32 Level
  )
{
  UINT64  Val;
  BOOLEAN SupportWB;
  BOOLEAN SupportWT;

  Val = ReadCCSIDR (Level);
  SupportWT = (Val & (1U << 31)) ? TRUE : FALSE;
  SupportWB = (Val & (1U << 30)) ? TRUE : FALSE;
  if (SupportWT && SupportWB) {
    return 2; /* Varies with Memory Address */
  }

  if (SupportWT) {
    return 0;
  }

  return 1; /* WB */
}

STATIC
UINTN
GetStringPackSize (
  CHAR8 *StringPack
  )
{
  UINTN StrCount;
  CHAR8 *StrStart;

  if ((*StringPack == 0) && (*(StringPack + 1) == 0)) {
    return 0;
  }

  // String section ends in double-null (0000h)
  for (StrCount = 0, StrStart = StringPack;
       ((*StrStart != 0) || (*(StrStart + 1) != 0)); StrStart++, StrCount++)
  {
  }

  return StrCount + 2; // Included the double NULL
}

// Update String at String number to String Pack
EFI_STATUS
UpdateStringPack (
  CHAR8 *StringPack,
  CHAR8 *String,
  UINTN StringNumber
  )
{
  CHAR8 *StrStart;
  UINTN StrIndex;
  UINTN InputStrLen;
  UINTN TargetStrLen;
  UINTN BufferSize;
  CHAR8 *Buffer;

  StrStart = StringPack;
  for (StrIndex = 1; StrIndex < StringNumber; StrStart++) {
    // A string ends in 00h
    if (*StrStart == 0) {
      StrIndex++;
    }
    // String section ends in double-null (0000h)
    if ((*StrStart == 0) && (*(StrStart + 1) == 0)) {
      return EFI_NOT_FOUND;
    }
  }

  if (*StrStart == 0) {
    StrStart++;
  }

  InputStrLen = AsciiStrLen (String);
  TargetStrLen = AsciiStrLen (StrStart);
  BufferSize = GetStringPackSize (StrStart + TargetStrLen + 1);

  // Replace the String if length matched
  // OR this is the last string
  if (InputStrLen == TargetStrLen || (BufferSize == 0)) {
    CopyMem (StrStart, String, InputStrLen);
  }
  // Otherwise, buffer is needed to apply new string
  else {
    Buffer = AllocateZeroPool (BufferSize);
    if (Buffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    CopyMem (Buffer, StrStart + TargetStrLen + 1, BufferSize);
    CopyMem (StrStart, String, InputStrLen + 1);
    CopyMem (StrStart + InputStrLen + 1, Buffer, BufferSize);

    FreePool (Buffer);
  }

  return EFI_SUCCESS;
}

STATIC
VOID
UpdateSmbiosType4 (
  PLATFORM_INFO_HOB  *PlatformHob
  )
{
  UINTN              Index;
  CHAR8              Str[40];
  CHAR8              *StringPack;
  SMBIOS_TABLE_TYPE4 *Table;
  CHAR8              *VersionString;

  ASSERT (PlatformHob != NULL);

  for (Index = 0; Index < GetNumberOfSupportedSockets (); Index++) {
    if (Index == 0) {
      Table = &mArmDefaultType4Sk0.Base;
      StringPack = mArmDefaultType4Sk0.Strings;
    } else {
      Table = &mArmDefaultType4Sk1.Base;
      StringPack = mArmDefaultType4Sk1.Strings;
    }

    AsciiSPrint (Str, sizeof (Str), "CPU %d", Index);
    UpdateStringPack (StringPack, Str, ADDITIONAL_STR_INDEX_1);

    if (IsAc01Processor ()) {
      VersionString = PROCESSOR_VERSION_ALTRA;
    } else {
      VersionString = PROCESSOR_VERSION_ALTRA_MAX;
    }

    UpdateStringPack (StringPack, VersionString, ADDITIONAL_STR_INDEX_3);

    Table->CoreCount = (UINT8)GetMaximumNumberOfCores ();
    Table->ThreadCount = (UINT8)GetMaximumNumberOfCores ();
    Table->EnabledCoreCount = (UINT8)GetNumberOfActiveCoresPerSocket (Index);

    if (Table->EnabledCoreCount) {
      Table->CurrentSpeed = (UINT16)(PlatformHob->CpuClk / MHZ_SCALE_FACTOR);
      Table->ExternalClock = (UINT16)(PlatformHob->PcpClk / MHZ_SCALE_FACTOR);
      Table->MaxSpeed = (UINT16)(PlatformHob->CpuClk / MHZ_SCALE_FACTOR);
      if (PlatformHob->TurboCapability[Index]) {
        Table->MaxSpeed = (UINT16)(PlatformHob->TurboFrequency[Index]);
      }
    } else {
      Table->CurrentSpeed = 0;
      Table->ExternalClock = 0;
      Table->MaxSpeed = 0;
      Table->Status = 0;
    }

    *((UINT32 *)&Table->ProcessorId) = (UINT32)ArmReadMidr ();
    *((UINT32 *)&Table->ProcessorId + 1) = 0;
    *((UINT8 *)&Table->Voltage) = 0x80 | PlatformHob->CoreVoltage[Index] / 100;

    /* Type 4 Part number and processor serial number */
    if (Table->EnabledCoreCount) {
      if ((PlatformHob->ScuProductId[Index] & 0xff) == 0x01) {
        AsciiSPrint (
          Str,
          sizeof (Str),
          "Q%02d-%02X",
          PlatformHob->SkuMaxCore[Index],
          PlatformHob->SkuMaxTurbo[Index]
          );
      } else {
        AsciiSPrint (
          Str,
          sizeof (Str),
          "M%02d-%02X",
          PlatformHob->SkuMaxCore[Index],
          PlatformHob->SkuMaxTurbo[Index]
          );
      }

      UpdateStringPack (StringPack, Str, ADDITIONAL_STR_INDEX_4);

      AsciiSPrint (
        Str,
        sizeof (Str),
        "%08X%08X%08X%08X",
        PlatformHob->Ecid[Index][0],
        PlatformHob->Ecid[Index][1],
        PlatformHob->Ecid[Index][2],
        PlatformHob->Ecid[Index][3]
        );

      UpdateStringPack (StringPack, Str, ADDITIONAL_STR_INDEX_5);
    }
  }
}

STATIC
VOID
UpdateCacheInfo (
  SMBIOS_TABLE_TYPE7 *Table,
  UINT32              Level,
  UINT32              Socket
  )
{
  ASSERT (Table != NULL);
  ASSERT (Level > 0 && Level < 8);
  ASSERT (Socket < 2);

  Table->Associativity = (UINT8)CpuGetAssociativity (Level);
  Table->CacheConfiguration = (1 << 7 | GetCacheConfig (Level) << 8 | (Level - 1));
  Table->MaximumCacheSize  = CACHE_SIZE (CpuGetCacheSize (Level), Socket);
  Table->InstalledSize     = CACHE_SIZE (CpuGetCacheSize (Level), Socket);
  Table->MaximumCacheSize2 = CACHE_SIZE_2 (CpuGetCacheSize (Level), Socket);
  Table->InstalledSize2    = CACHE_SIZE_2 (CpuGetCacheSize (Level), Socket);
}

STATIC
VOID
UpdateSmbiosType7 (
  PLATFORM_INFO_HOB  *PlatformHob
  )
{
  SMBIOS_TABLE_TYPE7 *Table;

  ASSERT (PlatformHob != NULL);

  Table = &mArmDefaultType7Sk0L1I.Base;
  UpdateCacheInfo (Table, 1, 0);
  Table = &mArmDefaultType7Sk0L1D.Base;
  UpdateCacheInfo (Table, 1, 0);
  Table = &mArmDefaultType7Sk0L2.Base;
  UpdateCacheInfo (Table, 2, 0);
  if (IsSlaveSocketActive ()) {
    Table = &mArmDefaultType7Sk1L1I.Base;
    UpdateCacheInfo (Table, 1, 1);
    Table = &mArmDefaultType7Sk1L1D.Base;
    UpdateCacheInfo (Table, 1, 1);
    Table = &mArmDefaultType7Sk1L2.Base;
    UpdateCacheInfo (Table, 2, 1);
  }
}

STATIC
VOID
UpdateSmbiosInfo (
  VOID
  )
{
  VOID               *Hob;
  PLATFORM_INFO_HOB  *PlatformHob;

  /* Get the Platform HOB */
  Hob = GetFirstGuidHob (&gPlatformInfoHobGuid);
  ASSERT (Hob != NULL);
  if (Hob == NULL) {
    return;
  }

  PlatformHob = (PLATFORM_INFO_HOB *)GET_GUID_HOB_DATA (Hob);

  UpdateSmbiosType4 (PlatformHob);
  UpdateSmbiosType7 (PlatformHob);
}

/**
   Install SMBIOS Type 7 table

   @param  Smbios               SMBIOS protocol.
**/
EFI_STATUS
InstallType7Structures (
  IN EFI_SMBIOS_PROTOCOL *Smbios
  )
{
  EFI_STATUS         Status = EFI_SUCCESS;
  EFI_SMBIOS_HANDLE  SmbiosHandle;
  SMBIOS_TABLE_TYPE4 *Type4Table;
  CONST VOID         **Tables;
  UINTN              Index;
  UINTN              Level;

  for ( Index = 0; Index < GetNumberOfSupportedSockets (); Index++ ) {
    if ( Index == 0) {
      Tables = DefaultType7Sk0Tables;
      Type4Table = &mArmDefaultType4Sk0.Base;
    } else {
      Tables = DefaultType7Sk1Tables;
      Type4Table = &mArmDefaultType4Sk1.Base;
    }

    for (Level = 0; Level < CPU_CACHE_LEVEL_COUNT; Level++ ) {
      SmbiosHandle = ((EFI_SMBIOS_TABLE_HEADER *)Tables[Level])->Handle;
      Status = Smbios->Add (
                         Smbios,
                         NULL,
                         &SmbiosHandle,
                         (EFI_SMBIOS_TABLE_HEADER *)Tables[Level]
                         );
      if (EFI_ERROR (Status)) {
        DEBUG ((
          DEBUG_ERROR,
          "%a: adding SMBIOS type 7 Socket %d L%d cache failed\n",
          __FUNCTION__,
          Index,
          Level + 1
          ));
        // stop adding rather than continuing
        return Status;
      }

      // Save handle to Type 4
      if (Level == 0) { // L1 cache
        Type4Table->L1CacheHandle = SmbiosHandle;
      } else if (Level == 2) { // L2 cache
        Type4Table->L2CacheHandle = SmbiosHandle;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
   Install a whole table worth of structructures

   @param  Smbios               SMBIOS protocol.
   @param  DefaultTables        A pointer to the default SMBIOS table structure.
**/
EFI_STATUS
InstallStructures (
  IN       EFI_SMBIOS_PROTOCOL *Smbios,
  IN CONST VOID                *DefaultTables[]
  )
{
  EFI_STATUS        Status = EFI_SUCCESS;
  EFI_SMBIOS_HANDLE SmbiosHandle;
  UINTN             Index;

  for (Index = 0; Index < GetNumberOfSupportedSockets () && DefaultTables[Index] != NULL; Index++) {
    SmbiosHandle = ((EFI_SMBIOS_TABLE_HEADER *)DefaultTables[Index])->Handle;
    Status = Smbios->Add (
                       Smbios,
                       NULL,
                       &SmbiosHandle,
                       (EFI_SMBIOS_TABLE_HEADER *)DefaultTables[Index]
                       );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: adding %d failed\n", __FUNCTION__, Index));

      // stop adding rather than continuing
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
   Install all structures from the DefaultTables structure

   @param  Smbios               SMBIOS protocol

**/
EFI_STATUS
InstallAllStructures (
  IN EFI_SMBIOS_PROTOCOL *Smbios
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  UpdateSmbiosInfo ();

  // Install Type 7 structures
  InstallType7Structures (Smbios);

  // Install Tables
  Status = InstallStructures (Smbios, DefaultCommonTables);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


EFI_STATUS
EFIAPI
SmbiosCpuDxeEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS          Status;
  EFI_SMBIOS_PROTOCOL *Smbios;

  //
  // Find the SMBIOS protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **)&Smbios
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Unable to locate SMBIOS Protocol"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  Status = InstallAllStructures (Smbios);
  DEBUG ((DEBUG_ERROR, "SmbiosCpu install: %r\n", Status));

  return Status;
}
