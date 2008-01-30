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

#include "types.h"
#include "atmegabus.h"
#include "errcode.h"

#include "e2app.h"

// Costruttore
AtMegaBus::AtMegaBus(BusInterface *ptr, int wpage_size, bool page_poll)
	: At90sBus(ptr)
{
	write_page_size = wpage_size;		//default to ATmega103 page size
	enable_flashpage_polling = page_poll;	//default to ATmega103: no flash polling
}

bool AtMegaBus::GetFlashPagePolling() const
{
	return enable_flashpage_polling;
}

void AtMegaBus::SetFlashPagePolling(bool val)
{
	enable_flashpage_polling = val;
}

int AtMegaBus::GetPageSize() const
{
	return write_page_size;
}

void AtMegaBus::SetPageSize(int size)
{
	write_page_size = size;
}

void AtMegaBus::WriteProgPage(long addr, UBYTE const *data, long page_size, long timeout)
{
	long k;
	long first_loc = -1;		//first location different from 0xFF

	if (page_size <= 0)
		page_size = write_page_size;

	//align addr to page boundary
	addr &= ~(page_size - 1);	//0xFFFFFF00

	for (k = 0; k < page_size; k++, data++)
	{
		if (first_loc < 0 && *data != 0xFF)
			first_loc = addr + k;

		WriteProgByte(k, *data);
	}

	SendDataByte(WriteProgPageMem);
	SendDataByte(addr >> 9);		//send word address
	SendDataByte(addr >> 1);
	SendDataByte(0);

	THEAPP->SetLastProgrammedAddress(addr + page_size - 1);

	if (enable_flashpage_polling)
	{
		for (k = timeout; k > 0; k--)
		{
			if ( ReadProgByte(first_loc) != 0xFF )
				break;
		}
	}
	else
		WaitMsec( THEAPP->GetMegaPageDelay() );
}

bool AtMegaBus::CheckBlankPage(UBYTE const *data, ULONG length)
{
	bool blank_page = true;

	while (length--)
	{
		if (*data++ != 0xFF)
		{
			blank_page = false;
			break;
		}
	}

	return blank_page;
}

long AtMegaBus::Write(int addr, UBYTE const *data, long length, int page_size)
{
	long len;

	if (addr || write_page_size <= 1)
	{	//EEprom of flash without page write
		len = At90sBus::Write(addr, data, length, page_size);		//Use standard routine
	}
	else
	{	//Flash Eprom with page write
		for (addr = 0, len = 0; len < length; addr += write_page_size, data += write_page_size, len += write_page_size)
		{
			//check for FF's page to skip blank pages
			if ( !CheckBlankPage(data, write_page_size) )
				WriteProgPage(addr, data, write_page_size);

			if ( CheckAbort(len * 100 / length) )
				break;
		}
		CheckAbort(100);
	}

	return len;
}
