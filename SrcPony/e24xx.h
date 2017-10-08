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

#ifndef _E24XX_H
#define _E24XX_H

#include "types.h"

#include "device.h"
#include "i2cbus.h"

class e2AppWinInfo;

class E24xx : public Device
{
  public:               //---------------------------------------- public

	E24xx(e2AppWinInfo *wininfo = 0, BusIO *busp = 0, int max_no_of_bank = 8, int def_banksize = 256);
	virtual ~E24xx();

	int Probe(int probe_size = 0);
	int Read(int probe = 1, int type = ALL_TYPE);
	int Write(int probe = 1, int type = ALL_TYPE);
	int Verify(int type = ALL_TYPE);

	int BankRollOverDetect(int force);

	int     const max_bank;         // max number of banks (max eeprom size)

  protected:    //--------------------------------------- protected

	int const timeout_loop;         //eeprom timeout

	//!!! 07/03/98
	int base_addr;                          //eeprom I2C base address
	int n_bank;                                     //numero indirizzi (o banchi reali)

	int eeprom_addr[128];              // indirizzi I2C a cui risponde la EEPROM

	I2CBus *GetBus()
	{
		return static_cast<I2CBus *>(Device::GetBus());
	}

	virtual int bank_out(uint8_t const *copy_buf, int bank_no, long size = -1, long idx = 0);
	virtual int bank_in(uint8_t *copy_buf, int bank_no, long size = -1, long idx = 0);

	//-- Parte riguardante la EEPROM
	int sequential_read;                    //1 --> legge un banco in una volta
	int writepage_size;                             //se > 1 scrive una pagina alla volta

  private:              //--------------------------------------- private

};
#endif
