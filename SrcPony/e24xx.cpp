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
#include "e2cmdw.h"
#include "e2profil.h"
#include "e2awinfo.h"
#include "e24xx.h"              // Header file
#include "errcode.h"
#include "eeptypes.h"

#include <QDebug>

//=====>>> Costruttore <<<======
E24xx::E24xx(e2AppWinInfo *wininfo, BusIO *busp, int max_no_of_bank, int def_banksize)
	:       Device(wininfo, busp, def_banksize),
			max_bank(max_no_of_bank),
			timeout_loop(200),
			n_bank(0),
			sequential_read(1),             // lettura di un banco alla volta
			writepage_size(1)               // scrittura di un byte alla volta (no page write)
{
	//      qDebug() << "E24xx" << THEAPP;
	base_addr = E2Profile::GetI2CBaseAddr();
	E2Profile::SetI2CBaseAddr(base_addr);
	/***
	        //Inizializza il vettori degli indirizzi di default
	        //  questo nel caso non venga determinata automaticamente
	        //  la dimensione con il Probe()
	        int addr, k;
	        for (addr = base_addr, k = 0; k < max_bank; k++, addr += 2)
	                        eeprom_addr[k] = addr;
	***/
}

//--- Distruttore
E24xx::~E24xx()
{
}

//Determina a quali indirizzi I2C si mappa, e
// se probe_size == 1 anche la dimensione (numero di banchi)
//---
int E24xx::Probe(int probe_size)
{
	int addr, error, k;
	uint8_t ch;

	qDebug() << "E24xx::Probe(" << probe_size << ") - IN";

	n_bank = 0;

	for (addr = base_addr, k = 0; k < max_bank; k++, addr += 2)
	{
		if (GetBus()->Read(addr, &ch, 1) != 1)
		{
			if ((error = GetBus()->Error()) != IICERR_NOADDRACK)
			{
				//      GetBus()->Open(E2Profile::GetPort());
				return error;
			}
		}
		else
		{
			eeprom_addr[n_bank++] = addr;
		}
	}

	if (probe_size)
	{
		SetNoOfBank(n_bank);
	}

	qDebug() << "E24xx::Probe() = " << n_bank << " - OUT";

	return n_bank;
}


int E24xx::Read(int probe, int type)
{
	int error = Probe(probe || GetNoOfBank() == 0);

	if (error < 0)
	{
		return error;
	}

	GetBus()->ReadStart();

	if (type & PROG_TYPE)
	{
		int k;

		// legge il contenuto attuale della EEPROM in memoria
		//      for (k = 0; k < GetNoOfBank(); k++)
		//modificata in modo che legga sempre tutto l'intero
		// contenuto del dispositivo indipendentemente dalla
		// capacita` correntemente selezionata
		for (k = 0; k < n_bank; k++)
		{
			if ((error = bank_in(GetBufPtr() + k * GetBankSize(), k)))
			{
				//      GetBus()->Open(E2Profile::GetPort());              //ormai obsoleta
				return error;
			}

			if (GetBus()->ReadProgress((k + 1) * 100 / n_bank))
			{
				return OP_ABORTED;
			}
		}
	}

	GetBus()->ReadEnd();

	if (n_bank > 0)
	{
		return GetNoOfBank();
	}
	else
	{
		return 0;
	}
}

int E24xx::Write(int probe, int type)
{
	int error = Probe(probe || GetNoOfBank() == 0);

	if (error < 0)
	{
		return error;
	}

	GetBus()->WriteStart();

	if (type & PROG_TYPE)
	{
		// program the eeprom
		int k;

		for (k = 0; k < GetNoOfBank(); k++)
		{
			if ((error = bank_out(GetBufPtr() + k * GetBankSize(), k)))
			{
				//      GetBus()->Open(E2Profile::GetPort());
				return error;
			}

			if (GetBus()->WriteProgress((k + 1) * 100 / GetNoOfBank()))
			{
				return OP_ABORTED;
			}
		}
	}

	GetBus()->WriteEnd();

	return GetNoOfBank();
}

