//=========================================================================//
//-------------------------------------------------------------------------//
// e2awinfo.cpp -- Source for e2AppWinInfo class                           //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C)  1997-2000  Claudio Lanconelli                           //
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

#include "e2app.h"
#include "e2cnv.h"
#include "e2awinfo.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

#include <v/vnotice.h>
#include <stdio.h>

//======================>>> e2AppWinInfo::e2AppWinInfo <<<=======================
e2AppWinInfo::e2AppWinInfo(vCmdWindow* win, char* name, BusIO** busvptr, void* ptr)
	:	vAppWinInfo(name),
		hex_per_line(16),
		buffer_size(BUFFER_SIZE),
		buf_ok(0),
		buf_changed(0),
		fname(0),
		no_block(0),
		splitted(0),
		roll_over(0),
		fuse_bits(0),
		lock_bits(0),
		crc(0)
{
	UserDebug(Constructor, "e2AppWinInfo::e2AppWinInfo()\n");

	// Constructor
	cmdWin = (e2CmdWindow*)win;
	cmdWin->SetAWInfo(this);


	//AutoTag
	//Initialize Device Pointers vector
	eep24xx.SetAWInfo(this);
	eep24xx.SetBus(busvptr[I2C-1]);
	eep24xxx.SetAWInfo(this);
	eep24xxx.SetBus(busvptr[I2C-1]);
	eep24xx5.SetAWInfo(this);
	eep24xx5.SetBus(busvptr[I2C-1]);
	eepAt90s.SetAWInfo(this);
	eepAt90s.SetBus(busvptr[AT90S-1]);
	eepAt89s.SetAWInfo(this);
	eepAt89s.SetBus(busvptr[AT89S-1]);
	eep93x6.SetAWInfo(this);
	eep93x6.SetBus(busvptr[AT93C-1]);
	eepPic16.SetAWInfo(this);
	eepPic16.SetBus(busvptr[PICB-1]);
	eep250xx.SetAWInfo(this);
	eep250xx.SetBus(busvptr[AT250-1]);
	eep25xxx.SetAWInfo(this);
	eep25xxx.SetBus(busvptr[AT250BIG-1]);
	eep2506.SetAWInfo(this);
	eep2506.SetBus(busvptr[SDEB-1]);

	if (name && strlen(name))
		SetFileName(name);

	//Initialize File Formats vector
	fbufvet[E2P] = &e2pfbuf;
	fbufvet[INTEL] = &intfbuf;
	fbufvet[MOTOS] = &motsfbuf;		//** 17/03/99
	fbufvet[BIN] = &hexfbuf;

	int k;
	for (k = 0; k < NO_OF_FILETYPE; k++)
		fbufvet[k]->SetAWInfo(this);

	eeprom_string[0] = '\0';	//eeprom string ID
	eeprom_comment[0] = '\0';	//eeprom comment

	//imposta il tipo di eeprom di default
//	SetEEProm(THEAPP->GetLastEEPType());	18/10/98
	SetEEProm( GetE2PPriType(THEAPP->GetLastDevType()), GetE2PSubType(THEAPP->GetLastDevType()) );
	SetFileBuf(E2P);

	// Test and initialize the hardware
	if (THEAPP->GetCounter() == 1)
	{
		cmdWin->About();

		//imposta il bus iniziale (relativo al tipo di eeprom)
		THEAPP->SetInitialBus( eep->GetBus() );

		// Nel caso di parametro passato da linea di comando
		if (GetFileName())
		{
			if (Load(0) <= 0)
				SetFileName(0);
		}
	/**
		else
		{
			int err = THEAPP->TestPort();

			if (err)
			{
				vNoticeDialog note(win);
				if (err == E2ERR_ACCESSDENIED)
				{
					note.Notice("Error: you must be root to run this program");
					theApp->Exit();
				}
				else
				{
					note.Notice("Error: the interface don't respond");
				}
			}
			else
				THEAPP->OpenPort();
		}
	**/
		else
		{
			int err = THEAPP->OpenPort();

			if (err == E2ERR_ACCESSDENIED)
			{
				vNoticeDialog note(win);
				note.Notice("I/O access denied. Run as root, or change the interface");
			//	theApp->Exit();
			}
			THEAPP->ClosePort();
		}
	}
	cmdWin->PostInit();
}


//======================>>> e2AppWinInfo::~e2AppWinInfo <<<=======================
e2AppWinInfo::~e2AppWinInfo()
{
	UserDebug(Destructor, "e2AppWinInfo::~e2AppWinInfo()\n");

	// Destructor
	if (fname)
		delete [] fname;
}

