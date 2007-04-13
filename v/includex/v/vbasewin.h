//===============================================================
// vbasewin.h - vBaseWindow class definitions - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VBASEWIN_H
#define VBASEWIN_H

#include <v/vbaseitm.h>		// base info class


    class vBaseWindow : public vBaseItem
      {

      public:		//---------------------------------------- public

	vBaseWindow(VCONST char* name);		// Constructor
	vBaseWindow(const vBaseWindow& w);	// Copy Constructor
	virtual ~vBaseWindow();	// Destructor

	virtual void initialize(void) {}	// Pure virtual function

	virtual void ShowWindow(void) { showBaseWindow(); }

	void closeBaseWindow(void);		// Kill window

	WindowType wType() VCONST {return _wType;}	// what kind of window we are


      protected:	//--------------------------------------- protected

	WindowType _wType;	// what kind of window we are building
	Widget _topLevel;	// the main window

	void showBaseWindow(void)
	  { XtPopup(_vHandle, XtGrabNone); }	// Pop us up


      private:		//--------------------------------------- private

      };
#endif
