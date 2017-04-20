//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: eeptypes.h,v 1.7 2013/10/31 16:10:58 lancos Exp $
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

#ifndef _EEPTYPES_H
#define _EEPTYPES_H

#include "globals.h"

#include <QVector>
#include <QString>
#include <QStringList>

#define E24XX           0x01
// Sub types
#define E2400           0x010000
#define E2402           0x010001
#define E2404           0x010002
#define E2408           0x010004
#define E2416           0x010008

#define E24XX2          0x02
// Sub types
#define E24000          0x020000
#define E24E16          0x020008
#define E2432           0x020010
#define E2464           0x020020
#define E24128          0x020040
#define E24256          0x020080
#define E24512          0x020100

#define AT90SXX         0x03
// Sub types
#define AT90S0000       0x030000
#define AT90S1200       0x030002
#define AT90S2313       0x030003
#define AT90S4414       0x030004
#define AT90S8515       0x030005

#define ATmega603       0x030008
#define ATmega103       0x030009

#define AT90S2323       0x03000A
#define AT90S2343       0x03000B
#define AT90S2333       0x03000C
#define AT90S4433       0x03000D
#define AT90S4434       0x03000E
#define AT90S8534       0x03000F
#define AT90S8535       0x030010

#define ATtiny22        0x030011
#define ATtiny12        0x030012
#define ATtiny13        0x030013
#define ATtiny15        0x030014

#define ATmega8515      0x030015
#define ATmega8535      0x030016

#define ATmega8         0x03001A
#define ATmega161       0x030020
#define ATmega163       0x030021
#define ATmega16        0x030022
#define ATmega323       0x030023
#define ATmega32        0x030024
#define ATmega162       0x030025
#define ATmega169       0x030026
#define ATmega164       0x030027
#define ATmega324       0x030028
#define ATmega644       0x030029

#define ATmega64        0x03002A
#define ATmega128       0x030030

#define ATmega48        0x030031
#define ATmega88        0x030032
#define ATmega168       0x030033
#define ATmega640       0x030034
#define ATmega1280      0x030035
#define ATmega1281      0x030036
#define ATmega2560      0x030037
#define ATmega2561      0x030038
#define ATmega328       0x030039 // new (RG 22.06.2012)

#define ATtiny11        0x030080
#define ATtiny10        0x030081
#define ATtiny28        0x030082
#define ATtiny26        0x030083
#define ATtiny2313      0x030084
#define ATtiny25        0x030085
#define ATtiny45        0x030086
#define ATtiny85        0x030087
#define ATtiny261       0x030088
#define ATtiny461       0x030089
#define ATtiny861       0x03008A

#define AT90CAN32       0x030100
#define AT90CAN64       0x030101
#define AT90CAN128      0x030102

#define E93X6           0x04
// Sub types
#define E9300           0x040000
#define E9306           0x040001
#define E9346           0x040004
#define E9356           0x040008
#define E9357           0x040009
#define E9366           0x040010
#define E9376           0x040020
#define E9386           0x040040

#define PIC16XX         0x05
// Sub types
#define PIC1600         0x050000
#define PIC1684         0x050001
#define PIC1683         0x050002

#define E250XX          0x06
// Sub types
#define E25000          0x060000
#define E25010          0x060001
#define E25020          0x060002
#define E25040          0x060004

#define E2506XX         0x07
// Sub types
#define E250600         0x070000
#define E2506           0x070001

#define E24XX5          0x08
// Sub types
#define E24325          0x080010
#define E24645          0x080020

#define E25XXX          0x09
// Sub types
#define E25XX0          0x090000
#define E25080          0x090004
#define E25160          0x090008
#define E25320          0x090010
#define E25640          0x090020
#define E25128          0x090040
#define E25256          0x090080

#define AT89SXX         0x0A
// Sub types
#define AT89S0000       0x0A0000
#define AT89S8252       0x0A0002
#define AT89S8253       0x0A0003
#define AT89S53         0x0A0004
#define AT89S52         0x0A0005
#define AT89S51         0x0A0006

