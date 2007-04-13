//===============================================================
// General Purpose V Utilities
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>           // for Win 32 stuff
#include <v/v_defs.h>

#include <v/vutil.h>

#define __USELOCALES__
#include <time.h>

//#include <sys/time.h>


//=============================>>> vTextLen  <<<============================
  int vTextLen(const char *str, int& numLines)
  {
    // calculate length of multiline text string
    int l, maxl;
    maxl = 0;           // no length so far
    numLines = 1;
    for (l = 0 ; *str; str++)
      {
        if (*str != '\n' && *str != '\r')
          {
            ++l;
          }
        else
          {
            ++numLines;
            if (l > maxl)
                maxl = l;
            l = 0;
          }

      }
    if (l > maxl)
        maxl = l;
    return maxl;
  }

//=============================>>> ByteToStr   <<<============================
  void  ByteToStr(unsigned char intg, char* str)
  {  // convert byte intg to char string in str
 
    int i;
    int d, intval, j;
    char k;
    static char digits[] = "0123456789ABCDEF";
 
    intval = intg;
    str[0] = '\0';
    i = 0;
    do
      {                         // generate digits
        i++;
        d = intval % 16;        // mod 10
        str[i] = digits[d];
        intval = intval / 16;
      }
    while (intval != 0);

    for (j = 0 ; j < i ; j++ )
      {                         // then reverse
        k = str[i];
        str[i--] = str[j];
        str[j] = k;
      }
    if (str[1] == 0)            // one char only
      {
        str[1] = str[0]; str[2] = 0; str[0] = '0';
      }
  }

//=============================>>> vLblLen  <<<============================
  int vLblLen(const char *str)
  {
    int l;
    for (l = 0 ; *str; str++)
      if (*str != '&')
          ++l;
    return l;
  }

//=============================>>> l_to_a   <<<============================
  long StrToLong(VCONST char* str)
  {  // convert int intg to char string in str

    long val = 0;
    long neg  = 1;
    if (*str == '-')
      {
        neg = -1;
        ++str;
      }
    while (*str)
      {
	if (*str >= '0' && *str <= '9')
	    val = (val*10) + (*str++ - '0');
	else
	    break;
      }
    return val * neg;
  }

//=============================>>> l_to_a   <<<============================
  void IntToStr(int intg, char* str)
  {  // convert int intg to char string in str

    LongToStr((long)intg, str);
  }

//=============================>>> l_to_a   <<<============================
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
      {                         // generate digits
        i++;
        d = intval % 10L;       // mod 10
        str[i] = digits[d];
        intval = intval / 10L;
      }
    while (intval != 0L);

    if (intg < 0L)
      {                         // then sign
        str[++i] = '-';
      }
    for (j = 0 ; j < i ; j++ )
      {                         // then reverse
        k = str[i];
        str[i--] = str[j];
        str[j] = k;
      }
  }

//=============================>>> vGetLocalTime <<<==========================
  void vGetLocalTime(char* tm)
  {

    char buff[20];
    time_t t;
    t = time(0);
    strftime(buff, 20, "%X", localtime(&t));
    strcpy(tm,buff);
  }

//=============================>>> vGetLocalDate <<<==========================
  void vGetLocalDate(char* tm)
  {

    char buff[20];
    time_t t;
    t = time(0);
    strftime(buff, 20, "%d %b %Y", localtime(&t));
    strcpy(tm,buff);
  }

//=============================>>> vBeep <<<==========================
  void vBeep()
  {
     ::MessageBeep(MB_OK);
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
  
  
