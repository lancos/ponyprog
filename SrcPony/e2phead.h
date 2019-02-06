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
// Header structure for .E2P files

#ifndef _E2PHEAD_H
#define _EP2HEAD_H

#include <QString>
#include <QtCore>

#include "types.h"

#define E2P_ID_SIZE     8
#define E2P_STRID_SIZE  28
#define E2P_COMM_SIZE   86

#define E2P_FVERSION    0x02

#ifdef Q_OS_WIN32
#pragma pack(push,1)
#endif

// Header dei file
struct e2pHeader
{
	char fileID[E2P_ID_SIZE];
	uint8_t e2pFuseBits;
	uint8_t e2pLockBits;
	uint32_t e2pType;
	int32_t e2pSize;
	uint8_t flags;          //rollOver, splitted
	uint16_t e2pExtFuseBits;
	uint16_t e2pExtLockBits;
	uint8_t fversion;       //file version
	uint16_t split_size_Low;        //used by splitted devices
	char e2pStringID[E2P_STRID_SIZE];
	uint32_t e2pProgBits;
	char e2pComment[E2P_COMM_SIZE];
	uint16_t split_size_High;
	uint16_t pad;
	uint16_t e2pCrc;
	uint16_t headCrc;
} PACK;

#ifdef Q_OS_WIN32
#pragma pack(pop)
#endif

#endif
