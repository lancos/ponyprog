//===============================================================
// vradiocc.cxx	- RadioButtons - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vradioc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vapp.h>
#include <v/vutil.h>

    vRadioButtonCmd* vRadioButtonCmd::_RBList = 0;	// empty list to start

//================>>> vRadioButtonCmd::vRadioButtonCmd <<<=====================
  vRadioButtonCmd::vRadioButtonCmd(vCmdParent* dp, CommandObject* dc) :
	    vCmd(dp, dc)
  {
    // Create bitmaps if need to


    SysDebug(Constructor,"vRadioButtonCmd::vRadioButtonCmd() constructor\n")

    initialize();			// and initialize
  }

//================>>> vRadioButtonCmd::vRadioButtonCmd <<<=====================
  vRadioButtonCmd::vRadioButtonCmd(const vRadioButtonCmd& r) :
	    vCmd(r)
  {
    vSysError("vRadioButtonCmd - V semantics do not support copy constructors!");
  }

//===========>>> vRadioButtonCmd::~vRadioButtonCmd <<<======================
  vRadioButtonCmd::~vRadioButtonCmd()
  {
    // We have to remove ourself from the list of all RBs


    SysDebug(Destructor,"vRadioButtonCmd::~vRadioButtonCmd() destructor\n")

    if (_RBList == this)		// first one special
      {
	_RBList = _nextRB;
      }
    else
      {
	for (vRadioButtonCmd* rbl = _RBList ; rbl != 0 ; rbl = rbl->_nextRB)
	  {
	    if (rbl->_nextRB == this)		// we found ourself!
	      {
		rbl->_nextRB = _nextRB;		// unlink us
		break;
	      }
	  }
      }
  }

//=====================>>> vRadioButtonCmd::initialize <<<=======================
  void vRadioButtonCmd::initialize(void)
  {
    // build a button command for use in a parent window


    _nextRB = _RBList;		// add us in to front of list
    _RBList = this;

    long style = WS_TABSTOP | WS_GROUP | BS_RADIOBUTTON;	// default for a button

    CopyToLocal();			// Make local copies of CmdObject

    if (!(dlgCmd->attrs & CA_Hidden))	// Check for Hidden
	style |= WS_VISIBLE;


    _w = vLblLen(_title)*4+14;		// set my width
    _h = 11;


    _parentWin->SetPosition(_x, _y, _w, _h+3, dlgCmd->cFrame, dlgCmd->cRightOf,
	 dlgCmd->cBelow);
    _y += 2;	// Center better
    _CtrlOffset = _parentWin->AddDlgControl(_x, _y, _w, _h, _cmdId,
	style, "BUTTON", _title, sizeof(vCmd*), (LPBYTE)this);
  }

//================>>> vRadioButtonCmd::ResetItemValue <<<======================
  void vRadioButtonCmd::ResetItemValue(void)
  {

    // We have to toggle things

    if (_retVal == _origVal)	// No op if no change
	return;

    _retVal = _origVal;		// restore
    ::CheckDlgButton(_parentWin->getParent(), _cmdId, _retVal);

    // let parent window handle now
    _parentWin->ProcessCmd(_cmdId, _retVal, dlgCmd->cmdType);
  }

//==================>>> vRadioButtonCmd::GetCmdValue <<<=========================
  int vRadioButtonCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _retVal;
  }

//================>>> vRadioButtonCmd::SetCmdVal <<<========================
  void vRadioButtonCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vRadioButtonCmd::SetCmdVal(val:%d)\n",val)

    HWND myHwnd = GetMyHwnd(_cmdId);

    if (st == Sensitive)
      {
	_Sensitive = val;		// set
	EnableWindow(myHwnd, val);
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
        if (_retVal)			// Need to turn off other
          {
            // We have to toggle things, so scan the list of all radio buttons,
    	    // searching for other buttons in the same frame

    	    ItemVal ourFrame = dlgCmd->cFrame;		// remember our frame

            for (vRadioButtonCmd* rbl = _RBList ; rbl != 0 ; rbl = rbl->_nextRB)
              {
		CommandObject* dc = rbl->dlgCmd;	// shorthand

		if (dc->cFrame == ourFrame)		// Radio Button in same frame
	  	  {
	    	    if (rbl->_retVal && rbl->_cmdId != _cmdId) 	// This one was ON, turn off
                      {
			rbl->_retVal = 0;			// toggle
			::CheckDlgButton(_parentWin->getParent(),rbl->_cmdId, 0);
                        break;
                      }
	  	  }
              }
          }

	::CheckDlgButton(_parentWin->getParent(), _cmdId, val);
      }
  }

//================>>> vRadioButtonCmd::SetCmdStr <<<=========================
  void vRadioButtonCmd::SetCmdStr(VCONST char *str)
  {

    SysDebug1(Misc,"vRadioButtonCmd::SetCmdStr(str:%s)\n",str)
    SetDlgItemText(_parentWin->getParent(), _cmdId, str);
    _title = str;
  }

//====================>>> vRadioButtonCmd::CmdCallback <<<=======================
  void vRadioButtonCmd::CmdCallback(int /* id */, WORD /*codeNotify*/)
  {

    // We have to toggle things, so scan the list of all radio buttons,
    // searching for other buttons in the same frame

    ItemVal ourFrame = dlgCmd->cFrame;		// remember our frame

    for (vRadioButtonCmd* rbl = _RBList ; rbl != 0 ; rbl = rbl->_nextRB)
      {
	CommandObject* dc = rbl->dlgCmd;	// shorthand

	if (dc->cFrame == ourFrame)		// Radio Button in same frame
	  {
	    if (rbl->_retVal && rbl->_cmdId != _cmdId) 	// This one was ON, turn off
	      {
		rbl->_retVal = 0;			// toggle
		::CheckDlgButton(_parentWin->getParent(),rbl->_cmdId, 0);

		// inform parent of change
		_parentWin->ProcessCmd(rbl->_cmdId, rbl->_retVal, dc->cmdType);
		break;
	      }
	  }
      }

    // Now, we need to turn this RadioButton ON
    if (!_retVal)
      {
	_retVal = 1;			// toggle
	SetCmdVal(1,Checked);

	// and let the parent window know about the change
	_parentWin->ProcessCmd(_cmdId, _retVal, dlgCmd->cmdType);
      }
  }
