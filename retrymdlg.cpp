//=========================================================================//
//-------------------------------------------------------------------------//
// retrymdlg.cpp -- Source for retryModalDialog classes                    //
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

#include <stdio.h>
#include "retrymdlg.h"
#include <v/vnotice.h>

#include "cmdenum.h"

//@V@:BeginDialogCmd DefaultCmds
static DialogCmd DefaultCmds[] =
{
//#ifdef	_LINUX_
//	{C_Label,lblMainMsg,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},
//#else
	{C_Label,lblMainMsg,0," ",NoList,CA_None,isSens,NoFrame,0,0},
//#endif

	{C_Button, M_Cancel, 0,
#ifdef	_WINDOWS
		" &Abort ",
#else
		"  Abort ",
#endif
			NoList,CA_DefaultButton, isSens,NoFrame, 0, lblMainMsg},

	{C_Button, M_OK, 0,
#ifdef	_WINDOWS
		" &Retry ",
#else
		"  Retry ",
#endif
			NoList, CA_None, isSens, NoFrame, M_Cancel, lblMainMsg},

	{C_Button, btnIgnore, 0,
#ifdef	_WINDOWS
		" &Ignore ",
#else
		"  Ignore ",
#endif
			NoList, CA_None, isSens, NoFrame, M_OK, lblMainMsg},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd


//======================>>> retryModalDialog::retryModalDialog <<<==================
retryModalDialog::retryModalDialog(vBaseWindow* bw, char* msg, char* title) :
    vModalDialog(bw, title)
{
	UserDebug(Constructor,"retryModalDialog::retryModalDialog() constructor\n")

	DefaultCmds[0].title = new char[strlen(msg)+1];
	strcpy(DefaultCmds[0].title, msg);

	AddDialogCmds(DefaultCmds);		// add the predefined commands
}

//===================>>> retryModalDialog::~retryModalDialog <<<====================
retryModalDialog::~retryModalDialog()
{
	delete DefaultCmds[0].title;

	UserDebug(Destructor,"retryModalDialog::~retryModalDialog() destructor\n")
}

//====================>>> retryModalDialog::infoAction <<<====================
int retryModalDialog::retryAction()
{
	ItemVal ans,rval;

	ans = ShowModalDialog("",rval);
	if (ans == M_Cancel)
		return 0;		//Abort
	else
	if (ans == M_OK)
		return 1;		//Retry
	else
		return 2;		//Ignore
}

//====================>>> retryModalDialog::DialogCommand <<<====================
void retryModalDialog::DialogCommand(ItemVal id, ItemVal retval, CmdType ctype)
{
	UserDebug2(CmdEvents,"retryModalDialog::DialogCommand(id:%d, val:%d)\n",id, retval)

	if (id == btnIgnore)
	{
		CloseDialog();
	}
	vModalDialog::DialogCommand(id,retval,ctype);
}
