//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2021   Claudio Lanconelli                           //
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

#ifndef _PIC12BUS_H
#define _PIC12BUS_H

#include "types.h"
#include "busio.h"
#include "pgminter.h"

class Pic12Bus : public BusIO
{
  public:
	Pic12Bus(BusInterface *ptr = 0);
	virtual ~Pic12Bus();

	long Read(int addr, quint8 *data, long length, int page_size = 0);
	long Write(int addr, quint8 const *data, long length, int page_size = 0);

	//      int Erase(int type = ALL_TYPE);

	int Reset();

	long ReadConfig(quint16 &data);
	long WriteConfig(quint16 data);

	void DisableCodeProtect();

	int CompareSingleWord(quint16 data1, quint16 data2, quint16 mask);
	int CompareMultiWord(quint8 *data1, quint8 *data2, long length, int split);

	void SetDelay();

	long BlankCheck(long length);
	void IncAddress(int n);

  protected:

	int SendDataWord(long wo, int wlen = 16);
	long RecDataWord(int wlen = 16);
	int WaitReadyAfterWrite(long timeout = 5000);

	int WriteProgWord(quint16 val, long rc_addr);
	int ProgramPulse(quint16 val, int verify = 0, int width = 100);        //100uSec default pulse width

	int SendCmdCode(int opcode)
	{
		return SendDataWord(opcode, 6);
	}
	int SendProgCode(quint16 data)
	{
		//the code is 14 bit data with leading and trailing 0's
		data &= ProgMask;
		data <<= 1;
		return SendDataWord(data);
	}
	quint16 RecvProgCode()
	{
		return (quint16)(RecDataWord() >> 1) & ProgMask;
	}
	/**
	int SendDataCode(quint16 data)
	{
	       //the code is 8 bit data with leading and trailing 0's
	       data &= DataMask;
	       data <<= 1;
	       return SendDataWord(data);
	}
	quint16 RecvDataCode()
	{
	       return (quint16)(RecDataWord() >> 1) & DataMask;
	}
	**/

	void SetMCLR()
	{
		busI->SetControlLine(1);
	}
	void ClearMCLR()
	{
		busI->SetControlLine(0);
	}

	long current_address;

	int OverProgrammingMult;
	int OverProgrammingAdd;

	//      const quint16 DataMask;
	const quint16 ProgMask;

	//Command Opcode
	const quint8 ReadProgCode;
	const quint8 LoadProgCode;
	//      const quint8 ReadDataCode;
	//      const quint8 LoadDataCode;
	//      const quint8 LoadConfigCode;
	const quint8 IncAddressCode;
	//      const quint8 EraseProgMem;
	//      const quint8 EraseDataMem;
	const quint8 BeginProgCode;
	//      const quint8 BeginProgOnlyCode;
	const quint8 EndProgCode;

  private:

	int SendDataBit(int b);
	int RecDataBit();

	void bitDI(int b)
	{
		busI->SetDataOut(!b);
	}

	void setDI()
	{
		busI->SetDataOut(0);
	}

	void clearDI()
	{
		busI->SetDataOut(1);
	}

	void setCLK()
	{
		busI->SetClock(1);
	}

	void clearCLK()
	{
		busI->SetClock(0);
	}

	int getDO() const
	{
		return busI->GetDataIn();
	}

};

#endif
