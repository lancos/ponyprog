//=========================================================================//
//-------------------------------------------------------------------------//
// filldlg.cpp -- Source for FillBuffer dialog class                       //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C)  1997-1999   Claudio Lanconelli                          //
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
#include <stdlib.h>
#include <v/vnotice.h>

#include "types.h"
#include "filldlg.h"

//@V@:BeginIDs
enum {
	lblFillMsg = 1000,
	frmFill,
	lblFrom,
	lblTo,
	lblVal,

	txiFrom,
	txiTo,
	txiVal,

	LastId
};
//@V@:EndIds

//@V@:BeginDialogCmd DefaultCmds
static DialogCmd DefaultCmds[] =
{
	{C_Label,lblFillMsg,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},

	{C_Frame, frmFill,0,"Fill Buffer",NoList,CA_None,isSens,NoFrame,0,lblFillMsg},
	{C_Label, lblFrom,0, " From  ",NoList,CA_None,isSens,frmFill,0,0},
	{C_Label, lblTo,  0, " To    ",NoList,CA_None,isSens,frmFill,0,lblFrom},
	{C_Label, lblVal, 0, " Value ",NoList,CA_None,isSens,frmFill,0,lblTo},

	{C_TextIn,txiFrom,0,"",NoList,CA_None,isSens,NoFrame,frmFill,lblFillMsg,8,"From address"},
	{C_TextIn,txiTo,  0,"",NoList,CA_None,isSens,NoFrame,frmFill,txiFrom,8,"To address"},
	{C_TextIn,txiVal, 0,"",NoList,CA_None,isSens,NoFrame,frmFill,txiTo,8,"Byte value"},

	{C_Button, M_Cancel, 0,
#ifdef	_WINDOWS
		" &Cancel ",
#else
		"  Cancel ",
#endif
			NoList,CA_None, isSens,NoFrame, 0, frmFill},
	{C_Button, M_OK, 0,
#ifdef	_WINDOWS
		" &OK ",
#else
		"  OK ",
#endif
			NoList, CA_DefaultButton, isSens, NoFrame, M_Cancel, frmFill},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd


//=========================>>> FillDialog::FillDialog <<<====================
FillDialog::FillDialog(vBaseWindow* bw, long cfrom, long cto, int cval, char* title) :
		vModalDialog(bw, title)
{
	UserDebug(Constructor,"FillDialog::FillDialog()\n")

	char str[32];

	mFrom = (cfrom < 0) ? 0 : cfrom;
	mTo = (cto < 0) ? 0xFFFFFF : cto;
	mVal = (cval < 0) ? 0xFF : cval;

	sprintf(str, "0x%04lX", mFrom);
	DefaultCmds[5].title = new char[strlen(str)+1];
	strcpy(DefaultCmds[5].title, str);

	sprintf(str, "0x%04lX", mTo);
	DefaultCmds[6].title = new char[strlen(str)+1];
	strcpy(DefaultCmds[6].title, str);

	sprintf(str, "0x%02X", mVal);
	DefaultCmds[7].title = new char[strlen(str)+1];
	strcpy(DefaultCmds[7].title, str);

	AddDialogCmds(DefaultCmds);		// add the predefined commands
}

//======================>>> FillDialog::~FillDialog <<<======================
FillDialog::~FillDialog()
{
	UserDebug(Destructor,"FillDialog::~FillDialog() destructor\n")

	delete DefaultCmds[5].title;
	delete DefaultCmds[6].title;
	delete DefaultCmds[7].title;
}

int FillDialog::fillAction(char* msg)
{
	ItemVal ans,rval;

	ans = ShowModalDialog(msg,rval);
	if (ans == M_Cancel)
		return 0;

	char str[10];
	GetTextIn(txiFrom, str, 8);
	mFrom = strtol(str,NULL,0);

	GetTextIn(txiTo, str, 8);
	mTo = strtol(str,NULL,0);

	GetTextIn(txiVal, str, 8);
	mVal = strtol(str,NULL,0);

	return ans == M_OK;
}
