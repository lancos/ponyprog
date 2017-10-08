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

#ifndef _PONYIOINTERFACE_H
#define _PONYIOINTERFACE_H

#include "businter.h"
#include "portint.h"

class PonyIOInterface : public BusInterface, public PortInterface
{
  public:                //------------------------------- public
	PonyIOInterface();
	virtual ~PonyIOInterface();

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

	int SetPower(bool onoff);
	void SetControlLine(int res = 1);

  protected:             //------------------------------- protected
	int GetPresence() const;

  private:               //------------------------------- private

};

#endif
