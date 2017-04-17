//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id$
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
#  elif defined(_MSC_VER) // _MICROSOFT_ VC++
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
long eep24xx1A_vector[MAXEEPSUBTYPE] = {
//	E24000,
	E2401_A
};

int eep24xx1A_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi
//	0,
	1
};

char const *eep24xx1Astr_vector[MAXEEPSUBTYPE] = {
//	"Auto",
	"2401-A",
	0
};

//--------------------------
long eep24xx1B_vector[MAXEEPSUBTYPE] = {
//	E24000,
	E2401_B
};

int eep24xx1B_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi
//	0,
	1
};

char const *eep24xx1Bstr_vector[MAXEEPSUBTYPE] = {
//	"Auto",
	"2401-B",
	0
};


//--------------------------
long eep24xx2_vector[MAXEEPSUBTYPE] = {
//	E24000,
	E24E16,
	E2432,
	E2464,
	E24128,
	E24256,
	E24512
};

int eep24xx2_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi
//	0,
	8,
	16,
	32,
	64,
	128,
	256
};

char const *eep24xx2str_vector[MAXEEPSUBTYPE] = {
//	"Auto",
	"24E16",
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
//	AT90S0000,
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
	AT90CAN32,
	AT90CAN64,
	AT90CAN128,

	ATmega8515,
	ATmega8535,

	ATmega48,
	ATmega8,
	ATmega88,
	ATmega16,
	ATmega161,
	ATmega162,
	ATmega163,
	ATmega164,
	ATmega168,
	ATmega169,
	ATmega32,
	ATmega323,
	ATmega324,
	ATmega328, //new (RG 22.06.2012)
	ATmega603,
	ATmega103,
	ATmega64,
	ATmega640,
	ATmega644,
	ATmega128,
	ATmega1280,
	ATmega1281,
	ATmega2560,
	ATmega2561,

	ATtiny12,
	ATtiny13,
	ATtiny15,
	ATtiny22,
	ATtiny25,
	ATtiny26,
	ATtiny261,
//	ATtiny28,
	ATtiny2313,
	ATtiny45,
	ATtiny461,
	ATtiny85,
	ATtiny861
};

int eepAt90s_size[MAXEEPSUBTYPE] = {
//	AUTOSIZE_ID,
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
	KB(32)+KB(1),	//AT90CAN32
	KB(64)+KB(2),	//AT90CAN64
	KB(128)+KB(4),	//AT90CAN128

	KB(8)+512,		//mega8515
	KB(8)+512,		//mega8535

	KB(4)+256,		//mega48
	KB(8)+512,		//mega8
	KB(8)+512,		//mega88
	KB(16)+512,		//mega16
	KB(16)+512,		//mega161
	KB(16)+512,		//mega162
	KB(16)+512,		//mega163
	KB(16)+512,		//mega164
	KB(16)+512,		//mega168
	KB(16)+512,		//mega169
	KB(32)+KB(1),	//mega32
	KB(32)+KB(1),	//mega323
	KB(32)+KB(1),	//mega324
	KB(32)+KB(1),	//mega328 new (RG 22.06.2012)
	KB(64)+KB(2),	//mega603
	KB(128)+KB(4),	//mega103
	KB(64)+KB(2),	//mega64
	KB(64)+KB(4),	//mega640
	KB(64)+KB(2),	//mega644
	KB(128)+KB(4),	//mega128
	KB(128)+KB(4),	//mega1280
	KB(128)+KB(4),	//mega1281
	KB(256)+KB(4),	//mega2560
	KB(256)+KB(4),	//mega2561

	KB(1)+64,		//tiny12
	KB(1)+64,		//tiny13
	KB(1)+64,		//tiny15
	KB(2)+128,		//tiny22
	KB(2)+128,		//tiny25
	KB(2)+128,		//tiny26
	KB(2)+128,		//tiny261
//	KB(2),			//tiny28
	KB(2)+128,		//tiny2313
	KB(4)+256,		//tiny45
	KB(4)+256,		//tiny461
	KB(8)+512,		//tiny85
	KB(8)+512		//tiny861
};
int eepAt90s_split[MAXEEPSUBTYPE] = {
//	1,			//0,	12/10/1999
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
	KB(32),		//AT90CAN32
	KB(64),		//AT90CAN64
	KB(128),	//AT90CAN128

	KB(8),		//mega8515
	KB(8),		//mega8535

	KB(4),		//mega48
	KB(8),		//mega8
	KB(8),		//mega88
	KB(16),		//mega16
	KB(16),		//mega161
	KB(16),		//mega162
	KB(16),		//mega163
	KB(16),		//mega164
	KB(16),		//mega168
	KB(16),		//mega169
	KB(32),		//mega32
	KB(32),		//mega323
	KB(32),		//mega324
	KB(32),		//mega328 new (RG 22.06.2012)
	KB(64),		//mega603
	KB(128),	//mega103
	KB(64),		//mega64
	KB(64),		//mega640
	KB(64),		//mega644
	KB(128),	//mega128
	KB(128),	//mega1280
	KB(128),	//mega1281
	KB(256),	//mega2560
	KB(256),	//mega2561

	KB(1),		//tiny12
	KB(1),		//tiny13
	KB(1),		//tiny15
	KB(2),		//tiny22
	KB(2),		//tiny25
	KB(2),		//tiny26
	KB(2),		//tiny261
//	KB(2),		//tiny28
	KB(2),		//tiny2313
	KB(4),		//tiny45
	KB(4),		//tiny461
	KB(8),		//tiny85
	KB(8)		//tiny861
};
int eepAt90s_wpagesize[MAXEEPSUBTYPE] = {
//	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	128,		//AT90CAN32
	256,		//AT90CAN64
	256,		//AT90CAN128

	64,			//mega8515
	64,			//mega8535

	64,			//mega48
	64,			//mega8
	64,			//mega88
	128,		//mega16
	128,		//mega161
	128,		//mega162
	128,		//mega163
	128,		//mega164
	128,		//mega168
	128,		//mega169
	128,		//mega32
	128,		//mega323
	128,		//mega324
	128,		//mega328 new (RG 22.06.2012)
	256,		//mega603
	256,		//mega103
	256,		//mega64
	256,		//mega640
	256,		//mega644
	256,		//mega128
	256,		//mega1280
	256,		//mega1281
	256,		//mega2560
	256,		//mega2561

	0,			//tiny12
	32,			//tiny13
	0,			//tiny15
	0,			//tiny22
	32,			//tiny25
	32,			//tiny26
	32,			//tiny261
//	0,			//tiny28
	32,			//tiny2313
	64,			//tiny45
	64,			//tiny461
	64,			//tiny85
	64			//tiny861
};

