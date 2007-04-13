//===============================================================
// vTipWindow.h - vTipWindow class definitions - X11R5
//
// Copyright (C) 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef vTipWindow_H
#define vTipWindow_H

#include <v/v_defs.h>

    class vTipWindow
      {

      public:		//---------------------------------------- public
	vTipWindow();		// Constructor
	vTipWindow(const vTipWindow& w); 		// Copy Constructor

	virtual ~vTipWindow();		// Destructor

	// Utility Methods

	virtual void GetPosition(int& left, int& top, int& width, int& height);
	virtual void SetPosition(int left, int top);
  	virtual void RaiseWindow(void);	// raise this window

	// Other Methods

	virtual void ShowTipWindow(int x, int y, VCONST char* tip);
	virtual void CloseTipWindow(void);

	// Internal Methods

      protected:	//--------------------------------------- protected
          virtual void initialize();

      private:		//--------------------------------------- private
	  Widget _popup;	// the popup widget
	  Widget _wLbl;		// the label

	  int PoppedUp;

      };
#endif
