//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2021   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
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
// along with this program (see LICENSE);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//=========================================================================//

#include <QString>
#include <QVector>
#include <QDebug>

#include "types.h"
#include "globals.h"
#include "eeptypes.h"



//AutoTag
//Device type description vectors

#if 0
static QVector<icElement> const eep24xx_map =
{
	{"24XX Auto", E2400, AUTOSIZE_ID / 256, -1, -1, -1 },
	{"2402", E2402, 1, -1, -1, -1},
	{"2404", E2404, 2, -1, -1, -1},
	{"2408", E2408, 4, -1, -1, -1},
	{"2416", E2416, 8, -1, -1, -1}
};


static QVector<icElement> const eep24xx1A_map =
{
	//      "Auto",
	{"2401-A", E2401_A, 1, -1, -1, -1}
};


static QVector<icElement> const eep24xx1B_map =
{
	//      "Auto",
	{"2401-B", E2401_B, 1, -1, -1, -1}
};



static QVector<icElement> const eep24xx2_map =
{
	//      "Auto",
	{"24E16", E24E16, 8, -1, -1, -1},
	{"2432", E2432, 16, -1, -1, -1},
	{"2464/2465", E2464, 32, -1, -1, -1},
	{"24128", E24128, 64, -1, -1, -1},
	{"24256", E24256, 128, -1, -1, -1},
	{"24512", E24512, 256, -1, -1, -1}
};


static QVector<icElement> const eep24xx5_map =
{
	//      "Auto",
	{"24325", E24325, 16, -1, -1, -1},
	{"24645", E24645, 32, -1, -1, -1}
};

static QVector<icElement> const eepAT90_map =
{
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
	{"ATmega1284", ATmega1284, (KB(128) + KB(4)), KB(128), 256, -1}, // new (RG 10.06.2017)
	{"ATmega2560", ATmega2560, (KB(256) + KB(4)), KB(256), 256, -1},
	{"ATmega2561", ATmega2561, (KB(256) + KB(4)), KB(256), 256, -1},

	{"ATtiny12", ATtiny12, (KB(1) + 64), KB(1), 0, -1},
	{"ATtiny13", ATtiny13, (KB(1) + 64), KB(1), 32, -1},
	{"ATtiny15", ATtiny15, (KB(1) + 64), KB(1), 0, -1},
	{"ATtiny22", ATtiny22, (KB(2) + 128), KB(2), 0, -1},
	{"ATtiny24", ATtiny24, (KB(2) + 128), KB(2), 32, -1}, // new 08.01.2015 @RG
	{"ATtiny25", ATtiny25, (KB(2) + 128), KB(2), 32, -1},
	{"ATtiny26", ATtiny26, (KB(2) + 128), KB(2), 32, -1},
	{"ATtiny261", ATtiny261, (KB(2) + 128), KB(2), 32, -1},
	//      {"ATtiny28", ATtiny28, },
	{"ATtiny2313", ATtiny2313, (KB(2) + 128), KB(2), 32, -1},
	{"ATtiny4313", ATtiny4313, (KB(4) + 256), KB(4), 64, -1}, // new 16.09.2015 @RG
	{"ATtiny44", ATtiny44, (KB(4) + 256), KB(4), 64, -1}, // new 08.01.2015 @RG
	{"ATtiny45", ATtiny45, (KB(4) + 256), KB(4), 64,  -1},
	{"ATtiny461", ATtiny461, (KB(4) + 256), KB(4), 64, -1},
	{"ATtiny84", ATtiny84, (KB(8) + 512), KB(8), 64, -1}, // new 08.01.2015 @RG
	{"ATtiny85", ATtiny85, (KB(8) + 512), KB(8), 64, -1},
	{"ATtiny861", ATtiny861, (KB(8) + 512), KB(8), 64, -1},

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
	{"AT90CAN128", AT90CAN128, (KB(128) + KB(4)), KB(128), 256, -1}
};


static QVector<icElement> const eepAt89s_map =
{
	//      "AT89SXX Auto", AT89S0000
	{"AT89S8252", AT89S8252, KB(8) + KB(2), KB(8), 0, -1 },
	{"AT89S8253", AT89S8253, KB(12) + KB(2), KB(12), 64, 1 },
	{"AT89S51", AT89S51, KB(4), KB(4), 256, -1 },
	{"AT89S52", AT89S52, KB(8), KB(8), 256, -1 },
	{"AT89S53", AT89S53, KB(12), KB(12), 0, -1 }
};


static QVector<icElement> const eep93x6_map =
{
	//      "93XX Auto", E9300
	{"9306", E9306, 16, -1, -1, 64},
	{"9346", E9346, 64, -1, -1, 64},
	{"9356", E9356, 128, -1, -1, 256},
	{"9357", E9357, 128, -1, -1, 128},
	{"9366", E9366, 256, -1, -1, 256},
	{"9376", E9376, 512, -1, -1, 1024},
	{"9386", E9386, 1024, -1, -1, 1024}
};