char const *eepAt90str_vector[MAXEEPSUBTYPE] = {
//	"AVR  Auto",
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
	"AT90CAN32",
	"AT90CAN64",
	"AT90CAN128",

	"ATmega8515",
	"ATmega8535",

	"ATmega48",
	"ATmega8",
	"ATmega88",
	"ATmega16",
	"ATmega161",
	"ATmega162",
	"ATmega163",
	"ATmega164",
	"ATmega168",
	"ATmega169",
	"ATmega32",
	"ATmega323",
	"ATmega324",
	"ATmega328", // new (RG 22.06.2012)
	"ATmega603",
	"ATmega103",
	"ATmega64",
	"ATmega640",
	"ATmega644",
	"ATmega128",
	"ATmega1280",
	"ATmega1281",
	"ATmega2560",
	"ATmega2561",

	"ATtiny12",
	"ATtiny13",
	"ATtiny15",
	"ATtiny22",
	"ATtiny25",
	"ATtiny26",
	"ATtiny261",
//	"ATtiny28",
	"ATtiny2313",
	"ATtiny45",
	"ATtiny461",
	"ATtiny85",
	"ATtiny861",
	0
};

//-----------------------------
long eepAt89s_vector[MAXEEPSUBTYPE] = {
//	AT89S0000,
	AT89S8252,
	AT89S8253,
	AT89S51,
	AT89S52,
	AT89S53,
};

int eepAt89s_size[MAXEEPSUBTYPE] = {
//	AUTOSIZE_ID,
	KB(8)+KB(2),
	KB(12)+KB(2),
	KB(4),
	KB(8),
	KB(12)
};
int eepAt89s_split[MAXEEPSUBTYPE] = {
//	0,
	KB(8),
	KB(12),
	KB(4),
	KB(8),
	KB(12)
};
int eepAt89s_wpagesize[MAXEEPSUBTYPE] = {
//	0,
	0,
	64,
	256,
	256,
	0
};

char const *eepAt89str_vector[MAXEEPSUBTYPE] = {
//	"AT89SXX Auto",
	"AT89S8252",
	"AT89S8253",
	"AT89S51",
	"AT89S52",
	"AT89S53",
	0
};


//-------------------------
long eep93x6_vector[MAXEEPSUBTYPE] = {
//	E9300,
	E9306,
	E9346,
	E9356,
	E9357,
	E9366,
	E9376,
	E9386
};

int eep93x6_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi (da 2 byte)
//	0,
	16,		//9306
	64,		//9346
	128,	//9356
	128,	//9357
	256,	//9366
	512,	//9376
	1024	//9386
};

int eep93x6_addrsize[MAXEEPSUBTYPE] = {	//dimensione dello spazio di indirizzamento in numero di banchi (da 2 byte)
//	0,
	64,		//9306
	64,		//9346
	256,	//9356
	128,	//9357
	256,	//9366
	1024,	//9376
	1024	//9386
};

