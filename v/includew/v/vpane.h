//===============================================================
// vpane.h - Base class for window panes - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VPANE_H
#define VPANE_H

//  panes will be canvases, menus, and command bars

#include <v/v_defs.h>	// basic definitons

// This is a small class, and everything is inline here
// in the .h file.

    class V_EXPORT vWindow;	// all panes are part-of a vWindow

    class V_EXPORT vPane
      {
	friend class V_EXPORT vWindow;
	friend class V_EXPORT vCmdWindow;

      public:		//---------------------------------------- public

	vPane(PaneType pt)
	  {
	    SysDebug1(Constructor,"vPane::vPane(type: %d) constructor\n",pt)
	    _paneType = pt;
	  }

	virtual ~vPane()
	  {
	    SysDebug(Destructor,"vPane::~vPane() destructor\n")
	  }

	virtual void initialize(vWindow* pw, HWND bw)
	  {_parentWin = pw; _baseW = (HWND)bw; }

	// define these to interface with specific Pane's setters

	virtual int GetPaneValue(ItemVal /* id */, int& /* v */) VCONST {return 0;}
	virtual int GetPaneTextIn(ItemVal /* id */, char* /* v */, int /*max*/) VCONST {return 0;}

	virtual void SetPaneValue(ItemVal /* id */, int /* v */, ItemSetType /* st */) {}

	virtual void SetPaneString(ItemVal /*id */, VCONST char* /* s */) {}

	virtual void ShowPane(int /* OnOrOff */) VCONST {}
	virtual vWindow* GetPaneParent() {return _parentWin; }

      protected:	//--------------------------------------- protected

	vWindow*	_parentWin;	// parent window of this object

	HWND		_baseW;		// base widget used for this pane

	PaneType	_paneType;	// type of this pane

      private:		//--------------------------------------- private


      };
#endif
