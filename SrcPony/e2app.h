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

#ifndef e2APP_H
#define e2APP_H


#include <QApplication>
#include <QCoreApplication>

#include "Translator.h"
// #include "busio.h"

//Include Bus Classes
#include "i2cbus.h"
#include "at90sbus.h"
#include "at93cbus.h"
#include "at250bus.h"
#include "at250bus2.h"
#include "picbus.h"
#include "pic12bus.h"
//#include "sxbus.h"
#include "sdebus.h"
#include "at89sbus.h"
//#include "atmegabus.h"
//#include "avr1200bus.h"
#include "picbusnew.h"
#include "imbus.h"
#include "x2444bus.h"

//Include Interface Classes
#include "pgminter.h"
#include "ponyioint.h"
#include "easyi2c_interf.h"
#include "ispinterf.h"
#include "jdminter.h"
//#include "jdmiointer.h"
#include "dt006interf.h"
#include "linuxsysfsint.h"

#include "e2profil.h"

class e2AppWinInfo;

enum AppStatus
{
	AppReady,
	AppBusy
};

class e2App : public cTranslator
{
  public:               //---------------------------------------- public

	e2App();
	virtual ~e2App();

	// Routines from vApp that are normally overridden

	//      virtual vWindow* NewAppWin(vWindow* win, char* name, int w, int h,
	//                                 vAppWinInfo* winInfo = 0);

	// replace about signal - slots
	//      virtual void AppCommand(vWindow* win, ItemVal id, ItemVal val, CmdType cType);

	//      virtual void KeyIn(vWindow*, vKey, unsigned int);

	// New routines for this particular app
	//      int GetCounter() const
	//      {
	//              return winCounter;
	//      }
	int TestPort(int port = -1, bool open_only = false);
	int OpenPort(int port = -1);
	void SetInitialBus(BusIO *p)
	{
		if (p)
		{
			iniBus = p;
		}
	}
	BusInterface *GetInterfPtr()
	{
		return busIntp;
	}
	BusIO **GetBusVectorPtr()
	{
		return busvetp;
	}
	void ClosePort();
	int OpenBus(BusIO *p);
	void SleepBus();


	void SetInterfaceType(HInterfaceType type = SIPROG_API);
	HInterfaceType GetInterfaceType() const
	{
		return iType;
	}

	int Calibration();

	QString GetHelpFile() const
	{
		return helpfile;
	}
	QString GetOkSound() const
	{
		return ok_soundfile;
	}
	QString GetErrSound() const
	{
		return err_soundfile;
	}

	uint8_t GetPolarity() const
	{
		return polarity_control;
	}
	void SetPolarity(uint8_t val)
	{
		polarity_control = val;
	}
	int GetPort() const
	{
		return port_number;
	}
	void SetPort(int port)
	{
		if (port >= 0)
		{
			port_number = port;
		}
	}

	int LoadDriver(int start);

	void SetAWInfo(e2AppWinInfo *awi)
	{
		//             qDebug() << "SetAWInfo" << awi;
		awip = awi;
	}
	e2AppWinInfo *GetAWInfo() const
	{
		return awip;
	}

	bool scriptMode;        //Script Mode
	int returnValue;        //return value in Command Line mode

	QString script_name;

  protected:    //--------------------------------------- protected
	// AppWinInfo associated with this window
	e2AppWinInfo *awip;

  private:              //--------------------------------------- private
	void initSettings();
	void LookForBogoMips(); //should get bogomips

	// EK 2017
	// we can fork the process
	//      int winCounter;         //open windows (child) counter

	//      int abortFlag;          //True if we have to abort current op
	//      int ignoreFlag;         //True if we have to ignore probe errors (usually false)

	HInterfaceType iType;   //current interface type

	BusInterface *busIntp;  //pointer to current interface type

	uint8_t polarity_control; //polarity for control lines

	//AutoTag
	//List of available interface types
	SIProgInterface siprog_apiI;
	PonyIOInterface siprog_ioI;
	EasyI2CInterface easyi2c_apiI;
	EasyI2CInterface easyi2c_ioI;
	AvrISPInterface avrisp_apiI;
	AvrISPInterface avrisp_ioI;
	Dt006Interface dt006_apiI;
	Dt006Interface dt006_ioI;
	JdmInterface jdm_apiI;
	//      JdmIOInterface jdm_ioI;
	LinuxSysFsInterface linuxsysfs_ioI;

	int port_number;        //port number used
	BusIO *iniBus;                           //pointer to current Bus
	BusIO *busvetp[NO_OF_BUSTYPE];  //array of pointers to available Bus
	//AutoTag
	//List of available bus types
	I2CBus iicB;
	At90sBus at90sB;
	At93cBus at93cB;
	At250Bus at250B;
	At250BigBus at250BigB;
	PicBus picB;
	Pic12Bus pic12B;
	//SxBus sxB;
	Sde2506Bus sdeB;
	At89sBus at89sB;
	//      AtMegaBus atMegaB;
	//      Avr1200Bus at1200B;
	PicBusNew picNewB;
	IMBus imB;
	X2444Bus x2444B;
	X2444Bus s2430B;

	QString helpfile;
	QString ok_soundfile;
	QString err_soundfile;
};
#endif
