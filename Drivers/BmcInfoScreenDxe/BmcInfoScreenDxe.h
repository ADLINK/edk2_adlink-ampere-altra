/** @file

  Copyright (c) 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef BMC_INFO_SCREEN_DXE_H_
#define BMC_INFO_SCREEN_DXE_H_

//
// This is the generated IFR binary data for each formset defined in VFR.
// This data array is ready to be used as input of HiiAddPackages() to
// create a packagelist (which contains Form packages, String packages, etc).
//
extern UINT8 VfrBin[];

//
// This is the generated String package data for all .UNI files.
// This data array is ready to be used as input of HiiAddPackages() to
// create a packagelist (which contains Form packages, String packages, etc).
//
extern UINT8 BmcInfoScreenDxeStrings[];

//
// Form definitions
//
#define MAIN_FORM_ID    0x1000
#define LABEL_UPDATE    0x2000
#define LABEL_END       0x2300

#define MAX_STRING_SIZE 40

#endif // BMC_INFO_SCREEN_DXE_H_
