//=========================================================================//
//-------------------------------------------------------------------------//
// retrymdlg.h -- Header for retryModalDialog class                        //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-1999  Claudio Lanconelli                            //
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

#ifndef retryMDLG_H
#define retryMDLG_H

#include "types.h"
#include <v/vmodald.h>

#include "string_table.h"

class retryModalDialog : public vModalDialog
{
  public:		//---------------------------------------- public
	retryModalDialog(vBaseWindow* bw, char* msg, char* title = STR_MSGALERT);
	virtual ~retryModalDialog();		// Destructor
	virtual void DialogCommand(ItemVal,ItemVal,CmdType); // action selected
	virtual int retryAction();

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

};

#endif