char const *eep93x6str_vector[MAXEEPSUBTYPE] = {
//	"93XX Auto",
	"9306",
	"9346",
	"9356",
	"9357",
	"9366",
	"9376",
	"9386",
	0
};

//-------------------------
long eep93xx8_vector[MAXEEPSUBTYPE] = {
//	E9300_8,
	E9306_8,
	E9346_8,
	E9356_8,
	E9357_8,
	E9366_8,
	E9376_8,
	E9386_8
};

int eep93xx8_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi (da 1 byte)
//	0,
	32,		//9306
	128,	//9346
	256,	//9356
	256,	//9357
	512,	//9366
	1024,	//9376
	2048	//9386
};

int eep93xx8_addrsize[MAXEEPSUBTYPE] = {	//dimensione dello spazio di indirizzamento in numero di banchi (da 1 byte)
//	0,
	128,	//9306
	128,	//9346
	512,	//9356
	256,	//9357
	512,	//9366
	2048,	//9376
	2048	//9386
};

char const *eep93xx8str_vector[MAXEEPSUBTYPE] = {
//	"93XX Auto",
	"9306",
	"9346",
	"9356",
	"9357",
	"9366",
	"9376",
	"9386",
	0
};


//-----------------------------
long eepPic16_vector[MAXEEPSUBTYPE] = {
//	PIC1600,
	PIC1683,
	PIC1684,
};

int eepPic16_size[MAXEEPSUBTYPE] = {
//	0,
	KB(1)+64,
	KB(2)+64,
};
int eepPic16_split[MAXEEPSUBTYPE] = {
//	0,
	KB(1),
	KB(2),
};

char const *eepPic16str_vector[MAXEEPSUBTYPE] = {
//	"PIC16XX Auto",
	"PIC16X83",
	"PIC16X84",
	0
};

//-----------------------------
long eepPic168xx_vector[MAXEEPSUBTYPE] = {
	PIC16800,
	PIC1684A,
	PIC16870,
	PIC16871,
	PIC16872,

	PIC16873,
	PIC16874,
	PIC16876,
	PIC16877,

	PIC16873A,
	PIC16874A,
	PIC16876A,
	PIC16877A,

	PIC16627,
	PIC16628
};

int eepPic168xx_size[MAXEEPSUBTYPE] = {
	AUTOSIZE_ID,
	KB(2)+64,
	KB(4)+64,
	KB(4)+64,
	KB(4)+64,

	KB(8)+128,
	KB(8)+128,
	KB(16)+256,
	KB(16)+256,

	KB(8)+128,
	KB(8)+128,
	KB(16)+256,
	KB(16)+256,

	KB(1)+128,
	KB(2)+128
};
int eepPic168xx_split[MAXEEPSUBTYPE] = {
	0,
	KB(2),
	KB(4),
	KB(4),
	KB(4),

	KB(8),
	KB(8),
	KB(16),
	KB(16),

	KB(8),
	KB(8),
	KB(16),
	KB(16),

	KB(1),
	KB(2)
};

char const *eepPic168xxstr_vector[MAXEEPSUBTYPE] = {
	"PIC168XX Auto",
	"PIC16F84A",
	"PIC16F870",
	"PIC16F871",
	"PIC16F872",

	"PIC16F873",
	"PIC16F874",
	"PIC16F876",
	"PIC16F877",

	"PIC16F873A",
	"PIC16F874A",
	"PIC16F876A",
	"PIC16F877A",

	"PIC16F627",
	"PIC16F628",
	0
};

//-----------------------------
long eepPic125_vector[MAXEEPSUBTYPE] = {
//	PIC12500,
	PIC12508,
	PIC12509,
	PIC12508A,
	PIC12509A,
	PIC12E518,
	PIC12E519,
	PIC12671,
	PIC12672,
	PIC12673,
	PIC12674
};

int eepPic125_size[MAXEEPSUBTYPE] = {
//	0,
	KB(1),
	KB(2),
	KB(1),
	KB(2),
	KB(1),
	KB(2),
	KB(1),
	KB(2),
	KB(1),
	KB(2)
};

int eepPic125_split[MAXEEPSUBTYPE] = {
//	0,
	KB(1),
	KB(2),
	KB(1),
	KB(2),
	KB(1),
	KB(2),
	KB(1),
	KB(2),
	KB(1),
	KB(2)
};

