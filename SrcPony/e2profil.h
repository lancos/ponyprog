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

#ifndef _E2PROFILE_H
#define _E2PROFILE_H

#include <QPrinter>
#include <QSettings>
#include "types.h"
#include "globals.h"

#include <QString>

#define CLOCKINV (1<<0)
#define RESETINV (1<<1)
#define DININV   (1<<2)
#define DOUTINV  (1<<3)

#define RELOAD_idx              0
#define READFLASH_idx           1
#define READEEP_idx             2
#define BYTESWAP_idx            3
#define SETID_idx               4
#define READOSCCAL_idx          5
#define ERASE_idx               6
#define FLASH_idx               7
#define EEPROM_idx              8
#define LOCK_idx                9

#define RELOAD_YES              (1<<RELOAD_idx)
#define READFLASH_YES           (1<<READFLASH_idx)
#define READEEP_YES             (1<<READEEP_idx)
#define BYTESWAP_YES            (1<<BYTESWAP_idx)
#define SETID_YES               (1<<SETID_idx)
#define ERASE_YES               (1<<ERASE_idx)
#define FLASH_YES               (1<<FLASH_idx)
#define EEPROM_YES              (1<<EEPROM_idx)
#define LOCK_YES                (1<<LOCK_idx)
#define READOSCAL_YES          (1<<READOSCCAL_idx)


typedef enum
{
	FMT_LITTLE_ENDIAN,
	FMT_BIG_ENDIAN
} FmtEndian;

enum
{
	VERYSLOW,
	SLOW,
	NORMAL,
	FAST,
	TURBO,
	ULTRASLOW
};

#define STRBUFSIZE      256


class E2Profile
{

  public:               //---------------------------------------- public
	//E2Profile(const QString &nm);
	//virtual ~E2Profile();

	static HInterfaceType GetParInterfType();
	static void SetParInterfType(HInterfaceType type);
	static int GetPortNumber();
	static void SetPortNumber(int value);

	static int GetBogoMips();
	static void SetBogoMips(int value);

	static long GetLastDevType();
	static void SetLastDevType(long devtype);

	static QString GetLastFile(int &data, int index = 0);
// 	static QString GetPrevFile(int &data);
	static void SetLastFile(const QString &name, int data = ALL_TYPE);

// 	static QString GetLastScript();
	static void SetLastScript(const QString &name);

	static uint8_t GetPolarityControl();
	static void SetPolarityControl(uint8_t polarity_control);

	static int GetSPIPageWrite();
	static void SetSPIPageWrite(int page_write = 1);

	static int GetI2CPageWrite();
	static void SetI2CPageWrite(int page_write = 1);

	static int GetI2CBaseAddr();
	static void SetI2CBaseAddr(int base_addr = 0xA0);

	static int GetI2CSpeed();
	static void SetI2CSpeed(int speed = NORMAL);

	static int GetSPIResetPulse();
	static void SetSPIResetPulse(int delay = 100);

	static int GetSPIDelayAfterReset();
	static void SetSPIDelayAfterReset(int delay = 50);

	static int GetAT89DelayAfterReset();
	static void SetAT89DelayAfterReset(int delay = 50);

	static void GetPrinterSettings(QPrinter &p);
	static void SetPrinterSettings(QPrinter &p);

	static int GetAVRDelayAfterReset();
	static void SetAVRDelayAfterReset(int delay = 50);

	static int GetPowerUpDelay();
	static void SetPowerUpDelay(int delay = 200);

	static int GetSPISpeed();
	static void SetSPISpeed(int speed = NORMAL);

	static int GetMegaPageDelay();
	static void SetMegaPageDelay(int delay = 25);

	static int GetMicroWireSpeed();
	static void SetMicroWireSpeed(int speed = NORMAL);

	static int GetPICSpeed();
	static void SetPICSpeed(int speed = NORMAL);

	static int GetSDESpeed();
	static void SetSDESpeed(int speed = NORMAL);

	static int GetIMBusSpeed();
	static void SetIMBusSpeed(int speed = NORMAL);

	static int GetAVRProgDelay();
	static void SetAVRProgDelay(int delay = 10);

	static int GetFontSize();
	static void SetFontSize(int sz = 9);

	static QStringList GetLastFiles();