int E24xx::Verify(int type)
{
	Probe();        //Moved here from 7 lines above (10/12/99)

	if (GetNoOfBank() == 0)
	{
		return BADPARAM;
	}

	unsigned char *localbuf = new unsigned char[GetBankSize()];

	if (localbuf == 0)
	{
		return OUTOFMEMORY;
	}

	GetBus()->ReadStart();

	int rval = 1;

	if (type & PROG_TYPE)
	{
		// legge il contenuto attuale della EEPROM in memoria
		int k;

		for (k = 0; k < GetNoOfBank(); k++)
		{
			int error;

			if ((error = bank_in(localbuf, k)))
			{
				//      GetBus()->Open(E2Profile::GetPort());
				rval = error;
				break;
			}

			if (memcmp(GetBufPtr() + k * GetBankSize(), localbuf, GetBankSize()) != 0)
			{
				rval = 0;
				break;
			}

			if (GetBus()->ReadProgress((k + 1) * 100 / GetNoOfBank()))
			{
				rval = OP_ABORTED;
				break;
			}
		}
	}

	GetBus()->ReadEnd();
	delete[] localbuf;
	return rval;
}

#define MAX_BANK_SIZE	256

// questa routine si aspetta che in ingresso i 256 bytes da programmare l'eeprom
// siano memorizzati nel iicbuffer nelle locazioni da 1 (non da 0!) a 256
int E24xx::bank_out(uint8_t const *copy_buf, int bank, long size, long idx)
{
	int k, j;
	uint8_t buffer[MAX_BANK_SIZE + 1];

	if (size <= 0)
	{
		size = GetBankSize();
	}

	qDebug() << "E24xx::bank_out() = bank " << bank << ", size = " << size << ", idx = " << idx;

	if (copy_buf == 0 || bank >= GetNoOfBank() || size > MAX_BANK_SIZE)
	{
		return BADPARAM;
	}

	memcpy(buffer + 1, copy_buf, size);
#if 0

	for (j = 0; j < size; j++)
	{
		buffer[j] = j;

		if (GetBus()->Write(eeprom_addr[bank], buffer + j, 2) != 2)
		{
			return GetBus()->Error();
		}

		for (k = timeout_loop; k > 0 && GetBus()->Read(eeprom_addr[bank], buffer, 1) != 1; k--)
			;

		if (k == 0)
		{
			return E2P_TIMEOUT;
		}
	}

#else

	for (j = 0; j < size; j += writepage_size)
	{
		buffer[j] = j;

		if (GetBus()->Write(eeprom_addr[bank], buffer + j, 1 + writepage_size) != (1 + writepage_size))
		{
			return GetBus()->Error();
		}

		for (k = timeout_loop; k > 0 && GetBus()->Read(eeprom_addr[bank], buffer, 1) != 1; k--)
			;

		if (k == 0)
		{
			return E2P_TIMEOUT;
		}
	}

#endif

	return OK;
}

int E24xx::bank_in(uint8_t *copy_buf, int bank, long size, long idx)
{
	uint8_t ch;

	if (copy_buf == 0)   // || bank >= GetNoOfBank())
	{
		return BADPARAM;
	}

	if (size <= 0)
	{
		size = GetBankSize();
	}

	ch = (uint8_t)idx;

	if (GetBus()->StartWrite(eeprom_addr[bank], &ch, 1) != 1)
	{
		return GetBus()->Error();
	}

	if (sequential_read)
	{
		if (GetBus()->Read(eeprom_addr[bank], copy_buf, size) < size)
		{
			return GetBus()->Error();
		}
	}
	else
	{
		int k;

		for (k = 0; k < size; k++)
		{
			ch = (uint8_t)(k + idx);

			if (GetBus()->StartWrite(eeprom_addr[bank], &ch, 1) != 1)
			{
				return GetBus()->Error();
			}

			if (GetBus()->Read(eeprom_addr[bank], copy_buf + k, 1) != 1)
			{
				return GetBus()->Error();
			}
		}
	}

	return OK;
}

