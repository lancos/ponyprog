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

#ifndef _E2PROFILE_H
#define _E2PROFILE_H

#include "types.h"
#include "globals.h"
#include "profile.h"

#define CLOCKINV (1<<0)
#define RESETINV (1<<1)
#define DININV   (1<<2)
#define DOUTINV  (1<<3)

#define	RELOAD_idx		0
#define	READFLASH_idx	1
#define	READEEP_idx		2
#define	BYTESWAP_idx	3
#define	SETID_idx		4
#define READOSCCAL_idx  5
#define	ERASE_idx		6
#define	FLASH_idx		7
#define	EEPROM_idx		8
#define	LOCK_idx		9

#define	RELOAD_YES		(1<<RELOAD_idx)
#define	READFLASH_YES	(1<<READFLASH_idx)
#define	READEEP_YES		(1<<READEEP_idx)
#define	BYTESWAP_YES	(1<<BYTESWAP_idx)
#define	SETID_YES		(1<<SETID_idx)
#define	ERASE_YES		(1<<ERASE_idx)
#define	FLASH_YES		(1<<FLASH_idx)
#define	EEPROM_YES		(1<<EEPROM_idx)
#define	LOCK_YES		(1<<LOCK_idx)
#define	READOSCAL_YES	(1<<READOSCCAL_idx)

typedef enum {
	FMT_LITTLE_ENDIAN,
	FMT_BIG_ENDIAN
} FmtEndian;

enum {
	VERYSLOW,
	SLOW,
	NORMAL,
	FAST,
	TURBO,
	ULTRASLOW
};

#define	STRBUFSIZE	256

class E2Profile : public Profile
{
  public:		//---------------------------------------- public

	E2Profile();

	HInterfaceType GetParInterfType();
	int SetParInterfType(HInterfaceType type);
	int GetParPortNo();
	int SetParPortNo(int value);

	int GetBogoMips();
	int SetBogoMips(int value);

	long GetLastDevType();
	int SetLastDevType(long devtype);

	char const *GetLastFile(int &data);
	char const *GetPrevFile(int &data);
	int SetLastFile(char const *name, int data = ALL_TYPE);

	char const *GetLastScript();
	int SetLastScript(char const *name);

	uint8_t GetPolarityControl();
	int SetPolarityControl(uint8_t polarity_control);

	int GetSPIPageWrite();
	int SetSPIPageWrite(int page_write = 1);

	int GetI2CPageWrite();
	int SetI2CPageWrite(int page_write = 1);

	int GetI2CBaseAddr();
	int SetI2CBaseAddr(int base_addr = 0xA0);

	int GetI2CSpeed();
	int SetI2CSpeed(int speed = NORMAL);

	int GetSPIResetPulse();
	int SetSPIResetPulse(int delay = 100);

	int GetSPIDelayAfterReset();
	int SetSPIDelayAfterReset(int delay = 50);

	int GetAT89DelayAfterReset();
	int SetAT89DelayAfterReset(int delay = 50);

	int GetAVRDelayAfterReset();
	int SetAVRDelayAfterReset(int delay = 50);

	int GetPowerUpDelay();
	int SetPowerUpDelay(int delay = 200);

	int GetSPISpeed();
	int SetSPISpeed(int speed = NORMAL);

	int GetMegaPageDelay();
	int SetMegaPageDelay(int delay = 25);

	int GetMicroWireSpeed();
	int SetMicroWireSpeed(int speed = NORMAL);

	int GetPICSpeed();
	int SetPICSpeed(int speed = NORMAL);

	int GetSDESpeed();
	int SetSDESpeed(int speed = NORMAL);

	int GetIMBusSpeed();
	int SetIMBusSpeed(int speed = NORMAL);

	int GetAVRProgDelay();
	int SetAVRProgDelay(int delay = 10);

	int GetAVREraseDelay();
	int SetAVREraseDelay(int delay = 30);

	int GetMDAProgDelay();
	int SetMDAProgDelay(int delay = 30);
	int GetNVMProgDelay();
	int SetNVMProgDelay(int delay = 30);

	unsigned long GetSerialNumVal();
	int SetSerialNumVal(unsigned long val = 0);
	int GetSerialNumAddress(long &start, int &size, bool &mtype);
	int SetSerialNumAddress(unsigned long start, int size, bool mtype);
	FmtEndian GetSerialNumFormat();
	int SetSerialNumFormat(FmtEndian fmt);
	bool GetSerialNumAutoInc();
	int SetSerialNumAutoInc(bool val = true);

	uint16_t GetProgramOptions();
	int SetProgramOptions(uint16_t prog_option);

	int GetCalibrationAddress(long &start, int &size, bool &mtype);
	int SetCalibrationAddress(unsigned long start, int size, bool mtype);

	char const *GetLogFileName();
	int SetLogFileName(char const *name);

	bool GetLogEnabled();
	int SetLogEnabled(bool enabled = false);

	bool GetClearBufBeforeLoad();
	int SetClearBufBeforeLoad(bool val = false);
	bool GetClearBufBeforeRead();
	int SetClearBufBeforeRead(bool val = false);

	bool GetAt89PageOp();
	int SetAt89PageOp(bool val = true);

	bool Get8253FallEdge();
	int Set8253FallEdge(bool falledge);

	char const *GetLanguageCode();
	int SetLanguageCode(char const *code);

	bool GetSoundEnabled();
	int SetSoundEnabled(bool val = true);

	bool GetVerifyAfterWrite();
	int SetVerifyAfterWrite(bool enabled = true);

	int GetJDMCmd2CmdDelay();
	int SetJDMCmd2CmdDelay(int delay = 4000);

	bool GetAutoDetectPorts();
	int SetAutoDetectPorts(bool enabled = true);

	int GetCOMAddress(int &com1, int &com2, int &com3, int &com4);
	int SetCOMAddress(int com1, int com2, int com3, int com4);
	int GetLPTAddress(int &lpt1, int &lpt2, int &lpt3);
	int SetLPTAddress(int lpt1, int lpt2, int lpt3);

	char const *GetLockDir();
	int SetLockDir(char const *name);
	char const *GetDevDir();
	int SetDevDir(char const *name);
	char const *GetDevName();
	int SetDevName(char const *name);

	FileType GetDefaultFileType();
	int SetDefaultFileType(FileType ft);

	char const *GetHtmlBrowseApp();
	int SetHtmlBrowseApp(char const *name);

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

};
#endif
