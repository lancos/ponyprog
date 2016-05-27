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

#ifndef	_LINUX_
#  ifdef	__BORLANDC__
#    define	strcasecmp stricmp
#  else // _MICROSOFT_ VC++
#    define strcasecmp	_stricmp
#  endif
#endif

struct Interf2Index {
	int vector;
	int index;
	const char *name;
	HInterfaceType type;
};

static Interf2Index index_interface[] = {
	//AutoTag
	//Interfaces vector
	{0,0,"SI-ProgAPI",SIPROG_API},
	{0,1,"SI-ProgI/O",SIPROG_IO},
	{0,2,"JDM-API",JDM_API},
	{1,0,"AvrISP-API",AVRISP},
	{1,1,"AvrISP-I/O",AVRISP_IO},
	{1,2,"DT-006-API",DT006_API},
	{1,3,"DT-006-I/O",DT006_IO},
	{1,4,"EasyI2C-API",EASYI2C_API},
	{1,5,"EasyI2C-I/O",EASYI2C_IO},
	{1,6,"Linux SysFs GPIO",LINUXSYSFS_IO},
	{0,0,0,LAST_HT}
};

HInterfaceType NameToInterfType(const char *name)
{
	int k = 0;

	if (name)
	{
		for (k = 0; index_interface[k].name != 0; k++)
			if ( strcasecmp(index_interface[k].name, name) == 0 )
				break;
	}

	return index_interface[k].type;
}

const char *TypeToInterfName(HInterfaceType type)
{
	int k;
	for (k = 0; index_interface[k].name != 0; k++)
		if (index_interface[k].type == type)
			break;
	return index_interface[k].name;
}

int TypeToInterfVector(HInterfaceType type)
{
	int k;
	for (k = 0; index_interface[k].name != 0; k++)
		if (index_interface[k].type == type)
			break;
	return index_interface[k].vector;
}

int TypeToInterfIndex(HInterfaceType type)
{
	int k;
	for (k = 0; index_interface[k].name != 0; k++)
		if (index_interface[k].type == type)
			break;
	return index_interface[k].index;
}

HInterfaceType VindexToInterfType(int vector, int index)
{
	int k;
	for (k = 0; index_interface[k].name != 0; k++)
	{
		if (index_interface[k].vector == vector &&
			index_interface[k].index == index)
			break;
	}
	return index_interface[k].type;
}
