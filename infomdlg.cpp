//=========================================================================//
//-------------------------------------------------------------------------//
// infomdlg.cpp -- Source for info, notes and edit ModalDialog classes     //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997, 1998  Claudio Lanconelli                           //
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
#include <ctype.h>
#include "infomdlg.h"
#include <v/vnotice.h>

#include "cmdenum.h"

//@V@:BeginDialogCmd DefaultCmds
static DialogCmd DefaultCmds[] =
{
#ifdef	_LINUX_
	{C_Label,lblMainMsg2,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},
#else
	{C_Blank,lblMainMsg2,0," ",NoList,CA_None,isSens,NoFrame,0,0},
#endif

	{C_Label,lblRllBnk,0,"Bank roll-over capability:",NoList,CA_None,isSens,NoFrame,0,lblMainMsg2},
	{C_Text,txtRllBnk,0,"",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblMainMsg2},

	{C_Label,lblCRC,0,"CRC:",NoList,CA_None,isSens,NoFrame,0,lblRllBnk},
	{C_Text,txtCRC,0,"0000h",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblRllBnk},

	{C_Label,lblSize,0,"Size:",NoList,CA_None,isSens,NoFrame,0,lblCRC},
	{C_Text,txtSize,0,"0 Bytes",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblCRC},

	{C_Label,lblSecurity,0,"Security block:",NoList,CA_None,isSens,NoFrame,0,lblSize},
	{C_Text,txtSecurity,0,"0",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblSize},

	{C_Label,lblHEndurance,0,"High endurance block:",NoList,CA_None,isSens,NoFrame,0,lblSecurity},
	{C_Text,txtHEndurance,0,"0",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblSecurity},

	{C_Button,M_OK,0,
#ifdef	_WINDOWS
	" &Close ",
#else
	"  Close ",
#endif
		NoList,CA_DefaultButton,isSens, NoFrame,0,lblHEndurance},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd


//======================>>> infoModalDialog::infoModalDialog <<<==================
infoModalDialog::infoModalDialog(vBaseWindow* bw, int rlv, UWORD crc, long size, int security, int seclen, int highendurance, char* title) :
//infoModalDialog::infoModalDialog(vBaseWindow* bw, int rlv, UWORD crc, long size, char* title) :
    vModalDialog(bw, title)
{
	UserDebug(Constructor,"infoModalDialog::infoModalDialog()\n")
	if (rlv > 0)
		DefaultCmds[2].title = (rlv == 1) ? "Yes " : " No ";
	else
		DefaultCmds[2].title = "Unknown";

	char str[32];
	sprintf(str, "%04Xh", crc);
	DefaultCmds[4].title = new char[strlen(str)+1];
	strcpy(DefaultCmds[4].title, str);

	sprintf(str, "%ld Bytes", size);
	DefaultCmds[6].title = new char[strlen(str)+1];
	strcpy(DefaultCmds[6].title, str);

	if (security >= 0)
		sprintf(str, "%d (len = %d)", security, seclen);
	else
		strcpy(str, "Unknown");
	DefaultCmds[8].title = new char[strlen(str)+1];
	strcpy(DefaultCmds[8].title, str);

	if (security >= 0)
		sprintf(str, "%d", highendurance);
	else
		strcpy(str, "Unknown");
	DefaultCmds[10].title = new char[strlen(str)+1];
	strcpy(DefaultCmds[10].title, str);

	AddDialogCmds(DefaultCmds);		// add the predefined commands
}

//===================>>> infoModalDialog::~infoModalDialog <<<====================
infoModalDialog::~infoModalDialog()
{
	delete DefaultCmds[4].title;
	delete DefaultCmds[6].title;
	delete DefaultCmds[8].title;
	delete DefaultCmds[10].title;

	UserDebug(Destructor,"infoModalDialog::~infoModalDialog() destructor\n")
}

//====================>>> infoModalDialog::infoAction <<<====================
int infoModalDialog::infoAction(char* msg)
{
	ItemVal ans,rval;

	ans = ShowModalDialog(msg,rval);
	if (ans == M_Cancel)
		return 0;

	// *** Add code to process dialog values here

	return ans == M_OK;
}

/**
//====================>>> infoModalDialog::DialogCommand <<<====================
void infoModalDialog::DialogCommand(ItemVal id, ItemVal retval, CmdType ctype)
{
	UserDebug2(CmdEvents,"infoModalDialog::DialogCommand(id:%d, val:%d)\n",id, retval)
	vModalDialog::DialogCommand(id,retval,ctype);
}
**/

/********************************** NOTES DIALOG ********************************/

extern DialogCmd Notes[];

notesModalDialog::notesModalDialog(vBaseWindow* bw, char* id, char* cm, char* title) :
    vModalDialog(bw, title)
{
	UserDebug(Constructor,"notesModalDialog::notesModalDialog()\n")
	if (id && cm)
	{
		Notes[2].title = new char[strlen(id)+1];
		strcpy(Notes[2].title, id);

		Notes[4].title = new char[strlen(cm)+1];
		strcpy(Notes[4].title, cm);
	}
	else
	{
		Notes[2].title = new char[2];
		Notes[2].title[0] = '\0';
		Notes[4].title = new char[2];
		Notes[4].title[0] = '\0';
	}
	AddDialogCmds(Notes);		// add the predefined commands
}

notesModalDialog::~notesModalDialog()
{
	UserDebug(Destructor,"notesModalDialog::~notesModalDialog() destructor\n")

	delete Notes[2].title;
	delete Notes[4].title;
}

int notesModalDialog::notesAction(char* msg)
{
	ItemVal ans,rval;

//	SetString(txiStrID, id);
//	SetString(txiCommnt, cm);
	ans = ShowModalDialog(msg,rval);
	if (ans == M_Cancel)
		return 0;

	// *** Add code to process dialog values here

	return ans == M_OK;
}


/********************************** EDIT DIALOG ********************************/

DialogCmd EditChar[] =
{
	{C_Label,lblEditMsg,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},

	{C_Frame, frmEditChar,0,"Edit Dialog",NoList,CA_None,isSens,NoFrame,0,lblEditMsg},
	{C_Label, lblHexval,0, " Hex     ",NoList,CA_None,isSens,frmEditChar,0,0},
	{C_Label, lblDecval,0, " Decimal ",NoList,CA_None,isSens,frmEditChar,0,lblHexval},
	{C_Label, lblChval,0,  " Char    ",NoList,CA_None,isSens,frmEditChar,0,lblDecval},

	{C_TextIn,txiHexval,0,"",NoList,CA_None,isSens,NoFrame,frmEditChar,lblEditMsg,5,"Hexadecimal value"},
	{C_TextIn,txiDecval,0,"",NoList,CA_None,isSens,NoFrame,frmEditChar,txiHexval,5,"Decimal value"},
	{C_TextIn,txiChval,0,"",NoList,CA_None,isSens,NoFrame,frmEditChar,txiDecval,5,"Char value"},

	{C_Button, M_Cancel, 0,
#ifdef	_WINDOWS
		" &Cancel ",
#else
		"  Cancel ",
#endif
			NoList,CA_None, isSens,NoFrame, 0, frmEditChar},
	{C_Button, M_OK, 0,
#ifdef	_WINDOWS
		" &OK ",
#else
		"  OK ",
#endif
			NoList, CA_DefaultButton, isSens, NoFrame, M_Cancel, frmEditChar},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};

editModalDialog::editModalDialog(vBaseWindow* bw, int curval, char* title) :
    vModalDialog(bw, title)
{
	UserDebug(Constructor,"editModalDialog::editModalDialog()\n")

	if (curval < 0)
		curval = 0;

	oldval = curval;

	char str[32];

	sprintf(str, "%02X", curval);
	EditChar[5].title = new char[strlen(str)+1];
	strcpy(EditChar[5].title, str);

	sprintf(str, "%d", curval);
	EditChar[6].title = new char[strlen(str)+1];
	strcpy(EditChar[6].title, str);

	sprintf(str, "%c", curval);
	EditChar[7].title = new char[strlen(str)+1];
	strcpy(EditChar[7].title, str);

	AddDialogCmds(EditChar);		// add the predefined commands
}

editModalDialog::~editModalDialog()
{
	UserDebug(Destructor,"editModalDialog::~editModalDialog() destructor\n")

	delete EditChar[5].title;
	delete EditChar[6].title;
	delete EditChar[7].title;
}

int editModalDialog::editAction(char* msg, int &retval)
{
	ItemVal ans,rval;

	ans = ShowModalDialog(msg,rval);
	if (ans == M_Cancel)
		return 0;

	int hexval, decval, chval;
	char str[8];
	GetTextIn(txiHexval, str, 3);
	hexval = strtol(str,NULL,16);
	GetTextIn(txiDecval, str, 4);
	decval = strtol(str,NULL,10);
	GetTextIn(txiChval, str, 2);
	chval = str[0];

	if (hexval != oldval)
		retval = hexval;
	else
	if (decval != oldval)
		retval = decval;
	else
	if (isprint(chval) && chval != oldval)
		retval = chval;
	else
		retval = oldval;

	return ans == M_OK;
}

//////////////////////////////////////////////////////////////
DialogCmd EditChar2[] =
{
	{C_Label,lblEditMsg,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},

	{C_TextIn,txiEditText,0,"",NoList,CA_Large,isSens,NoFrame,0,lblEditMsg,80},

	{C_Button, M_Cancel, 0,
#ifdef	_WINDOWS
		" &Cancel ",
#else
		"  Cancel ",
#endif
			NoList,CA_None, isSens,NoFrame, 0, txiEditText},
	{C_Button, M_OK, 0,
#ifdef	_WINDOWS
		" &OK ",
#else
		"  OK ",
#endif
			NoList, CA_DefaultButton, isSens, NoFrame, M_Cancel, txiEditText},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};

editModalDialog2::editModalDialog2(vBaseWindow* bw, char* curval, char* title) :
    vModalDialog(bw, title)
{
	UserDebug(Constructor,"editModalDialog::editModalDialog()\n")

	if (curval == 0)
		curval = "";

	const unsigned int MAXLEN = 80;

	unsigned int len = MAXLEN;
	if (strlen(curval) < MAXLEN)
		len = strlen(curval);
	EditChar2[1].title = new char[len+1];
	strncpy(EditChar2[1].title, curval, len);
	EditChar2[1].title[len] = '\0';

	AddDialogCmds(EditChar2);		// add the predefined commands
}

editModalDialog2::~editModalDialog2()
{
	UserDebug(Destructor,"editModalDialog::~editModalDialog() destructor\n")

	delete EditChar2[1].title;
}

int editModalDialog2::editAction(char* msg, char* text, int len)
{
	ItemVal ans,rval;

	if (len < 1 || text == 0)
		return 0;

	ans = ShowModalDialog(msg,rval);
	if (ans == M_Cancel)
		return 0;

	GetTextIn(txiEditText, text, len);

	return ans == M_OK;
}

