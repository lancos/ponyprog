//=========================================================================//
//-------------------------------------------------------------------------//
// eeptypes.h -- Header for for eeptypes conversion utilities              //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-1998   Claudio Lanconelli                           //
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

#ifndef	_EEPTYPES_H
#define	_EEPTYPES_H

#define	E24XX	0x01
// Sub types
#define	E2400	0x010000
#define	E2402	0x010001
#define	E2404	0x010002
#define	E2408	0x010004
#define	E2416	0x010008

#define	E24XXX	0x02
// Sub types
#define	E24000	0x020000
#define	E2432	0x020010
#define	E2464	0x020020
#define	E24128	0x020040
#define	E24256	0x020080
#define	E24512	0x020100

#define	AT90SXX	0x03
// Sub types
#define	AT90S0000	0x030000
#define	AT90S1200	0x030002
#define	AT90S2313	0x030003
#define	AT90S4414	0x030004
#define	AT90S8515	0x030005

#define	ATmega603	0x030008
#define	ATmega103	0x030009

#define	AT90S2323	0x03000A
#define	AT90S2343	0x03000B
#define	AT90S2333	0x03000C
#define	AT90S4433	0x03000D
#define	AT90S4434	0x03000E
#define	AT90S8534	0x03000F
#define	AT90S8535	0x030010

#define	ATtiny22	0x030011
#define	ATtiny12	0x030012
#define	ATtiny11	0x030080
#define	ATtiny10	0X030081

#define	E93X6	0x04
// Sub types
#define	E9300	0x040000
#define	E9306	0x040001
#define	E9346	0x040004
#define	E9356	0x040008
#define	E9366	0x040010
#define	E9376	0x040020
#define	E9386	0x040040

#define	PIC16XX	0x05
// Sub types
#define	PIC1600	0x050000
#define	PIC1684	0x050001

#define	E250XX	0x06
// Sub types
#define	E25000	0x060000
#define	E25010	0x060001
#define	E25020	0x060002
#define	E25040	0x060004

#define	E2506XX	0x07
// Sub types
#define	E250600	0x070000
#define	E2506	0x070001

#define	E24XX5	0x08
// Sub types
#define	E24325	0x080010
#define	E24645	0x080020

#define	E25XXX	0x09
// Sub types
#define	E25XX0	0x090000
#define	E25080	0x090004
#define	E25160	0x090008
#define	E25320	0x090010
#define	E25640	0x090020
#define	E25128	0x090040
#define	E25256	0x090080

#define	AT89SXX	0x0A
// Sub types
#define	AT89S0000	0x0A0000
#define	AT89S8252	0x0A0002
#define	AT89S53		0x0A0004

#define	NO_OF_EEPTYPE	0x0A

#define	MAXEEPSUBTYPE	32

long BuildE2PType(int x, int y = 0);
int GetE2PSubType(long x);
int GetE2PPriType(long x);

char const *GetEEPTypeString(int pritype, int subtype);
char const *GetEEPTypeString(long type);

int GetEEPTypeIndex(int type);
char const **GetEEPSubTypeList(int type);
long *GetEEPSubTypeVector(int type);
int GetEEPSubTypeIndex(long type);
int GetEEPTypeSize(int pritype, int subtype);
int GetEEPAddrSize(int pritype, int subtype);
int GetEEPTypeSplit(int pritype, int subtype);

long GetEEPTypeFromSize(int pritype, int size);
long GetEEPTypeFromString(char const *name);

extern int eeptype_vector[NO_OF_EEPTYPE];
extern char const *eepstr_vector[NO_OF_EEPTYPE+1];

extern char const *eepAt90str_vector[MAXEEPSUBTYPE];

#endif
