//===============================================================
// vtextinc.h - Button Command class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VTEXTIN_H
#define VTEXTIN_H

#include <v/vcmd.h>	// We are derived from vcmd

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    class vCmdParent;

    extern "C"
      {
	void CtextReturnA(Widget w, XEvent* ev, String* pars, Cardinal* np);
      }

    class vTextInCmd : public vCmd
      {
	friend void CtextReturnA(Widget w, XEvent* ev, String* pars, Cardinal* np);

      public:		//---------------------------------------- public

	vTextInCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vTextInCmd();

	virtual void initialize(void);

	int GetTextIn(ItemVal id, char* str, int maxlen) VCONST; // to get the text back

	virtual void SetCmdVal(ItemVal val, ItemSetType st);

	virtual void SetCmdStr(VCONST char* str);


      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private

	void textReturn(void);		// to handle Return for default exit
	static vTextInCmd* _TList;	// list of all TextInCmds
	vTextInCmd* _nextT;		// ptr to next TextIn

      };
#endif
