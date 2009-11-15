/* types.h

   Defines types for portability.

   Copyright (c) Claudio Lanconelli 1993,94,95,96
   All Rights Reserved.
*/

#ifndef _TYPES_H
#define _TYPES_H

#if defined(_MSC_VER) && _MSC_VER == 1200
//MSVC Version 6 doesn't define stdint.h

typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef signed long int32_t;
typedef signed short int16_t;
typedef signed char int8_t;

#else

#include <stdint.h>

#endif

#ifndef	HIDDEN
#define	HIDDEN	static
#endif

/* Amiga conventions */
typedef uint32_t	ULONG;		/* unsigned 32-bit quantity */
typedef uint16_t	UWORD;		/* unsigned 16-bit quantity */
typedef uint8_t		UBYTE;		/* unsigned  8-bit quantity  */
//typedef unsigned char	*STRPTR;	/* string pointer           */

/* Windows conventions */
typedef	uint32_t	DWORD;		/* unsigned 32-bit quantity */
typedef	uint16_t	WORD;		/* unsigned 16-bit quantity */
typedef	uint8_t		BYTE;		/* unsigned  8-bit quantity */

/* Other conventions */
typedef uint32_t	WORD32;		/* unsigned 32-bit quantity */
typedef uint16_t	WORD16;		/* unsigned 16-bit quantity */
typedef uint8_t		WORD8;		/* unsigned  8-bit quantity */

#ifdef	__GNUC__
#define	PACK	__attribute__ ((packed))
#else
#define	PACK
#endif

#endif