//===================>>> e2AppWinInfo::Reset <<<=============
void e2AppWinInfo::Reset()
{
	OpenBus();
	SleepBus();
}

//======================>>> e2AppWinInfo::SetEEProm <<<=======================
void e2AppWinInfo::SetEEProm(int type, int subtype)
{
	extern long BuildE2PType(int x, int y = 0);
	extern int GetE2PSubType(long x);
	extern int GetE2PPriType(long x);

	if (type == 0)
		type = E24XX;		//to avoid segV

	eep_type = type;
	eep_subtype = subtype;			//0 indica di usare GetNoOfBlock()
	long xtype = BuildE2PType(eep_type, eep_subtype);

	switch(eep_type)
	{
	//AutoTag
	//Setting the device pointer to selected type
	case E24XX:
		eep = &eep24xx;
		break;
	case E24XXX:
		eep = &eep24xxx;
		if (eep_subtype == 0)
		{
			//Forza impostazione manuale
			eep_subtype = GetE2PSubType(E2432);
		}
		eep->DefaultBankSize();		//reimposta la dimensione dei banchi di default
		break;
	case E24XX5:
		eep = &eep24xx5;
		break;
	case AT90SXX:
		eep = &eepAt90s;
/** commentato il 02/06/98, introdotto l'autodeterminazione del dispositivo
		if (eep_subtype == 0)
		{
			//Forza impstazione manuale
			eep_subtype = GetE2PSubType(AT90S1200);
		}
**/
		switch(xtype)
		{
		case AT90S0000:		//The AT1200S bus have to work with all AVR device
		case AT90S1200:
			eep->SetBus(THEAPP->GetBusVectorPtr()[AT1200S-1]);
			break;
		case ATmega603:
		case ATmega103:
			eep->SetBus(THEAPP->GetBusVectorPtr()[MEGAS-1]);
			break;
		default:	//AT90S std
			eep->SetBus(THEAPP->GetBusVectorPtr()[AT90S-1]);
			break;
		}
		break;
	case AT89SXX:
		eep = &eepAt89s;
		if (eep_subtype == 0)
		{
			//Forza impstazione manuale
			eep_subtype = GetE2PSubType(AT89S8252);
		}
		break;
	case E93X6:
		eep = &eep93x6;
		if (eep_subtype == 0)
		{
			//Forza impostazione manuale
			eep_subtype = GetE2PSubType(E9306);
		}
		break;
	case PIC16XX:
		eep = &eepPic16;
		if (eep_subtype == 0)
		{
			//Forza impostazione manuale
			eep_subtype = GetE2PSubType(PIC1684);
		}
		break;
	case E250XX:
		eep = &eep250xx;
		if (eep_subtype == 0)
		{
			//Forza impostazione manuale
			eep_subtype = GetE2PSubType(E25010);
		}
		break;
	case E25XXX:
		eep = &eep25xxx;
		if (eep_subtype == 0)
		{
			//Forza impostazione manuale
			eep_subtype = GetE2PSubType(E25080);
		}
//		eep->SetBus(THEAPP->GetBusVectorPtr()[AT250BIG-1]);
		break;
	case E2506XX:
		eep = &eep2506;
		if (eep_subtype == 0)
		{
			//Forza impostazione manuale
			eep_subtype = GetE2PSubType(E2506);
		}
		break;
	default:
		eep = &eep24xx;		//20/07/99	-- to prevent crash
		break;
	}

	SetSplittedInfo( GetEEPTypeSplit(type, eep_subtype) );

	//Imposta la nuova dimensione della memoria in
	// base al tipo di eeprom.
	SetNoOfBlock( GetEEPTypeSize(type, eep_subtype) );

	//Imposta la dimensione del banco che dipende
	// dal tipo di eeprom.
	SetBlockSize( eep->GetBankSize() );
}

