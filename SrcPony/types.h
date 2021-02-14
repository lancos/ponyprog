/* types.h

   Defines types for portability.

   Copyright (c) Claudio Lanconelli 1993,94,95,96
   All Rights Reserved.
*/

#ifndef _TYPES_H
#define _TYPES_H

#ifdef  __GNUC__

#include <stdint.h>

#elif defined(_MSC_VER) // && _MSC_VER == 1200
//MSVC doesn't define stdint.h

typedef unsigned long quint32;
typedef unsigned short quint16;
typedef unsigned char quint8;

typedef signed long qint32;
typedef signed short qint16;
typedef signed char qint8;

#endif

#ifndef HIDDEN
#define HIDDEN  static
#endif

#ifdef  __GNUC__
#define PACK    __attribute__ ((packed))
#else
#define PACK
#endif

#endif
