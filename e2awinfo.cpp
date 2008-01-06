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

#include "e2app.h"
#include "e2cnv.h"
#include "e2awinfo.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

#include <v/vnotice.h>
#include <stdio.h>

#ifdef	WIN32
#  ifdef	__BORLANDC__
#    define	strcasecmp stricmp
#  else // _MICROSOFT_ VC++
#    define strcasecmp	_stricmp
#    define snprintf	_snprintf
#  endif
#endif

//======================>>> e2AppWinInfo::e2AppWinInfo <<<=======================
e2AppWinInfo::e2AppWinInfo(vCmdWindow* win, char* name, BusIO** busvptr, void* ptr)
	:	vAppWinInfo(name),
		hex_per_line(16),
		buffer_size(BUFFER_SIZE),
		load_type(ALL_TYPE),
		save_type(ALL_TYPE),
		load_relocation(0),
		save_relocation(0),
		buf_ok(false),
		buf_changed(false),
		fname(0),
		no_block(0),
		splitted(0),
		roll_over(0),
		fuse_bits(0),
		lock_bits(0),
		fuse_ok(false),
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
	eep2401.SetAWInfo(this);
	eep2401.SetBus(busvptr[I2C-1]);
	eep24xx1.SetAWInfo(this);
	eep24xx1.SetBus(busvptr[I2C-1]);
	eep24xx2.SetAWInfo(this);
	eep24xx2.SetBus(busvptr[I2C-1]);
	eep24xx5.SetAWInfo(this);
	eep24xx5.SetBus(busvptr[I2C-1]);
	eepAt90s.SetAWInfo(this);
	eepAt90s.SetBus(busvptr[AT90S-1]);
	eepAt89s.SetAWInfo(this);
	eepAt89s.SetBus(busvptr[AT89S-1]);
	eep93xx16.SetAWInfo(this);
	eep93xx16.SetBus(busvptr[AT93C-1]);
	eep93xx8.SetAWInfo(this);
	eep93xx8.SetBus(busvptr[AT93C-1]);
	eepPic16.SetAWInfo(this);
	eepPic16.SetBus(busvptr[PICB-1]);
	eep250xx.SetAWInfo(this);
	eep250xx.SetBus(busvptr[AT250-1]);
	eep25xxx.SetAWInfo(this);
	eep25xxx.SetBus(busvptr[AT250BIG-1]);
	eep2506.SetAWInfo(this);
	eep2506.SetBus(busvptr[SDEB-1]);
	eepPic168xx.SetAWInfo(this);
	eepPic168xx.SetBus(busvptr[PICNEWB-1]);
	eep3060.SetAWInfo(this);
	eep3060.SetBus(busvptr[IMBUS-1]);
	eepPic125xx.SetAWInfo(this);
	eepPic125xx.SetBus(busvptr[PIC12B-1]);
	eep17xxx.SetAWInfo(this);
	eep17xxx.SetBus(busvptr[I2C-1]);
	eep2444.SetAWInfo(this);
	eep2444.SetBus(busvptr[X2444B-1]);
	eep2444.DefaultBankSize();
	eep2430.SetAWInfo(this);
	eep2430.SetBus(busvptr[S2430B-1]);
	eep2430.DefaultBankSize();

	if (name && strlen(name))
		SetFileName(name);

	//Initialize File Formats vector
	fbufvet[E2P] = &e2pfbuf;
	fbufvet[INTEL] = &intfbuf;
	fbufvet[MOTOS] = &motsfbuf;		//** 17/03/99
	fbufvet[BIN] = &binfbuf;
	fbufvet[CSM] = &csmfbuf;

	int k;
	for (k = 0; k < NO_OF_FILETYPE; k++)
		fbufvet[k]->SetAWInfo(this);

	eeprom_string[0] = '\0';	//eeprom string ID
	eeprom_comment[0] = '\0';	//eeprom comment

	ClearBuffer();				//Clear the new buffer

	//imposta il tipo di eeprom di default