//======================>>> e2AppWinInfo::Read <<<=======================
int e2AppWinInfo::Read()
{
	int probe = !eep_subtype;
	int rval;

	UserDebug(UserApp1, "e2AppWinInfo::Read() - IN\n");

	ClearBuffer();

	rval = OpenBus();		//Set correct bus and power up

	UserDebug1(UserApp1, "e2AppWinInfo::Read() ** OpenBus = %d\n", rval);

	if (rval == OK)
	{
		THEAPP->CheckEvents();

		if ( (rval = eep->Read(probe)) > 0 )
		{
			UserDebug1(UserApp1, "e2AppWinInfo::Read() ** Read = %d\n", rval);

			SetFileName(0);				//Questo per evitare che al prossimo save() si utilizzi il nome vecchio
			buf_ok = 1;
			buf_changed = 0;

			//Aggiunto il 18/03/99 con la determinazione dei numeri di banchi nelle E24xx2,
			// affinche` la dimensione rimanga quella impostata bisogna correggere la dimensione
			// del banco.
			SetBlockSize( eep->GetBankSize() );
		}

	}
	RecalcCRC();
	SleepBus();			//Close bus and power off

	UserDebug1(UserApp1, "e2AppWinInfo::Read() = %d - OUT\n", rval);

	return rval;
}

//======================>>> e2AppWinInfo::Write <<<=======================
int e2AppWinInfo::Write(int leave_on)	//07/06/98 -- added leaveON to avoid the power-off between write and verify
{
	int probe = !eep_subtype;
	int rval;

	UserDebug1(UserApp1, "e2AppWinInfo::Write(%d) - IN\n", leave_on);

	rval = OpenBus();
	if ( rval == OK )
	{
		THEAPP->CheckEvents();
		if ( (rval = eep->Write(probe)) > 0 )
		{
			//Aggiunto il 18/03/99 con la determinazione dei numeri di banchi nelle E24xx2,
			// affinche` la dimensione rimanga quella impostata bisogna correggere la dimensione
			// del banco.
			SetBlockSize( eep->GetBankSize() );
		}
		if ( !(rval > 0 && leave_on) )		//23/01/1999
			SleepBus();
	}

	UserDebug1(UserApp1, "e2AppWinInfo::Write() = %d - OUT\n", rval);

	return rval;
}

//======================>>> e2AppWinInfo::Verify <<<=======================
int e2AppWinInfo::Verify(int raise_power)
{
	int rval = OK;

	UserDebug1(UserApp1, "e2AppWinInfo::Verify(%d) - IN\n", raise_power);

	if (raise_power)		//23/01/1999 -- if the Verify follow the Write don't reset the micro again
		rval = OpenBus();
	if (rval == OK)
		rval = eep->Verify();
	SleepBus();

	UserDebug1(UserApp1, "e2AppWinInfo::Verify() = %d - OUT\n", rval);

	return rval;
}

//===================>>> e2AppWinInfo::Erase <<<=============
int e2AppWinInfo::Erase()
{
	int rval;

	rval = OpenBus();
	if (rval == OK)
		rval = eep->Erase();
	SleepBus();

	return rval;
}

//===================>>> e2AppWinInfo::BankRollOverDetect <<<=============
int e2AppWinInfo::BankRollOverDetect(int force)
{
	if ( OpenBus() == OK )
		roll_over = eep->BankRollOverDetect(force);
	SleepBus();

	return roll_over;
}

//======================>>> e2AppWinInfo::Load <<<=======================
int e2AppWinInfo::Load(int split_bank)
{
	int rval;

	SetFileBuf(E2P);
	rval = LoadFile(split_bank);
	if ( rval <= 0 )
	{
		SetFileBuf(MOTOS);
		rval = LoadFile(split_bank);
		if ( rval <= 0 )
		{
			SetFileBuf(INTEL);
			rval = LoadFile(split_bank);
		}
		if ( rval <= 0 )
		{
			SetFileBuf(BIN);
			rval = LoadFile(split_bank);
		}

		//spostato qui il 10/04/99
		if ( rval > 0 && GetEEPPriType() == PIC16XX )
		{	//Altro barbatrucco, questa volta per i PIC.
			//L'assemblatore colloca la eeprom e la configuration
			//ad indirizzi alti, mentre noi li collochiamo subito
			//dopo la flash. Qui facciamo la rilocazione.
			memcpy(GetBufPtr()+GetSplittedInfo(), GetBufPtr() + (0x2000 * 2), 16);
			memcpy(GetBufPtr()+GetSplittedInfo()+16, GetBufPtr() + (0x2100 * 2), GetSize() - (GetSplittedInfo() + 16) );

			SetFuseBits( ~*(GetBufPtr()+GetSplittedInfo()+14) );
		}
	}

	return rval;
}

//======================>>> e2AppWinInfo::Load <<<=======================
int e2AppWinInfo::LoadFile(int bank)
{
	if (!bank)
		ClearBuffer();
	int rval = fbufp->Load(bank);
	if (rval > 0)
	{
		buf_ok = 1;
		buf_changed = 0;
	}
	RecalcCRC();
	return rval;
}

