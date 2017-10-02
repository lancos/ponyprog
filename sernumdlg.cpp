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
#include <v/vnotice.h>

#include "types.h"
#include "sernumdlg.h"
#include "modaldlg_utilities.h"

#ifdef	WIN32
#  ifdef	__BORLANDC__
#    define	strcasecmp stricmp
#  else // _MICROSOFT_ VC++
#    define strcasecmp	_stricmp
#    define snprintf	_snprintf
#  endif
#endif

//@V@:BeginIDs
enum {
	lblSNMsg = 1000,
	frmSN,
	lblLoc,
	lblMProgType,
	lblMDataType,
	lblLen,
	lblVal,
	lblFmt,

	txiLoc,
	txiMProgType,
	txiMDataType,
	txiLen,

	chkMemOffset,
	chkAutoInc,

	txiVal,
	frmFormat,
	rdbLittleEnd,
	rdbBigEnd,

	chkEnableLoc,

	LastId
};
//@V@:EndIds

//@V@:BeginDialogCmd DefaultCmds
static DialogCmd DefaultCmds[] =
{
//	{C_Label,lblSNMsg,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},

	{C_Frame, frmSN,0,"Serial Number",NoList,CA_None,isSens,NoFrame,0,0},
	{C_Label, lblLoc, 0, STR_MSGADDRESS, NoList,CA_None,isSens,frmSN,0,0},
	{C_Label, lblLen, 0, STR_MSGSIZE2, NoList,CA_None,isSens,frmSN,0,lblLoc},
	{C_Label, lblVal, 0, STR_MSGVALUE,NoList,CA_None,isSens,frmSN,0,lblLen},

	{C_TextIn,txiLoc, 0,"",NoList,CA_None,isSens,NoFrame,frmSN,0,10,STR_TTSNMEMADDR},
	{C_TextIn,txiLen, 0,"",NoList,CA_None,isSens,NoFrame,frmSN,txiLoc,10,STR_TTSNSIZE},
	{C_TextIn,txiVal, 0,"",NoList,CA_None,isSens,NoFrame,frmSN,txiLen,10,STR_TTSNVALUE},

	{C_CheckBox, chkMemOffset, 0,STR_MSGOFFSET,NoList,CA_None,isSens,NoFrame,0,frmSN, 0,STR_TTSNOFFSET},
	{C_CheckBox, chkAutoInc, 0,STR_MSGSNAUTOINC,NoList,CA_None,isSens,NoFrame,0,chkMemOffset, 0, STR_TTSNAUTOINC},

	{C_Frame,frmFormat,0,STR_MSGFORMAT,NoList,CA_None,isSens,NoFrame,0,chkAutoInc},
	{C_Label, lblFmt, 0, STR_MSGFORMAT, NoList,CA_None,isSens,frmFormat, 0,0},
	{C_RadioButton,rdbLittleEnd,0,STR_MSGLITTLEEND,NoList,CA_None,isSens,frmFormat,0,lblFmt, 0,STR_TTLITTLEEND},
	{C_RadioButton,rdbBigEnd,   0,STR_MSGBIGENDIAN,NoList,CA_None,isSens,frmFormat,rdbLittleEnd,lblFmt, 0,STR_TTBIGENDIAN},

	{C_Button, M_Cancel, 0, STR_BTNCANC, NoList,CA_None, isSens,NoFrame, 0, frmFormat},
	{C_Button, M_OK, 0, STR_BTNOK, NoList, CA_DefaultButton, isSens, NoFrame, M_Cancel, frmFormat},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd


//=========================>>> SerNumDialog::SerNumDialog <<<====================
SerNumDialog::SerNumDialog(vBaseWindow* bw, char* title) :
		vModalDialog(bw, title)
{
	UserDebug(Constructor,"SerNumDialog::SerNumDialog()\n")
}

//======================>>> SerNumDialog::~SerNumDialog <<<======================
SerNumDialog::~SerNumDialog()
{
	UserDebug(Destructor,"SerNumDialog::~SerNumDialog() destructor\n")
}

int SerNumDialog::SerNumAction(long &cLoc, bool &cMemType, bool &cAutoInc, FmtEndian &cFmt, int &cLen, uint32_t &cVal)
{
	cLoc = (cLoc < 0) ? 0 : cLoc;
	cLen = (cLen < 0 || cLen > 4) ? 4 : cLen;
	cMemType = (cMemType == 0 || cMemType == 1) ? cMemType : 0;

	char str1[MAXNUMDIGIT];
	char str2[MAXNUMDIGIT];
	char str3[MAXNUMDIGIT];

	snprintf(str1, MAXNUMDIGIT, "0x%04lX", cLoc);
	snprintf(str2, MAXNUMDIGIT, "%d", cLen);
	snprintf(str3, MAXNUMDIGIT, "%lu", (unsigned long)cVal);
	SetCommandLabel(txiLoc, str1, DefaultCmds);
	SetCommandLabel(txiLen, str2, DefaultCmds);
	SetCommandLabel(txiVal, str3, DefaultCmds);

	SetCommandObject(chkMemOffset, cMemType, DefaultCmds);
	SetCommandObject(chkAutoInc, cAutoInc, DefaultCmds);

	if (cFmt == FMT_LITTLE_ENDIAN)
	{
		SetCommandObject(rdbLittleEnd, 1, DefaultCmds);
		SetCommandObject(rdbBigEnd, 0, DefaultCmds);
	}
	else
	{
		SetCommandObject(rdbLittleEnd, 0, DefaultCmds);
		SetCommandObject(rdbBigEnd, 1, DefaultCmds);
	}

	AddDialogCmds(DefaultCmds);		// add the predefined commands

	ItemVal ans,rval;
	ans = ShowModalDialog("",rval);
	if (ans == M_Cancel)
		return 0;

	char str[MAXNUMDIGIT];
	GetTextIn(txiLoc, str, 10);
	cLoc = strtol(str,NULL,0);

	GetTextIn(txiLen, str, 10);
	cLen = strtol(str,NULL,0);

	GetTextIn(txiVal, str, 10);
	cVal = strtol(str,NULL,0);

	if ( GetValue(rdbLittleEnd) )
		cFmt = FMT_LITTLE_ENDIAN;
	else
		cFmt = FMT_BIG_ENDIAN;

	cMemType = GetValue(chkMemOffset) ? true : false;
	cAutoInc = GetValue(chkAutoInc) ? true : false;

	return ans == M_OK;
}

//------------------------ OscCalibDialog -----------------------------------//

enum {
	lblMainMsg = 5000,
	btnRead
};

static DialogCmd OscCalibCmds[] =
{
	{C_Frame, frmSN,0,"Osc.Calibration",NoList,CA_None,isSens,NoFrame,0,0},
	{C_Label, lblLoc, 0, STR_MSGADDRESS, NoList,CA_None,isSens,frmSN,0,0},
	{C_Label, lblVal, 0, STR_MSGVALUE,NoList,CA_None,isSens,frmSN,0,lblLoc},

	{C_TextIn,txiLoc, 0,"",NoList,CA_None,isSens,NoFrame,frmSN,0,10,STR_TTSNMEMADDR},
	{C_TextIn,txiVal, 0,"",NoList,CA_None,notSens,NoFrame,frmSN,txiLoc,10,STR_MSGOSCCALIBCFG},

	{C_CheckBox, chkMemOffset, 0,STR_MSGOFFSET,NoList,CA_None,isSens,NoFrame,0,frmSN, 0,STR_TTSNOFFSET},
	{C_CheckBox, chkEnableLoc, 0,"Save",NoList,CA_None,isSens,NoFrame,0,chkMemOffset, 0,"Save the result in the buffer at given location"},

	{C_Button, M_Cancel, 0, STR_BTNCANC, NoList,CA_None, isSens,NoFrame, 0, chkEnableLoc},
	{C_Button, M_OK, 0, STR_BTNOK, NoList, CA_DefaultButton, isSens, NoFrame, M_Cancel, chkEnableLoc},
	{C_Button, btnRead,0,STR_BTNREAD, NoList, CA_None, isSens,NoFrame, M_OK, chkEnableLoc,	0, STR_READOSCCALIB},


	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd


//=========================>>> OscCalibDialog::OscCalibDialog <<<====================
OscCalibDialog::OscCalibDialog(vBaseWindow* bw, e2AppWinInfo* aw, char* title) :
		vModalDialog(bw, title)
{
	UserDebug(Constructor,"OscCalibDialog::OscCalibDialog()\n")

	awip = aw;
}

//======================>>> OscCalibDialog::~OscCalibDialog <<<======================
OscCalibDialog::~OscCalibDialog()
{
	UserDebug(Destructor,"OscCalibDialog::~OscCalibDialog() destructor\n")
}

//======================>>> OscCalibDialog::OscCalibAction <<<======================
int OscCalibDialog::OscCalibAction(bool &cEnable, long &cLoc, bool &cMemType, uint8_t &cVal)
{
	cLoc = (cLoc < 0) ? 0 : cLoc;

	char str1[MAXNUMDIGIT];
	char str3[MAXNUMDIGIT];

	sprintf(str1, "0x%04lX", cLoc);
	sprintf(str3, "%d", cVal);
	SetCommandLabel(txiLoc, str1, OscCalibCmds);
	SetCommandLabel(txiVal, str3, OscCalibCmds);

	SetCommandObject(chkMemOffset, cMemType, OscCalibCmds);
	SetCommandObject(chkEnableLoc, cEnable, OscCalibCmds);

	AddDialogCmds(OscCalibCmds);		// add the predefined commands

	ItemVal ans,rval;
	ans = ShowModalDialog("",rval);
	if (ans == M_Cancel)
		return 0;

	char str[MAXNUMDIGIT];
	GetTextIn(txiLoc, str, 10);
	cLoc = strtol(str,NULL,0);

	GetTextIn(txiVal, str, 10);
	cVal = (uint8_t)strtol(str,NULL,0);

	cMemType = GetValue(chkMemOffset) ? true : false;
	cEnable = GetValue(chkEnableLoc) ? true : false;

	return (ans == M_OK);
}

//====================>>> OscCalibDialog::DialogCommand <<<====================
void OscCalibDialog::DialogCommand(ItemVal id, ItemVal retval, CmdType ctype)
{
	int val;
	char str[MAXNUMDIGIT];

	UserDebug2(CmdEvents,"OscCalibDialog::DialogCommand(id:%d, val:%d)\n",id, retval)

	switch (id)		// We will do some things depending on value
	{
	case btnRead:
		val = awip->ReadOscCalibration();
		sprintf(str, "0x%02X", val);
		SetString(txiVal, str);
		vModalDialog::DialogCommand(id,retval,ctype);
		break;

	default:
		vModalDialog::DialogCommand(id,retval,ctype);
		break;
	}
}

