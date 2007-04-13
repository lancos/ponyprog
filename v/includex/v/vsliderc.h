//===============================================================
// vslider.h - List Command class
//
// Copyright (C) 1995-1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VSLIDERC_H
#define VSLIDERC_H

#include <v/vcmd.h>	// We are derived from vcmd

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    class vCmdParent;

    extern "C"
     {
	void CSliderJumpProcCB(Widget w, XtPointer This, XtPointer pc_ptr);
	void CSliderScrollProcCB(Widget w, XtPointer This, XtPointer position);
      }

    class vSliderCmd : public vCmd
      {
	friend void CSliderJumpProcCB(Widget w, XtPointer This, XtPointer pc_ptr);
	friend void CSliderScrollProcCB(Widget w, XtPointer This, XtPointer position);

      public:		//---------------------------------------- public

	vSliderCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vSliderCmd();

	virtual void initialize();

	virtual int GetCmdValue(ItemVal id) VCONST;
	virtual void SetCmdVal(ItemVal val, ItemSetType st);

      protected:	//--------------------------------------- protected



      private:		//--------------------------------------- private

	int _ScrlTop;		// Position of top of thumb
	int _ScrlShown;		// Portion of thumb shown

	int _curVal;		// current value

	int _minVal;		// Can scale to min and val
	int _maxVal;

	int mapVal(int val);

	void SetScroll(int Shown, int Top);	// set scroll values

	void ScrollProcCB(int position);
	void JumpProcCB(float percent, int mVal);

      };
#endif
