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

#ifndef _SIPROGINTERFACE_H
#define _SIPROGINTERFACE_H

#include "businter.h"
#include "rs232int.h"

class SIProgInterface : public BusInterface, public RS232Interface
{
  public:                //------------------------------- public
	SIProgInterface();
	//      virtual ~SIProgInterface();

	virtual int Open(int com_no);
	virtual void Close();

	virtual void SetDataOut(int sda = 1);
	virtual void SetClock(int scl = 1);
	virtual int GetDataIn() ;
	virtual int GetClock() ;
	virtual void SetClockData();
	virtual void ClearClockData();
	virtual int IsClockDataUP() ;
	virtual int IsClockDataDOWN() ;

	virtual int TestPort(int port);
	//      int TestSave(int port);
	//      void TestRestore();

	int SetPower(bool onoff);
	void SetControlLine(int res = 1);

  protected:             //------------------------------- protected

  private:               //------------------------------- private

};

#endif
