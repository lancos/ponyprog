//===============================================================
// vrepldlg.cxx - vReplaceDialog class functions - common win/x
//
// Copyright (C) 1995-1999  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vrepldlg.h>           // our header
#include <v/vicon.h>           // for icon
#include <v/vutil.h>

// Define static data of the class

    static CommandObject ReplDialog[] =
      {
	
	{C_Frame, 79, 0, "",NoList,CA_NoBorder,isSens,NoFrame,0,0},
	{C_Frame, 80, 0, "",NoList,CA_None,isSens,79,0,0},

	{C_Label, 69, 0, " ",NoList,
		CA_None,isSens,80, 0,0},
	{C_Label, 91, 0, "Text to find",NoList,
		CA_None,isSens,80, 69,0},
        {C_TextIn, 92, 0, "", NoList,
		CA_Large,isSens,80,91,0},
	{C_Label, 98, 0, "    New Text",NoList,
		CA_None,isSens,80, 69,92},
        {C_TextIn, 99, 0, "", NoList,
		CA_Large,isSens,80,91,92},
	

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
		notSens,82,0,61},
	{C_RadioButton, 96, 0,"Backward",NoList,CA_None,
		notSens,82,0,95},
	
	{C_Frame, 83, 0, "",NoList,CA_None,isSens,79,82,80},
	{C_Label, 62, 0, "Options",NoList,
		CA_None,isSens,83, 0,0},
	{C_CheckBox, 97, 0,"Case Sensitive",NoList,CA_None,
		isSens,83,0,62},
	{C_CheckBox, 100, 0,"Confirm replace",NoList,CA_None,
		isSens,83,0,97},
	
	{C_Button, M_OK, 0, "Replace Next", NoList,CA_DefaultButton,
		isSens,NoFrame,79,0},
	{C_Button, M_Replace, 0, "Replace All ", NoList,CA_DefaultButton,
		isSens,NoFrame,79,M_OK},
        {C_Button, M_Cancel, 0, "   Cancel   ", NoList,CA_None,
		isSens,NoFrame,79,M_Replace},
        {C_EndOfList,0,0,0,0,CA_None,0,0,0}
      };

//======================>>> vReplaceDialog::AskReplacePat <<<=======================
  int vReplaceDialog::AskReplacePat(char* findpat, const int maxlen,
				char* replpat, const int maxrlen,
				int& caseSens, int& Down, int& Wrap,
				int& Prompt, int& All)
  {
    //	Show a message, wait for a reply
    //	no important return

    int ans;

    if (!findpat || !replpat)
	return 0;

    if (!added)
      {
	ReplDialog[vGetcmdIdIndex(92, ReplDialog)].title = findpat;
	ReplDialog[vGetcmdIdIndex(99, ReplDialog)].title = replpat;
	AddDialogCmds(ReplDialog);		// Set up standard dialog
	added = 1;
      }

    if (*findpat)
    	SetString(92,findpat);
    if (*replpat)
	SetString(99,replpat);
    SetValue(97,caseSens,Value);
    SetValue(94,Wrap,Value);
    SetValue(95,Down,Value);
    
    int rv = ShowModalDialog("", ans);	// show and WAIT

    if (rv != M_Cancel)
      {
	(void) GetTextIn(92, findpat, maxlen);
	(void) GetTextIn(99, replpat, maxrlen);
	caseSens = GetValue(97);	// Case sensitive
	Wrap = GetValue(94);		// Entire file?
	Down = GetValue(95);		// Down?
	Prompt = GetValue(100);
        if (rv == M_Replace)
            All = 1;
        else
            All = 0;
        return 1;
      }

   return 0;
  }

//====================>>> vReplaceDialog::DialogCommand <<<=======================
  void vReplaceDialog::DialogCommand(ItemVal id, ItemVal val, CmdType ctype)
  {
    vModalDialog::DialogCommand(id,val,ctype);
    if (id == M_Replace)
	CloseDialog();
  }
// --------------------------------------------------------------------- 
