//=========================================================================//
//-------------------------------------------------------------------------//
// interfconv.cpp -- interface id conversion utilities                     //
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

#include <string.h>
#include "types.h"
#include "globals.h"

#ifndef	_LINUX_
#  ifdef	__BORLANDC__
#    define	strcasecmp stricmp
#  else // _MICROSOFT_ VC++
#    define strcasecmp	stricmp
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
	{0,2,"EasyI2C-COM",EASYI2C_COM},
	{1,0,"EasyI2C-LPT",EASYI2C_LPT},
	{1,1,"AvrISP-API",AVRISP},
	{1,2,"AvrISP-I/O",AVRISPIO},
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