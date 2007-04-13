//===============================================================
// vdebug.cxx - vDebugDialog class methods
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vdebug.h>		// our header

// Define static data of the class

#define dSystem 10
#define dUser 11

#define dCmdEvents 12
#define dMouseEvents 13
#define dWindowEvents 14

#define dBuild 15
#define dMisc 16
#define dBadVals 17

#define dConstructor 18
#define dDestructor 19
#define dText 30

#define dUserApp1 20
#define dUserApp2 21
#define dUserApp3 22

    static int SetCommandObject(const int id, const int val, CommandObject* CmdList);

    static CommandObject DebugDialog[] =
      {
	{C_Label, 1, 0, "Set Debug Options",
		 NoList, CA_MainMsg, isSens, 0,0},

	{C_Blank, 8, 0, "       ", NoList,CA_None,isSens,NoFrame,1,0},
	{C_Label, 9, 0, V_Version, NoList,CA_None,isSens,NoFrame,8,0},

	// A frame around the check boxes
	{C_Frame, 2, 0, "", NoList, CA_None, isSens,NoFrame, 0, 1},

	// note: the id corresponds to the #defines above
	{C_CheckBox,10,0,"System Msgs   ",NoList,CA_None,isSens,2,0,0},
	{C_CheckBox,11,0,"User Messages ",NoList,CA_None,isSens,2,10,0},

	{C_CheckBox,12,0,"Command Events",NoList,CA_None,isSens,2,0,10},
	{C_CheckBox,13,0,"Mouse Events  ",NoList,CA_None,isSens,2,12,10},
	{C_CheckBox,14,0,"Window Events ",NoList,CA_None,isSens,2,13,10},

	{C_CheckBox,15,0,"Build Windows ",NoList,CA_None,isSens,2,0,12},
	{C_CheckBox,16,0,"Misc Messages ",NoList,CA_None,isSens,2,15,12},
	{C_CheckBox,17,0,"Illegal Values",NoList,CA_None,isSens,2,16,12},

	{C_CheckBox,18,0,"Constructors  ",NoList,CA_None,isSens,2,0,15},
	{C_CheckBox,19,0,"Destructors   ",NoList,CA_None,isSens,2,18,15},
	{C_CheckBox,30,0,"Text          ",NoList,CA_None,isSens,2,19,15},

	{C_CheckBox,20,0,"User Debug 1  ",NoList,CA_None,isSens,2,0,18},
	{C_CheckBox,21,0,"User Debug 2  ",NoList,CA_None,isSens,2,20,18},
	{C_CheckBox,22,0,"User Debug 3  ",NoList,CA_None,isSens,2,21,18},

	{C_Button, M_All,M_All," All On ",NoList,CA_None,isSens,NoFrame,0,2},
	{C_Button, M_None,M_None," All Off ",NoList,CA_None,isSens,NoFrame,M_All,2},
	{C_Blank, 5, 0, "          ", NoList,CA_None,isSens,NoFrame,M_None,2},
	{C_Button, M_Cancel, M_Cancel, " Cancel ", NoList,
		CA_None,isSens,NoFrame, 5,2},
	{C_Button, M_OK, M_OK, " OK ", NoList,
		CA_DefaultButton, isSens, NoFrame, M_Cancel, 2},
	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
      };


