//===============================================================
// vcmdwin.h - vWin class definitions - X11R5
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

    class vCmdWindow : public vWindow
      {
	friend class vMenuPane;

      public:		//---------------------------------------- public
	// the constructor

	vCmdWindow(VCONST char* name = "+", int width = 0, int height = 0);

	virtual ~vCmdWindow();			// Destructor

	// virtual methods
	virtual void CloseWin(void);		// callback for close button
        virtual void _CloseWinFinal(void);
        int NumCmdWindows() VCONST { return _numCmdWindows; }

      protected:	//--------------------------------------- protected
        vCmdWindow* GetCmdWinList() VCONST { return CmdWins; }
        vCmdWindow* GetNextCmdWin() VCONST { return NextCmdWin; }

      private:		//--------------------------------------- private
	// data
	static int _numCmdWindows;	// how many total cmd windows are up

	static vCmdWindow* CmdWins;	// list of all command windows

	int destroyed;			// prevent multiple destruction

	vCmdWindow* NextCmdWin;		// link to next window

      };
#endif
