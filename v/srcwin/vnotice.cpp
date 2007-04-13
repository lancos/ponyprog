//===============================================================
// vnotice.cxx - vNoticeDialog class functions - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vnotice.h>		// our header
#include <v/vicon.h>
#include <v/vapp.h>		// for calc. of dialog position -  TEH Dec97

// Define static data of the class

#define bang_width 40
#define bang_height 32
static unsigned char bang_bits[] = {
   0x55, 0x55, 0x3d, 0x55, 0x55, 0xaa, 0xaa, 0x7e, 0xaa, 0xaa, 0x55, 0x55,
   0x7e, 0x55, 0x55, 0xaa, 0x2a, 0xe7, 0xaa, 0xaa, 0x55, 0x55, 0xc3, 0x54,
   0x55, 0xaa, 0xaa, 0xc3, 0xa9, 0xaa, 0x55, 0xd5, 0x99, 0x53, 0x55, 0xaa,
   0xca, 0x34, 0xab, 0xaa, 0x55, 0xe5, 0x6a, 0x57, 0x55, 0xaa, 0x72, 0x76,
   0xae, 0xaa, 0x55, 0x75, 0x6a, 0x4e, 0x55, 0xaa, 0x3a, 0x76, 0x9c, 0xaa,
   0x55, 0x19, 0x6a, 0x58, 0x55, 0xaa, 0x1c, 0x76, 0xb8, 0xaa, 0x55, 0x0e,
   0x6a, 0x70, 0x55, 0xaa, 0x06, 0x76, 0x60, 0xaa, 0x55, 0x07, 0x6a, 0xe0,
   0x54, 0xaa, 0x03, 0x76, 0xc0, 0xa9, 0x95, 0x03, 0x6a, 0xc0, 0x55, 0xca,
   0x01, 0x76, 0x80, 0xab, 0xd5, 0x00, 0x6a, 0x00, 0x53, 0xea, 0x00, 0x34,
   0x00, 0xa7, 0x75, 0x00, 0x18, 0x00, 0x4e, 0x72, 0x00, 0x00, 0x00, 0xae,
   0x39, 0x00, 0x3c, 0x00, 0x5c, 0x1c, 0x00, 0x76, 0x00, 0xb8, 0x1d, 0x00,
   0x6a, 0x00, 0x38, 0x0e, 0x00, 0x76, 0x00, 0xf0, 0x0f, 0x00, 0x3c, 0x00,
   0x70, 0x1e, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfc,
   0xff, 0xff, 0xff, 0xbf};
    static vIcon bang((unsigned char*)&bang_bits[0], bang_height, bang_width);

    // Modified: TEH Jan98
    // Put icon + text in a frame so button is below frame.
    // With many-line text displays, the button will always be below text.
    static CommandObject NoticeDialog[] =
      {
	{C_Frame, 93, 0, "",
		NoList, CA_NoSpace | CA_NoBorder, isSens, NoFrame, 0, 0},
	{C_Icon, 91, 0, "Notice", (void*)&bang, CA_None,isSens,93, 0, 0},
	{C_Text, 92, 0, "",
		 NoList, CA_MainMsg | CA_NoBorder, isSens, 93, 91, 0},
	{C_Button, M_OK, M_OK, " OK ", NoList, CA_DefaultButton, isSens,
		NoFrame, 0, 93},
	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
      };

//======================>>> vNoticeDialog::Notice <<<=======================
  void vNoticeDialog::Notice(VCONST char* msg)
  {
    //	Show a message, and OK
    //	no important return

    int dummy;

    // TEH jan98; move before "if" as it is in vYNReplyDialog
    NoticeDialog[2].title = msg;		// BEW: V:1.13
    if (!_cmdsAdded)				// Only once
      {
	AddDialogCmds(NoticeDialog);		// Set up standard dialog
	_cmdsAdded = 1;
      }
    (void) ShowModalDialog(msg,dummy);		// show and wait
  }

//====================>>> vNoticeDialog::DialogCommand <<<=======================
  void vNoticeDialog::DialogCommand(ItemVal id, ItemVal val, CmdType ctype)
  {
    vModalDialog::DialogCommand(id,val,ctype);
  }

// TEH Dec97
// Added DialogDisplayed to handle the centering of the dialog in the app.
//==================>>> vNoticeDialog::DialogDisplayed <<<=====================
  void vNoticeDialog::DialogDisplayed()
  {
    // center the dialog in the app window
    RECT rctApp, rctDlg;			// app and dialog corners
    ::GetWindowRect(theApp->winHwnd(), &rctApp);
    ::GetWindowRect(_wDialog, &rctDlg);
    int widApp, htApp, widDlg, htDlg;		// app and dialog dimensions
    widApp = rctApp.right - rctApp.left;
    htApp = rctApp.bottom - rctApp.top;
    widDlg = rctDlg.right - rctDlg.left;
    htDlg = rctDlg.bottom - rctDlg.top;
    int left, top;				// new dialog upper-left corner
    if ( widApp > widDlg )
	left = (widApp - widDlg)/2; // don't add to left - SetDialogPosition does
    else
	left = rctApp.left;
    if ( htApp > htDlg )
	top = (htApp - htDlg)/2;
    else
	top = rctApp.top;
    SetDialogPosition (left , top);	      // set the position, in V coords
  }

// ---------------------------------------------------------------------
