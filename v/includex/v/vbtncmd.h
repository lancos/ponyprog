//===============================================================
// vbtncmd.h - Button Command class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VBTNCMD_H
#define VBTNCMD_H

#include <v/vapp.h>
#include <v/vcmd.h>	// We are derived from vcmd

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    class vCmdParent;

    extern "C"
      {
	void CButtonCmdCB(Widget w, XtPointer This, XtPointer call_data);
      }

    class vButtonCmd : public vCmd
      {
	friend void CButtonCmdCB(Widget w, XtPointer This, XtPointer call_data);

      public:		//---------------------------------------- public

	vButtonCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vButtonCmd(void);

	virtual void initialize(void);

	virtual int GetCmdValue(ItemVal id) VCONST;
	virtual void ResetItemValue(void);
	virtual void SetCmdVal(ItemVal val, ItemSetType st);

	virtual void SetCmdStr(char* str);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

	void ButtonCmdCB(void);	// callback for user action

      };
#endif
