//=========================================================================//
//-------------------------------------------------------------------------//
// e2app.h -- Header for for e2App class                                   //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997, 1998  Claudio Lanconelli                           //
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

#ifndef e2APP_H
#define e2APP_H

// Include standard V files as needed
#ifdef vDEBUG
#include <v/vdebug.h>
#endif

#include <v/vapp.h>
#include <v/vawinfo.h>

#include "e2profil.h"

//Include Bus Classes
#include "i2cbus.h"
#include "at90sbus.h"
#include "at93cbus.h"
#include "at250bus.h"
#include "at250bus2.h"
#include "picbus.h"
#include "sxbus.h"
#include "sdebus.h"
#include "at89sbus.h"
#include "atmegabus.h"
#include "avr1200bus.h"

//Include Interface Classes
#include "pgminter.h"
#include "ponyioint.h"
#include "easyi2c_com.h"
#include "easyi2c_lpt.h"
#include "ispinterf.h"

enum AppStatus {
	AppReady,
	AppBusy
};

class e2App : public vApp, public E2Profile
{
	friend int AppMain(int, char**);	// allow AppMain access

  public:		//---------------------------------------- public

	e2App(char* name, int w = 0, int h = 0);
	virtual ~e2App();

	// Routines from vApp that are normally overridden

	virtual vWindow* NewAppWin(vWindow* win, char* name, int w, int h,
		vAppWinInfo* winInfo = 0);

	virtual void Exit(void);

	virtual int CloseAppWin(vWindow*);

	virtual void AppCommand(vWindow* win, ItemVal id, ItemVal val, CmdType cType);

	virtual void KeyIn(vWindow*, vKey, unsigned int);

	// New routines for this particular app
	int GetCounter() const
		{ return winCounter; }
	int GetPort() const
		{ return port_number; }
	void SetPort(int port)
		{ if (port > 0) port_number = port; }
	int OpenPort(int port = 0);
	void SetInitialBus(BusIO *p)
		{ if (p) iniBus = p; }
	void ClosePort();
	int TestPort(int port = 0, int open_only = 0);
	int OpenBus(BusIO *p);
	void SleepBus();
	
	void SetInterfaceType(HInterfaceType type = SIPROG_API);
	HInterfaceType GetInterfaceType() const
		{ return iType; }
	BusInterface* GetInterfPtr()
		{ return busIntp; }
	BusIO **GetBusVectorPtr()
		{ return busvetp; }

	int Calibration();
//	void SetDelayLoop(int value)
//		{ iicB.SetDelayLoop(value); }
//	int GetDelayLoop() const
//		{ return iicB.GetDelayLoop(); }

	const char *GetHelpFile() const
		{ return helpfile; }

	int GetAbortFlag();
	void SetAbortFlag(int a = 1)
		{ abortFlag = a; }

	void SetProgress(int progress = 0);

	UBYTE GetPolarity() const 
		{ return polarity_control;} 
	void SetPolarity(UBYTE val) 
		{ polarity_control = val;} 

	int IsAppBusy()
		{ return (app_status == AppBusy); }
	int IsAppReady()
		{ return (app_status == AppReady); }
	void SetAppBusy();
	void SetAppReady();

	int GetIgnoreFlag() const
		{ return ignoreFlag; }
	void SetIgnoreFlag()
		{ ignoreFlag = 1; }
	void ClearIgnoreFlag()
		{ ignoreFlag = 0; }

	long GetLastProgrammedAddress() const
		{ return last_programmed_addr; }

	void ClearLastProgrammedAddress()
		{ last_programmed_addr = 0; }

	void SetLastProgrammedAddress(long addr);

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

	void LookForBogoMips();	//should get bogomips

	int winCounter;		//open windows (child) counter
	int port_number;	//port number used

	int abortFlag;		//True if we have to abort current op
	int ignoreFlag;		//True if we have to ignore probe errors (usually false)

	HInterfaceType iType;	//current interface type
	BusInterface *busIntp;	//pointer to current interface type

	UBYTE polarity_control; //polarity for control lines 
	AppStatus app_status;		//tell if the App is busy (reading, writing, ...) or can react to user events

	long last_programmed_addr;	//record last programmed address for verify

	//AutoTag
	//List of available interface types
	PGMInterface siprog_apiI;
	PonyIOInterface siprog_ioI;
	EasyI2CInterface easyi2c_comI;
	EasyI2CLPTInterface easyi2c_lptI;
	AvrISPInterface avrispI;

	BusIO *iniBus;					//pointer to current Bus
	BusIO *busvetp[NO_OF_BUSTYPE];	//array of pointers to available Bus
	//AutoTag
	//List of available bus types
	I2CBus iicB;
	At90sBus at90sB;
	At93cBus at93cB;
	At250Bus at250B;
	At250BigBus at250BigB;
	PicBus picB;
	SxBus sxB;
	Sde2506Bus sdeB;
	At89sBus at89sB;
	AtMegaBus megaB;
	Avr1200Bus at1200B;

	char helpfile[128];
};
#endif
