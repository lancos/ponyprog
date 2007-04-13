//===============================================================
// vcmdwin.h - vWin class definitions - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VCMDWIN_H
#define VCMDWIN_H

#include <v/vwindow.h>		// we are derived from vWindow class

    class V_EXPORT vCmdWindow : public vWindow
      {
	friend class V_EXPORT vMenuPane;

      public:		//---------------------------------------- public
	// the constructor

	vCmdWindow(VCONST char* name = "+", int width = 0, int height = 0);

	virtual ~vCmdWindow();			// Destructor

        int NumCmdWindows() VCONST { return _numCmdWindows; }

	// virtual methods
	virtual void CloseWin(void);		// callback for close button

      protected:	//--------------------------------------- protected
        vCmdWindow* GetCmdWinList() VCONST { return CmdWins; }
        vCmdWindow* GetNextCmdWin() { return NextCmdWin; }

      private:		//--------------------------------------- private
        static int _numCmdWindows;	// how many total cmd windows are up
	int destroyed;			// prevent multiple destruction
	vCmdWindow* NextCmdWin;		// link to next window

	static vCmdWindow* CmdWins;	// list of all command windows
      };
#endif
