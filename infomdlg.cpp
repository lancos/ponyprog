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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <v/vnotice.h>

#include "globals.h"
#include "infomdlg.h"
#include "cmdenum.h"
#include "modaldlg_utilities.h"

#ifdef	WIN32
#  ifdef	__BORLANDC__
#    define	strcasecmp stricmp
#  else // _MICROSOFT_ VC++
#    define strcasecmp	_stricmp
#    define snprintf	_snprintf
#  endif
#endif

//@V@:BeginDialogCmd DefaultCmds
static DialogCmd e24xxCmds[] =
{
#ifdef	_LINUX_
	{C_Label,lblMainMsg2,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},
#else
	{C_Blank,lblMainMsg2,0," ",NoList,CA_None,isSens,NoFrame,0,0},
#endif

	{C_Label,lblRllBnk,0,STR_MSGBANKROLL,NoList,CA_None,isSens,NoFrame,0,lblMainMsg2},
	{C_Text,txtRllBnk,0,"",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblMainMsg2},

	{C_Label,lblCRC,0,STR_MSGCRC,NoList,CA_None,isSens,NoFrame,0,lblRllBnk},
	{C_Text,txtCRC,0,"0000h",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblRllBnk},

	{C_Label,lblSize,0,STR_MSGSIZE,NoList,CA_None,isSens,NoFrame,0,lblCRC},
	{C_Text,txtSize,0,"0 Byte",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblCRC},
/**
	{C_Label,lblSecurity,0,STR_MSGSECBLOCK,NoList,CA_None,isSens,NoFrame,0,lblSize},
	{C_Text,txtSecurity,0,"0",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblSize},

	{C_Label,lblHEndurance,0,STR_HIGHENDURAN,NoList,CA_None,isSens,NoFrame,0,lblSecurity},
	{C_Text,txtHEndurance,0,"0",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblSecurity},
**/
	{C_Button,M_OK,0, STR_BTNCLOSE, NoList,CA_DefaultButton,isSens, NoFrame,0,lblSize},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd

static DialogCmd otherCmds[] =
{
#ifdef	_LINUX_
	{C_Label,lblMainMsg2,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},
#else
	{C_Blank,lblMainMsg2,0," ",NoList,CA_None,isSens,NoFrame,0,0},
#endif

	{C_Label,lblSecurity,0,STR_MSGEEPSIZE ":",NoList,CA_None,isSens,NoFrame,0,lblMainMsg2},
	{C_Text,txtSecurity,0,"0 Byte",NoList,CA_None,isSens,NoFrame,lblSecurity,lblMainMsg2},

	{C_Label,lblSize,0,STR_MSGFLASHSIZE ":",NoList,CA_None,isSens,NoFrame,0,lblSecurity},
	{C_Text,txtSize,0,"0 Byte",NoList,CA_None,isSens,NoFrame,lblSecurity,lblSecurity},

	{C_Label,lblCRC,0,STR_MSGCRC,NoList,CA_None,isSens,NoFrame,0,lblSize},
	{C_Text,txtCRC,0,"0000h",NoList,CA_None,isSens,NoFrame,lblSecurity,lblSize},

	{C_Button,M_OK,0, STR_BTNCLOSE, NoList,CA_DefaultButton,isSens, NoFrame,0,lblCRC},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};


//======================>>> infoModalDialog::infoModalDialog <<<==================
e24xx_infoModalDialog::e24xx_infoModalDialog(vBaseWindow* bw, int rlv, UWORD crc, long size, char* title) :
    vModalDialog(bw, title)
{
	if (rlv == 1)
	{
		SetCommandLabel(txtRllBnk, STR_MSGYES, e24xxCmds);
	}
	else
	if (rlv == 2)
	{
		SetCommandLabel(txtRllBnk, STR_MSGNO, e24xxCmds);
	}
	else
	{
		SetCommandLabel(txtRllBnk, STR_MSGUNKNOWN, e24xxCmds);
	}

	char str[MAXMSG];
	snprintf(str, MAXMSG, "%04Xh", crc);
	str[MAXMSG-1] = '\0';
	strptr[0] = new char[strlen(str)+1];
	strcpy(strptr[0], str);
	SetCommandLabel(txtCRC, strptr[0], e24xxCmds);

	snprintf(str, MAXMSG, "%ld Byte", size);
	str[MAXMSG-1] = '\0';
	strptr[1] = new char[strlen(str)+1];
	strcpy(strptr[1], str);
	SetCommandLabel(txtSize, strptr[1], e24xxCmds);

	strptr[2] = 0;
	strptr[3] = 0;

	AddDialogCmds(e24xxCmds);		// add the predefined commands
}

//======================>>> infoModalDialog::infoModalDialog <<<==================
other_infoModalDialog::other_infoModalDialog(vBaseWindow* bw, long fsize, long esize, UWORD crc, char* title) :
    vModalDialog(bw, title)
{
	char str[MAXMSG];
	snprintf(str, MAXMSG, "%04Xh", crc);
	str[MAXMSG-1] = '\0';
	strptr[0] = new char[strlen(str)+1];
	strcpy(strptr[0], str);
	SetCommandLabel(txtCRC, strptr[0], otherCmds);

	snprintf(str, MAXMSG, "%ld Byte", fsize);
	str[MAXMSG-1] = '\0';
	strptr[1] = new char[strlen(str)+1];
	strcpy(strptr[1], str);
	SetCommandLabel(txtSize, strptr[1], otherCmds);

	snprintf(str, MAXMSG, "%ld Byte", esize);
	str[MAXMSG-1] = '\0';
	strptr[2] = new char[strlen(str)+1];
	strcpy(strptr[2], str);
	SetCommandLabel(txtSecurity, strptr[2], otherCmds);

	strptr[3] = 0;

	AddDialogCmds(otherCmds);		// add the predefined commands
}

//===================>>> infoModalDialog::~infoModalDialog <<<====================
e24xx_infoModalDialog::~e24xx_infoModalDialog()
{
	int k;
	for (k = 0; k < 4; k++)
		delete strptr[k];

	UserDebug(Destructor,"infoModalDialog::~infoModalDialog() destructor\n")
}

//===================>>> infoModalDialog::~infoModalDialog <<<====================
other_infoModalDialog::~other_infoModalDialog()
{
	int k;
	for (k = 0; k < 4; k++)
		delete strptr[k];

	UserDebug(Destructor,"infoModalDialog::~infoModalDialog() destructor\n")
}

//====================>>> infoModalDialog::infoAction <<<====================
int other_infoModalDialog::infoAction(char* msg)
{
	ItemVal ans,rval;

	ans = ShowModalDialog(msg,rval);
	if (ans == M_Cancel)
		return 0;

	// *** Add code to process dialog values here

	return ans == M_OK;
}

//====================>>> infoModalDialog::infoAction <<<====================
int e24xx_infoModalDialog::infoAction(char* msg)
{
	ItemVal ans,rval;

	ans = ShowModalDialog(msg,rval);
	if (ans == M_Cancel)
		return 0;

	// *** Add code to process dialog values here

	return ans == M_OK;
}


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
	{C_Label, lblHexval,0,STR_MSGHEX,NoList,CA_None,isSens,frmEditChar,0,0},
	{C_Label, lblDecval,0,STR_MSGDECIMAL,NoList,CA_None,isSens,frmEditChar,0,lblHexval},
	{C_Label, lblChval,0, STR_MSGCHAR,NoList,CA_None,isSens,frmEditChar,0,lblDecval},

	{C_TextIn,txiHexval,0,"",NoList,CA_None,isSens,NoFrame,frmEditChar,lblEditMsg,5,STR_TTHEX},
	{C_TextIn,txiDecval,0,"",NoList,CA_None,isSens,NoFrame,frmEditChar,txiHexval,5,STR_TTDECIMAL},
	{C_TextIn,txiChval,0,"",NoList,CA_None,isSens,NoFrame,frmEditChar,txiDecval,5,STR_TTCHAR},

	{C_Button, M_Cancel, 0, STR_BTNCANC, NoList,CA_None, isSens,NoFrame, 0, frmEditChar},
	{C_Button, M_OK, 0, STR_BTNOK, NoList, CA_DefaultButton, isSens, NoFrame, M_Cancel, frmEditChar},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};

