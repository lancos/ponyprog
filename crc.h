//=========================================================================//
//-------------------------------------------------------------------------//
// crc.h -- Header for for crc utilities routine                           //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997, 1998  Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lanconel@cs.unibo.it                                           //
//  http://www.cs.unibo.it/~lanconel                                       //
//                                                                         //
//-------------------------------------------------------------------------//
//                                                                         //
// This program is free software; you can redistribute it and/or           //
// modify it under the terms of the GNU  General Public License            //
// as published by the Free Software Foundation; either version2 of        //
// the License, or (at your option) any later version.                     //
//                                                                         //
// This program is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
// General Public License for more details.                                //
//                                                                         //
// You should have received a copy of the GNU  General Public License      //
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//

#include "types.h"

/* Calcola il CRC in un file a partire da ini_ofs per un numero di bytes
   dato da len. Se len e` zero prosegue fino alla fine del file, il file
   deve essere aperto in modo da permettere la lettura (r, r+, w+) */
UWORD fcalc_crc(FILE *fh, long ini_ofs, ULONG len);

/* Calcola il CRC in una zona di memoria a partire da ini_addr per un numero
   di bytes dato da len */
UWORD mcalc_crc(void *ini_addr, ULONG len);

/* update crc reverse */
UWORD updcrcr(UWORD crc, UWORD code);