char const *eepPic125str_vector[MAXEEPSUBTYPE] = {
//	"PIC125XX Auto",
	"PIC12C508",
	"PIC12C509",
	"PIC12C508A",
	"PIC12C509A",
	"PIC12E518",
	"PIC12E519",
	"PIC12C671",
	"PIC12C672",
	"PIC12CE673",
	"PIC12CE674",
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



//-------------------------
long eepnvm3060_vector[MAXEEPSUBTYPE] = {
//	ENVM3000,
	EMDA2062,
	ENVM3060,
};

int eepnvm3060_size[MAXEEPSUBTYPE] = {
//	0,
	128,
	512,
};

char const *eepnvm3060str_vector[MAXEEPSUBTYPE] = {
//	"NVMXX Auto",
	"MDA2062",
	"NVM3060",
	0
};


//--------------------------
long eep17xxx_vector[MAXEEPSUBTYPE] = {
//	AT1700,
	AT1765,
	AT17128,
	AT17256,
	AT17512,
	AT17010
};

int eep17xxx_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi
//	0,
	KB(8),
	KB(16),
	KB(32),
	KB(64),
	KB(128)
};

char const *eep17xxxstr_vector[MAXEEPSUBTYPE] = {
//	"Auto",
	"AT1765",
	"AT17128",
	"AT17256",
	"AT17512",
	"AT17010",
	0
};

//-------------------------
long eep2444_vector[MAXEEPSUBTYPE] = {
//	XX,
	S24H30,
	X24C44,
	X24C45
};

int eep2444_size[MAXEEPSUBTYPE] = {	//dimensione in numero di banchi (da 2 byte)
//	0,
	8,		//S24H30
	16,		//X24C44
	16		//X24C45
};

int eep2444_addrsize[MAXEEPSUBTYPE] = {	//dimensione dello spazio di indirizzamento in numero di banchi (da 2 byte)
//	0,
	8,
	16,		//X24C44
	16		//X24C45
};

char const *eep2444str_vector[MAXEEPSUBTYPE] = {
//	"XX Auto",
	"S2430",
	"X2444",
	"X2445",
	0
};



static void GetArrayPtrs(int pritype, long* &vp, int* &sp, int* &asp, int* &ssp, const char ** &rp, int* &wp)
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
	case E24XX1_A:
		asp = sp = eep24xx1A_size;
		vp = eep24xx1A_vector;
		rp = eep24xx1Astr_vector;
		break;
	case E24XX1_B:
		asp = sp = eep24xx1B_size;
		vp = eep24xx1B_vector;
		rp = eep24xx1Bstr_vector;
		break;
	case E24XX2:
		asp = sp = eep24xx2_size;
		vp = eep24xx2_vector;
		rp = eep24xx2str_vector;
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
		wp = eepAt90s_wpagesize;
		break;
	case AT89SXX:
		ssp = eepAt89s_split;
		asp = sp = eepAt89s_size;
		vp = eepAt89s_vector;
		rp = eepAt89str_vector;
		wp = eepAt89s_wpagesize;
		break;
	case E93X6:
		asp = eep93x6_addrsize;
		sp = eep93x6_size;
		vp = eep93x6_vector;
		rp = eep93x6str_vector;
		break;
	case E93XX_8:
		asp = eep93xx8_addrsize;
		sp = eep93xx8_size;
		vp = eep93xx8_vector;
		rp = eep93xx8str_vector;
		break;
	case PIC16XX:
		ssp = eepPic16_split;
		asp = sp = eepPic16_size;
		vp = eepPic16_vector;
		rp = eepPic16str_vector;
		break;
	case PIC168XX:
		ssp = eepPic168xx_split;
		asp = sp = eepPic168xx_size;
		vp = eepPic168xx_vector;
		rp = eepPic168xxstr_vector;
		break;
  	case PIC125XX:
		ssp = eepPic125_split;
		asp = sp = eepPic125_size;
		vp = eepPic125_vector;
		rp = eepPic125str_vector;
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
	case ENVMXXX:
		sp = eepnvm3060_size;
		vp = eepnvm3060_vector;
		rp = eepnvm3060str_vector;
		break;
	case AT17XXX:
		asp = sp = eep17xxx_size;
		vp = eep17xxx_vector;
		rp = eep17xxxstr_vector;
		break;
	case X24C44XX:
		asp = eep2444_addrsize;
		sp = eep2444_size;
		vp = eep2444_vector;
		rp = eep2444str_vector;
		break;
	default:
		ssp = asp = sp = 0;
		vp = 0;
		rp = 0;
		wp = 0;
		break;
	}
}


long GetEEPTypeFromSize(int pritype, int size)
{
	int *sp, *asp, *ssp;
	long *vp = 0;
	char const **rp;
	int *wp;

	GetArrayPtrs(pritype, vp, sp, asp, ssp, rp, wp);

	if (sp == 0 || vp == 0)
		return -1;

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
	int *wp;

	if (pritype == 0)
		return 0;

	GetArrayPtrs(pritype, vp, sp, asp, ssp, rp, wp);

	if (sp == 0)
		return 0;

	long type = BuildE2PType(pritype, subtype);
	int index = GetEEPSubTypeIndex(type);
	if (index >= 0)
		return sp[index];
	else
		return 0;
}

