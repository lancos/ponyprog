//===============================================================
// vprogrsc.h - List Command class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VPROGRESSC_H
#define VPROGRESSC_H

#include <v/vcmd.h>	// We are derived from vcmd

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    class vCmdParent;

    class vProgressCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vProgressCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vProgressCmd();

	virtual void initialize();

	virtual int GetCmdValue(ItemVal id) VCONST;
	virtual void SetCmdVal(ItemVal val, ItemSetType st);

      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private

	Widget _wScroll;			// For a surrounding box
	Widget _wLabel;			// For a label
	int _ScrlTop;			// Position of top of thumb
	int _ScrlShown;		// Portion of thumb shown

	int _minVal;
	int _maxVal;

	int _curVal;			// current value
	int _isVert;
	int _perCent;
	int _noLabel;

	void SetScroll(int Shown, int Top);	// set scroll values
	int mapVal(int val);

      };
#endif