#define PIC168XX        0x0B
// Sub types
#define PIC16800        0x0B0000
#define PIC1684A        0x0B0001
#define PIC16870        0x0B0002
#define PIC16871        0x0B0003
#define PIC16872        0x0B0004
#define PIC16873        0x0B0005
#define PIC16874        0x0B0006
#define PIC16876        0x0B0008
#define PIC16877        0x0B0009
#define PIC16873A       0x0B000A
#define PIC16874A       0x0B000B
#define PIC16876A       0x0B000C
#define PIC16877A       0x0B000D

#define PIC16627        0x0B0020
#define PIC16628        0x0B0021

#define ENVMXXX         0x0C
// Sub types
#define ENVM3000        0x0C0000
#define EMDA2061        0x0C0002
#define EMDA2062        0x0C0003
#define ENVM3060        0x0C0004

#define E93XX_8         0x0D
// Sub types
#define E9300_8         0x0d0000
#define E9306_8         0x0d0001
#define E9346_8         0x0d0004
#define E9356_8         0x0d0008
#define E9357_8         0x0d0009
#define E9366_8         0x0d0010
#define E9376_8         0x0d0020
#define E9386_8         0x0d0040

#define PIC125XX        0x0E
// Sub types
#define PIC12500        0x0E0000
#define PIC12508        0x0E0004
#define PIC12509        0x0E0005
#define PIC12508A       0x0E0008
#define PIC12509A       0x0E0009
#define PIC12E518       0x0E0010
#define PIC12E519       0x0E0011

#define PIC12608        0x0E0024
#define PIC12609        0x0E0025
#define PIC12608A       0x0E0026
#define PIC12609A       0x0E0027
#define PIC12671        0x0E0030
#define PIC12672        0x0E0031
#define PIC12673        0x0E0032
#define PIC12674        0x0E0033

//#define PIC162XX      0x0F
// Sub types
//#define       PIC16200        0x0F0000
//#define       PIC1627         0x0F0004
//#define       PIC1628         0x0F0005

#define X24C44XX        0x10
// Sub types
#define S24H30          0x100001
#define X24C44          0x100002
#define X24C45          0x100003

#define E24XX1_A        0x11
// Sub types
#define E2401_A         0x110001

#define AT17XXX         0x12
// Sub types
#define AT1765          0x120002
#define AT17128         0x120003
#define AT17256         0x120004
#define AT17512         0x120005
#define AT17010         0x120006

#define E24XX1_B        0x13
// Sub types
#define E2401_B         0x130001

#define NO_OF_EEPTYPE   0x13

#define MAXEEPSUBTYPE   64


#define KB(x)   ((x) * 1024)



// EK 2017
// common structure for chips
struct chipInfo
{
	QString name;
	long id;    // pretype | subtype
	int  sz;    //dimensione in numero di banchi
	int  splt;  // split
	int  wpgsz; // page size
	int  adrsz; //dimensione dello spazio di indirizzamento in numero di banchi
};


//AutoTag
//Device type description vectors


static QVector<chipInfo> const eep24xx_map =
{
	{ "24XX Auto", E2400, AUTOSIZE_ID / 256, -1, -1, -1 },
	{"2402", E2402, 1, -1, -1, -1},
	{"2404", E2404, 2, -1, -1, -1},
	{"2408", E2408, 4, -1, -1, -1},
	{"2416", E2416, 8, -1, -1, -1}
};


static QVector<chipInfo> const eep24xx1A_map =
{
	//      "Auto",
	{"2401-A", E2401_A, 1, -1, -1, -1}
};


static QVector<chipInfo> const eep24xx1B_map =
{
	//      "Auto",
	{"2401-B", E2401_B, 1, -1, -1, -1}
};



static QVector<chipInfo> const eep24xx2_map =
{
	//      "Auto",
	{"24E16", E24E16, 8, -1, -1, -1},
	{"2432", E2432, 16, -1, -1, -1},
	{"2464/2465", E2464, 32, -1, -1, -1},
	{"24128", E24128, 64, -1, -1, -1},
	{"24256", E24256, 128, -1, -1, -1},
	{"24512", E24512, 256, -1, -1, -1}
};


static QVector<chipInfo> const eep24xx5_map =
{
	//      "Auto",
	{"24325", E24325, 16, -1, -1, -1},
	{"24645", E24645, 32, -1, -1, -1}
};


