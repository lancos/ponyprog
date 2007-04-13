//===============================================================
// vcmd.cxx - vCmd class
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vapp.h>
#include <v/vcmd.h>
#include <v/vcmdprnt.h>
#include <v/vutil.h>

//===================>>> vCmd::vCmd <<<=======================
  vCmd::vCmd(vCmdParent* dp, CommandObject* co)
  {
    // We had to make this a non-inline to get access
    // to the vcmdprnt definitons.

    _parentWin = dp;			// dialog class parent
    dlgCmd = co;			// static data structure
    wParent = 0;			// parent window
    _origVal = co->retVal;		// save orignal state
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

//===================>>> vCmd::vCmdCallback <<<=======================
  void vCmd::CmdCallback(int /* id */, WORD codeNotify)
  {

   _parentWin->ProcessCmd(dlgCmd->cmdId, dlgCmd->retVal, dlgCmd->cmdType);
  }

//===================>>> vCmd::GetMyHwnd <<<=======================
  HWND vCmd::GetMyHwnd(ItemVal id) VCONST
  {
    return ::GetDlgItem(_parentWin->getParent(), id);
  }

//===================>>> vCmd::LabelWidth <<<=======================
  int vCmd::LabelWidth(VCONST char* lbl) VCONST
  {
    // Ok, I know this is not the right way to do this, but I tried
    // other ways and just couldn't get it to work right before the
    // dialog realy exists, so this kludge helps.

    // map of dialog box width units of Helv 8 characters

    // _!"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ
    // 22344532333423234444444444224444655555455244465555555557454

    // [\]^_`abcdefghijklmnopqrstuvwxyz{|}~
    // 333443444442442242544443424454443235

    int clen[] =
      {
	2,2,3,4,4,5,3,2,3,3,3,4,2,3,2,3,4,4,4,4,4,4,4,4,4,4,
	2,2,4,4,4,4,6,5,5,5,5,5,4,5,5,2,4,4,4,6,5,5,5,5,5,5,
	5,5,5,7,4,5,4,3,3,3,4,4,3,4,4,4,4,4,2,4,4,2,2,4,2,5,
	4,4,4,4,3,4,2,4,4,5,4,4,4,3,2,3,5,0,0,0,0
      };
    int len = 0;
    for (VCONST char* cp = lbl ; *cp ; cp++)
      {
	if (*cp >= ' ' && *cp <= '~')
	    len += clen[*cp-' '];
	else
	    len += 4;
      }
    return len;
  }