#define CMP_LEN 16

//====================>>> E24xx::BankRollOverDetect <<<====================
int E24xx::BankRollOverDetect(int force)
{
	int rlv = 0;

	if (GetNoOfBank() > 1)
	{
		uint8_t index;
		uint8_t buf[CMP_LEN + 1], buf1[CMP_LEN], buf2[CMP_LEN];
#if 0
		//Lettura dal primo banco
		index = 0;

		if (GetBus()->StartWrite(eeprom_addr[0], &index, 1) != 1)
		{
			return GetBus()->Error();
		}

		if (GetBus()->Read(eeprom_addr[0], buf1, CMP_LEN) != CMP_LEN)
		{
			return GetBus()->Error();
		}

		//Lettura dal secondo banco
		index = 0;

		if (GetBus()->StartWrite(eeprom_addr[1], &index, 1) != 1)
		{
			return GetBus()->Error();
		}

		if (GetBus()->Read(eeprom_addr[1], buf2, CMP_LEN) != CMP_LEN)
		{
			return GetBus()->Error();
		}

#else
		int error;

		if ((error = bank_in(buf1, 0, CMP_LEN)))
		{
			return error;
		}

		if ((error = bank_in(buf2, 1, CMP_LEN)))
		{
			return error;
		}

#endif

		//Se i primi due banchi sono differenti e` possibile
		//  determinare il bank-rollover
		if (memcmp(buf1, buf2, CMP_LEN) != 0)
		{
			//lettura iniziando dalla fine del primo banco
			index = GetBankSize() - 1;
#if 0

			if (GetBus()->StartWrite(eeprom_addr[0], &index, 1) != 1)
			{
				return GetBus()->Error();
			}

			if (GetBus()->Read(eeprom_addr[0], buf, CMP_LEN + 1) != CMP_LEN + 1)
			{
				return GetBus()->Error();
			}

#else

			if ((error = bank_in(buf, 0, CMP_LEN + 1, GetBankSize() - 1)))
			{
				return error;
			}

#endif

			rlv = (memcmp(buf + 1, buf1, CMP_LEN) == 0) ? 1 : 2;
		}
		else if (force)
		{
			//i banchi sono uguali: modifichiamo temporaneamente
			//  il primo byte del secondo banco (inversione di bit)
			buf[1] = ~buf2[0];      //inversione di bit
			buf[0] = 0;                     //indice (address byte)

			if (GetBus()->Write(eeprom_addr[1], buf, 2) != 2)
			{
				return GetBus()->Error();
			}

			int k;

			for (k = timeout_loop; k > 0 && GetBus()->Read(eeprom_addr[1], buf, 1) != 1; k--)
				;

			if (k == 0)
			{
				return E2P_TIMEOUT;
			}

			//Ok, ora procedi all roll-over detect
			index = GetBankSize() - 1;

			if (GetBus()->StartWrite(eeprom_addr[0], &index, 1) != 1)
			{
				return GetBus()->Error();
			}

			if (GetBus()->Read(eeprom_addr[0], buf, CMP_LEN + 1) != CMP_LEN + 1)
			{
				return GetBus()->Error();
			}

			rlv = (memcmp(buf + 1, buf1, CMP_LEN) == 0) ? 1 : 2;

			//rimetti a posto il contenuto dell'eeprom
			buf[1] = buf2[0];       //contenuto originale
			buf[0] = 0;                     //indice (address byte)

			if (GetBus()->Write(eeprom_addr[1], buf, 2) != 2)
			{
				return GetBus()->Error();
			}

			for (k = timeout_loop; k > 0 && GetBus()->Read(eeprom_addr[1], buf, 1) != 1; k--)
				;

			if (k == 0)
			{
				return E2P_TIMEOUT;
			}
		}
	}

	return rlv;
}

