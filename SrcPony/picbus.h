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

#ifndef _PICBUS_H
#define _PICBUS_H

#include "busio.h"
#include "pgminter.h"

class PicBus : public BusIO
{
  public:
	PicBus(BusInterface *ptr = 0);
	//virtual ~PicBus();

	long Read(int addr, quint8 *data, long length, int page_size = 0);
	long Write(int addr, quint8 const *data, long length, int page_size = 0);

	int Erase(int type = ALL_TYPE);

	int Reset();

	long ReadConfig(quint16 *data);
	long WriteConfig(quint16 *data);

	void DisableCodeProtect();

	int CompareSingleWord(quint16 data1, quint16 data2, quint16 mask);
	int CompareMultiWord(quint8 *data1, quint8 *data2, long length, int split);

	void SetDelay();

  protected:
	int SendDataWord(long wo, int wlen = 16);
	long RecDataWord(int wlen = 16);
	int WaitReadyAfterWrite(long timeout = 5000);

	int SendCmdCode(int opcode);
	int SendProgCode(quint16 data);
	quint16 RecvProgCode();
	int SendDataCode(quint16 data);
	quint16 RecvDataCode();

	void SetMCLR()
	{
		busI->SetControlLine(1);
	}
	void ClearMCLR()
	{
		busI->SetControlLine(0);
	}

	const quint16 DataMask;
	const quint16 ProgMask;

	//Command Opcode
	const quint8 ReadProgCode;
	const quint8 LoadProgCode;
	const quint8 ReadDataCode;
	const quint8 LoadDataCode;
	const quint8 LoadConfigCode;
	const quint8 IncAddressCode;
	const quint8 EraseProgMem;
	const quint8 EraseDataMem;
	const quint8 BeginEraseProgCode;
	const quint8 BeginProgOnlyCode;

  private:
	int SendDataBit(int b);
	int RecDataBit();

	void bitDI(int b)
	{
		busI->SetInvDataOut(b);
	}

	void setDI()
	{
		busI->SetInvDataOut(1);
	}

	void clearDI()
	{
		busI->SetInvDataOut(0);
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
