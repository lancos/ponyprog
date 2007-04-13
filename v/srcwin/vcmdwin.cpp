//===============================================================
// vCmdWin.cxx - vCmdWindow class functions - X11R5 AND Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vcmdwin.h>		// our header
#include <v/vpane.h>		// we have panes

#include <stdlib.h>

// Define static data of the class

    int vCmdWindow::_numCmdWindows = 0;
    vCmdWindow* vCmdWindow::CmdWins = 0;	// no command windows to start

//====================>>> vCmdWindow::vCmdWindow <<<=======================
  vCmdWindow::vCmdWindow(VCONST char *name, int width, int height ) :
    vWindow(name, width, height, CMDWINDOW)
  {
    // make widgets null

    SysDebug(Constructor,"vCmdWindow::vCmdWindow() constructor\n")

    destroyed = 0;		// we haven't been destroyed

    NextCmdWin = CmdWins;	// link in to list of cmdwindows
    CmdWins = this;		// put on front of the list
    ++_numCmdWindows;		// bump how many windows up
  }

//====================>>> vCmdWindow::~vCmdWindow <<<=======================
  vCmdWindow::~vCmdWindow()			// destructor
  {

    SysDebug(Destructor,"vCmdWindow::~vCmdWindow() destructor\n")

  }

//============================>>> vCmdWindow::CloseWin <<<===================
  void vCmdWindow::CloseWin(void)
  {
    //	close button class callback

    vCmdWindow* cw_list;

    SysDebug(Build,"vCmdWindow::CloseWin()\n");

    --_numCmdWindows;			// windows down by one
    // Remove from active list
    if (CmdWins == this)		// first one on list
      {
	CmdWins = CmdWins->NextCmdWin;	// point to next
      }
    else
      {
	for (cw_list = CmdWins ; cw_list != 0 ; cw_list = cw_list->NextCmdWin)
	  {
	    if (cw_list->NextCmdWin == this)
	      {
		// remove from list
		cw_list->NextCmdWin = (cw_list->NextCmdWin)->NextCmdWin;
		break;
	      }
	  }
      }
    
    vWindow::CloseWin();		// Close Stuff in parent
    if (CmdWins == 0)			// last one?
      {
        theApp->selectCmdPanes(0);	// turn off all command panes
	// following added 25Aug99 after input from brad@cs.unt.edu
	// to force menu bar to be correctly updated.
	::DrawMenuBar(theApp->winHwnd()); 
	theApp->CloseLastCmdWindow(this,0);
      }
  }
