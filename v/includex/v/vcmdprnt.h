//===============================================================
// vcmdpar.h - A parent class for Cmds
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================
//
//	This class is used by vDialog and CmdPane classes to
//	allow the vCmd objects to have access to the ProcessCmd

#ifndef VCMDPAR_H
#define VCMDPAR_H

#include <v/v_defs.h>
#include <v/vcmd.h>

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
}

    class vCmd;

    typedef struct DlgCmdList
      {
	vCmd* cmdP;		// pointer to a command object
	DlgCmdList* nextDCL;	// next button
      } DlgCmdList;

    class vCmdParent
      {
      public:		//---------------------------------------- public

	vCmdParent();

	virtual ~vCmdParent();

	virtual void ProcessCmd(ItemVal id, ItemVal rv, CmdType ct) {}

  	virtual int GetTextIn(ItemVal id, char* strout, int maxlen);
	virtual int GetValue(ItemVal id) VCONST;
	int HasId(ItemVal id) VCONST ;
	virtual void SetValue(ItemVal id, ItemVal val, ItemSetType setType);
	virtual void SetString(ItemVal id, VCONST char* str);

	virtual void SetFrameChildren(ItemVal frameId, int frameVal);

	DlgCmdList* GetCmdList() {return _cmdList;}	// list of command items

	// These are platform dependent...

	Widget getParent(ItemVal id) VCONST;		// to get parent widget
	Widget getWidgetFromId(ItemVal id) VCONST;	// also to get parent
	Widget getFrameWidgetFromId(ItemVal id) VCONST;	// also to get frame parent

      protected:	//--------------------------------------- protected

	DlgCmdList* _cmdList;		// list of command items

	Widget _wDialog;		// widget used to hold dialog commands

  	virtual vCmd* AddCmd(CommandObject* cmd);	// Add a Cmd to the dialog

      private:		//--------------------------------------- private

      };
#endif
