//=========================================================================//
//-------------------------------------------------------------------------//
// atmegabus.cpp -- ATmega (AVR micro) Bus Driver                          //
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

#include "types.h"
#include "atmegabus.h"
#include "errcode.h"

#include "e2app.h"

/*Attenzione!! il format Intel Hex e` Little Endian */
#undef	_BIG_ENDIAN_

#define	PAGE_SIZE	256

// Costruttore
AtMegaBus::AtMegaBus(BusInterface *ptr)
	: At90sBus(ptr)
{
}

//added 19/01/1999 for the mega device
void AtMegaBus::WriteProgPage(long addr, UBYTE const *data, long page_size)
{
	long k;

	for (k = 0; k < page_size; k++)
	{
		WriteProgByte(k, *data++);
	}

	//addr is the address of the 127 words page just transmitted
	addr &= 0xFFFFFF00;
	THEAPP->SetLastProgrammedAddress(addr + page_size - 1);
	addr >>= 1;			//convert to word address

	SendDataByte(WriteProgPageMem);
	SendDataByte(addr >> 8);
	SendDataByte(addr);
	SendDataByte(0);

	WaitMsec( THEAPP->GetMegaPageDelay() );
}

/***  commented 20/07/99 because is the same of At90sbus
int AtMegaBus::Reset()
{
	SPIBus::Reset();

	WaitMsec(50);	// Almeno 20msec dai datasheet AVR atmel

	SendDataByte(EnableProg0);
	SendDataByte(EnableProg1);
	int val = RecDataByte();	//19/01/1999
	SendDataByte(0);

	//19/01/1999 -- check as suggested in the 8515 datasheets
					//don't work with AT90S1200
	int k = 0;
	while (val != EnableProg1 && k++ < 32)
	{
		setSCK();
		WaitMsec(10);
		clearSCK();
		WaitMsec(10);

		SendDataByte(EnableProg0);
		SendDataByte(EnableProg1);
		val = RecDataByte();
		SendDataByte(0);
	}

	if (k == 32)
		return -1;

	return OK;
}
***/

int AtMegaBus::CheckBlankPage(UBYTE const *data, ULONG length)
{
	int blank_page = TRUE;

	while (length--)
	{
		if (*data++ != 0xFF)
		{
			blank_page = FALSE;
			break;
		}
	}

	return blank_page;
}

long AtMegaBus::Write(int addr, UBYTE const *data, long length)
{
	long len;

	if (addr)
	{	//EEprom
		for (addr = 0, len = 0; len < length; addr++, data++, len++)
		{
			//09/10/98 -- program only locations that really need to be programmed
			int val = ReadEEPByte(addr);

			if (val != *data)
			{
				WriteEEPByte(addr, *data);

				//Interrupt the writing and exit (device missing?)
				if ( WaitReadyAfterWrite(1, addr, *data) != OK )
					return 0;
			}

			if ( CheckAbort(len * 100 / length) )
				break;
		}
		CheckAbort(100);
	}
	else
	{	//Flash Eprom

		//Erase command
		SendDataByte(ChipErase0);
		SendDataByte(ChipErase1);
		SendDataByte(0);
		SendDataByte(0);

		WaitMsec(twd_erase);		//Erase delay
		Reset();

		//Write
		for (addr = 0, len = 0; len < length; addr += PAGE_SIZE, data += PAGE_SIZE, len += PAGE_SIZE)
		{
			//check for FF's page
			if ( !CheckBlankPage(data, PAGE_SIZE) )
				WriteProgPage(addr, data, PAGE_SIZE);

			if ( CheckAbort(len * 100 / length) )
				break;
		}
		CheckAbort(100);
	}

	return len;
}
