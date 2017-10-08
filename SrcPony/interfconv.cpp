//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
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

struct Interf2Index
{
	int vector;
	int index;
	QString name;
	HInterfaceType type;
};

static QVector<Interf2Index> index_interface =
{
	//AutoTag
	//Interfaces vector
	{0, 0, "SI-ProgAPI", SIPROG_API},
	{0, 1, "SI-ProgI/O", SIPROG_IO},
	{0, 2, "JDM-API", JDM_API},
	{1, 0, "AvrISP-API", AVRISP},
	{1, 1, "AvrISP-I/O", AVRISP_IO},
	{1, 2, "DT-006-API", DT006_API},
	{1, 3, "DT-006-I/O", DT006_IO},
	{1, 4, "EasyI2C-API", EASYI2C_API},
	{1, 5, "EasyI2C-I/O", EASYI2C_IO},
	{1, 6, "Linux SysFs GPIO", LINUXSYSFS_IO},
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

// EK 2017
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