//Serve solamente nei bus che utilizzano la dimensione
// di word degli indirizzi dinamica (per es. Microwire)
int GetEEPAddrSize(int pritype, int subtype)
{
	int *sp, *asp = 0, *ssp;
	long *vp;
	char const **rp;
	int *wp;

	GetArrayPtrs(pritype, vp, sp, asp, ssp, rp, wp);

	if (asp == 0)
		return -1;

	long type = BuildE2PType(pritype, subtype);
	int index = GetEEPSubTypeIndex(type);
	if (index >= 0)
		return asp[index];
	else
		return -1;
}

int GetEEPTypeSplit(int pritype, int subtype)
{
	int *sp, *asp, *ssp = 0;
	long *vp;
	char const **rp;
	int *wp;

	GetArrayPtrs(pritype, vp, sp, asp, ssp, rp, wp);

	if (ssp)
	{
		long type = BuildE2PType(pritype, subtype);
		int index = GetEEPSubTypeIndex(type);
		if (index >= 0)
			return ssp[index];
		else
			return 0;
	}
	else
		return 0;
}

int GetEEPTypeWPageSize(int pritype, int subtype)
{
	int *sp, *asp, *ssp = 0;
	long *vp;
	char const **rp;
	int *wp = 0;

	GetArrayPtrs(pritype, vp, sp, asp, ssp, rp, wp);

	if (wp)
	{
		long type = BuildE2PType(pritype, subtype);
		int index = GetEEPSubTypeIndex(type);
		if (index >= 0)
			return wp[index];
		else
			return 0;
	}
	else
		return 0;
}

char const **GetEEPSubTypeList(int type)
{
	int *sp, *asp, *ssp;
	long *vp;
	char const **rp = 0;
	int *wp;

	GetArrayPtrs(type, vp, sp, asp, ssp, rp, wp);

	return rp;
}

long *GetEEPSubTypeVector(int type)
{
	int *sp, *asp, *ssp;
	long *vp = 0;
	char const **rp;
	int *wp;

	GetArrayPtrs(type, vp, sp, asp, ssp, rp, wp);

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
	char const **rp = GetEEPSubTypeList(pritype);
	if (rp)
		return rp[idx];
	else
		return 0;
}

//18/10/98
char const *GetEEPTypeString(long type)
{
	int pritype = GetE2PPriType(type);

	int idx = GetEEPSubTypeIndex(type);
	char const **rp = GetEEPSubTypeList(pritype);
	if (rp)
		return rp[idx];
	else
		return 0;
}

/**
long GetEEPTypeFromString(char const *name)
{
	char const **rp;
	long *vp;
	long ret_type = 0;

	int k;
	for (k = 0; k < NO_OF_EEPTYPE; k++)
	{
		long new_type = CbxIdToType(k, 0);
		if (new_type > 0)
		{
			vp = GetEEPSubTypeVector( GetE2PPriType(new_type) );
			rp = GetEEPSubTypeList( GetE2PPriType(new_type) );
			if (rp && vp)
			{
				int j;
				for (j = 0; rp[j] != 0; j++)
					if (strcasecmp(rp[j], name) == 0)
						break;
				if (rp[j])
				{
					ret_type = vp[j];
					break;
				}
			}
		}
	}

	return ret_type;
}
**/

