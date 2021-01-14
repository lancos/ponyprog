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
// Some global ID and #defines


#ifndef _GLOBALS_H
#define _GLOBALS_H


#define AUTOSIZE_ID     64768


#define PROG_TYPE       0x1
#define DATA_TYPE       0x2
#define CONFIG_TYPE     0x4

#define ALL_TYPE        0x7 // (PROG_TYPE|DATA_TYPE|CONFIG_TYPE)


#define NO_OF_FILETYPE  LAST_FT
enum FileType
{
	E2P,
	INTEL,
	MOTOS,
	BIN,
	CSM,
	LAST_FT
};

// #define NO_OF_INTERFACETYPE     (LAST_HT-1)
enum HInterfaceType
{
	//AutoTag
	//Interface Identification
	SIPROG_API = 0,
	SIPROG_IO,
	JDM_API,
	AVRISP,
	AVRISP_IO,
	EASYI2C_API,
	EASYI2C_IO,
	DT006_API,
	DT006_IO,
	LINUXSYSFS_IO,
	FTDI_JTAGKEY,
	PONYPROG_FT,
	//PONYPROG_RPI,
	LAST_HT
};

#define NO_OF_BUSTYPE (LAST_BT-1)
enum BusType
{
	//AutoTag
	//Bus Type Identification
	I2C = 1,
	AT90S,
	//ATMEGAB,
	//AT1200S,
	AT89S,
	AT93C,
	AT250,
	AT250BIG,
	PICB,
	//SXB,
	SDEB,
	PICNEWB,
	IMBUS,
	PIC12B,
	X2444B,
	S2430B,
	LAST_BT
};

class VidPid
{
  public:

	VidPid(unsigned int _vid = 0, unsigned int _pid = 0)
	{
		vid = _vid;
		pid = _pid;
	}
	VidPid(const VidPid &p2)
	{
		vid = p2.vid;
		pid = p2.pid;
	}
	bool operator==(const VidPid &a)
	{
		return (a.pid == pid && a.vid == vid);
	}
	VidPid &operator=(const VidPid &a)
	{
		vid = a.vid;
		pid = a.pid;
		return *this;
	}
	unsigned int vid;
	unsigned int pid;
};

#endif
