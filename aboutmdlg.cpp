//=========================================================================//
//-------------------------------------------------------------------------//
// aboutmdlg.cpp -- Source for AboutModalDialog class                      //
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
// $Id$
//=========================================================================//

#include <v/vicon.h>
#include <v/vnotice.h>

#include "globals.h"
#include "aboutmdlg.h"
#include "cmdenum.h"
#include "string_table.h"

#include "e2app.h"

#ifdef	_PONYPROG_
# define	VERSIONE	"2.06e Beta"
#endif

//@V@:About Dialog Commands
#include "icons/PonyProg2000.vbm"
static vIcon ponyI(&ponyprog2000_bits[0],ponyprog2000_height,ponyprog2000_width,ponyprog2000_depth,Transparent);

#include "icons/about.vbm"
static vIcon aboutI(&about_bits[0],about_height,about_width,about_depth);

static CommandObject AboutVguiDlg[] = {
	{C_Frame,frmAboutVgui,0,"V GUI",NoList,CA_NoBorder,isSens,NoFrame,0,0},
	{C_Icon,icoVgui,0,"V GUI",&aboutI,CA_None,isSens,frmAboutVgui,0,0},
	{C_Text,txtVgui,0,STR_VGUIABOUT,NoList,CA_NoBorder,isSens,frmAboutVgui,icoVgui,0},

	{C_Button,M_OK,0,STR_BTNOK,NoList,CA_DefaultButton,isSens,NoFrame,0,frmAboutVgui},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};

static DialogCmd AboutDlg[] = {
	{C_Text,lblAbout0,0,APPNAME " - " APPNAME_EXT "\n    " STR_MSGVERSION "  " VERSIONE "  " __DATE__,
			NoList,CA_NoBorder,isSens,NoFrame,icoPonyProg,0},
	{C_Frame,frmAbout1,0,"About",NoList,CA_NoBorder,isSens,NoFrame,0,lblAbout0},
	{C_Icon,icoPonyProg,0,APPNAME,&ponyI,CA_None,isSens,frmAbout1,0,0},
	{C_Text,lblAbout1,0,
		"Copyright (C) 1997-2002  by  " AUTHORNAME "\n\nE-mail: " AUTHORMAIL "\n" STR_APPDOWNLOAD1 " " APPNAME " " STR_APPDOWNLOAD2 "\n" AUTHORWEB,
			NoList,CA_None,isSens,frmAbout1,icoPonyProg,0},
	{C_Text,lblAbout2,0,
		MSG_TRANSLATORCREDITS "\n" MSG_TRANSLATORNAME,
			NoList,CA_None,isSens,frmAbout1,icoPonyProg,lblAbout1},
	{C_Button,M_OK,0,STR_BTNOK,	NoList,CA_DefaultButton,isSens,NoFrame,0,frmAbout1},

	{C_Button,btnAboutHelp,0,STR_BTNHELP,NoList,CA_None,isSens,NoFrame,M_OK,frmAbout1},

	{C_Button,btnAboutVgui,0,STR_ABOUT " V lib",NoList,CA_None,isSens,NoFrame,btnAboutHelp,frmAbout1},
#ifdef	WIN32
	{C_Blank,blkBlank,0,"      ",NoList,CA_None,isSens,NoFrame,btnAboutVgui,frmAbout1},
	{C_CheckBox,chkSound,0,STR_LBLNOSOUND,NoList,CA_None,isSens,NoFrame,blkBlank,frmAbout1},
#endif
	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};

extern int SetCommandObject(const int id, const int val, CommandObject* CmdList);
extern int SetCommandSensitive(const int id, const int val, CommandObject* CmdList);
extern int SetCommandLabel(const int id, char *str, CommandObject* CmdList);
extern int SetCommandHidden(const int id, const bool val, CommandObject* CmdList);

AboutModalDialog::AboutModalDialog(vBaseWindow* bw, char* title) :
    vModalDialog(bw, title)
{
	UserDebug(Constructor,"AboutModalDialog::AboutModalDialog()\n")

	cmdw = (e2CmdWindow *)bw;
	help = 0;

	if ( THEAPP->GetSoundEnabled() )
		SetCommandObject(chkSound, 0, AboutDlg);
	else
		SetCommandObject(chkSound, 1, AboutDlg);

	if (strlen(MSG_TRANSLATORNAME) == 0)
		SetCommandHidden(lblAbout2, true, AboutDlg);

	AddDialogCmds(AboutDlg);		// add the predefined commands
}

AboutModalDialog::~AboutModalDialog()
{
	UserDebug(Destructor,"AboutModalDialog::~AboutModalDialog() destructor\n")
}

int AboutModalDialog::AboutAction(char* msg)
{
	ItemVal ans,rval;

	help = 0;
	ans = ShowModalDialog(msg,rval);

	if (ans == M_OK)
	{
		if (help)
			return 2;
		else
			return 1;
	}
	else
		return 0;
}

//====================>>> AboutModalDialog::DialogCommand <<<====================
void AboutModalDialog::DialogCommand(ItemVal id, ItemVal retval, CmdType ctype)
{
	UserDebug2(CmdEvents,"AboutModalDialog::DialogCommand(id:%d, val:%d)\n",id, retval)

	switch (id)		// We will do some things depending on value
	{
	case btnAboutVgui:
		{
			ItemVal dummy;
			vModalDialog aboutv(this,STR_ABOUT " V Library");

			aboutv.AddDialogCmds(AboutVguiDlg);
			aboutv.ShowModalDialog("",dummy);
		}
		break;

	case btnAboutHelp:
		//Execute the command
		help = 1;

		vModalDialog::DialogCommand(id,retval,ctype);
		AboutModalDialog::DialogCommand(M_OK,1,ctype);
		break;
	case chkSound:
		if (retval)
			THEAPP->SetSoundEnabled(false);
		else
			THEAPP->SetSoundEnabled(true);
		break;
	default:
		vModalDialog::DialogCommand(id,retval,ctype);
		break;
	}
}
