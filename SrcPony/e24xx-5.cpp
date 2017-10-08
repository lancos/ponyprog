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

#include "types.h"
#include "e2awinfo.h"
#include "e24xx-5.h"            // Header file
#include "errcode.h"
#include "eeptypes.h"


//=====>>> Costruttore <<<======
E24xx5::E24xx5(e2AppWinInfo *wininfo, BusIO *busp)
	:       E24xx(wininfo, busp, 128)
{
	//      writepage_size = E2Profile::GetI2CPageWrite();
	//      E2Profile::SetI2CPageWrite(writepage_size);
	writepage_size = 32;

	base_addr = 0x00;               // 24C325 or 24C645 use non standard I2C Bus address, Probe() will try ALL possible I2C Adresses from 0x00 to 0xFE
}


//--- Distruttore
E24xx5::~E24xx5()
{
}

int E24xx5::Write(int probe, int type)
{
	E24xx5::Probe(0);

	if (type & PROG_TYPE)
	{
		//Enable writing
		uint8_t buffer[4];

		buffer[0] = 0xFF;       //last address (Write protect register)
		buffer[1] = 0x02;       //set WEL bit

		if (GetBus()->StartWrite(eeprom_addr[n_bank - 1], buffer, 2) != 2)
		{
			return GetBus()->Error();
		}

		buffer[0] = 0xFF;       //last address (Write protect register)
		buffer[1] = 0x06;       //set RWEL+WEL bit

		if (GetBus()->StartWrite(eeprom_addr[n_bank - 1], buffer, 2) != 2)
		{
			return GetBus()->Error();
		}

		buffer[0] = 0xFF;       //last address (Write protect register)
		buffer[1] = 0x02;       //reset WPEN, BP1, BP0 (disable any write protection)

		if (GetBus()->Write(eeprom_addr[n_bank - 1], buffer, 2) != 2)
		{
			return GetBus()->Error();
		}

		//Ack polling
		int k;

		for (k = timeout_loop; k > 0 && GetBus()->Read(eeprom_addr[0], buffer, 1) != 1; k--)
			;

		if (k == 0)
		{
			return E2P_TIMEOUT;
		}
	}

	return E24xx::Write(probe);
}
