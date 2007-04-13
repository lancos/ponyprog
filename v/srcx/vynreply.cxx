//===============================================================
// vynreply.cxx - vYNReplyDialog class functions - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vynreply.h>		// our header
#include <v/vicon.h>		// for icon

// Define static data of the class

#define prompt_width 32
#define prompt_height 32
static unsigned char prompt_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x1f, 0x04, 0x00, 0x00, 0x20,
   0xe4, 0xff, 0xff, 0x27, 0x14, 0x00, 0x00, 0x28, 0x14, 0xc0, 0x01, 0x28,
   0x14, 0xe0, 0x03, 0x28, 0x14, 0x30, 0x06, 0x28, 0x14, 0x30, 0x06, 0x28,
   0x14, 0x00, 0x03, 0x28, 0x14, 0x80, 0x01, 0x28, 0x14, 0xc0, 0x00, 0x28,
   0x14, 0xc0, 0x00, 0x28, 0x14, 0xc0, 0x00, 0x28, 0x14, 0xc0, 0x00, 0x28,
   0x14, 0x00, 0x00, 0x28, 0x14, 0xc0, 0x00, 0x28, 0x14, 0x00, 0x00, 0x28,
   0xe4, 0xff, 0xff, 0x27, 0x04, 0x00, 0x00, 0x20, 0xf8, 0xff, 0xff, 0x1f,
   0xfe, 0xff, 0xff, 0x7f, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40,
   0x02, 0x00, 0xe0, 0x47, 0x02, 0x00, 0x00, 0x40, 0xfa, 0xff, 0xff, 0x5f,
   0xae, 0xaa, 0xaa, 0x6a, 0x56, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0x6a,
   0xfe, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00};

    static vIcon prompt(&prompt_bits[0], prompt_height, prompt_width);

    static CommandObject YNDialog[] =
      {
	{C_Icon,  10, 0, "Pick?", (void*)&prompt, CA_None, isSens,NoFrame,0,0},
	{C_Label, 1, 0, "Select:",
		 NoList, CA_MainMsg, isSens,NoFrame, 10,0},

	{C_Blank, 2, 0, "  ", NoList, CA_None,isSens,NoFrame, 0, 10},
	{C_Button, M_Yes, M_Yes, " Yes ", NoList,
		CA_DefaultButton, isSens, NoFrame, 2, 10},
	{C_Button, M_No, M_No, " No ", NoList,CA_None,isSens,NoFrame, M_Yes, 10},
	{C_Button, M_Cancel, M_Cancel, " Cancel ", NoList,
		CA_None,isSens,NoFrame, M_No, 10},
	{C_Blank, 6, 0, "  ", NoList, CA_None,isSens,NoFrame, 5, 10},
	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
      };


//========================>>> vYNReplyDialog::AskYN <<<=======================
  int vYNReplyDialog::AskYN(VCONST char* msg)
  {
    //	Show a message, ask user Yes/No/Cancel
    //	returns < 0 for cancel, 0 for No, 1 for Yes

    int id, val;

    if (!_cmdsAdded)			// only add commands once
      {
	AddDialogCmds(YNDialog);	// Set up standard dialog
	_cmdsAdded = 1;
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

// --------------------------------------------------------------------- 
