//=========================================================================//
//-------------------------------------------------------------------------//
// globals.h -- Some global ID and #defines                                //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-1999   Claudio Lanconelli                           //
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

#ifndef	_GLOBALS_H
#define	_GLOBALS_H

# define	APPNAME	"PonyProg"
# define	AUTHORNAME	"Claudio Lanconelli"
# define	APPNAME_EXT	"Serial Device Programmer"

#define	THEAPP	((e2App*)theApp)

#define	NO_OF_FILETYPE	LAST_FT
enum FileType {
	E2P,
	INTEL,
	MOTOS,
	BIN,
	LAST_FT
};

#define	NO_OF_INTERFACETYPE	(LAST_HT-1)
enum HInterfaceType {
	//AutoTag
	//Interface Identification
	SIPROG_API = 1,
	SIPROG_IO,
	EASYI2C_COM,
	EASYI2C_LPT,
	AVRISP,
	LAST_HT
};

#define NO_OF_BUSTYPE (LAST_BT-1)
enum BusType {
	//AutoTag
	//Bus Type Identification
	I2C = 1,
	AT90S,
	MEGAS,
	AT1200S,
	AT89S,
	AT93C,
	AT250,
	AT250BIG,
	PICB,
	SXB,
	SDEB,
	LAST_BT
};

#define	AUTOSIZE_ID	64768

#endif