static QVector<icElement> const eep93xx8_map =
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


static QVector<icElement> const eepPic16_map =
{
	//      "PIC16XX Auto",
	{"PIC16X83", PIC1683, KB(1) + 64, KB(1), -1, -1},
	{"PIC16X84", PIC1684, KB(2) + 64, KB(2), -1, -1}
};


static QVector<icElement> const eepPic168xx_map =
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


static QVector<icElement> const eepPic125_map =
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


static QVector<icElement> const eep250xx_map =
{
	//      "250X0 Auto",
	{"25010", E25010, 128, -1, -1, -1},
	{"25020", E25020, 256, -1, -1, -1},
	{"25040", E25040, 512, -1, -1, -1}
};


static QVector<icElement> const eep25xxx_map =
{
	//      "25XXX Auto",E25XX0
	{"25080", E25080, KB(1), -1, -1, -1},
	{"25160", E25160, KB(2), -1, -1, -1},
	{"25320", E25320, KB(4), -1, -1, -1},
	{"2564X/95640", E25640, KB(8), -1, -1, -1},
	{"25128", E25128, KB(16), -1, -1, -1},
	{"25256", E25256, KB(32), -1, -1, -1}
};


static QVector<icElement> const eep2506_map =
{
	//      "250X0 Auto",E25000
	{"SDE2506", E2506, 128, -1, -1, -1}
};


static QVector<icElement> const eepnvm3060_map =
{
	//      "NVMXX Auto",ENVM3000
	{"MDA2062", EMDA2062, 128, -1, -1, -1},
	{"NVM3060", ENVM3060, 512, -1, -1, -1}
};


static QVector<icElement> const eep2444_map =
{
	//      "XX Auto",
	{"S2430", S24H30, 8, -1, -1, 8},
	{"X2444", X24C44, 16, -1, -1, 16},
	{"X2445", X24C45, 16, -1, -1, 16}
};


static QVector<icElement> const eep17xxx_map =
{
	//      "Auto",AT1700
	{"AT1765", AT1765, KB(8), -1, -1, -1},
	{"AT17128", AT17128, KB(16), -1, -1, -1},
	{"AT17256", AT17256, KB(32), -1, -1, -1},
	{"AT17512", AT17512, KB(64), -1, -1, -1},
	{"AT17010", AT17010, KB(128), -1, -1, -1},

};




long BuildE2PType(int pritype, int subtype)
{
	return (((long)pritype & 0x7FFF) << 16) | (subtype & 0x7FFF);
}

int GetE2PSubType(quint32 type)
{
	if (type == EID_INVALID)
	{
		return -1;
	}
	else
	{
		return (int)(type & 0x7FFF);
	}
}

int GetE2PPriType(quint32 type)
{
	//      qDebug() << "GetE2PPriType" << type << ((type >> 16) & 0x7F);
	if (type == EID_INVALID)
	{
		return -1;
	}
	else
	{
		return (int)((type >> 16) & 0x7F);
	}
}


chipMap GetChipMap(QVector<icElement> &c, quint32 subtype)
{
	chipMap info = {};

	for (int i = 0; i < c.count(); i++)
	{
		// extract only subtype
		if ((c.at(i).id & 0x7FFF) == (subtype & 0x7FFF))
		{
			info = c.at(i).chMap;
			break;
		}
	}

	return info;
}

groupElement *GetMap(quint32 pritype)
{
	for (int i = 0; i < groupList.count(); i++)
	{
		if (groupList.at(i).id == pritype)
		{
			return &groupList[i];
		}
	}

	return NULL;
#if 0
	switch (pritype)
	{
	//AutoTag
	//Returns vector
	case E24XX:
		return eep24xx_map;
		break;
#if 0
	case E24XX1_A:
		return eep24xx1A_map;
		break;

	case E24XX1_B:
		return eep24xx1B_map;
		break;
#endif
	case E24XX2:
		return eep24xx2_map;
		break;

	case E24XX5:
		return eep24xx5_map;
		break;

	case AT90SXX:
		return eepAT90_map;
		break;

	case AT89SXX:
		return eepAt89s_map;
		break;

	case E93X6:
		return eep93x6_map;
		break;

	case E93XX_8:
		return eep93xx8_map;
		break;

	case PIC16XX:
		return eepPic16_map;
		break;

	case PIC168XX:
		return eepPic168xx_map;
		break;

	case PIC125XX:
		return eepPic125_map;
		break;

	case E250XX:
		return eep250xx_map;
		break;

	case E25XXX:
		return eep25xxx_map;
		break;

	case E2506XX:
		return eep2506_map;
		break;

	case ENVMXXX:
		return eepnvm3060_map;
		break;

	case AT17XXX:
		return eep17xxx_map;
		break;

	case X24C44XX:
		return eep2444_map;
		break;

	default:
		return QVector<icElement>();
		break;
	}
#endif
}


