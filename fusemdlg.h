//=========================================================================//
//-------------------------------------------------------------------------//
// fusemdlg.h -- Header for for fuseModalDialog class                      //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C)  1997-2001  Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lancos@libero.it                                               //
//  http://www.LancOS.com                                                  //
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
// $Id$
//=========================================================================//

#ifndef fuseMDLG_H
#define fuseMDLG_H

#include <v/vmodald.h>

#include "e2cmdw.h"
#include "string_table.h"

class fuseModalDialog : public vModalDialog
{
  public:		//---------------------------------------- public
	fuseModalDialog(vBaseWindow* bw, char* title = STR_MSGFUSEDLG);
	virtual ~fuseModalDialog();		// Destructor
	virtual void DialogCommand(ItemVal,ItemVal,CmdType); // action selected
	virtual int fuseAction(char* msg, long type, DWORD &lock, DWORD &fuse, int readonly = 0);

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

	int write;
	int read;
	e2CmdWindow *cmdw;
};
#endif