//======================>>> e2AppWinInfo::Save <<<=======================
int e2AppWinInfo::Save()
{
	int rval = fbufp->Save();
	if (rval > 0)
		buf_changed = 0;

	return rval;
}

//======================>>> e2AppWinInfo::GetSize <<<=======================
long e2AppWinInfo::GetSize() const
{
	return (long)GetNoOfBlock() * (long)GetBlockSize();
}

void e2AppWinInfo::SetFileBuf(FileType type)
{
	if (type >= 0 && type < NO_OF_FILETYPE)
		fbufp = fbufvet[type];
	else
		fbufp = fbufvet[E2P];	//Tipo di default
}

void e2AppWinInfo::SetFileName(char const *name)
{
	if (name)
		THEAPP->SetLastFile(name);

	if (fname)
		delete [] fname;
	fname = 0;				//nel caso in cui name sia 0
	if ( name && (fname = new char[strlen(name)+1]) )
		strcpy(fname, name);
}

char *e2AppWinInfo::GetStringID(char *s)
{
	if (s)
	{
		strncpy(s, eeprom_string, STRINGID_SIZE-1);
		s[STRINGID_SIZE-1] = '\0';
		return s;
	}
	else
		return eeprom_string;
}

void e2AppWinInfo::SetStringID(char const *s)
{
	if (s)
	{
		strncpy(eeprom_string, s, STRINGID_SIZE-1);
		eeprom_string[STRINGID_SIZE-1] = '\0';
	}
}

char *e2AppWinInfo::GetComment(char *s)
{
	if (s)
	{
		strncpy(s, eeprom_comment, COMMENT_SIZE-1);
		s[COMMENT_SIZE-1] = '\0';
		return s;
	}
	else
		return eeprom_comment;
}

void e2AppWinInfo::SetComment(char const *s)
{
	if (s)
		strncpy(eeprom_comment, s, COMMENT_SIZE-1);
	eeprom_comment[COMMENT_SIZE-1] = '\0';
}

void e2AppWinInfo::ClearBuffer()
{
	memset(GetBufPtr(), 0xFF, GetBufSize());
}

void e2AppWinInfo::FillBuffer(int init_pos, int ch, long len)
{
	long l;

	if (len > 0)
		l = len;
	else
		l = GetBufSize();

	if (init_pos >= GetBufSize())
		return;			//Bad value

	// Check for buffer overflow
	if (init_pos + l > GetBufSize())
		l = GetBufSize() - init_pos;

	// If the buffer was not yet initialized we first
	//   clear it.
	if (!buf_ok)
		ClearBuffer();

	memset(GetBufPtr() + init_pos, ch, l);

	buf_ok = 1;		//Validate buffer

	RecalcCRC();
	BufChanged();
}

void e2AppWinInfo::SwapBytes()
{
	long size = GetNoOfBlock() * GetBlockSize();
	long k;
	UBYTE tmp;

	for (k = 0; k < size; k += 2)
	{
		tmp = buffer[k];
		buffer[k] = buffer[k+1];
		buffer[k+1] = tmp;
	}

	RecalcCRC();
	BufChanged();
}

void e2AppWinInfo::DoubleSize()
{
	if (GetSize() == AUTOSIZE_ID)
		return;

	//Attenzione!! Il buffer deve essere capiente il doppio
	//  della dimensione del dispositivo attuale
//	int n = GetNoOfBlock();
	int k;
	for (k = GetNoOfBlock()-1; k >= 0; k--)
	{	//Copia l'ultimo banco nei due banchi in fondo
		if (k)
		{	//Non copiarlo su se stesso!
		memcpy(	buffer + k*2 * GetBlockSize(),
				buffer + k * GetBlockSize(), 
				GetBlockSize()
				);
		}
		memcpy(	buffer + (k*2+1) * GetBlockSize(),
				buffer + k * GetBlockSize(),
				GetBlockSize()
				);
	}

	RecalcCRC();
	BufChanged();
}

int e2AppWinInfo::SecurityRead(int &blocks)
{
	int rv, val;

	OpenBus();
	rv = eep->SecurityRead(blocks, val);
	SleepBus();

	if (rv == OK)
	{
		lock_bits = blocks;
		if (val >= 0)
			fuse_bits = val;
	}
	return rv;
}

int e2AppWinInfo::SecurityWrite(int blocks)
{
	int rv;
	if (blocks < 0)
		blocks = lock_bits;

	OpenBus();
	rv = eep->SecurityWrite(blocks, fuse_bits);
	SleepBus();

	return rv;
}

