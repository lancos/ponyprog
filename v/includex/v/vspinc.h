//===============================================================
// vspinc.h - Value Box
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VSPINC_H
#define VSPINC_H

#include <v/vcmd.h>	// We are derived from vcmd

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    class vCmdParent;

    extern "C"
     {
	void CUpCB(Widget w, XtPointer This, XtPointer lr);
	void CDownCB(Widget w, XtPointer This, XtPointer lr);
      }

    class vSpinnerCmd : public vCmd
      {
	friend void CUpCB(Widget w, XtPointer This, XtPointer lr);
	friend void CDownCB(Widget w, XtPointer This, XtPointer lr);

      public:		//---------------------------------------- public

	vSpinnerCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vSpinnerCmd();

	virtual void initialize(void);

	virtual int GetCmdValue(ItemVal id) VCONST;
	virtual void SetCmdVal(ItemVal val, ItemSetType st);

      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private

	Widget _wUp;			// Up arrow widget
	Widget _wDown;			// Down arrow widget

	char** _fullList;		// alias pointer to full list
	int _maxWidth;			// max width of any item
	int _numItems;			// number of items on list
	int _maxVal;
	int _minVal;
	int _step;
	int _curSelection;		// index of current selected item
	int _TextList;

	void SetupList(void);

	void DownUpCB(int down);	// callback for user action

      };
#endif
