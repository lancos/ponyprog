//===============================================================
// General Purpose V Utilities
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vapp.h>
#include <v/vutil.h>

#include <time.h>
#include <sys/time.h>
#include <string.h>

//=============================>>> IntToStr <<<============================
  void IntToStr(int intg, char* str)
  {  // convert int intg to char string in str

    LongToStr((long)intg, str);
  }

//=============================>>> StrToLong <<<============================
  long StrToLong(VCONST char* str)
  {  // convert char string into long

    long val = 0;
    long neg = 1;
    if (*str == '-')
      {
	neg = -1;
	str++;
      }
    while (str && *str)
      {
	if (*str < '0' || *str > '9')
	   break;
	val = (val * 10) + (*str - '0');
	str++;
      }
    return neg*val;
  }

//=============================>>> LongToStr   <<<============================
  void LongToStr(long intg, char* str)
  {  // convert long intg to char string in str
 
    long i;
    long d, intval, j;
    char k;
    static char digits[] = "0123456789";
 
    intval = intg >= 0L ? intg : (-intg);
    str[0] = '\0';
    i = 0;
    do
      {				// generate digits
        i++;
        d = intval % 10L;	// mod 10
        str[i] = digits[d];
        intval = intval / 10L;
      }
    while (intval != 0L);

    if (intg < 0L)
      {				// then sign
        str[++i] = '-';
      }
    for (j = 0 ; j < i ; j++ )
      {				// then reverse
        k = str[i];
        str[i--] = str[j];
        str[j] = k;
      }
  }

//=============================>>> ByteToStr   <<<============================
  void ByteToStr(unsigned char intg, char* str)
  {  // convert byte intg to char string in str
 
    int i;
    int d, intval, j;
    char k;
    static char digits[] = "0123456789ABCDEF";
 
    intval = intg;
    str[0] = '\0';
    i = 0;
    do
      {				// generate digits
        i++;
        d = intval % 16;	// mod 10
        str[i] = digits[d];
        intval = intval / 16;
      }
    while (intval != 0);

    for (j = 0 ; j < i ; j++ )
      {				// then reverse
        k = str[i];
        str[i--] = str[j];
        str[j] = k;
      }
    if (str[1] == 0)		// one char only
      {
	str[1] = str[0]; str[2] = 0; str[0] = '0';
      }
  }

//=============================>>> vGetLocalTime <<<==========================
  void vGetLocalTime(char* tm)
  {
    char buff[20];
    time_t t;
    t = time(0);
    strftime(buff,20,"%r",localtime(&t));
    strcpy(tm,buff);
  }

//=============================>>> vGetLocalDate <<<==========================
  void vGetLocalDate(char* tm)
  {
    char buff[20];
    time_t t;
    t = time(0);
    strftime(buff,20,"%d %b %Y",localtime(&t));
    strcpy(tm,buff);
  }

//=============================>>> vBeep <<<==========================
  void vBeep()
  {
     XBell(theApp->display(),0);
  }

//==================================================================
//
//    Some special case utils for working with CommandObjects
//

//=============================>>> vGetcmdIdIndex <<<==========================
  int vGetcmdIdIndex(ItemVal cmdId, CommandObject *cmdObj)
  {
    //    Return the index to the CommandObject array for the
    //    I which has the matching cmdId.
    for (int i = 0 ; cmdObj[i].cmdType != C_EndOfList ; ++i)
      {
        if (cmdObj[i].cmdId == cmdId)
            return i;
      }
    return -1;
      }

//=============================>>> vGetMenuIdIndex <<<==========================
  int vGetMenuIdIndex(ItemVal cmdId, vMenu *cmdObj)
  {
    //    Return the index to the CommandObject array for the
    //    I which has the matching cmdId.
    for (int i = 0 ; cmdObj[i].label != 0 ; ++i)
      {
        if (cmdObj[i].menuId == cmdId)
            return i;
      }
    return -1;
  }
