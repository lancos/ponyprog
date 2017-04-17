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

typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef signed long int32_t;
typedef signed short int16_t;
typedef signed char int8_t;

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