static QVector<chipInfo> const eepAt90s_map =
{
	//      "AVR  Auto", AT90S0000
	{"AT90S1200", AT90S1200, (KB(1) + 64), KB(1), 0, -1},
	{"AT90S2313", AT90S2313, (KB(2) + 128), KB(2), 0, -1},
	{"AT90S2323", AT90S2323, (KB(2) + 128), KB(2), 0, -1},
	{"AT90S2333", AT90S2333, (KB(2) + 128), KB(2), 0, -1},
	{"AT90S2343", AT90S2343, (KB(2) + 128), KB(2), 0, -1},
	{"AT90S4414", AT90S4414, (KB(4) + 256), KB(4), 0, -1},
	{"AT90S4433", AT90S4433, (KB(4) + 256), KB(4), 0, -1},
	{"AT90S4434", AT90S4434, (KB(4) + 256), KB(4), 0, -1},
	{"AT90S8515", AT90S8515, (KB(8) + 512), KB(8), 0, -1},
	{"AT90C8534", AT90S8534, (KB(8) + 512), KB(8), 0, -1},
	{"AT90S8535", AT90S8535, (KB(8) + 512), KB(8), 0, -1},
	{"AT90CAN32", AT90CAN32, (KB(32) + KB(1)), KB(32), 128, -1},
	{"AT90CAN64", AT90CAN64, (KB(64) + KB(2)), KB(64), 256, -1},
	{"AT90CAN128", AT90CAN128, (KB(128) + KB(4)), KB(128), 256, -1},

	{"ATmega8515", ATmega8515, (KB(8) + 512), KB(8), 64, -1 },
	{"ATmega8535", ATmega8535, (KB(8) + 512), KB(8), 64, -1 },
	{"ATmega48", ATmega48, (KB(4) + 256), KB(4), 64, -1},
	{"ATmega8", ATmega8, (KB(8) + 512), KB(8), 64, -1},
	{"ATmega88", ATmega88, (KB(8) + 512), KB(8),  64, -1},
	{"ATmega16", ATmega16, (KB(16) + 512), KB(16), 128, -1},
	{"ATmega161", ATmega161, (KB(16) + 512), KB(16), 128, -1},
	{"ATmega162", ATmega162, (KB(16) + 512), KB(16), 128, -1},
	{"ATmega163", ATmega163, (KB(16) + 512), KB(16), 128, -1},
	{"ATmega164", ATmega164, (KB(16) + 512), KB(16), 128, -1},
	{"ATmega168", ATmega168, (KB(16) + 512), KB(16), 128, -1},
	{"ATmega169", ATmega169, (KB(16) + 512), KB(16), 128, -1},
	{"ATmega32", ATmega32, (KB(32) + KB(1)), KB(32), 128, -1},

	{"ATmega323", ATmega323, (KB(32) + KB(1)), KB(32), 128, -1},
	{"ATmega324", ATmega324, (KB(32) + KB(1)), KB(32), 128, -1},
	{"ATmega328", ATmega328, (KB(32) + KB(1)), KB(32), 128, -1}, // new (RG 22.06.2012)
	{"ATmega603", ATmega603, (KB(64) + KB(2)), KB(64), 256, -1},
	{"ATmega103", ATmega103, (KB(128) + KB(4)), KB(128), 256, -1},
	{"ATmega64", ATmega64, (KB(64) + KB(2)), KB(64), 256, -1},
	{"ATmega640", ATmega640, (KB(64) + KB(4)), KB(64), 256, -1},
	{"ATmega644", ATmega644, (KB(64) + KB(2)), KB(64), 256, -1},
	{"ATmega128", ATmega128, (KB(128) + KB(4)), KB(128), 256, -1},
	{"ATmega1280", ATmega1280, (KB(128) + KB(4)), KB(128), 256, -1},
	{"ATmega1281", ATmega1281, (KB(128) + KB(4)), KB(128), 256, -1},
	{"ATmega2560", ATmega2560, (KB(256) + KB(4)), KB(256), 256, -1},
	{"ATmega2561", ATmega2561, (KB(256) + KB(4)), KB(256), 256, -1},

	{"ATtiny12", ATtiny12, (KB(1) + 64), KB(1), 0, -1},
	{"ATtiny13", ATtiny13, (KB(1) + 64), KB(1), 32, -1},
	{"ATtiny15", ATtiny15, (KB(1) + 64), KB(1), 0, -1},
	{"ATtiny22", ATtiny22, (KB(2) + 128), KB(2), 0, -1},
	{"ATtiny25", ATtiny25, (KB(2) + 128), KB(2), 32, -1},
	{"ATtiny26", ATtiny26, (KB(2) + 128), KB(2), 32, -1},
	{"ATtiny261", ATtiny261, (KB(2) + 128), KB(2), 32, -1},
	//      {"ATtiny28", ATtiny28, },
	{"ATtiny2313", ATtiny2313, (KB(2) + 128), KB(2), 32, -1},
	{"ATtiny45", ATtiny45, (KB(4) + 256), KB(4), 64,  -1},
	{"ATtiny461", ATtiny461, (KB(4) + 256), KB(4), 64, -1},
	{"ATtiny85", ATtiny85, (KB(8) + 512), KB(8), 64, -1},
	{"ATtiny861", ATtiny861, (KB(8) + 512), KB(8), 64, -1}
};


