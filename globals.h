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
// Some global ID and #defines

#ifndef	_GLOBALS_H
#define	_GLOBALS_H

#define	APPNAME		"PonyProg2000"
#define	AUTHORNAME	"Claudio Lanconelli"
#define	AUTHORMAIL	"PonyProg2000@gmail.com"
#define	AUTHORWEB	"http://ponyprog.sourceforge.net"
#define COPYRIGHTYEAR	"1997-2013"
#define	APPNAME_EXT	STR_APPNAME_EXT

#define	APPVERSION	"2.08c Beta"

#define	THEAPP	((e2App*)theApp)

#ifdef	MAX_PATH
#define	MAXPATH	MAX_PATH
#else
#define	MAXPATH	1024
#endif

#define	MAXMSG	256
#define	MAXFNAMEMSG	40
#define	MAXNUMDIGIT	64

#define	NO_OF_FILETYPE	LAST_FT
enum FileType {
	E2P,
	INTEL,
	MOTOS,
	BIN,
	CSM,
	LAST_FT
};

#define	NO_OF_INTERFACETYPE	(LAST_HT-1)
enum HInterfaceType {
	//AutoTag
	//Interface Identification
	SIPROG_API = 1,
	SIPROG_IO,
	EASYI2C_API,
	EASYI2C_IO,
	AVRISP,
	AVRISP_IO,
	DT006_API,
	DT006_IO,
	JDM_API,
//	JDM_IO,
	LAST_HT
};

#define NO_OF_BUSTYPE (LAST_BT-1)
enum BusType {
	//AutoTag
	//Bus Type Identification
	I2C = 1,
	AT90S,
//	ATMEGAB,
//	AT1200S,
	AT89S,
	AT93C,
	AT250,
	AT250BIG,
	PICB,
	SXB,
	SDEB,
	PICNEWB,
	IMBUS,
	PIC12B,
	X2444B,
	S2430B,
	LAST_BT
};

#define	AUTOSIZE_ID	64768


#define	PROG_TYPE	1
#define	DATA_TYPE	2
#define	CONFIG_TYPE	4

#define	ALL_TYPE	(PROG_TYPE|DATA_TYPE|CONFIG_TYPE)

#endif