editModalDialog::editModalDialog(vBaseWindow* bw, int curval, char* title) :
    vModalDialog(bw, title)
{
	UserDebug(Constructor,"editModalDialog::editModalDialog()\n")

	if (curval < 0)
		curval = 0;

	oldval = curval;

	char str[MAXNUMDIGIT];

	snprintf(str, MAXNUMDIGIT, "%02X", curval);
	str[MAXNUMDIGIT-1] = '\0';
	EditChar[5].title = new char[strlen(str)+1];
	strcpy(EditChar[5].title, str);

	snprintf(str, MAXNUMDIGIT, "%d", curval);
	str[MAXNUMDIGIT-1] = '\0';
	EditChar[6].title = new char[strlen(str)+1];
	strcpy(EditChar[6].title, str);

	snprintf(str, MAXNUMDIGIT, "%c", curval);
	str[MAXNUMDIGIT-1] = '\0';
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
	char str[MAXNUMDIGIT];
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

//************************ EDIT DIALOG2 *******************************
DialogCmd EditChar2[] =
{
	{C_Label,lblEditMsg,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},

	{C_TextIn,txiEditText,0,"",NoList,CA_Large,isSens,NoFrame,0,lblEditMsg,80},

	{C_Button, M_Cancel, 0, STR_BTNCANC, NoList,CA_None, isSens,NoFrame, 0, txiEditText},
	{C_Button, M_OK, 0, STR_BTNOK, NoList, CA_DefaultButton, isSens, NoFrame, M_Cancel, txiEditText},

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