static QVector<chipInfo> const eepAt89s_map =
{
	//      "AT89SXX Auto", AT89S0000
	{"AT89S8252", AT89S8252, KB(8) + KB(2), KB(8), 0, -1 },
	{"AT89S8253", AT89S8253, KB(12) + KB(2), KB(12), 64, 1 },
	{"AT89S51", AT89S51, KB(4), KB(4), 256, -1 },
	{"AT89S52", AT89S52, KB(8), KB(8), 256, -1 },
	{"AT89S53", AT89S53, KB(12), KB(12), 0, -1 }
};


static QVector<chipInfo> const eep93x6_map =
{
	//      "93XX Auto", E9300
	{"9306", E9306, 16 , -1, -1, 64},
	{"9346", E9346, 64, -1, -1, 64},
	{"9356", E9356, 128, -1, -1, 256},
	{"9357", E9357, 128, -1, -1, 128},
	{"9366", E9366, 256, -1, -1, 256},
	{"9376", E9376, 512, -1, -1, 1024},
	{"9386", E9386, 1024, -1, -1, 1024}
};


static QVector<chipInfo> const eep93xx8_map =
{
	//      "93XX Auto",
	{"9306", E9306_8, 32, -1, -1, 128},
	{"9346", E9346_8, 128, -1, -1, 128},
	{"9356", E9356_8, 256, -1, -1, 512},
	{"9357", E9357_8, 256, -1, -1, 256},
	{"9366", E9366_8, 512, -1, -1, 512},
	{"9376", E9376_8, 1024, -1, -1, 2048},
	{"9386", E9386_8, 2048, -1, -1, 2048},
};


static QVector<chipInfo> const eepPic16_map =
{
	//      "PIC16XX Auto",
	{"PIC16X83", PIC1683, KB(1) + 64, KB(1), -1, -1},
	{"PIC16X84", PIC1684, KB(2) + 64, KB(2), -1, -1}
};


static QVector<chipInfo> const eepPic168xx_map =
{
	{"PIC168XX Auto", PIC16800, AUTOSIZE_ID, 0, -1, -1},
	{"PIC16F84A", PIC1684A, KB(2) + 64, KB(2), -1, -1},
	{"PIC16F870", PIC16870, KB(4) + 64, KB(4), -1, -1},
	{"PIC16F871", PIC16871, KB(4) + 64, KB(4), -1, -1},
	{"PIC16F872", PIC16872, KB(4) + 64, KB(4), -1, -1},

	{"PIC16F873", PIC16873, KB(8) + 128, KB(8), -1, -1},
	{"PIC16F874", PIC16874, KB(8) + 128, KB(8), -1, -1},
	{"PIC16F876", PIC16876, KB(16) + 256, KB(16), -1, -1},
	{"PIC16F877", PIC16877, KB(16) + 256, KB(16), -1, -1},

	{"PIC16F873A", PIC16873A, KB(8) + 128, KB(8), -1, -1},
	{"PIC16F874A", PIC16874A, KB(8) + 128, KB(8), -1, -1},
	{"PIC16F876A", PIC16876A, KB(16) + 256, KB(16), -1, -1},
	{"PIC16F877A", PIC16877A, KB(16) + 256, KB(16), -1, -1},

	{"PIC16F627", PIC16627, KB(1) + 128, KB(1), -1, -1},
	{"PIC16F628", PIC16628, KB(2) + 128, KB(2), -1, -1}
};


