//=========================================================================//
//-------------------------------------------------------------------------//
// eeptypes.cpp -- eeprom id translation utilities                         //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-1999  Claudio Lanconelli                            //
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

#include <string.h>

#include "types.h"
#include "globals.h"
#include "eeptypes.h"

#ifndef	_LINUX_
#  ifdef	__BORLANDC__
#    define	strncasecmp strnicmp
#    define	strcasecmp stricmp
#  else // _MICROSOFT_ VC++
#    define	strncasecmp strnicmp
#    define	strcasecmp stricmp
#  endif
#endif

#define	KB(x)	( (x) * 1024 )

long BuildE2PType(int x, int y)
{
	return (((long)x & 0x7FFF) << 16) | (y & 0x7FFF);
}

int GetE2PSubType(long x)
{
	return (int)(x & 0x7FFF);
}

int GetE2PPriType(long x)
{
	return (int)((x >> 16) & 0x7FFF);
}

int eeptype_vector[NO_OF_EEPTYPE] = {
	//AutoTag
	//List of device types Id
	E24XX,
	E24XXX,
	E24XX5,
	AT90SXX,
	AT89SXX,
	E93X6,
	PIC16XX,
	E250XX,
	E25XXX,
	E2506XX,
};

char const *eepstr_vector[NO_OF_EEPTYPE+1] = {
	//AutoTag
	//List of device types label
	"2402-16",
	"2432-512",
	"24XX5",
	"AVR",
	"AT89S",
	"93X6",
	"PIC16XX",
	"250X0",
	"25XXX",
	"SDE2506",
	0
};

int GetEEPTypeIndex(int type)
{
	int k;
	for (k = 0; k < NO_OF_EEPTYPE; k++)
		if (eeptype_vector[k] == type)
			break;
	if (k >= NO_OF_EEPTYPE)
		k = -1;		//non trovato
	return k;
}

//AutoTag
//Device type description vectors

// ----------------------------
long eep24xx_vector[MAXEEPSUBTYPE] = {
	E2400,
	E2402,
	E2404,
	E2408,
	E2416
};

int eep24xx_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi
	AUTOSIZE_ID/256,
	1,
	2,
	4,
	8
};

char const *eep24xxstr_vector[MAXEEPSUBTYPE] = {
	"24XX Auto",
	"2402",
	"2404",
	"2408",
	"2416",
	0
};

//--------------------------
long eep24xxx_vector[MAXEEPSUBTYPE] = {
//	E24000,
	E2432,
	E2464,
	E24128,
	E24256,
	E24512
};

int eep24xxx_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi
//	0,
	16,
	32,
	64,
	128,
	256
};

char const *eep24xxxstr_vector[MAXEEPSUBTYPE] = {
//	"Auto",
	"2432",
	"2464/2465",
	"24128",
	"24256",
	"24512",
	0
};

// ----------------------------
long eep24xx5_vector[MAXEEPSUBTYPE] = {
//	E2400,
	E24325,
	E24645
};

int eep24xx5_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi
//	AUTOSIZE_ID/256,
	16,
	32
};

char const *eep24xx5str_vector[MAXEEPSUBTYPE] = {
//	"Auto",
	"24325",
	"24645",
	0
};

//-----------------------------
long eepAt90s_vector[MAXEEPSUBTYPE] = {
	AT90S0000,
	AT90S1200,
	AT90S2313,
	AT90S2323,
	AT90S2333,
	AT90S2343,
	AT90S4414,
	AT90S4433,
	AT90S4434,
	AT90S8515,
	AT90S8534,
	AT90S8535,
	ATmega603,
	ATmega103,
	ATtiny12,
	ATtiny22
};

int eepAt90s_size[MAXEEPSUBTYPE] = {
	AUTOSIZE_ID,
	KB(1)+64,
	KB(2)+128,
	KB(2)+128,
	KB(2)+128,
	KB(2)+128,
	KB(4)+256,
	KB(4)+256,
	KB(4)+256,
	KB(8)+512,
	KB(8)+512,
	KB(8)+512,
	KB(64)+KB(2),
	KB(128)+KB(4),
	KB(1)+64,
	KB(2)+128
};
int eepAt90s_split[MAXEEPSUBTYPE] = {
	0,
	KB(1),
	KB(2),
	KB(2),
	KB(2),
	KB(2),
	KB(4),
	KB(4),
	KB(4),
	KB(8),
	KB(8),
	KB(8),
	KB(64),
	KB(128),
	KB(1),
	KB(2)
};

