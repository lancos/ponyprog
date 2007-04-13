//===============================================================
// vradioc.h - Radio Button Command class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VRADIOC_H
#define VRADIOC_H

#include <v/vcmd.h>	// We are derived from vcmd

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    class vCmdParent;

    extern "C"
      {
	void CRadioButtonCmdCB(Widget w, XtPointer This, XtPointer call_data);
      }

    class vRadioButtonCmd : public vCmd
      {
	friend void CRadioButtonCmdCB(Widget w, XtPointer This, XtPointer call_data);

      public:		//---------------------------------------- public

	vRadioButtonCmd(vCmdParent* dp, CommandObject* co);
	vRadioButtonCmd(const vRadioButtonCmd&);	// Copy constructor

	virtual ~vRadioButtonCmd();

	virtual void initialize(void);

	virtual void ResetItemValue(void);

	virtual int GetCmdValue(ItemVal id) VCONST;

	virtual void SetCmdVal(ItemVal val, ItemSetType st);

	virtual void SetCmdStr(VCONST char* str);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

	static vRadioButtonCmd* _RBList;	// list of all radio buttons

	int _copied;				// For copy constructor

	vRadioButtonCmd* _nextRB;		// ptr to next rb

	Widget _wLabel;				// Widget for button label

	void RadioButtonCmdCB(int NotifyParent);	// callback for user action
      };
#endif
