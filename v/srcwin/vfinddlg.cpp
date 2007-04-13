//===============================================================
// vFinddlg.cxx - vFindDialog class functions - Common Win/X
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vfinddlg.h>           // our header
#include <v/vicon.h>           // for icon
#include <v/vutil.h>

// Define static data of the class

    static CommandObject FindDialog[] =
      {
	
	{C_Frame, 79, 0, "",NoList,CA_NoBorder,isSens,NoFrame,0,0},
	{C_Frame, 80, 0, "",NoList,CA_None,isSens,79,0,0},

	{C_Label, 69, 0, " ",NoList,
		CA_None,isSens,80, 0,0},
	{C_Label, 91, 0, "Text to find",NoList,
		CA_None,isSens,80, 69,0},
        {C_TextIn, 92, 0, "", NoList,
		CA_Large,isSens,80,91,0},
	

	{C_Frame, 81, 0, "",NoList,CA_None,isSens,79,0,80},
	{C_Label, 60, 0, "Search Range",NoList,
		CA_None,isSens,81, 0,0},
	{C_RadioButton, 93, 1,"From Cursor",NoList,CA_None,
		notSens,81,0,60},
	{C_RadioButton, 94, 0,"Entire File",NoList,CA_None,
		notSens,81,0,93},

	{C_Frame, 82, 0, "",NoList,CA_None,isSens,79,81,80},
	{C_Label, 61, 0, "Search Direction",NoList,
		CA_None,isSens,82, 0,0},
	{C_RadioButton, 95, 1,"Forward",NoList,CA_None,
		isSens,82,0,61},
	{C_RadioButton, 96, 0,"Backward",NoList,CA_None,
		isSens,82,0,95},
	
	{C_Frame, 83, 0, "",NoList,CA_None,isSens,79,82,80},
	{C_Label, 62, 0, "Options",NoList,
		CA_None,isSens,83, 0,0},
	{C_CheckBox, 97, 0,"Case Sensitive",NoList,CA_None,
		isSens,83,0,62},
	
	{C_Button, M_OK, M_OK, "  Find  ", NoList,CA_DefaultButton,
		isSens,NoFrame,79,0},
        {C_Button, M_Cancel, M_Cancel, " Cancel ", NoList,CA_None,
		isSens,NoFrame,79,M_OK},
        {C_EndOfList,0,0,0,0,CA_None,0,0,0}
      };

//======================>>> vFindDialog::AskFindPat <<<=======================
  int vFindDialog::AskFindPat(char* reply, const int maxlen, int& caseSens,
	int& Down, int& Wrap)
  {
    //	Show a message, wait for a reply
    //	no important return

    int ans;

    if (!reply)
	return 0;

    if (!added)
      {
	FindDialog[vGetcmdIdIndex(92,FindDialog)].title = reply;
	AddDialogCmds(FindDialog);		// Set up standard dialog
	added = 1;
      }

    if (*reply)
	SetString(92,reply);
    SetValue(97,caseSens,Value);
    SetValue(94,Wrap,Value);
    SetValue(95,Down,Value);
    int rID = ShowModalDialog("", ans);	// show and WAIT

    if (rID != M_Cancel)
      {
	(void) GetTextIn(92, reply, maxlen);
	caseSens = GetValue(97);
	Wrap = GetValue(94);		// Entire file?
	Down = GetValue(95);		// Down?
      }

   return rID == M_OK;
  }

//====================>>> vFindDialog::DialogCommand <<<=======================
  void vFindDialog::DialogCommand(ItemVal id, ItemVal val, CmdType ctype)
  {
    vModalDialog::DialogCommand(id,val,ctype);
//    if (id == M_OK || id == M_Cancel)
//	CloseDialog();
  }
// --------------------------------------------------------------------- 
