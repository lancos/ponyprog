//=========================================================================//
//-------------------------------------------------------------------------//
// aboutmdlg.h -- Header for AboutModalDialog class                        //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2002   Claudio Lanconelli                           //
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
//=========================================================================//

#ifndef aboutMDLG_H
#define aboutMDLG_H

#include "types.h"
#include "e2cmdw.h"
#include <v/vmodald.h>

class AboutModalDialog : public vModalDialog
{
  public:		//---------------------------------------- public
	AboutModalDialog(vBaseWindow* bw, char* title = "About");
	virtual ~AboutModalDialog();		// Destructor
	virtual int AboutAction(char* msg = "");
	virtual void DialogCommand(ItemVal,ItemVal,CmdType); // action selected

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

	int help;
	e2CmdWindow *cmdw;
};

#endif

