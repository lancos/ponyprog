//=========================================================================//
//-------------------------------------------------------------------------//
// e2phead.h -- Header structure for .E2P files                            //
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

#ifndef	_E2PHEAD_H
#define	_EP2HEAD_H

//#define	BUSTYPE_I2C		1

// Header dei file
struct e2pHeader {
	char fileID[8] PACK;
	UBYTE e2pFuseBits PACK;
	UBYTE e2pLockBits PACK;
	ULONG e2pType PACK;
	ULONG e2pSize PACK;
	UBYTE flags PACK;		//rollOver, splitted
	UBYTE pad[5] PACK;		//reserved for future use
	UWORD split_size;		//used by splitted devices
	char e2pStringID[32] PACK;
	char e2pComment[90] PACK;
	UWORD e2pCrc PACK;
	UWORD headCrc PACK;
};

#endif
