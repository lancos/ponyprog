//===============================================================
// vynreply.cxx - vYNReplyDialog class functions - X11R5
//
// Copyright (C) 1995,1996,1997,1998   Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vynreply.h>		// our header
#include <v/vicon.h>
#include <v/vapp.h>		// for calc. of dialog position -  TEH Dec97

// Define static data of the class

#define prompt_width 32
#define prompt_height 32
static unsigned char prompt_bits[] = {
   0x01, 0x00, 0x00, 0x40, 0xf8, 0xff, 0xff, 0x9f, 0x05, 0x00, 0x00, 0x20,
   0xe4, 0xff, 0xff, 0xa7, 0x15, 0x00, 0x00, 0x28, 0x14, 0xc0, 0x01, 0xa8,
   0x15, 0xe0, 0x03, 0x28, 0x14, 0x30, 0x06, 0xa8, 0x15, 0x30, 0x06, 0x28,
   0x14, 0x00, 0x03, 0xa8, 0x15, 0x80, 0x01, 0x28, 0x14, 0xc0, 0x00, 0xa8,
   0x15, 0xc0, 0x00, 0x28, 0x14, 0xc0, 0x00, 0xa8, 0x15, 0xc0, 0x00, 0x28,
   0x14, 0x00, 0x00, 0xa8, 0x15, 0xc0, 0x00, 0x28, 0x14, 0x00, 0x00, 0xa8,
   0xe5, 0xff, 0xff, 0x27, 0x04, 0x00, 0x00, 0xa0, 0xf9, 0xff, 0xff, 0x1f,
   0xfe, 0xff, 0xff, 0x7f, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40,
   0x02, 0x00, 0xe0, 0x47, 0x02, 0x00, 0x00, 0x40, 0xfa, 0xff, 0xff, 0x5f,
   0xae, 0xaa, 0xaa, 0x6a, 0x56, 0x55, 0x55, 0x55, 0xab, 0xaa, 0xaa, 0xea,
   0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00};

    static vIcon prompt(&prompt_bits[0], prompt_height, prompt_width);
    // =
    //  {
    //	BitMap, prompt_height, prompt_width, 1,	// type, h, w, depth
    //	(unsigned char*)&prompt_bits[0]
    //  };

    CommandObject vYNReplyDialog::YNDialog[] =
      {
	// Modified: TEH 12/97 
	// Changed "C_Label" to "C_Text" to allow for multiple line messages.
	// Modified: TEH Jan98
	// Put icon + text in a frame so button is below frame.
	// With many-line text displays, the button will always be below text.
	// Also remove "Blanks" around buttons; not consistant with V look!
	// Finally: made a member of vYNReplyDialog so is inheritable and
	// modifiable (removed "static" modifier).
	{C_Frame, 30, 0, "",
		NoList, CA_NoSpace | CA_NoBorder, isSens, NoFrame, 0, 0},
	{C_Icon,  10, 0, "Pick?",
		(void*)&prompt, CA_None, isSens, 30, 0, 0},
	{C_Text, 91, 0, "",
		NoList, CA_MainMsg | CA_NoBorder, isSens, 30, 10, 0},

	{C_Button, M_Yes, M_Yes, " Yes ", NoList,
		CA_DefaultButton, isSens, NoFrame, 0, 30},
	{C_Button, M_No, M_No, " No ",
		NoList, CA_None, isSens, NoFrame, M_Yes, 30},
	{C_Button, M_Cancel, M_Cancel, " Cancel ", NoList,
		CA_None, isSens, NoFrame, M_No, 30},
	{C_EndOfList, 0, 0, 0, 0, CA_None, 0, 0, 0}
      };


//========================>>> vYNReplyDialog::AskYN <<<=======================
  int vYNReplyDialog::AskYN(VCONST char* msg)
  {
    //	Show a message, ask user Yes/No/Cancel
    //	returns < 0 for cancel, 0 for No, 1 for Yes

    int id, val;

    YNDialog[2].title = msg;		// V:1.13
    if (!added)
      {
	AddDialogCmds(YNDialog);		// Set up standard dialog
	added = 1;
      }
    id = ShowModalDialog(msg, val);	// show and wait
    switch (id)				// convert to our system
      {
	case M_Yes:
	    return 1;

	case M_No:
	    return 0;

	case M_Cancel:
	    return -1;
      };
    return -1;
  }

//====================>>> vYNReplyDialog::DialogCommand <<<=======================
  void vYNReplyDialog::DialogCommand(ItemVal id, ItemVal val, CmdType ctype)
  {
    vModalDialog::DialogCommand(id,val,ctype);
    if (id == M_Yes || id == M_No)
	CloseDialog();
  }

// TEH Dec97
// Added DialogDisplayed to handle the centering of the dialog in the app.
//====================>>> vYNReplyDialog::DialogDisplayed <<<=======================
  void vYNReplyDialog::DialogDisplayed()
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
	left = /* rctApp.left */ (widApp - widDlg)/2;
    else
	left = rctApp.left;
    if ( htApp > htDlg )
	top = /* rctApp.top + */ (htApp - htDlg)/2;
    else
	top = rctApp.top;
    SetDialogPosition (left , top);		// set the position
  }

// ---------------------------------------------------------------------