//=======================>>> vDebugDialog::SetDebug <<<=======================
  void vDebugDialog::SetDebug()
  {
    //	Show a message, ask user Yes/No/Cancel
    //	returns < 0 for cancel, 0 for No, 1 for Yes

    int id;
    ItemVal val;

    // first, set with current values

    if (!_cmdsAdded)		// Only do this once
      {
	(void) SetCommandObject(dSystem, DebugState.System, DebugDialog);
	(void) SetCommandObject(dUser, DebugState.User, DebugDialog);
	(void) SetCommandObject(dCmdEvents, DebugState.CmdEvents, DebugDialog);
	(void) SetCommandObject(dMouseEvents, DebugState.MouseEvents, DebugDialog);
	(void) SetCommandObject(dWindowEvents, DebugState.WindowEvents, DebugDialog);
	(void) SetCommandObject(dUserApp1, DebugState.UserApp1, DebugDialog);
	(void) SetCommandObject(dUserApp2, DebugState.UserApp2 , DebugDialog);
	(void) SetCommandObject(dUserApp3, DebugState.UserApp3 , DebugDialog);
	(void) SetCommandObject(dBuild, DebugState.Build, DebugDialog);
	(void) SetCommandObject(dMisc, DebugState.Misc, DebugDialog);
	(void) SetCommandObject(dText, DebugState.Text, DebugDialog);
	(void) SetCommandObject(dBadVals, DebugState.BadVals, DebugDialog);
	(void) SetCommandObject(dConstructor, DebugState.Constructor, DebugDialog);
	(void) SetCommandObject(dDestructor, DebugState.Destructor, DebugDialog);

	AddDialogCmds(DebugDialog);		// Set up standard dialog
	_cmdsAdded = 1;
      }

    id = ShowModalDialog("Set Debug Options",val);	// show and wait
    switch (val)			// convert to our system
      {
	case M_OK:
	    DebugState.System = GetValue(dSystem);
	    DebugState.User = GetValue(dUser);
	    DebugState.CmdEvents = GetValue(dCmdEvents);
	    DebugState.MouseEvents = GetValue(dMouseEvents);
	    DebugState.WindowEvents = GetValue(dWindowEvents);
	    DebugState.UserApp1 = GetValue(dUserApp1);
	    DebugState.UserApp2  = GetValue(dUserApp2);
	    DebugState.UserApp3  = GetValue(dUserApp3);
	    DebugState.Build = GetValue(dBuild);
	    DebugState.Misc = GetValue(dMisc);
	    DebugState.Text = GetValue(dText);
	    DebugState.BadVals = GetValue(dBadVals);
	    DebugState.Constructor = GetValue(dConstructor);
	    DebugState.Destructor = GetValue(dDestructor);
	    break;

	case M_Cancel:
	    break;

	default:
	    SysDebug1(BadVals,"vDebugDialog::SetDebug() - can't handle %d\n",val)
	    break;
      };
  }

//====================>>> vDebugDialog::DialogCommand <<<=======================
  void vDebugDialog::DialogCommand(ItemVal id, ItemVal val, CmdType ctype)
  {

    vModalDialog::DialogCommand(id,val,ctype);

    switch (id)
      {
	case M_OK:
	    CloseDialog();
	    break;

	case M_All:				// set all to 1
	    SetValue(dSystem, 1, Value);
	    SetValue(dUser, 1, Value);
	    SetValue(dCmdEvents, 1, Value);
	    SetValue(dMouseEvents, 1, Value);
	    SetValue(dWindowEvents, 1, Value);
	    SetValue(dUserApp1, 1, Value);
	    SetValue(dUserApp2, 1, Value);
	    SetValue(dUserApp3, 1, Value);
	    SetValue(dBuild, 1, Value);
	    SetValue(dMisc, 1, Value);
	    SetValue(dText, 1, Value);
	    SetValue(dBadVals, 1, Value);
	    SetValue(dConstructor, 1, Value);
	    SetValue(dDestructor, 1, Value);
	    break;
	    
	case M_None:				// set all to 0
	    SetValue(dSystem, 0, Value);
	    SetValue(dUser, 0, Value);
	    SetValue(dCmdEvents, 0, Value);
	    SetValue(dMouseEvents, 0, Value);
	    SetValue(dWindowEvents, 0, Value);
	    SetValue(dUserApp1, 0, Value);
	    SetValue(dUserApp2, 0, Value);
	    SetValue(dUserApp3, 0, Value);
	    SetValue(dBuild, 0, Value);
	    SetValue(dMisc, 0, Value);
	    SetValue(dText, 0, Value);
	    SetValue(dBadVals, 0, Value);
	    SetValue(dConstructor, 0, Value);
	    SetValue(dDestructor, 0, Value);
	    break;
      }
  }

//###########################################################################
// Utilities - these may be useful for others, too.

//========================>>> SetCommandObject <<<==============================
  int SetCommandObject(const int id, const int val, CommandObject* CmdList)
  {
    for (CommandObject* cur = CmdList ; cur->cmdType != C_EndOfList ; ++cur)
      {
	if (cur->cmdId == id)
	  {
	    cur->retVal = val;
	    return 1;
	  }
      }

    SysDebug1(BadVals,"SetCommandObject(id:%d...) - No match in list\n",id)

    return 0;
  }

// --------------------------------------------------------------------- 
