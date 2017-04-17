//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id$
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

#ifndef e2DIALOG_H
#define e2DIALOG_H
#include <v/vmodald.h>

#include "types.h"
#include "globals.h"

#include "string_table.h"

class e2CmdWindow;

class e2Dialog : public vModalDialog
{
  public:		//---------------------------------------- public
	e2Dialog(vBaseWindow* bw, char* title = STR_MSGINTSETUP);
	virtual ~e2Dialog();		// Destructor
	virtual void DialogCommand(ItemVal,ItemVal,CmdType); // action selected
	void DialogDisplayed();
	int DialogAction(char *msg);

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private
	void UpdateDialog(int init, int type = 0);
	int Test(int p = 0, int open_only = 0) const;

	e2CmdWindow* _myCmdWin;
	int port_no;			// Numero della porta utilizzata
	int lpt_no, com_no;
	HInterfaceType interf_type;
};

class e2ProgressDialog : public vDialog
{
  public:
	e2ProgressDialog(vBaseWindow* bw, char* title = STR_MSGSTATUS);
	virtual ~e2ProgressDialog();		// Destructor
	virtual void DialogCommand(ItemVal,ItemVal,CmdType); // action selected
//	void AddDefaultCmds();		// to add the defined commands

	virtual void ShowDialog(char *msg = 0);
	void DialogDisplayed();

	void UpdateDialog(int val = 0, char *msg = 0);

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private
	e2CmdWindow* _myCmdWin;
	int pbr_value;
	char last_msg[80];
};

#endif