const FuseBit eep_fusebits[] = {
//	{ AT90S0000,
//		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
//		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },

	{ AT90S1200,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	{ AT90S2313,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	{ AT90S4414,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	{ AT90S8515,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },

	{ AT90S4434,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x01,{0,0,0,0,0,0,0,"FSTRT "} },
	{ AT90S8535,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x01,{0,0,0,0,0,0,0,"FSTRT "} },

	{ AT90S2333,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x1f,{0,0,"SPIEN ","BODLEVEL ","BODEN ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ AT90S4433,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x1f,{0,0,"SPIEN ","BODLEVEL ","BODEN ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },

	{ AT90S2323,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x01,{0,0,0,0,0,0,0,"FSTRT "} },
	{ AT90S2343,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x01,{0,0,0,0,0,0,0,"RCEN "} },

	{ AT90S8534,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },

//	{ ATtiny10,
//		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
//		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
//	{ ATtiny11,
//		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
//		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x17,{0,0,0,"FSTRT ","RSTDISBL ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATtiny12,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0xCF,{"BODLEVEL ","BODEN ","SPIEN ","RSTDISBL ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATtiny13,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x03,{0,0,0,0,0, 0,"Lock2 ","Lock1 "},
		0x00,{0}, 0x00,{0}, 0x1E,{0,0,0,"SELFPRGEN ","DWEN ","BODLEVEL1 ","BODLEVEL0 ","RSTDISBL "}, 0x7F,{"SPIEN ","EESAVE ","WDTON ","CKDIV8 ","SUT1 ","SUT0 ","CKSEL1 ","CKSEL0 "} },
	{ ATtiny15,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0xC3,{"BODLEVEL ","BODEN ","SPIEN ","RSTDISBL ",0,0,"CKSEL1 ","CKSEL0 "} },
	{ ATtiny22,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x01,{0,0,0,0,0,0,0,"RCEN "} },
	{ ATtiny26,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x03,{0,0,0,0,0, 0,"Lock2 ","Lock1 "},
		0x00,{0}, 0x00,{0}, 0x07,{0,0,0,"RSTDISBL ","SPIEN ","EESAVE ","BODLEVEL ","BODEN "}, 0xFF,{"PLLCK ","CKOPT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
//	{ ATtiny28,
//		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
//		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },

	{ ATtiny2313,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x03,{0,0,0,0,0, 0,"Lock2 ","Lock1 "},
		0x00,{0}, 0x01,{0,0,0,0, 0,0,0, "SPMEN "}, 0xDE,{"DWEN ","EESAVE ","SPIEN ","WDTON ","BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 ","RSTDISBL "}, 0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATtiny25,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x03,{0,0,0,0,0, 0,"Lock2 ","Lock1 "},
		0x00,{0}, 0x01,{0,0,0,0, 0,0,0, "SELFPRGEN "}, 0x5F,{"RSTDISBL ","DWEN ","SPIEN ","WDTON ","EESAVE ","BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATtiny45,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x03,{0,0,0,0,0, 0,"Lock2 ","Lock1 "},
		0x00,{0}, 0x01,{0,0,0,0, 0,0,0, "SELFPRGEN "}, 0x5F,{"RSTDISBL ","DWEN ","SPIEN ","WDTON ","EESAVE ","BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATtiny85,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x03,{0,0,0,0,0, 0,"Lock2 ","Lock1 "},
		0x00,{0}, 0x01,{0,0,0,0, 0,0,0, "SELFPRGEN "}, 0x5F,{"RSTDISBL ","DWEN ","SPIEN ","WDTON ","EESAVE ","BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATtiny261,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x03,{0,0,0,0,0, 0,"Lock2 ","Lock1 "},
		0x00,{0}, 0x01,{0,0,0,0, 0,0,0, "SELFPRGEN "}, 0x5F,{"RSTDISBL ","DWEN ","SPIEN ","WDTON ","EESAVE ","BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATtiny461,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x03,{0,0,0,0,0, 0,"Lock2 ","Lock1 "},
		0x00,{0}, 0x01,{0,0,0,0, 0,0,0, "SELFPRGEN "}, 0x5F,{"RSTDISBL ","DWEN ","SPIEN ","WDTON ","EESAVE ","BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATtiny861,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x03,{0,0,0,0,0, 0,"Lock2 ","Lock1 "},
		0x00,{0}, 0x01,{0,0,0,0, 0,0,0, "SELFPRGEN "}, 0x5F,{"RSTDISBL ","DWEN ","SPIEN ","WDTON ","EESAVE ","BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega48,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x01,{0,0,0,0, 0,0,0, "SELFPRGEN "}, 0x5F,{"RSTDISBL ","DWEN ","SPIEN ","WDTON ","EESAVE ","BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega88,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x07,{0,0,0,0, 0,"BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0x5F,{"RSTDISBL ","DWEN ","SPIEN ","WDTON ","EESAVE ","BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega168,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x07,{0,0,0,0, 0,"BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0x5F,{"RSTDISBL ","DWEN ","SPIEN ","WDTON ","EESAVE ","BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega328, // new (RG 22.06.2012)
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x07,{0,0,0,0, 0,"BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0x5F,{"RSTDISBL ","DWEN ","SPIEN ","WDTON ","EESAVE ","BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega164,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x07,{0,0,0,0, 0,"BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","WDTON ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "},	0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega324,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x07,{0,0,0,0, 0,"BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","WDTON ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "},	0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega644,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x07,{0,0,0,0, 0,"BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","WDTON ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "},	0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega640,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x07,{0,0,0,0, 0,"BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","WDTON ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "},	0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega1280,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x07,{0,0,0,0, 0,"BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","WDTON ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "},	0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega1281,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x07,{0,0,0,0, 0,"BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","WDTON ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "},	0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega2560,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x07,{0,0,0,0, 0,"BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","WDTON ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "},	0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega2561,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x07,{0,0,0,0, 0,"BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","WDTON ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "},	0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ AT90CAN32,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x0E,{0,0,0,0, "BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 ","TA0SEL "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","WDTON ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "},	0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ AT90CAN64,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x0E,{0,0,0,0, "BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 ","TA0SEL "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","WDTON ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "},	0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ AT90CAN128,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x0E,{0,0,0,0, "BODLEVEL2 ","BODLEVEL1 ","BODLEVEL0 ","TA0SEL "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","WDTON ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "},	0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },

	{ ATmega603,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x0B,{0,0,0,0,"EESAVE ",0,"SUT1 ","SUT0 "}},
	{ ATmega103,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x0B,{0,0,0,0,"EESAVE ",0,"SUT1 ","SUT0 "}},
	{ ATmega8,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F, {0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x00,{0}, 0x5F,{"RSTDISBL ","WDTON  ","SPIEN ","CKOPT ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0xFF,{"BODLEVEL ","BODEN ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega8515,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F, {0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x00,{0}, 0xDF,{"S8515C ","WDTON  ","SPIEN ","CKOPT ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0xFF,{"BODLEVEL ","BODEN ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega8535,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F, {0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x00,{0}, 0xDF,{"S8535C ","WDTON  ","SPIEN ","CKOPT ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0xFF,{"BODLEVEL ","BODEN ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega64,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F, {0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x03,{0,0,0,0, 0,0, "M103C ", "WDTON  "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","CKOPT ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0xFF,{"BODLEVEL ","BODEN ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega128,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F, {0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x03,{0,0,0,0, 0,0, "M103C ", "WDTON  "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","CKOPT ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0xFF,{"BODLEVEL ","BODEN ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega16,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F, {0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x00,{0}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","CKOPT ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0xFF,{"BODLEVEL ","BODEN ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega161,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x7F,{0,"BOOTRST ","SPIEN ","BODLEVEL ","BODEN ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega163,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F,{0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x00,{0}, 0x07,{0,0,0,0,0,"BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0xCF,{"BODLEVEL ","BODEN ",0,0,"CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega162,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F, {0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x1E,{0,0,0,"M161C ","BOD2LEVEL ","BOD1LEVEL ","BOD0LEVEL ",0}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","WDTON  ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega169,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F, {0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x0E,{0,0,0,0,"BOD2LEVEL ","BOD1LEVEL ","BOD0LEVEL ","RSTDISBL "}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","WDTON  ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0xFF,{"CKDIV8 ","CKOUT ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega323,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F, {0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x00,{0}, 0xCF,{"OCDEN ","JTAGEN ","SPIEN ",0,"EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0xCF,{"BODLEVEL ","BODEN ",0,0,"CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{ ATmega32,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x3F, {0,0,"BootLock12 ","BootLock11 ","BootLock02 ","BootLock01 ","Lock2 ","Lock1 "},
		0x00,{0}, 0x00,{0}, 0xDF,{"OCDEN ","JTAGEN ","SPIEN ","CKOPT ","EESAVE ","BOOTSZ1 ","BOOTSZ0 ","BOOTRST "}, 0xFF,{"BODLEVEL ","BODEN ","SUT1 ","SUT0 ","CKSEL3 ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },

	{ AT89S8252,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0xE0,{"LB1 ","LB2 ","LB3 ",0, 0,0,0,0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	{ AT89S8253,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x07,{0,0,0,0, 0,"LB1 ","LB2 ","LB3 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x0F,{0,0,0,0, "ClkSel ", "UserRow ", "x2Mode ", "SerProg "} },
	{ AT89S51,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x1C,{0,0,0,"LB1 ", "LB2 ","LB3 ",0,0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	{ AT89S52,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x1C,{0,0,0,"LB1 ", "LB2 ","LB3 ",0,0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	{ AT89S53,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0xE0,{"LB1 ","LB2 ","LB3 ",0, 0,0,0,0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },

	{ PIC12508,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x1F,{0,0,0,"MCLRE ","CP ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	{ PIC12509,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x1F,{0,0,0,"MCLRE ","CP ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	{ PIC12508A,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x1F,{0,0,0,"MCLRE ","CP ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	{ PIC12509A,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x1F,{0,0,0,"MCLRE ","CP ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	{ PIC12E518,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x1F,{0,0,0,"MCLRE ","CP ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	{ PIC12E519,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x1F,{0,0,0,"MCLRE ","CP ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },

 	{ PIC12671,
		0x00,{0}, 0x00,{0}, 0x3F,{0,0,"CP1 ","CP0 ","CP1 ","CP0 ","CP1 ","CP0 "}, 0xFF,{"MCLRE ","CP1 ","CP0 ","PWRTE ","WDTE ","FOSC2 ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
 	{ PIC12672,
		0x00,{0}, 0x00,{0}, 0x3F,{0,0,"CP1 ","CP0 ","CP1 ","CP0 ","CP1 ","CP0 "}, 0xFF,{"MCLRE ","CP1 ","CP0 ","PWRTE ","WDTE ","FOSC2 ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
 	{ PIC12673,
		0x00,{0}, 0x00,{0}, 0x3F,{0,0,"CP1 ","CP0 ","CP1 ","CP0 ","CP1 ","CP0 "}, 0xFF,{"MCLRE ","CP1 ","CP0 ","PWRTE ","WDTE ","FOSC2 ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
 	{ PIC12674,
		0x00,{0}, 0x00,{0}, 0x3F,{0,0,"CP1 ","CP0 ","CP1 ","CP0 ","CP1 ","CP0 "}, 0xFF,{"MCLRE ","CP1 ","CP0 ","PWRTE ","WDTE ","FOSC2 ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },

	{ PIC1683,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x1F,{0,0,0,"CP ","PWRTE ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	{ PIC1684,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x1F,{0,0,0,"CP ","PWRTE ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	{ PIC1684A,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x1F,{0,0,0,"CP ","PWRTE ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },

	{ PIC16870,
		0x00,{0}, 0x00,{0}, 0x3B,{0,0,"CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "}, 0xFF,{"LVP ","BODEN ","CP1 ","CP0 ","PWRTE ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} },
	{ PIC16871,
		0x00,{0}, 0x00,{0}, 0x3B,{0,0,"CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "}, 0xFF,{"LVP ","BODEN ","CP1 ","CP0 ","PWRTE ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} },
	{ PIC16872,
		0x00,{0}, 0x00,{0}, 0x3B,{0,0,"CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "}, 0xFF,{"LVP ","BODEN ","CP1 ","CP0 ","PWRTE ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} },
	{ PIC16873,
		0x00,{0}, 0x00,{0}, 0x3B,{0,0,"CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "}, 0xFF,{"LVP ","BODEN ","CP1 ","CP0 ","PWRTE ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} },
	{ PIC16874,
		0x00,{0}, 0x00,{0}, 0x3B,{0,0,"CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "}, 0xFF,{"LVP ","BODEN ","CP1 ","CP0 ","PWRTE ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} },
	{ PIC16876,
		0x00,{0}, 0x00,{0}, 0x3B,{0,0,"CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "}, 0xFF,{"LVP ","BODEN ","CP1 ","CP0 ","PWRTE ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} },
	{ PIC16877,
		0x00,{0}, 0x00,{0}, 0x3B,{0,0,"CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "}, 0xFF,{"LVP ","BODEN ","CP1 ","CP0 ","PWRTE ","WDTE ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} },
	{ PIC16873A,
		0x00,{0}, 0x00,{0}, 0x2F,{0,0,"CP ",0,"DEBUG ","WRT1 ","WRT0 ", "CPD "}, 0xCF,{"LVP ","BOREN ",0,0,"/PWRTEN ","WDTEN ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} },
	{ PIC16874A,
		0x00,{0}, 0x00,{0}, 0x2F,{0,0,"CP ",0,"DEBUG ","WRT1 ","WRT0 ", "CPD "}, 0xCF,{"LVP ","BOREN ",0,0,"/PWRTEN ","WDTEN ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} },
	{ PIC16876A,
		0x00,{0}, 0x00,{0}, 0x2F,{0,0,"CP ",0,"DEBUG ","WRT1 ","WRT0 ", "CPD "}, 0xCF,{"LVP ","BOREN ",0,0,"/PWRTEN ","WDTEN ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} },
	{ PIC16877A,
		0x00,{0}, 0x00,{0}, 0x2F,{0,0,"CP ",0,"DEBUG ","WRT1 ","WRT0 ", "CPD "}, 0xCF,{"LVP ","BOREN ",0,0,"/PWRTEN ","WDTEN ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} },
	{ PIC16627,
		0x00,{0}, 0x00,{0}, 0x3D,{0,0,"CP1 ","CP0 ","CP1 ","CP0 ",0, "CPD "}, 0xFF,{"LVP ","BOREN ","MCLRE ","FOSC2 ","/PWRTEN ","WDTEN ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} },
	{ PIC16628,
		0x00,{0}, 0x00,{0}, 0x3D,{0,0,"CP1 ","CP0 ","CP1 ","CP0 ",0, "CPD "}, 0xFF,{"LVP ","BOREN ","MCLRE ","FOSC2 ","/PWRTEN ","WDTEN ","FOSC1 ","FOSC0 "},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} },

	{ 0,
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0},
		0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00, {0} }	//EndTag
};

int eep_FindFuses(long type)
{
	int k;
	int retval = -1;

	for (k = 0; eep_fusebits[k].type; k++)
	{
		if (eep_fusebits[k].type == type)
		{
			retval = k;
			break;
		}
	}

	return retval;
}

