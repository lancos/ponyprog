//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2019   Claudio Lanconelli                           //
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
#include <QStringList>
#include <QDebug>

#include "types.h"
#include "globals.h"
#include "interfconv.h"

struct Interf2Index
{
	int vector;
	int index;
	QString name;
	HInterfaceType type;
};

static QVector<Interf2Index> index_interface =
{
	//Interfaces vector
	{INTERF_COM, 0, "SI-ProgAPI", SIPROG_API},
	{INTERF_COM, 1, "SI-ProgI/O", SIPROG_IO},
	{INTERF_COM, 2, "JDM-API", JDM_API},
	{INTERF_LPT, 0, "AvrISP-API", AVRISP},
	{INTERF_LPT, 1, "AvrISP-I/O", AVRISP_IO},
	{INTERF_LPT, 2, "DT-006-API", DT006_API},
	{INTERF_LPT, 3, "DT-006-I/O", DT006_IO},
	{INTERF_LPT, 4, "EasyI2C-API", EASYI2C_API},
	{INTERF_LPT, 5, "EasyI2C-I/O", EASYI2C_IO},
	{INTERF_USB, 0, "FTDI PonyProgFT", PONYPROG_FT},
	{INTERF_USB, 1, "FTDI JtagKey", FTDI_JTAGKEY},
	{INTERF_GPIO, 0, "Linux SysFs GPIO", LINUXSYSFS_IO},
};

QStringList GetInterfList(int vector)
{
	QStringList lst;

	for (int k = 0; k < index_interface.count(); k++)
	{
		if (index_interface.at(k).vector == vector)
		{
			lst << index_interface.at(k).name;
		}
	}

	return lst;
}

HInterfaceType NameToInterfType(const QString &name)
{
	if (name.length())
	{
		QString n = name;
		n.remove(QChar('-'));
		n.remove(QChar(' '));
		n.remove(QChar('/'));

		for (int k = 0; k < index_interface.count(); k++)
		{
			QString v = index_interface.at(k).name;
			v.remove(QChar('-'));
			v.remove(QChar(' '));
			v.remove(QChar('/'));

			if (v.indexOf(n) >= 0)
			{
				return index_interface.at(k).type;
			}
		}
	}

	return LAST_HT;
}

QString TypeToInterfName(HInterfaceType type)
{
	for (int k = 0; k < index_interface.count(); k++)
	{
		if (index_interface.at(k).type == type)
		{
			return index_interface.at(k).name;
		}
	}

	return "";
}

int TypeToInterfVector(HInterfaceType type)
{
	for (int k = 0; k < index_interface.count(); k++)
	{
		if (index_interface.at(k).type == type)
		{
			return index_interface.at(k).vector;
		}
	}

	return -1;
}

int TypeToInterfIndex(HInterfaceType type)
{
	for (int k = 0; k < index_interface.count(); k++)
	{
		if (index_interface.at(k).type == type)
		{
			return index_interface.at(k).index;
		}
	}

	return -1;
}

HInterfaceType VindexToInterfType(int vector, int index)
{
	for (int k = 0; k < index_interface.count(); k++)
	{
		if (index_interface.at(k).vector == vector &&
				index_interface.at(k).index == index)
		{
			return index_interface.at(k).type;
		}
	}

	return LAST_HT;
}
