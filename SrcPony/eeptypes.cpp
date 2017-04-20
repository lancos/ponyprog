//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: eeptypes.cpp,v 1.11 2013/10/31 16:10:58 lancos Exp $
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

#include <QString>
#include <QVector>
#include <QDebug>

#include "types.h"
#include "globals.h"
#include "eeptypes.h"



long BuildE2PType(int pritype, int subtype)
{
	return (((long)pritype & 0x7FFF) << 16) | (subtype & 0x7FFF);
}

int GetE2PSubType(unsigned long type)
{
	if (type == EID_INVALID)
		return -1;
	else
		return (int)(type & 0x7FFF);
}

int GetE2PPriType(unsigned long type)
{
	//      qDebug() << "GetE2PPriType" << type << ((type >> 16) & 0x7F);
	if (type == EID_INVALID)
		return -1;
	else
		return (int)((type >> 16) & 0x7F);
}


chipInfo GetChipInfo(QVector<chipInfo> &c, long subtype)
{
	chipInfo info = {};

	for (int i = 0; i < c.count(); i++)
	{
		// extract only subtype
		if ((c.at(i).id & 0x7FFF) == (subtype & 0x7FFF))
		{
			info = c.at(i);
			break;
		}
	}

	return info;
}

// EK 2017
// don't worry, it's not a copy of vector
// it's smart pointer to vector
static QVector<chipInfo> const GetMap(int pritype)
{
	switch(pritype)
	{
	//AutoTag
	//Returns vector
	case E24XX:
		return eep24xx_map;
		break;

	case E24XX1_A:
		return eep24xx1A_map;
		break;

	case E24XX1_B:
		return eep24xx1B_map;
		break;

	case E24XX2:
		return eep24xx2_map;
		break;

	case E24XX5:
		return eep24xx5_map;
		break;

	case AT90SXX:
		return eepAt90s_map;
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
		return QVector<chipInfo>();
		break;
	}
}

#if 0
long GetEEPTypeFromSize(int pritype, int size)
{
	QVector<chipInfo> mp = GetMap(pritype);

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
#endif


long GetEEPTypeFromSize(unsigned long type, int size)
{
	int pritype = GetE2PPriType(type);
	QVector<chipInfo> mp = GetMap(pritype);

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

#if 0
int GetEEPTypeSize(int pritype, int subtype)
{
	if (pritype == 0)
	{
		return 0;
	}

	QVector<chipInfo> mp = GetMap(pritype);

	if (mp.count() == 0)
	{
		return 0;
	}

	chipInfo i = GetChipInfo(mp, subtype);

	return i.sz;
}
#endif

int GetEEPTypeSize(unsigned long type)
{
	int pritype = GetE2PPriType(type);
	int subtype = GetE2PSubType(type);

	if (pritype == 0)
	{
		return 0;
	}

	QVector<chipInfo> mp = GetMap(pritype);

	if (mp.count() == 0)
	{
		return 0;
	}

	chipInfo i = GetChipInfo(mp, subtype);

	return i.sz;
}

//Serve solamente nei bus che utilizzano la dimensione
// di word degli indirizzi dinamica (per es. Microwire)
#if 0
int GetEEPAddrSize(int pritype, int subtype)
{
	QVector<chipInfo> mp = GetMap(pritype);

	if (mp.count() == 0)
	{
		return -1;
	}

	chipInfo i = GetChipInfo(mp, subtype);

	return i.adrsz;
}
#endif

int GetEEPAddrSize(unsigned long type)
{
	int pritype = GetE2PPriType(type);
	int subtype = GetE2PSubType(type);

	QVector<chipInfo> mp = GetMap(pritype);

	if (mp.count() == 0)
	{
		return -1;
	}

	chipInfo i = GetChipInfo(mp, subtype);

	return i.adrsz;
}

#if 0
int GetEEPTypeSplit(int pritype, int subtype)
{
	QVector<chipInfo> mp = GetMap(pritype);

	if (mp.count() == 0)
	{
		return -1;
	}

	chipInfo i = GetChipInfo(mp, subtype);

	return i.splt;
}
#endif

int GetEEPTypeSplit(unsigned long type)
{
	int pritype = GetE2PPriType(type);
	int subtype = GetE2PSubType(type);

	QVector<chipInfo> mp = GetMap(pritype);

	if (mp.count() == 0)
	{
		return -1;
	}

	chipInfo i = GetChipInfo(mp, subtype);

	return i.splt;
}

#if 0
int GetEEPTypeWPageSize(int pritype, int subtype)
{
	QVector<chipInfo> mp = GetMap(pritype);

	if (mp.count() == 0)
	{
		return -1;
	}

	chipInfo i = GetChipInfo(mp, subtype);

	return i.wpgsz;
}
#endif

int GetEEPTypeWPageSize(unsigned long type)
{
	int pritype = GetE2PPriType(type);
	int subtype = GetE2PSubType(type);

	QVector<chipInfo> mp = GetMap(pritype);

	if (mp.count() == 0)
	{
		return -1;
	}

	chipInfo i = GetChipInfo(mp, subtype);

	return i.wpgsz;
}


QVector<chipInfo> GetEEPSubTypeVector(int type)
{
	QVector<chipInfo> mp = GetMap(type);

	return mp;
}


int GetEEPSubTypeIndex(long type)
{
	QVector<chipInfo> mp = GetEEPSubTypeVector(GetE2PPriType(type));

	if (mp.count() == 0)
	{
		return -1;
	}

	chipInfo i = GetChipInfo(mp, type);

	return i.id;
}

#if 0
QString  GetEEPTypeString(int pritype, int subtype)
{
	QVector<chipInfo> mp = GetMap(pritype);

	if (mp.count())
	{
		qDebug() << "mp" << mp.count();

		foreach(chipInfo i, mp)
		{
			if ((i.id & 0x07fff) == subtype)
			{
				return i.name;
			}
		}
	}

	return "";
}
#endif

QString GetEEPTypeString(unsigned long type)
{
	int pritype = GetE2PPriType(type);
	int subtype = GetE2PSubType(type);

	QVector<chipInfo> mp = GetMap(pritype);

	if (mp.count())
	{
		foreach(chipInfo i, mp)
		{
			if ((i.id & 0x07fff) == subtype)
			{
				return i.name;
			}
		}
	}

	return "";
}

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