char const *eepAt90str_vector[MAXEEPSUBTYPE] = {
	"AVR  Auto",
	"AT90S1200",
	"AT90S2313",
	"AT90S2323",
	"AT90S2333",
	"AT90S2343",
	"AT90S4414",
	"AT90S4433",
	"AT90S4434",
	"AT90S8515",
	"AT90C8534",
	"AT90S8535",
	"ATmega603",
	"ATmega103",
	"ATtiny12",
	"ATtiny22",
	0
};

//-----------------------------
long eepAt89s_vector[MAXEEPSUBTYPE] = {
//	AT89S0000,
	AT89S8252,
	AT89S53
};

int eepAt89s_size[MAXEEPSUBTYPE] = {
//	AUTOSIZE_ID,
	KB(8)+KB(2),
	KB(12)
};
int eepAt89s_split[MAXEEPSUBTYPE] = {
//	0,
	KB(8),
	KB(12)
};

char const *eepAt89str_vector[MAXEEPSUBTYPE] = {
//	"AT89SXX Auto",
	"AT89S8252",
	"AT89S53",
	0
};


//-------------------------
long eep93x6_vector[MAXEEPSUBTYPE] = {
//	E9300,
	E9306,
	E9346,
	E9356,
	E9366,
	E9376,
	E9386
};

int eep93x6_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi (da 2 byte)
//	0,
	16,
	64,
	128,
	256,
	512,
	1024
};

int eep93x6_addrsize[MAXEEPSUBTYPE] = {	//dimensione dello spazio di indirizzamento in numero di banchi (da 2 byte)
//	0,
	64,		//9306
	64,		//9346
	256,	//9356
	256,	//9366
	1024,	//9376
	1024	//9386
};

char const *eep93x6str_vector[MAXEEPSUBTYPE] = {
//	"93XX Auto",
	"9306",
	"9346",
	"9356",
	"9366",
	"9376",
	"9386",
	0
};

//-----------------------------
long eepPic16_vector[MAXEEPSUBTYPE] = {
//	PIC1600,
	PIC1684,
};

int eepPic16_size[MAXEEPSUBTYPE] = {
//	0,
	2048+16+64,
};
int eepPic16_split[MAXEEPSUBTYPE] = {
//	0,
	2048,
};

char const *eepPic16str_vector[MAXEEPSUBTYPE] = {
//	"PIC16XX Auto",
	"PIC16X84",
	0
};

//-------------------------
long eep250xx_vector[MAXEEPSUBTYPE] = {
//	E25000,
	E25010,
	E25020,
	E25040,
};

int eep250xx_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi (da 2 byte)
//	0,
	128,
	256,
	512,
};

char const *eep250xxstr_vector[MAXEEPSUBTYPE] = {
//	"250X0 Auto",
	"25010",
	"25020",
	"25040",
	0
};

//-------------------------
long eep25xxx_vector[MAXEEPSUBTYPE] = {
//	E25XX0,
	E25080,
	E25160,
	E25320,
	E25640,
	E25128,
	E25256
};

int eep25xxx_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi (da 2 byte)
//	0,
	KB(1),
	KB(2),
	KB(4),
	KB(8),
	KB(16),
	KB(32)
};

char const *eep25xxxstr_vector[MAXEEPSUBTYPE] = {
//	"25XXX Auto",
	"25080",
	"25160",
	"25320",
	"2564X/95640",
	"25128",
	"25256",
	0
};


//-------------------------
long eep2506_vector[MAXEEPSUBTYPE] = {
//	E25000,
	E2506,
};

int eep2506_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi (da 2 byte)
//	0,
	128,
};

char const *eep2506str_vector[MAXEEPSUBTYPE] = {
//	"250X0 Auto",
	"SDE2506",
	0
};



static void GetArrayPtrs(int pritype, long* &vp, int* &sp, int* &asp, int* &ssp, const char ** &rp)
{
	switch(pritype)
	{
	//AutoTag
	//Returns array pointers relative to device
	case E24XX:
		asp = sp = eep24xx_size;
		vp = eep24xx_vector;
		rp = eep24xxstr_vector;
		break;
	case E24XXX:
		asp = sp = eep24xxx_size;
		vp = eep24xxx_vector;
		rp = eep24xxxstr_vector;
		break;
	case E24XX5:
		asp = sp = eep24xx5_size;
		vp = eep24xx5_vector;
		rp = eep24xx5str_vector;
		break;
	case AT90SXX:
		ssp = eepAt90s_split;
		asp = sp = eepAt90s_size;
		vp = eepAt90s_vector;
		rp = eepAt90str_vector;
		break;
	case AT89SXX:
		ssp = eepAt89s_split;
		asp = sp = eepAt89s_size;
		vp = eepAt89s_vector;
		rp = eepAt89str_vector;
		break;
	case E93X6:
		asp = eep93x6_addrsize;
		sp = eep93x6_size;
		vp = eep93x6_vector;
		rp = eep93x6str_vector;
		break;
	case PIC16XX:
		ssp = eepPic16_split;
		asp = sp = eepPic16_size;
		vp = eepPic16_vector;
		rp = eepPic16str_vector;
		break;
	case E250XX:
		sp = eep250xx_size;
		vp = eep250xx_vector;
		rp = eep250xxstr_vector;
		break;
	case E25XXX:
		sp = eep25xxx_size;
		vp = eep25xxx_vector;
		rp = eep25xxxstr_vector;
		break;
	case E2506XX:
		sp = eep2506_size;
		vp = eep2506_vector;
		rp = eep2506str_vector;
		break;
	default:
		ssp = asp = sp = 0;
		vp = 0;
		rp = 0;
		break;
	}
}


