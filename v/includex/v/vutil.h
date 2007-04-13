//===============================================================
// vutil.h - V utility funcitons
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VUTIL_H
#define VUTIL_H
#include <v/v_defs.h>
    extern void LongToStr(long intg, char* str);	// prototype
    extern void IntToStr(int intg, char* str);
    extern void vGetLocalTime(char* tm);
    extern void vGetLocalDate(char* tm);
    extern void ByteToStr(unsigned char byteval, char* str);
    extern long StrToLong(VCONST char* str);
    extern void vBeep();
    extern int vGetcmdIdIndex(ItemVal cmdId, CommandObject *cmdObj);
    extern int vGetMenuIdIndex(ItemVal cmdId, vMenu *cmdObj);
#endif
