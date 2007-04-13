//===============================================================
// vreply.cxx - vReplyDialog class functions - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vreply.h>           // our header
#include <v/vicon.h>	// icons
#include <v/vapp.h>		// for calc. of dialog position -  TEH Dec97

// Define static data of the class

#define prompt_depth 1
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

    static vIcon prompt((unsigned char*)&prompt_bits[0],prompt_height,
       prompt_width,prompt_depth);

    static CommandObject ReplyDialog[] =
      {
	// Modified: TEH Jan98
	// Put icon + text in a frame so button is below frame.
	// With many-line text displays, the button will always be below text.
	// Also remove "Blanks" around buttons; not consistant with V look!
	// Put OK button to left, and Cancel to right for proper style.
	{C_Frame, 30, 0, "",
		NoList, CA_NoSpace | CA_NoBorder, isSens, NoFrame, 0, 0},
	{C_Icon,  10, 0, "Reply?", (void*)&prompt, CA_None, isSens, 30, 0, 0},
	{C_Text, 91, 91, "", NoList,
		CA_MainMsg | CA_NoBorder ,isSens, 30, 10,0},
	{C_TextIn, 92, 92, "", NoList,
		CA_Large, isSens, NoFrame, 0, 30},
	{C_Button, M_OK, M_OK, " OK ",
		NoList, CA_DefaultButton, isSens, NoFrame, 0, 92},
	{C_Button, M_Cancel, M_Cancel, " Cancel ",
		NoList, CA_None, isSens, NoFrame, M_OK, 92},
	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
      };

//======================>>> vReplyDialog::Reply <<<=======================
  int vReplyDialog::Reply(VCONST char* msg, char* reply, int maxlen,
       VCONST char* dflt)
  {
    //	Show a message, wait for a reply
    //	no important return

    int ans;

    // TEH jan98; move before "if" as it is in vYNReplyDialog
    ReplyDialog[2].title = (char *)msg;
    // TEH jan98; added ability to have a "default" reply to display
    ReplyDialog[3].title = dflt;
    if (!added)
      {
	AddDialogCmds(ReplyDialog);		// Set up standard dialog
	added = 1;
      }

    int rv = ShowModalDialog((char*)msg, ans);	// show and wait

    reply[0] = 0;

    if (rv != M_Cancel)
	(void) GetTextIn(92, reply, maxlen);

    return rv;
  }

//====================>>> vReplyDialog::DialogCommand <<<=======================
  void vReplyDialog::DialogCommand(ItemVal id, ItemVal val, CmdType ctype)
  {
   // vModalDialog::DialogCommand(id,val,ctype);
   // if (id == M_OK || id == M_Cancel)
   //	CloseDialog();
       vModalDialog::DialogCommand(id, val, ctype);
  }

// TEH Dec97
// Added DialogDisplayed to handle the centering of the dialog in the app.
//====================>>> vReplyDialog::DialogDisplayed <<<=======================
  void vReplyDialog::DialogDisplayed()
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
	left = /* rctApp.left + */(widApp - widDlg)/2;
    else
	left = rctApp.left;
    if ( htApp > htDlg )
	top = /* rctApp.top + */ (htApp - htDlg)/2;
    else
	top = rctApp.top;
    SetDialogPosition (left , top);		// set the position
  }

// ---------------------------------------------------------------------
