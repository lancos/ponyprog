/* types.h

   Defines types for portability.

   Copyright (c) Claudio Lanconelli 1993,94,95,96
   All Rights Reserved.
*/

#ifndef _TYPES_H
#define _TYPES_H

#ifndef	HIDDEN
#define	HIDDEN	static
#endif

/* Amiga conventions */
typedef unsigned long	ULONG;		/* unsigned 32-bit quantity */
typedef unsigned short	UWORD;		/* unsigned 16-bit quantity */
typedef unsigned char	UBYTE;		/* unsigned  8-bit quantity  */
typedef unsigned char	*STRPTR;	/* string pointer           */

/* Windows conventions */
typedef	unsigned long	DWORD;		/* unsigned 32-bit quantity */
typedef	unsigned short	WORD;		/* unsigned 16-bit quantity */
typedef	unsigned char	BYTE;		/* unsigned  8-bit quantity */

/* Other conventions */
typedef unsigned long	WORD32;		/* unsigned 32-bit quantity */
typedef unsigned short	WORD16;		/* unsigned 16-bit quantity */
typedef unsigned char	WORD8;		/* unsigned  8-bit quantity */

#ifdef	__GNUC__
#define	PACK	__attribute__ ((packed))
#else
#define	PACK
#endif

#endif
