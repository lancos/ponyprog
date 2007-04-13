//===============================================================
// vcmd.h - Base class for Cmds - normally used on Dialogs
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VCMD_H
#define VCMD_H

#include <v/v_defs.h>	// basic definitons

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    // The definitions for this class are all contained in the .h file.

    class vCmdParent;
    class vDialog;

    class vCmd
      {

	friend class vCmdParent;
	friend class vDialog;

      public:		//---------------------------------------- public

	vCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vCmd() {}

	void CopyToLocal(void);

	virtual void initialize(void) {}

	// define these to interface with specific Pane's setters

	virtual int GetCmdValue(ItemVal iv) VCONST {return 0;}

	virtual void ResetItemValue(void) { dlgCmd->retVal = _origVal;}

	virtual void SaveItemValue(void) { _origVal = dlgCmd->retVal;}

	virtual void SetCmdVal(ItemVal val, ItemSetType st) {}

	virtual void SetCmdStr(VCONST char* s) {}

	Widget GetWidget() { return (wBox != 0) ? wBox : wCmd;}
	// stuff for tips
	XtIntervalId GetIntId() { return _intervalId; }
	void SetIntId(XtIntervalId iv) { _intervalId = iv; }
	CommandObject*	GetDlgCmd() { return dlgCmd; }
	
      protected:	//--------------------------------------- protected

	vCmdParent*	_parentWin;	// parent window of this object
	ItemVal		_origVal;	// original value of object
	VCONST char*	_title;
	ItemVal		_cmdId;
	ItemVal		_retVal;
	int		_Sensitive;
	void*		_itemList;

	Widget		wParent;	// parent widget used for this Cmd
	Widget		wBox;		// many Cmds use a surrounding box
	Widget		wCmd;		// main widget for this Cmd

	CommandObject*	dlgCmd;		// pointer to the static definition

      private:		//--------------------------------------- private
	XtIntervalId _intervalId;	// stuff for tips
      };
#endif
