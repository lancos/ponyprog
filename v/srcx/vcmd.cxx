//===============================================================
// vcmd.cxx - vCmd class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vcmd.h>
#include <v/vcmdprnt.h>

//===================>>> vCmd::vCmd <<<=======================
  vCmd::vCmd(vCmdParent* dp, CommandObject* co)
  {
    // We had to make this a non-inline to get access
    // to the vcmdprnt definitons.

    _parentWin = dp;			// dialog class parent
    dlgCmd = co;			// static data structure
    wParent = dp->getParent(co->cFrame); // our parent widget
    _origVal = co->retVal;		// save orignal state

    wBox = 		// many Cmds use a surrounding box
    wCmd = 0;		// main widget for this Cmd

    _intervalId = 0;
  }

//===================>>> vCmd::CopyToLocal <<<=======================
  void vCmd::CopyToLocal(void)
  {
    // Copy static values to local copy
    _title = dlgCmd->title;
    if (!_title || *_title == 0)
        _title = "";
    _cmdId = dlgCmd->cmdId;
    _retVal = dlgCmd->retVal;
    _Sensitive = dlgCmd->Sensitive;
    _itemList = dlgCmd->itemList;
  }