//The 24C65 microchip device has high endurance bits
// in place of Fuse bits.
// Use the same variable because there is no room for
// both in the e2p header.
int e2AppWinInfo::HighEnduranceRead(int &block_no)
{
	int rv;

	OpenBus();
	rv = eep->HighEnduranceRead(block_no);
	SleepBus();

	if (rv == OK)
		fuse_bits = block_no;
	return rv;
}

int e2AppWinInfo::HighEnduranceWrite(int block_no)
{
	int rv;
	if (block_no < 0)
		block_no = fuse_bits;

	OpenBus();
	rv = eep->HighEnduranceWrite(block_no);
	SleepBus();

	if (rv == OK)
	{
		int block2;

		OpenBus();
		rv = eep->HighEnduranceRead(block2);
		SleepBus();

		if (rv == OK && block_no != block2)
			rv = E2ERR_WRITEFAILED;
	}

	return rv;
}

int e2AppWinInfo::FusesRead(int &bits)
{
	int rv;

	OpenBus();
	rv = eep->FusesRead(bits);
	SleepBus();

	if (rv == OK && bits >= 0)
		fuse_bits = bits;
	return rv;
}

int e2AppWinInfo::FusesWrite(int bits)
{
	int rv;
	if (bits < 0)
		bits = fuse_bits;

	OpenBus();
	rv = eep->FusesWrite(bits);
	SleepBus();

/**	the read doesn't work with every device
	if (rv == OK)
	{
		int bits2;

		OpenBus();
		rv = eep->FusesRead(bits2);
		SleepBus();

		if (rv == OK && bits != bits2)
			rv = E2ERR_WRITEFAILED;
	}
**/
	return rv;
}

#include "crc.h"

UWORD e2AppWinInfo::RecalcCRC()
{
	UWORD crc_val = mcalc_crc(GetBufPtr(), GetSize());
	SetCRC(crc_val);
	return crc_val;
}


#include <ctype.h>

//======================>>> e2AppWinInfo::Dump <<<=======================
char const *e2AppWinInfo::Dump(int line, int type)
{
	long idx;
	long upperlimit;

	if (!buf_ok)
		return "";

	upperlimit = GetSize() == 0 ? GetBufSize() : GetSize();

	idx = line * hex_per_line;
	if (idx < upperlimit)
	{
		if (type == 0)
		{
			char tmpbuf[16+1];
			for (int k = 0; k < hex_per_line; k++)
				tmpbuf[k] = isprint(buffer[idx+k]) ? buffer[idx+k] : '.';
			tmpbuf[hex_per_line] = 0;

		//*** 13/09/99
		//	sprintf(linebuf, "%04lX) %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X    %s\n",
			sprintf(linebuf, "  %06lX) %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X    %s\n",
					idx,
					buffer[idx+0],
					buffer[idx+1],
					buffer[idx+2],
					buffer[idx+3],
					buffer[idx+4],
					buffer[idx+5],
					buffer[idx+6],
					buffer[idx+7],

					buffer[idx+ 8],
					buffer[idx+ 9],
					buffer[idx+10],
					buffer[idx+11],
					buffer[idx+12],
					buffer[idx+13],
					buffer[idx+14],
					buffer[idx+15],

					tmpbuf
			);
		}
		else
		if (type == 1)
		{
		//*** 13/09/99
		//	sprintf(linebuf, "%04lX)",	idx);
			sprintf(linebuf, "  %06lX)",	idx);
		}
		else
		if (type == 2)
		{
			sprintf(linebuf, " %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X  ",
					buffer[idx+0],
					buffer[idx+1],
					buffer[idx+2],
					buffer[idx+3],
					buffer[idx+4],
					buffer[idx+5],
					buffer[idx+6],
					buffer[idx+7],

					buffer[idx+ 8],
					buffer[idx+ 9],
					buffer[idx+10],
					buffer[idx+11],
					buffer[idx+12],
					buffer[idx+13],
					buffer[idx+14],
					buffer[idx+15]
			);
		}
		else
		{
			char tmpbuf[16+1];
			for (int k = 0; k < hex_per_line; k++)
				tmpbuf[k] = isprint(buffer[idx+k]) ? buffer[idx+k] : '.';
			tmpbuf[hex_per_line] = 0;

			sprintf(linebuf, "  %s\n", tmpbuf);
		}
	}

	return linebuf;
}