static QVector<chipInfo> const eepPic125_map =
{
	//      "PIC125XX Auto",PIC12500
	{"PIC12C508", PIC12508, KB(1), KB(1), -1, -1},
	{"PIC12C509", PIC12509, KB(2), KB(2), -1, -1},
	{"PIC12C508A", PIC12508A, KB(1), KB(1), -1, -1},
	{"PIC12C509A", PIC12509A, KB(2), KB(2), -1, -1},
	{"PIC12E518", PIC12E518, KB(1), KB(1), -1, -1},
	{"PIC12E519", PIC12E519, KB(2), KB(2), -1, -1},
	{"PIC12C671", PIC12671, KB(1), KB(1), -1, -1},
	{"PIC12C672", PIC12672, KB(2), KB(2), -1, -1},
	{"PIC12CE673", PIC12673, KB(1), KB(1), -1, -1},
	{"PIC12CE674", PIC12674, KB(2), KB(2), -1, -1}
};


static QVector<chipInfo> const eep250xx_map =
{
	//      "250X0 Auto",
	{"25010", E25010, 128, -1, -1, -1},
	{"25020", E25020, 256, -1, -1, -1},
	{"25040", E25040, 512, -1, -1, -1}
};


static QVector<chipInfo> const eep25xxx_map =
{
	//      "25XXX Auto",E25XX0
	{"25080", E25080, KB(1), -1, -1, -1},
	{"25160", E25160, KB(2), -1, -1, -1},
	{"25320", E25320, KB(4), -1, -1, -1},
	{"2564X/95640", E25640, KB(8), -1, -1, -1},
	{"25128", E25128, KB(16), -1, -1, -1},
	{"25256", E25256, KB(32), -1, -1, -1}
};


static QVector<chipInfo> const eep2506_map =
{
	//      "250X0 Auto",E25000
	{"SDE2506", E2506, 128, -1, -1, -1}
};


static QVector<chipInfo> const eepnvm3060_map =
{
	//      "NVMXX Auto",ENVM3000
	{"MDA2062", EMDA2062, 128, -1, -1, -1},
	{"NVM3060", ENVM3060, 512, -1, -1, -1}
};


static QVector<chipInfo> const eep2444_map =
{
	//      "XX Auto",
	{"S2430", S24H30, 8, -1, -1, 8},
	{"X2444", X24C44, 16, -1, -1, 16},
	{"X2445", X24C45, 16, -1, -1, 16}
};


static QVector<chipInfo> const eep17xxx_map =
{
	//      "Auto",AT1700
	{"AT1765", AT1765, KB(8), -1, -1, -1},
	{"AT17128", AT17128, KB(16), -1, -1, -1},
	{"AT17256", AT17256, KB(32), -1, -1, -1},
	{"AT17512", AT17512, KB(64), -1, -1, -1},
	{"AT17010", AT17010, KB(128), -1, -1, -1},

};


// extern long BuildE2PType(int pritype, int subtype = 0);
extern long BuildE2PType(unsigned long type);
extern int GetE2PSubType(unsigned long type);
extern int GetE2PPriType(unsigned long type);

chipInfo GetChipInfo(QVector<chipInfo> &c, long subtype);

// extern QString GetEEPTypeString(int pritype, int subtype);
extern QString GetEEPTypeString(unsigned long type);

extern int GetEEPTypeIndex(int type);
// extern QStringList GetEEPSubTypeList(int type);
extern QVector<chipInfo> GetEEPSubTypeVector(int type);
extern int GetEEPSubTypeIndex(unsigned long type);
// extern int GetEEPTypeSize(int pritype, int subtype);
extern int GetEEPTypeSize(unsigned long type);
// extern int GetEEPAddrSize(int pritype, int subtype);
extern int GetEEPAddrSize(unsigned long type);
// extern int GetEEPTypeSplit(int pritype, int subtype);
extern int GetEEPTypeSplit(unsigned long type);

// extern long GetEEPTypeFromSize(int pritype, int size);
extern long GetEEPTypeFromSize(unsigned long type, int size);
extern long GetEEPTypeFromString(const QString &name);

// extern int GetEEPTypeWPageSize(int pritype, int subtype);
extern int GetEEPTypeWPageSize(unsigned long type);


#endif