	static QStringList GetLastScripts();
	static void SetLastScripts(const QStringList &l);

	static int GetAVREraseDelay();
	static void SetAVREraseDelay(int delay = 30);

	static int GetMDAProgDelay();
	static void SetMDAProgDelay(int delay = 30);
	static int GetNVMProgDelay();
	static void SetNVMProgDelay(int delay = 30);

	static unsigned long GetSerialNumVal();
	static void SetSerialNumVal(unsigned long val = 0);
	static int GetSerialNumAddress(long &start, int &size, bool &mtype);
	static void SetSerialNumAddress(unsigned long start, int size, bool mtype);
	static FmtEndian GetSerialNumFormat();
	static void SetSerialNumFormat(FmtEndian fmt);
	static bool GetSerialNumAutoInc();
	static void SetSerialNumAutoInc(bool val = true);

	static long GetProgramOptions();
	static void SetProgramOptions(long prog_option);

	static int GetCalibrationAddress(bool &enabled, long &start, int &size, bool &mtype);
	static void SetCalibrationAddress(bool enabled, unsigned long start, int size, bool mtype);

	static QString GetLogFileName();
	static void SetLogFileName(const QString &name);

	static QString GetLangDir();
	static void SetLangDir(const QString &name);

	static QString GetCurrentLang();
	static void SetCurrentLang(const QString &name);

	static bool GetLogEnabled();
	static void SetLogEnabled(bool enabled = false);

	static bool GetSkipStartupDialog();
	static void SetSkipStartupDialog(bool skip = false);

	static bool GetClearBufBeforeLoad();
	static void SetClearBufBeforeLoad(bool val = false);
	static bool GetClearBufBeforeRead();
	static void SetClearBufBeforeRead(bool val = false);

	static bool GetAt89PageOp();
	static void SetAt89PageOp(bool val = true);

	static bool Get8253FallEdge();
	static void Set8253FallEdge(bool falledge);

	static QString GetLanguageCode();
	static void SetLanguageCode(const QString &code);

	static bool GetSoundEnabled();
	static void SetSoundEnabled(bool val = true);

	static bool GetVerifyAfterWrite();
	static void SetVerifyAfterWrite(bool enabled = true);

	static int GetJDMCmd2CmdDelay();
	static void SetJDMCmd2CmdDelay(int delay = 4000);

	static bool GetAutoDetectPorts();
	static void SetAutoDetectPorts(bool enabled = true);

	static int GetCOMAddress(int &com1, int &com2, int &com3, int &com4);
	static void SetCOMAddress(int com1, int com2, int com3, int com4);
	static int GetLPTAddress(int &lpt1, int &lpt2, int &lpt3);
	static void SetLPTAddress(int lpt1, int lpt2, int lpt3);

//	static void SetConfigFile(const QString &n);
	static QString GetConfigFile()
	{
		return s->fileName();
	}

	//static QString GetLockDir();
	//static void SetLockDir(const QString &name);
	//static QString GetDevDir();
	//static void SetDevDir(const QString &name);
	static QString GetCOMDevName();
	static void SetCOMDevName(const QString &name);
	static QStringList GetCOMDevList();
	static void SetCOMDevList(const QStringList &lst);
	static QString GetLPTDevName();
	static void SetLPTDevName(const QString &name);
	static QStringList GetLPTDevList();
	static void SetLPTDevList(const QStringList &lst);

	static FileType GetDefaultFileType();
	static void SetDefaultFileType(FileType ft);

	static QString GetHtmlBrowseApp();
	static void SetHtmlBrowseApp(const QString &name);

	static int GetGpioPinCtrl();
	static int GetGpioPinClock();
	static int GetGpioPinDataIn();
	static int GetGpioPinDataOut();

	static void SetGpioPinCtrl(int pin);
	static void SetGpioPinClock(int pin);
	static void SetGpioPinDataIn(int pin);
	static void SetGpioPinDataOut(int pin);

	static bool GetEditBufferEnabled();
	static void SetEditBufferEnabled(bool enable);

  protected:    //--------------------------------------- protected
	static void SetLastFiles(const QStringList &l);
	static QSettings *s;

  private:              //--------------------------------------- private
//	static QString filename;       //name of .INI file
};

#endif
