//=========================================================================//
//-------------------------------------------------------------------------//
// e24xx.h -- Header for for E24xx class                                   //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2000   Claudio Lanconelli                           //
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

#ifndef _E24XX_H
#define _E24XX_H

#include "types.h"

#include "eeprom.h"
#include "i2cbus.h"

#define BANK_SIZE	256
#define	MAX_BANK	128

class E24xx : public EEProm
{
  public:		//---------------------------------------- public

	E24xx(e2AppWinInfo *wininfo = 0, BusIO *busp = 0, int max_no_of_bank = 8);
	virtual ~E24xx();

	int Probe(int probe_size = 0);
	int Read(int probe = 1);
	int Write(int probe = 1);
	int Verify();
	int BankRollOverDetect(int force);

	int	const max_bank;		// max number of banks (max eeprom size)

  protected:	//--------------------------------------- protected

	int const timeout_loop;		//eeprom timeout

	//!!! 07/03/98
	int base_addr;				//eeprom I2CBus base address
	int n_bank;					//numero indirizzi (o banchi reali)

	int eeprom_addr[MAX_BANK];		// indirizzi I2C a cui risponde la EEPROM

	int sequential_read;			//if 1 whole bank every read
	int writepage_size;				//if > 1 perform page write

	I2CBus *GetBus()
		{ return (I2CBus *)EEProm::GetBus(); }

	virtual int bank_out(BYTE const *copy_buf, int bank_no, long size = -1, long idx = 0);
	virtual int bank_in(BYTE *copy_buf, int bank_no, long size = -1, long idx = 0);

  private:		//--------------------------------------- private

};
#endif
