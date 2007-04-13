//===============================================================
// vbasewin.cxx - vBaseWindow class functions - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vbasewin.h>		// our header

//======================>>> vBaseWindow::vBaseWindow <<<=======================
  vBaseWindow::vBaseWindow(VCONST char *name) :
    vBaseItem(name)
  {

    SysDebug1(Constructor,"vBaseWindow::vBaseWindow(%s) constructor\n", name)

    _topLevel = 0;			// null the main window
  }

//======================>>> vBaseWindow::vBaseWindow <<<=======================
  vBaseWindow::vBaseWindow(const vBaseWindow& w) :
    vBaseItem(w)
  {
    vSysError("vBaseWindow: V Semantics do not allow copy constructors.");
  }

//====================>>> vBaseWindow::~vBaseWindow <<<=======================
  vBaseWindow::~vBaseWindow(void)
  {

    SysDebug(Destructor,"vBaseWindow::~vBaseWindow destructor\n")

  }

//======================>>> vWindow::closeBaseWindow <<<=======================
  void vBaseWindow::closeBaseWindow(void)
  {
    if (_vHandle != 0)
      {
	XtDestroyWidget(_vHandle);	// kill us off
	_vHandle = 0;			// and mark our handle null
      }
  }