//	SetEEProm(THEAPP->GetLastEEPType());	18/10/98
	SetEEProm( GetE2PPriType(THEAPP->GetLastDevType()), GetE2PSubType(THEAPP->GetLastDevType()) );
	SetFileBuf(THEAPP->GetDefaultFileType());	//	SetFileBuf(E2P);

	// Test and initialize the hardware
	if (THEAPP->GetCounter() == 1)
	{
		if ( !THEAPP->scriptMode )
			cmdWin->About();

		THEAPP->LoadDriver(1);

		//imposta il bus iniziale (relativo al tipo di eeprom)
		THEAPP->SetInitialBus( eep->GetBus() );

		//case of command line parameter
		if (GetFileName())
		{
			if (Load() <= 0)
				SetFileName(0);
		}
		else
		{
			int err = THEAPP->OpenPort();

			if (err == E2ERR_ACCESSDENIED)
			{
				vNoticeDialog note(win);
#ifdef	_WINDOWS
				note.Notice("I/O access denied. Driver not found, try to install the software again");
#else
				note.Notice("I/O access denied. Run as root, or change the interface");
#endif
			}
			THEAPP->ClosePort();
		}
	}
	else
	if (GetFileName())
	{
		if (Load() <= 0)
			SetFileName(0);
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
//	extern long BuildE2PType(int x, int y = 0);
	extern int GetE2PSubType(long x);
	extern int GetE2PPriType(long x);

	if (type == 0)
		type = E24XX;		//to avoid segV

	eep_type = type;
	eep_subtype = subtype;			//0 indica di usare GetNoOfBlock()
	long xtype = GetEEPType();

	switch(eep_type)
	{
	//AutoTag
	//Setting the device pointer to selected type
	case E24XX:
		eep = &eep24xx;
		break;
	case E24XX1_A:
		eep = &eep24xx1;
		if (eep_subtype == 0)
		{
			//Forza impostazione manuale
			eep_subtype = GetE2PSubType(E2401_A);
		}
		break;
	case E24XX1_B:
		eep = &eep2401;
		if (eep_subtype == 0)
		{
			//Forza impostazione manuale
			eep_subtype = GetE2PSubType(E2401_B);
		}
		break;
	case E24XX2:
		eep = &eep24xx2;
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
		eep->SetWritePageSize( GetEEPTypeWPageSize(eep_type, eep_subtype) );

		switch(xtype)
		{
		case AT90S0000:		//The AT1200S bus have to work with all AVR device
		case AT90S1200:
			eep->SetBus(THEAPP->GetBusVectorPtr()[AT1200S-1]);
			break;
		case ATmega603:		//Old mega don't support page polling
		case ATmega103:
			{
				AtMegaBus *b = (AtMegaBus *)THEAPP->GetBusVectorPtr()[ATMEGAB-1];
				if (b)
				{
					b->SetFlashPagePolling(false);
					b->SetPageSize( eep->GetWritePageSize() );
				}
				eep->SetBus(b);
			}
			break;
		default:
			{
				AtMegaBus *b = (AtMegaBus *)THEAPP->GetBusVectorPtr()[ATMEGAB-1];
				if (b)
				{
					b->SetFlashPagePolling(true);
					b->SetPageSize( eep->GetWritePageSize() );
				}
				eep->SetBus(b);
			}
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
		eep = &eep93xx16;
		if (eep_subtype == 0)
		{
			//Forza impostazione manuale
			eep_subtype = GetE2PSubType(E9306);
		}
		break;
	case E93XX_8:
		eep = &eep93xx8;
		if (eep_subtype == 0)
		{
			//Forza impostazione manuale
			eep_subtype = GetE2PSubType(E9306_8);
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
	case PIC168XX:
		eep = &eepPic168xx;
	//	if (eep_subtype == 0)
	//	{
	//		eep_subtype = GetE2PSubType(PIC1684A);
	//	}
		break;
 	case PIC125XX:
		eep = &eepPic125xx;
		if (eep_subtype == 0)
		{
			//Forza impstazione manuale
			eep_subtype = GetE2PSubType(PIC12508);
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
	case ENVMXXX:
		eep = &eep3060;
		if (eep_subtype == 0)
		{
			//Forza impostazione manuale
			eep_subtype = GetE2PSubType(ENVM3060);
		}
		break;
	case AT17XXX:
		eep = &eep17xxx;
		if (eep_subtype == 0)
		{
			//Forza impostazione manuale
			eep_subtype = GetE2PSubType(AT1765);
		}
		break;
	case X24C44XX:
		if (eep_subtype == 0)
		{
			//Forza impostazione manuale
			eep_subtype = GetE2PSubType(S24H30);
		}
		if (GetEEPType() == S24H30)
			eep = &eep2430;
		else
			eep = &eep2444;
		break;
	default:
		eep = &eep24xx;		//20/07/99	-- to prevent crash
		break;
	}
	fuse_ok = false;		//invalidate current fuse settings
	SetSplittedInfo( GetEEPTypeSplit(type, eep_subtype) );

	//Imposta la nuova dimensione della memoria in
	// base al tipo di eeprom.
	SetNoOfBlock( GetEEPTypeSize(type, eep_subtype) );

	//Imposta la dimensione del banco che dipende
	// dal tipo di eeprom.
	SetBlockSize( eep->GetBankSize() );
}

//======================>>> e2AppWinInfo::Read <<<=======================
int e2AppWinInfo::Read(int type, int raise_power, int leave_on)
{
	int probe = !eep_subtype;
	int rval = OK;

	UserDebug3(UserApp1, "e2AppWinInfo::Read(%d,%d,%d) - IN\n",type,raise_power,leave_on);

	if ( THEAPP->GetClearBufBeforeRead() )
	{
		if (load_type == ALL_TYPE)
		{
			ClearBuffer(ALL_TYPE);
		}
		else
		{
			if (load_type & PROG_TYPE)
			{
				ClearBuffer(PROG_TYPE);
			}
			if (load_type & DATA_TYPE)
			{
				ClearBuffer(DATA_TYPE);
			}
		}
	}

	if (raise_power)
		rval = OpenBus();		//Set correct bus and power up

	UserDebug1(UserApp1, "e2AppWinInfo::Read() ** OpenBus = %d\n", rval);

	if (rval == OK)
	{
		THEAPP->CheckEvents();

		if ( (rval = eep->Read(probe,type)) > 0 )
		{
			UserDebug1(UserApp1, "e2AppWinInfo::Read() ** Read = %d\n", rval);

			SetFileName(0);				//Questo per evitare che al prossimo save() si utilizzi il nome vecchio
			buf_ok = true;
			buf_changed = false;

			//Aggiunto il 18/03/99 con la determinazione dei numeri di banchi nelle E24xx2,
			// affinche` la dimensione rimanga quella impostata bisogna correggere la dimensione
			// del banco.
			SetBlockSize( eep->GetBankSize() );
		}

		RecalcCRC();

		if ( !(rval > 0 && leave_on) )
			SleepBus();
	}

	UserDebug1(UserApp1, "e2AppWinInfo::Read() = %d - OUT\n", rval);

	return rval;
}

//======================>>> e2AppWinInfo::Write <<<=======================
int e2AppWinInfo::Write(int type, int raise_power, int leave_on)
{
	int probe = !eep_subtype;
	int rval = OK;

	UserDebug3(UserApp1, "e2AppWinInfo::Write(%d,%d,%d) - IN\n", type,raise_power,leave_on);

	if (raise_power)
		rval = OpenBus();
	if ( rval == OK )
	{
		THEAPP->CheckEvents();
		if ( (rval = eep->Write(probe,type)) > 0 )
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
int e2AppWinInfo::Verify(int type, int raise_power, int leave_on)
{
	int rval = OK;

	UserDebug3(UserApp1, "e2AppWinInfo::Verify(%d,%d,%d) - IN\n", type,raise_power,leave_on);

	if (raise_power)
		rval = OpenBus();
	if (rval == OK)
	{
		rval = eep->Verify(type);
		if ( !(rval >= 0 && leave_on) )
			SleepBus();
	}

	UserDebug1(UserApp1, "e2AppWinInfo::Verify() = %d - OUT\n", rval);

	return rval;
}

//======================>>> e2AppWinInfo::Erase <<<=======================
int e2AppWinInfo::Erase(int type, int raise_power, int leave_on)
{
	int rval = OK;

	UserDebug3(UserApp1, "e2AppWinInfo::Erase(%d,%d,%d) - IN\n", type,raise_power,leave_on);

	if (raise_power)
		rval = OpenBus();
	if (rval == OK)
	{
		rval = eep->Erase(1, type);
		if ( !(rval >= 0 && leave_on) )
			SleepBus();
	}

	UserDebug1(UserApp1, "e2AppWinInfo::Erase() = %d - OUT\n", rval);

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
int e2AppWinInfo::Load()
{
	int rval;

	SetFileBuf(E2P);
	rval = LoadFile();
	if ( rval <= 0 )
	{
		SetFileBuf(MOTOS);
		rval = LoadFile();
		if ( rval <= 0 )
		{
			SetFileBuf(INTEL);
			rval = LoadFile();
		}
		if ( rval <= 0 )
		{
			SetFileBuf(CSM);
			rval = LoadFile();
		}
		if ( rval <= 0 )
		{
			SetFileBuf(BIN);
			rval = LoadFile();
		}

		if ( rval > 0 )
		{
			if ( GetEEPPriType() == PIC16XX ||
				  GetEEPPriType() == PIC168XX )
      			{
				//It seems a bit tricky...
				//Relocate the DATA and CONFIG memory with PIC devices
				//The assembler store the DATA at 0x2100 (word address) and
				//CONFIG at 0x2000 (word address).
				//We move them after PROGRAM memory
				// PROGRAM MEMORY + DATA MEMORY + CONFIG (8 word)
				if (GetSplittedInfo() > 0 && GetSize() > GetSplittedInfo())
				{
					//Copy Config memory
					if ( GetSize()+16 <= GetBufSize() )
						memcpy(GetBufPtr()+GetSize(), GetBufPtr() + (0x2000 * 2), 16);

					//Now copy data memory (copy only low byte every word)
					int k;
					BYTE *dst = GetBufPtr() + GetSplittedInfo();
					WORD *src = (WORD *)GetBufPtr() + 0x2100;
					for (k = 0; k < GetSize()-GetSplittedInfo(); k++)
						*dst++ = (BYTE)(*src++ & 0xff);
					//memcpy(GetBufPtr()+GetSplittedInfo()+16, GetBufPtr() + (0x2100 * 2), GetSize() - (GetSplittedInfo() + 16) );

					//Set fuse bits so the dialog shows the correct values
					BYTE *ptr = GetBufPtr() + GetSize() + 14;
					WORD config = ptr[0] + ((WORD)ptr[1] << 8);		//little endian buffer
					config = ~config & 0x3fff;		//	CodeProtectAdjust(config, 1);
					SetLockBits(config);
				}
    			}
			else
			if ( GetEEPPriType() == PIC125XX )
			{
				//Copy Config memory
				if ( GetSize()+16 <= GetBufSize() )
					memcpy(GetBufPtr()+GetSize()+14, GetBufPtr() + (0xFFF * 2), 2);

				//Set fuse bits so the dialog shows the correct values
				BYTE *ptr = GetBufPtr() + GetSize() + 14;
				WORD config = ptr[0] + ((WORD)ptr[1] << 8);		//little endian buffer
				SetLockBits( ~config & 0x0fff );
			}
		}
	}

	return rval;
}

//======================>>> e2AppWinInfo::LoadFile <<<=======================
int e2AppWinInfo::LoadFile()
{
	if ( THEAPP->GetClearBufBeforeLoad() )
	{
		if (load_type == ALL_TYPE)
		{
			ClearBuffer(ALL_TYPE);
		}
		else
		{
			if (load_type & PROG_TYPE)
			{
				ClearBuffer(PROG_TYPE);
			}
			if (load_type & DATA_TYPE)
			{
				ClearBuffer(DATA_TYPE);
			}
		}
	}

	int rval = fbufp->Load(load_type, load_relocation);
	if (rval > 0)
	{
		buf_ok = true;
		buf_changed = false;
	}
	RecalcCRC();

	return rval;
}

//======================>>> e2AppWinInfo::Save <<<=======================
int e2AppWinInfo::Save()
{
	int rval;

	BYTE *localbuf;
	localbuf = new BYTE[GetBufSize()];
	if (localbuf == 0)
		return OUTOFMEMORY;

	//save buffer
	memcpy( localbuf, GetBufPtr(), GetBufSize() );
	memset( localbuf+GetSize(), 0xFF, GetBufSize()-GetSize() );

	/**
	 ** NON va qui!!! Va messo in IntelFileBuf::Save(int savetype)
	 **               e MotorolaFileBus::Save
	 **/
 	if (save_type == ALL_TYPE &&
			(GetFileBuf() == INTEL || GetFileBuf() == MOTOS) )
	{
		if ( GetEEPPriType() == PIC16XX ||
			  GetEEPPriType() == PIC168XX )
		{
			//It seems a bit tricky...
			//Relocate the DATA and CONFIG memory with PIC devices
			//The assembler store the DATA at 0x2100 (word address) and
			//CONFIG at 0x2000 (word address).
			//We move them after PROGRAM memory
			// PROGRAM MEMORY + DATA MEMORY + CONFIG (8 word)
			if (GetSplittedInfo() > 0 && GetSize() > GetSplittedInfo() )
			{
				//Set ALL overbound buffer to 0xFF
				memset(GetBufPtr(), 0xFF, GetBufSize());

				//Set fuse bits so the dialog shows the correct values
				if ( 0x2000*2+16 <= GetBufSize() )
				{
					WORD config = (WORD)GetLockBits();

					if ( GetEEPType() == PIC1683 ||
						 GetEEPType() == PIC1684 ||
						 GetEEPType() == PIC1684A )
					{
						if (config & (1<<4))
							config |= 0xfff0;
						else
							config &= 0x000f;
					}
					config = ~config & 0x3fff;		//	CodeProtectAdjust(config, 0);

					BYTE *ptr = GetBufPtr() + (0x2000*2) + 14;
					ptr[0] = (BYTE)(config & 0xFF);			//little endian buffer
					ptr[1] = (BYTE)(config >> 8);
				}

				//Copy prog memory
				if ( GetSplittedInfo() <= GetBufSize() )
				{
					memcpy( GetBufPtr(), localbuf, GetSplittedInfo() );
				}

				//Copy data memory
				if ( 0x2100*2 + (GetSize()-GetSplittedInfo()) <= GetBufSize() )
				{
					//copy only low byte every word
					int k;
					BYTE *src = localbuf + GetSplittedInfo();
					WORD *dst = (WORD *)GetBufPtr() + 0x2100;
					for (k = 0; k < GetSize()-GetSplittedInfo(); k++)
						*dst++ = *src++;
				}
			}
		}
		else
		if ( GetEEPPriType() == PIC125XX )
		{
			//Set ALL overbound buffer to 0xFF
			memset(GetBufPtr(), 0xFF, GetBufSize());

			//Copy prog memory
			if ( GetSplittedInfo() <= GetBufSize() )
			{
				memcpy( GetBufPtr(), localbuf, GetSize() );
			}

			//Set fuse bits so the dialog shows the correct values
			if ( 0xFFF*2+2 <= GetBufSize() )
			{
				WORD config = (WORD)GetLockBits();

				config = ~config & 0x0fff;		//	CodeProtectAdjust(config, 0);

				BYTE *ptr = GetBufPtr() + (0xFFF*2);
				ptr[0] = (BYTE)(config & 0xFF);			//little endian buffer
				ptr[1] = (BYTE)(config >> 8);
			}
		}
	}

	rval = fbufp->Save(save_type, save_relocation);
	if (rval > 0)
		buf_changed = false;

	//restore buffer
	memcpy(GetBufPtr(), localbuf, GetBufSize());

	delete localbuf;

	return rval;
}

//====================>>> e2AppWinInfo::GetLoadRelocation <<<===================
long e2AppWinInfo::GetLoadRelocation() const
{
	return load_relocation;
}

//===================>>> e2AppWinInfo::SetLoadRelocation <<<====================
void e2AppWinInfo::SetLoadRelocation(long val)
{
	if ( val >= 0 && val < GetBufSize() )
		load_relocation = val;
}

//====================>>> e2AppWinInfo::GetSaveRelocation <<<===================
long e2AppWinInfo::GetSaveRelocation() const
{
	return save_relocation;
}

//===================>>> e2AppWinInfo::SetSaveRelocation <<<====================
void e2AppWinInfo::SetSaveRelocation(long val)
{
	if ( val >= 0 && val < GetBufSize() )
		save_relocation = val;
}


//======================>>> e2AppWinInfo::GetLoadType <<<=======================
int e2AppWinInfo::GetLoadType() const
{
	return load_type;
}

//======================>>> e2AppWinInfo::SetLoadType <<<=======================
int e2AppWinInfo::SetLoadType(int val)
{
	int rval = 0;

	if (val == ALL_TYPE || val == PROG_TYPE || val == DATA_TYPE || val == CONFIG_TYPE)
	{
		load_type = val;
		rval = 1;
	}

	return rval;
}

//======================>>> e2AppWinInfo::GetSaveType <<<=======================
int e2AppWinInfo::GetSaveType() const
{
	return save_type;
}

//======================>>> e2AppWinInfo::SetSaveType <<<=======================
int e2AppWinInfo::SetSaveType(int val)
{
	int rval = 0;

	if (val == ALL_TYPE || val == PROG_TYPE || val == DATA_TYPE || val == CONFIG_TYPE)
	{
		save_type = val;
		rval = 1;
	}

	return rval;
}

//======================>>> e2AppWinInfo::SetLockBits <<<=======================
void e2AppWinInfo::SetLockBits(DWORD bits)
{
	lock_bits = bits;
}

//======================>>> e2AppWinInfo::SetFuseBits <<<=======================
void e2AppWinInfo::SetFuseBits(DWORD bits)
{
	fuse_bits = bits;
	fuse_ok = true;
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

FileType e2AppWinInfo::GetFileBuf() const
{
	FileType type;
	int n;

	for (n = 0; n < NO_OF_FILETYPE; n++)
	{
		type = (FileType)n;
		if (fbufvet[type] == fbufp)
			break;
	}

	return type;
}

void e2AppWinInfo::SetFileName(char const *name)
{
	if (name)
		THEAPP->SetLastFile(name, load_type);

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

void e2AppWinInfo::ClearBuffer(int type)
{
	if (type == ALL_TYPE)
	{
		memset(GetBufPtr(), 0xFF, GetBufSize());
	}
	else
	if (type == PROG_TYPE)
	{
		long s = GetSplittedInfo();
		if (s <= 0 || s > GetBufSize())
			s = GetSize();

		memset(GetBufPtr(), 0xFF, s);
	}
	else
	if (type == DATA_TYPE)
	{
		long s = GetSplittedInfo();
		if (s < 0 || s >= GetSize())
			s = 0;

		memset(GetBufPtr() + s, 0xFF, GetSize() - s);
	}
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

	buf_ok = true;		//Validate buffer

	RecalcCRC();
//	BufChanged();
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

int e2AppWinInfo::SecurityRead(DWORD &bits)
{
	int rv;

	OpenBus();
	rv = eep->SecurityRead(bits);
	SleepBus();

	RecalcCRC();

	if (rv == OK)
	{
		lock_bits = bits;
	}
	return rv;
}

int e2AppWinInfo::SecurityWrite(DWORD bits, bool no_param)
{
	int rv;
	if (no_param)
		bits = lock_bits;

	OpenBus();
	rv = eep->SecurityWrite(bits);
	SleepBus();

	return rv;
}

//The 24C65 microchip device has high endurance bits
// in place of Fuse bits.
// Use the same variable because there is no room for
// both in the e2p header.
int e2AppWinInfo::HighEnduranceRead(DWORD &block_no)
{
	int rv;

	OpenBus();
	rv = eep->HighEnduranceRead(block_no);
	SleepBus();

	if (rv == OK)
		SetFuseBits(block_no);

	return rv;
}

int e2AppWinInfo::HighEnduranceWrite(DWORD block_no, bool no_param)
{
	int rv;
	if (no_param)
		block_no = GetFuseBits();

	OpenBus();
	rv = eep->HighEnduranceWrite(block_no);
	SleepBus();

	if (rv == OK)
	{
		DWORD block2;

		OpenBus();
		rv = eep->HighEnduranceRead(block2);
		SleepBus();

		if (rv == OK && block_no != block2)
			rv = E2ERR_WRITEFAILED;
	}

	return rv;
}

int e2AppWinInfo::FusesRead(DWORD &bits)
{
	int rv;

	OpenBus();
	rv = eep->FusesRead(bits);
	SleepBus();

	if (rv == OK)
		SetFuseBits(bits);

	return rv;
}

int e2AppWinInfo::FusesWrite(DWORD bits, bool no_param)
{
	int rv;

	if (no_param)
		bits = GetFuseBits();

	OpenBus();
	rv = eep->FusesWrite(bits);
	SleepBus();

/**	the read op doesn't work with every device
	if (rv == OK)
	{
		DWORD bits2;

		OpenBus();
		rv = eep->FusesRead(bits2);
		SleepBus();

		if (rv == OK && bits != bits2)
			rv = E2ERR_WRITEFAILED;
	}
**/
	return rv;
}

int e2AppWinInfo::ReadOscCalibration(int addr)
{
	int val;

	OpenBus();
	val = eep->ReadCalibration(addr);
	SleepBus();

	return val;
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

			snprintf(linebuf, LINEBUF_SIZE, "  %06lX) %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X    %s\n",
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
			snprintf(linebuf, LINEBUF_SIZE, "  %06lX)",	idx);
		}
		else
		if (type == 2)
		{
			snprintf(linebuf, LINEBUF_SIZE, " %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X  ",
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

			snprintf(linebuf, LINEBUF_SIZE, "  %s\n", tmpbuf);
		}

		linebuf[LINEBUF_SIZE-1] = '\0';
	}

	return linebuf;
}