long GetEEPTypeFromSize(int pritype, int size)
{
	QVector<icElement> mp = GetMap(pritype);

	if (mp.count() == 0)
	{
		return -1;
	}

	for (int j = 0; j < mp.count(); j++)
	{
		if (mp.at(j).sz == size)
		{
			return mp.at(j).id;
		}
	}

	return -1;
}



long GetEEPTypeFromSize(quint32 type, int size)
{
	int pritype = GetE2PPriType(type);
	groupElement *mp = GetMap(pritype);

	if (mp == NULL)
	{
		return -1;
	}

	for (int j = 0; j < mp->chip.count(); j++)
	{
		if ((mp->chip.at(j).chMap.prog_sz + mp->chip.at(j).chMap.data_sz) == size)
		{
			return mp->chip.at(j).id;
		}
	}

	return -1;
}


int GetEEPTypeSize(int pritype, int subtype)
{
	if (pritype == 0)
	{
		return 0;
	}

	QVector<icElement> mp = GetMap(pritype);

	if (mp.count() == 0)
	{
		return 0;
	}

	icElement i = GetChipMap(mp, subtype);

	return i.sz;
}


int GetEEPTypeSize(quint32 type)
{
	int pritype = GetE2PPriType(type);
// 	int subtype = GetE2PSubType(type);

	if (pritype == 0)
	{
		return 0;
	}

	groupElement *mp = GetMap(pritype);

	if (mp == 0)
	{
		return 0;
	}

	chipMap i = GetChipMap(mp->chip, type);

	return (i.prog_sz + i.data_sz);
}

//Serve solamente nei bus che utilizzano la dimensione
// di word degli indirizzi dinamica (per es. Microwire)

int GetEEPAddrSize(int pritype, int subtype)
{
	QVector<icElement> mp = GetMap(pritype);

	if (mp.count() == 0)
	{
		return -1;
	}

	icElement i = GetChipMap(mp, subtype);

	return i.adr_sz;
}


int GetEEPAddrSize(quint32 type)
{
	int pritype = GetE2PPriType(type);
// 	int subtype = GetE2PSubType(type);

	groupElement *mp = GetMap(pritype);

	if (mp == NULL)
	{
		return -1;
	}

	chipMap i = GetChipMap(mp->chip, type);

	return i.adr_sz;
}


int GetEEPTypeSplit(int pritype, int subtype)
{
	QVector<icElement> mp = GetMap(pritype);

	if (mp.count() == 0)
	{
		return -1;
	}

	icElement i = GetChipMap(mp->chip, subtype);

	return i.splt;
}


int GetEEPTypeSplit(quint32 type)
{
	int pritype = GetE2PPriType(type);
// 	int subtype = GetE2PSubType(type);

	groupElement *mp = GetMap(pritype);

	if (mp == NULL)
	{
		return -1;
	}

	chipMap i = GetChipMap(mp->chip, type);

	return i.data_sz;
}


int GetEEPTypeWPageSize(int pritype, int subtype)
{
	QVector<icElement> mp = GetMap(pritype);

	if (mp.count() == 0)
	{
		return -1;
	}

	icElement i = GetChipMap(mp->chip, subtype);

	return i.wpg_sz;
}


int GetEEPTypeWPageSize(quint32 type)
{
	int pritype = GetE2PPriType(type);
// 	int subtype = GetE2PSubType(type);

	groupElement *mp = GetMap(pritype);

	if (mp == NULL)
	{
		return -1;
	}

	chipMap i = GetChipMap(mp->chip, type);

	return i.wpg_sz;
}


QVector<icElement> GetEEPSubTypeVector(int type)
{
	groupElement *mp = GetMap(type);

	return &mp;
}


int GetEEPSubTypeIndex(quint32 type)
{
	QVector<icElement> mp = GetEEPSubTypeVector(GetE2PPriType(type));

	if (mp.count() == 0)
	{
		return -1;
	}

	chipMap i = GetChipMap(mp.chip, type);

	return i.id;
}
#endif

#if 0
QString  GetEEPTypeString(int pritype, int subtype)
{
	QVector<icElement> mp = GetMap(pritype);

	if (mp.count())
	{
		qDebug() << "mp" << mp.count();

		foreach (icElement i, mp)
		{
			if ((i.id & 0x07fff) == subtype)
			{
				return i.name;
			}
		}
	}

	return "";
}


QString GetEEPTypeString(quint32 type)
{
	int pritype = GetE2PPriType(type);
// 	int subtype = GetE2PSubType(type);

	groupElement *mp = GetMap(pritype);

	if (mp == NULL)
	{
		return "";
	}

	foreach (icElement i, mp->chip)
	{
		if ((i.id & 0x07fff) == (type & 0x07fff))
		{
			return i.name;
		}
	}

	return "";
}
#endif

//18/10/98
#if 0
QString GetEEPTypeString(long type)
{
	int pritype = GetE2PPriType(type);
	int subtype = GetE2PSubType(type);

	qDebug() << "GetEEPTypeString" << pritype << subtype << GetEEPTypeString(pritype, subtype);
	return GetEEPTypeString(pritype, subtype);
}
#endif

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


