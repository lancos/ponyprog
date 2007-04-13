//===============================================================
// vchkboxc.cxx	- Checkbox - Windows version
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vchkboxc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vapp.h>
#include <v/vutil.h>

//=================>>> vCheckBoxCmd::~vCheckBoxCmd <<<=======================
  vCheckBoxCmd::~vCheckBoxCmd()
  {
    SysDebug(Destructor,"vCheckBoxCmd::~vCheckBoxCmd() destructor\n")
  }

//=================>>> vCheckBoxCmd::vCheckBoxCmd <<<=======================
  vCheckBoxCmd::vCheckBoxCmd(vCmdParent* dp, CommandObject* dc) :
	    vCmd(dp, dc)
  {
    // Create bitmaps if need to


    SysDebug(Constructor,"vCheckBoxCmd::vCheckBoxCmd() constructor\n")

    initialize();			// and initialize
  }

//=====================>>> vCheckBoxCmd::initialize <<<=======================
  void vCheckBoxCmd::initialize(void)
  {

    long style = WS_TABSTOP | WS_GROUP | BS_CHECKBOX;	// default for a button

    CopyToLocal();			// Make local copies of CmdObject

    if (!(dlgCmd->attrs & CA_Hidden))	// Check for Hidden
	style |= WS_VISIBLE;

    _w = vLblLen(_title)*4+14;		// set my width
    _h = 11;				// default height



    _parentWin->SetPosition(_x, _y, _w, _h + 3, dlgCmd->cFrame, dlgCmd->cRightOf,
	 dlgCmd->cBelow);
   
    _y += 2;  // center better
    _CtrlOffset = _parentWin->AddDlgControl(_x, _y, _w, _h, _cmdId,
	style, "BUTTON", _title, sizeof(vCmd*), (LPBYTE)this);

  }

//================>>> vCheckBoxCmd::ResetItemValue <<<======================
  void vCheckBoxCmd::ResetItemValue(void)
  {

    // We have to toggle things

    if (_retVal == _origVal)	// No op if no change
	return;

    _retVal = _origVal;		// restore

    if (_retVal)			// depends on state
      {
	SetCmdVal(1, Checked);
      }
    else
      {
	SetCmdVal(0, Checked);
      }

    // let parent window handle now
    _parentWin->ProcessCmd(_cmdId, _retVal, dlgCmd->cmdType);
  }

//==================>>> vCheckBoxCmd::GetCmdValue <<<=========================
  int vCheckBoxCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _retVal;
  }

//================>>> vCheckBoxCmd::SetCmdVal <<<============================
  void vCheckBoxCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vCheckBoxCmd::SetCmdVal(val:%d)\n",val)

    HWND myHwnd = GetMyHwnd(_cmdId);

    if (st == Sensitive)
      {
	_Sensitive = val;		// set
	::EnableWindow(myHwnd, val);
      }
    else if (st == Hidden)		// hide or unhide
      {
	if (val)
	  {
	    ::ShowWindow(myHwnd,SW_HIDE);
	  }
	else
	  {
	    ::ShowWindow(myHwnd,SW_SHOW);
	  }
      }
    else if (st == Value || st == Checked)
      {
	_retVal = val;			// set
	::CheckDlgButton(_parentWin->getParent(),_cmdId, val);
      }
  }

//================>>> vCheckBoxCmd::SetCmdStr <<<============================
  void vCheckBoxCmd::SetCmdStr(VCONST char* str)
  {

    SysDebug1(Misc,"vCheckBoxCmd::SetCmdStr(str:%s)\n",str)
    ::SetDlgItemText(_parentWin->getParent(), _cmdId, str);
    _title = str;
  }

//====================>>> vCheckBoxCmd::CmdCallback <<<=======================
  void vCheckBoxCmd::CmdCallback(int id, WORD codeNotify)
  {

    if (_retVal)			// depends on state
      {
        SetCmdVal(0, Checked);
      }
    else
      {
	SetCmdVal(1, Checked);
      }

    // let parent window handle now
    _parentWin->ProcessCmd(_cmdId, _retVal, dlgCmd->cmdType);
  }
