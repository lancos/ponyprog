//=========================================================================//
//-------------------------------------------------------------------------//
// progoption.h -- Header for for progOptionDialog class                   //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C)  1997-1999  Claudio Lanconelli                           //
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

#ifndef progOPTION_H
#define progOPTION_H

#include <v/vmodald.h>

#include "e2cmdw.h"
#include "string_table.h"

class progOptionDialog : public vModalDialog
{
  public:		//---------------------------------------- public
	progOptionDialog(vBaseWindow* bw, char* title = STR_MSGPROGOPT);
	virtual ~progOptionDialog();		// Destructor
	virtual void DialogCommand(ItemVal,ItemVal,CmdType); // action selected
	virtual int progAction(char* msg, long type, int &reload, int &reep, int &erase, int &flash, int &eeprom, int &lock);

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

	e2CmdWindow *cmdw;
};
#endif

