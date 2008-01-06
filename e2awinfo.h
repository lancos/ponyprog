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

#ifndef e2AWINFO_H
#define e2AWINFO_H

// Include standard V files as needed

#ifdef	vDEBUG
#include <v/vdebug.h>
#endif

#include <v/vawinfo.h>

#include "types.h"

#include "e2cmdw.h"

#include "device.h"
//AutoTag
//Include Device Classes
#include "e24xx.h"
#include "e2401.h"
#include "e24xx-1.h"
#include "e24xx-2.h"
#include "e24xx-5.h"
#include "at90sxx.h"
#include "at89sxx.h"
#include "at93cxx.h"
#include "at93cxx8.h"
#include "pic16xx.h"
#include "pic168xx.h"
#include "pic125xx.h"
#include "at250xx.h"
#include "at25xxx.h"
#include "sde2506.h"
#include "nvm3060.h"
#include "at17xxx.h"
#include "x2444.h"

#include "filebuf.h"
#include "e2pfbuf.h"
#include "binfbuf.h"
#include "intfbuf.h"
#include "motsfbuf.h"
#include "csmfbuf.h"

#include "eeptypes.h"
#include "e2app.h"

#include "e2phead.h"

//At the moment the bigger device is ATmega2560 (256KiB + 4KiB)
#define	BUFFER_SIZE (1024 * 260)

#ifdef	_WINDOWS
# ifndef WIN32
#  undef	BUFFER_SIZE
#  define	BUFFER_SIZE	(1024 * 16)
# endif
#endif

#define	STRINGID_SIZE	E2P_STRID_SIZE
#define	COMMENT_SIZE	E2P_COMM_SIZE

#define LINEBUF_SIZE	128

class e2AppWinInfo : public vAppWinInfo
{
  public:		//---------------------------------------- public

	e2AppWinInfo(vCmdWindow* win = 0, char* name = "", BusIO** busptr = 0, void* ptr = 0);
	~e2AppWinInfo();

	int Read(int type = ALL_TYPE, int raise_power = TRUE, int leave_on = FALSE);
	int Write(int type = ALL_TYPE, int raise_power = TRUE, int leave_on = FALSE);
	int Verify(int type = ALL_TYPE, int raise_power = TRUE, int leave_on = FALSE);
	int Erase(int type = ALL_TYPE, int raise_power = TRUE, int leave_on = FALSE);
//	int BlankCheck(int type = ALL_TYPE, int raise_power = TRUE, int leave_on = FALSE);

//	int Load(int bank = 0);
	int Load();
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
	FileType GetFileBuf() const;

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
  	int GetEEPType() const
		{ return BuildE2PType(eep_type, eep_subtype); }

	int GetBankRollOver() const
		{ return roll_over; }
	void SetBankRollOver(int rlv = 0)
		{ roll_over = rlv; }
	int BankRollOverDetect(int force = 0);

	void Reset();

	long GetSplittedInfo() const
		{ return splitted; }
	void SetSplittedInfo(long spl = 0)
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

	int SecurityRead(DWORD &bits);
	int SecurityWrite(DWORD bits, bool no_param = false);
	int FusesRead(DWORD &bits);
	int FusesWrite(DWORD bits, bool no_param = false);
	int HighEnduranceRead(DWORD &block_no);
	int HighEnduranceWrite(DWORD block_no, bool no_param = false);

	int ReadOscCalibration(int addr = 0);

	DWORD GetLockBits() const
		{ return lock_bits; }
	DWORD GetFuseBits() const
		{ return fuse_bits; }
	void SetLockBits(DWORD bits);
	void SetFuseBits(DWORD bits);

	bool IsFuseValid() const
		{ return fuse_ok; }
	bool IsBufferValid() const
		{ return buf_ok; }

	void BufChanged(bool val = true)
		{ buf_changed = val; }
	bool IsBufChanged() const
		{ return buf_changed; }

	int SetLoadType(int val);
	int GetLoadType() const;
	int SetSaveType(int val);
	int GetSaveType() const;

	void SetLoadRelocation(long val);
	long GetLoadRelocation() const;
	void SetSaveRelocation(long val);
	long GetSaveRelocation() const;

  protected:	//--------------------------------------- protected
	e2CmdWindow* cmdWin;

  private:		//--------------------------------------- private
	int OpenBus()
		{ return THEAPP->OpenBus(eep->GetBus()); }
	void SleepBus()
		{ THEAPP->SleepBus(); }
	void SetBlockSize(int blk)
		{ block_size = blk; }
	void ClearBuffer(int type = ALL_TYPE);
	int LoadFile();

	int const hex_per_line;
	int const buffer_size;

	int load_type;				//load ALL, Flash only or EEPROM only
	int save_type;				//save ALL, Flash only or EEPROM only

	long load_relocation;
	long save_relocation;

	UBYTE buffer[BUFFER_SIZE];	//device content buffer
	char linebuf[LINEBUF_SIZE];	//print line buffer
	bool buf_ok;				//true if buffer is valid
	bool buf_changed;			//true if buffer changed/edited

	int eep_type;				//indica il tipo di chip di eeprom
	int eep_subtype;			//sottotipo (in pratica il numero di banchi)
								//se zero viene usato GetNoOfBank(), serve per una forzatura manuale

	char *fname;				//nome del file
	int block_size;				//dimensione del blocco (puo` essere anche 1, dipende dal tipo di eeprom)
	int no_block;				//numero dei blocchi che contiene l'eeprom, indica la dimensione

	int splitted;				//indica se la EEPROM e` divisa in due parti distinte (EEPROM - FLASH)
	int roll_over;				//indica se e`presente una features della eeprom

	DWORD fuse_bits;			//device dependent bits
	DWORD lock_bits;			//device dependent lock (security) bits
	bool fuse_ok;

	UWORD crc;					//CRC del contenuto della eeprom

	char eeprom_string[STRINGID_SIZE];	//eeprom string ID
	char eeprom_comment[COMMENT_SIZE];	//eeprom comment

	Device *eep;						//current device pointer (can be any of the following list)
	//AutoTag
	//List of available device types
	E24xx eep24xx;
	mE2401 eep2401;
	E24xx1 eep24xx1;
	E24xx2 eep24xx2;
	E24xx5 eep24xx5;
	At90sxx eepAt90s;
	At89sxx eepAt89s;
	At93cxx eep93xx16;
	At93cxx8 eep93xx8;
	Pic16xx eepPic16;
	Pic168xx eepPic168xx;
	Pic125xx eepPic125xx;
	At250xx eep250xx;
	At25xxx eep25xxx;
	Sde2506 eep2506;
	Nvm3060 eep3060;
	At17xxx eep17xxx;
	X2444 eep2444;
	X2444 eep2430;

	FileBuf *fbufp;						//current filebuffer pointer
	FileBuf *fbufvet[NO_OF_FILETYPE];	//filebuffer list
	//List of available file types
	e2pFileBuf e2pfbuf;
	binFileBuf binfbuf;
	IntelFileBuf intfbuf;
	MotorolaSFileBuf motsfbuf;
	csmFileBuf csmfbuf;
};
#endif
