//=========================================================================//
//-------------------------------------------------------------------------//
// e2awinfo.h -- Header for for e2AppWinInfo class                         //
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

#ifndef e2AWINFO_H
#define e2AWINFO_H

// Include standard V files as needed

#ifdef	vDEBUG
#include <v/vdebug.h>
#endif

#include <v/vawinfo.h>

#include "types.h"

#include "e2cmdw.h"

#include "eeprom.h"
//AutoTag
//Include Device Classes
#include "e24xx.h"
#include "e24xx-2.h"
#include "e24xx-5.h"
#include "at90sxx.h"
#include "at89sxx.h"
#include "at93cxx.h"
#include "pic16xx.h"
#include "at250xx.h"
#include "at25xxx.h"
#include "sde2506.h"

#include "filebuf.h"
#include "e2pfbuf.h"
#include "hexfbuf.h"
#include "intfbuf.h"
#include "motsfbuf.h"

#include "eeptypes.h"
#include "e2app.h"

//At the moment the bigger device is ATmega (128Kb + 4Kb)
#define	BUFFER_SIZE (1024 * 132)

#ifdef	_WINDOWS
# ifndef WIN32
#  undef	BUFFER_SIZE
#  define	BUFFER_SIZE	(1024 * 16)
# endif
#endif

#define	STRINGID_SIZE	32
#define	COMMENT_SIZE	90

class e2AppWinInfo : public vAppWinInfo
{
  public:		//---------------------------------------- public

	e2AppWinInfo(vCmdWindow* win = 0, char* name = "", BusIO** busptr = 0, void* ptr = 0);
	~e2AppWinInfo();

	int Read();
	int Write(int leave_on = FALSE);
	int Verify(int raise_power = TRUE);
	int Load(int bank = 0);
	int Save();
	char const *Dump(int line, int type = 0);

	int GetNoOfBlock() const
		{ return no_block; }
	void SetNoOfBlock(int blk)
		{ no_block = blk; }
	int GetBlockSize() const
		{ return block_size; }
	
	long GetSize() const;
	int GetHexPerLine() const
		{ return hex_per_line; }

	void SetEEProm(int type = E24XX, int subtype = 0);
	
	void SetFileBuf(FileType type);

	void SetFileName(char const *name);
	char const *GetFileName() const
		{ return fname; }
	
	char *GetStringID(char *s = 0);
	void SetStringID(char const *s);

	char *GetComment(char *s = 0);
	void SetComment(char const *s);

	int GetEEPPriType() const
		{ return eep_type; }
	int GetEEPSubType() const
		{ return eep_subtype ? eep_subtype : GetE2PSubType( GetEEPTypeFromSize(eep_type, GetNoOfBlock()) ); } //GetNoOfBlock(); }

	int GetBankRollOver() const
		{ return roll_over; }
	void SetBankRollOver(int rlv = 0)
		{ roll_over = rlv; }
	int BankRollOverDetect(int force = 0);

	void Reset();
	int Erase();

	int GetSplittedInfo() const
		{ return splitted; }
	void SetSplittedInfo(int spl = 0)
		{ splitted = spl; }

	UWORD GetCRC() const
		{ return crc; }
	void SetCRC(int c = 0)
		{ crc = c; }
	UWORD RecalcCRC();
	
	UBYTE *GetBufPtr() const
		{ return (UBYTE *)buffer; }
	int GetBufSize() const
		{ return buffer_size; }

	void DoubleSize();
	void SwapBytes();
	void FillBuffer(int first_pos = 0, int ch = 0xFF, long len = -1);

	int SecurityRead(int &blocks);
	int SecurityWrite(int blocks = -1);
	int HighEnduranceRead(int &block_no);
	int HighEnduranceWrite(int block_no = -1);
	int FusesRead(int &bits);
	int FusesWrite(int bits = -1);

	UBYTE GetLockBits() const
		{ return lock_bits; }
	UBYTE GetFuseBits() const
		{ return fuse_bits; }
	void SetLockBits(UBYTE bits)
		{ lock_bits = bits; }
	void SetFuseBits(UBYTE bits)
		{ fuse_bits = bits; }

	int IsBufferValid() const
		{ return buf_ok; }

	void BufChanged(int val = 1)
		{ buf_changed = val; }
	int IsBufChanged() const
		{ return buf_changed; }



  protected:	//--------------------------------------- protected
	e2CmdWindow* cmdWin;

  private:		//--------------------------------------- private
	int OpenBus()
		{ return THEAPP->OpenBus(eep->GetBus()); }
	void SleepBus()
		{ THEAPP->SleepBus(); }
	void SetBlockSize(int blk)
		{ block_size = blk; }
	void ClearBuffer();
	int LoadFile(int bank = 0);

	int const hex_per_line;
	int const buffer_size;

	UBYTE buffer[BUFFER_SIZE];	//buffer in cui risiede il contenuto dell'eeprom
	char linebuf[128];			//buffer di appoggio per la visualizzazione
	int buf_ok;					//indica se il contenuto del buffer e` valido
	int buf_changed;			//indica se il contenuto del buffer e` stato modificato

	int eep_type;				//indica il tipo di chip di eeprom
	int eep_subtype;			//sottotipo (in pratica il numero di banchi)
								//se zero viene usato GetNoOfBank(), serve per una forzatura manuale

	char *fname;				//nome del file
	int block_size;				//dimensione del blocco (puo` essere anche 1, dipende dal tipo di eeprom)
	int no_block;				//numero dei blocchi che contiene l'eeprom, indica la dimensione

	int splitted;				//indica se la EEPROM e` divisa in due parti distinte (EEPROM - FLASH)
	int roll_over;				//indica se e`presente una features della eeprom

	//** 13/09/99
	UBYTE fuse_bits;			//device dependent bits
	UBYTE lock_bits;			//device dependent lock (security) bits

	UWORD crc;					//CRC del contenuto della eeprom

	char eeprom_string[STRINGID_SIZE];	//eeprom string ID
	char eeprom_comment[COMMENT_SIZE];	//eeprom comment

	EEProm *eep;				//puntatore al tipo di eeprom su cui si lavora
	//AutoTag
	//List of available device types
	E24xx eep24xx;
	E24xx2 eep24xxx;
	E24xx5 eep24xx5;
	At90sxx eepAt90s;
	At89sxx eepAt89s;
	At93cxx eep93x6;
	Pic16xx eepPic16;
	At250xx eep250xx;
	At25xxx eep25xxx;
	Sde2506 eep2506;

	FileBuf *fbufp;				//puntatore al tipo di file su cui si lavora
	FileBuf *fbufvet[NO_OF_FILETYPE];	//puntatori ai tipi di file conosciuti
	//List of available file types
	e2pFileBuf e2pfbuf;
	hexFileBuf hexfbuf;
	IntelFileBuf intfbuf;
	MotorolaSFileBuf motsfbuf;
};
#endif
