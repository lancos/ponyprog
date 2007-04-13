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
    extern V_EXPORT void LongToStr(long intg, char* str);	// prototype
    extern V_EXPORT void IntToStr(int intg, char* str);
    extern V_EXPORT int  vLblLen(const char *str);
    extern V_EXPORT int  vTextLen(const char *str, int& numLines);
    extern V_EXPORT void vGetLocalTime(char* tm);
    extern V_EXPORT void vGetLocalDate(char* tm);
    extern V_EXPORT void ByteToStr(unsigned char byteval, char* str);
    extern V_EXPORT long StrToLong(VCONST char* str);
    extern V_EXPORT void vBeep();
    extern V_EXPORT int  vGetcmdIdIndex(ItemVal cmdId, CommandObject *cmdObj);
    extern V_EXPORT int  vGetMenuIdIndex(ItemVal cmdId, vMenu *cmdObj);
#endif