long GetEEPTypeFromSize(int pritype, int size)
{
	int *sp, *asp, *ssp;
	long *vp = 0;
	char const **rp;

	GetArrayPtrs(pritype, vp, sp, asp, ssp, rp);

	int j;
	for (j = 0; j < MAXEEPSUBTYPE; j++)
		if (sp[j] == size)
			break;
	if (j < MAXEEPSUBTYPE)
	{
		return vp[j];
	}
	else
		return -1;
}

int GetEEPTypeSize(int pritype, int subtype)
{
	int *sp = 0, *asp, *ssp;
	long *vp;
	char const **rp;

	if (pritype == 0)
		return -1;

	GetArrayPtrs(pritype, vp, sp, asp, ssp, rp);

	long type = BuildE2PType(pritype, subtype);
	return sp[GetEEPSubTypeIndex(type)];
}

//Serve solamente nei bus che utilizzano la dimensione
// di word degli indirizzi dinamica (per es. Microwire)
int GetEEPAddrSize(int pritype, int subtype)
{
	int *sp, *asp = 0, *ssp;
	long *vp;
	char const **rp;

	GetArrayPtrs(pritype, vp, sp, asp, ssp, rp);

	long type = BuildE2PType(pritype, subtype);
	return asp[GetEEPSubTypeIndex(type)];
}

int GetEEPTypeSplit(int pritype, int subtype)
{
	int *sp, *asp, *ssp = 0;
	long *vp;
	char const **rp;

	GetArrayPtrs(pritype, vp, sp, asp, ssp, rp);

	if (ssp)
	{
		long type = BuildE2PType(pritype, subtype);
		return ssp[GetEEPSubTypeIndex(type)];
	}
	else
		return 0;
}

char const **GetEEPSubTypeList(int type)
{
	int *sp, *asp, *ssp;
	long *vp;
	char const **rp = 0;

	GetArrayPtrs(type, vp, sp, asp, ssp, rp);

	return rp;
}

long *GetEEPSubTypeVector(int type)
{
	int *sp, *asp, *ssp;
	long *vp = 0;
	char const **rp;

	GetArrayPtrs(type, vp, sp, asp, ssp, rp);

	return vp;
}

int GetEEPSubTypeIndex(long type)
{
	long *vp = GetEEPSubTypeVector(GetE2PPriType(type));
	int k = -1;
	if (vp)
	{
		for (k = 0; k < MAXEEPSUBTYPE; k++)
			if (vp[k] == type)
				break;
		if (k >= MAXEEPSUBTYPE)
			k = -1;		//non trovato
	}

	return k;
}

char const *GetEEPTypeString(int pritype, int subtype)
{
	long type = BuildE2PType(pritype, subtype);

	//14/08/98
	int idx = GetEEPSubTypeIndex(type);
	return GetEEPSubTypeList(pritype)[idx];
}

//18/10/98
char const *GetEEPTypeString(long type)
{
	int pritype = GetE2PPriType(type);

	int idx = GetEEPSubTypeIndex(type);
	return GetEEPSubTypeList(pritype)[idx];
}

long GetEEPTypeFromString(char const *name)
{
	char const **rp;
	long *vp;
	long type = 0;

	int k;
	for (k = 0; k < NO_OF_EEPTYPE; k++)
	{
		vp = GetEEPSubTypeVector(eeptype_vector[k]);
		rp = GetEEPSubTypeList(eeptype_vector[k]);
		if (rp)
		{
			int j;
			for (j = 0; rp[j] != 0; j++)
				if (strcasecmp(rp[j], name) == 0)
					break;
			if (rp[j])
			{
				type = vp[j];
				break;
			}
		}
	}

	return type;
}
