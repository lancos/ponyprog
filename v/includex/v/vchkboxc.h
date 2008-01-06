//===============================================================
// vckboxc.h - CheckBox Command class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VCKBOXC_H
#define VCKBOXC_H

#include <v/vcmd.h>	// We are derived from vcmd

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    class vCmdParent;

    extern "C"
      {
	void CCheckBoxCmdCB(Widget, XtPointer, XtPointer);
      }

    class vCheckBoxCmd : public vCmd
      {
	friend void CCheckBoxCmdCB(Widget, XtPointer, XtPointer);

      public:		//---------------------------------------- public

	vCheckBoxCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vCheckBoxCmd();

	virtual void initialize(void);

	virtual void ResetItemValue(void);

	virtual int GetCmdValue(ItemVal id) VCONST;

	virtual void SetCmdVal(ItemVal val, ItemSetType st);

	virtual void SetCmdStr(VCONST char* str);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

	Widget _wLabel;			// widget of the label
	void CheckBoxCmdCB(void);	// callback for user action

      };
#endif
