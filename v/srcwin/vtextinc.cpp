//===============================================================
// vtextinc.xxx	- TextIn Cmd - Windows
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
#include <v/vtextinc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent


#define TextInFont SysVariableFont        // The font to use

//=====================>>> vTextInCmd::vTextInCmd <<<=======================
  vTextInCmd::vTextInCmd(vCmdParent* dp, CommandObject* co) :
    vCmd(dp, co)
  {
    initialize();
  }

//=====================>>> vTextInCmd::~vTextInCmd <<<=======================
  vTextInCmd::~vTextInCmd()
  {

    SysDebug(Destructor,"vTextInCmd::~vTextInCmd() destructor\n")

  }

//=====================>>> vTextInCmd::initialize <<<=======================
  void vTextInCmd::initialize(void)
  {
    // build a TextIn command for use in a parent window

    SysDebug(Constructor,"vTextInCmd::vTextInCmd() constructor\n")

    _msg[0] = 0;			// no string yet

    CopyToLocal();			// Make local copies of CmdObject

    int defLen = strlen(_title);	// length of supplied string

    if (defLen < 130) strcpy(_msg,_title);

    int defWidth = (dlgCmd->attrs & CA_Large) ? 35 : 20;

    if (dlgCmd->attrs & CA_Small)	// allow small, too.
	defWidth = 10;

    if (dlgCmd->size > 0)		// V:1.13
    	defWidth = dlgCmd->size;

    if (defLen > defWidth)    		// Some work space
	defWidth = defLen + 6;

    long style = ES_LEFT | ES_AUTOHSCROLL | WS_TABSTOP; // V 1.18 BEW

    if (dlgCmd->attrs & CA_Password)
    	style |= ES_PASSWORD;

    if (!(dlgCmd->attrs & CA_NoBorder))
	style |= WS_BORDER;

    if (!(dlgCmd->attrs & CA_Hidden))	// Check for Hidden
	style |= WS_VISIBLE;

    _w = defWidth * 4 + 6;

    _h = 14;				// default height


    _parentWin->SetPosition(_x, _y, _w, _h, dlgCmd->cFrame, dlgCmd->cRightOf,
         dlgCmd->cBelow);

    _CtrlOffset = _parentWin->AddDlgControl(_x, _y , _w, _h, _cmdId,
	   style, "EDIT", _title, sizeof(vTextInCmd*), (LPBYTE)this);
  }

//=====================>>> vTextInCmd::GetTextIn <<<=======================
  int vTextInCmd::GetTextIn(ItemVal /*id*/, char* str, int maxlen) VCONST
  {
    // recover the data from a TextInCmd - return text + length
    int ix;
    for (ix = 0 ; _msg[ix] != 0 && ix < maxlen-1 ; ++ix)
       str[ix] = _msg[ix];
    str[ix] = 0;
    return strlen(str);
  }

//================>>> vTextInCmd::SetCmdVal <<<============================
  void vTextInCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vTextInCmd::SetCmdVal(val:%d)\n",val)
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

  }

//================>>> vTextInCmd::SetCmdStr <<<============================
  void vTextInCmd::SetCmdStr(VCONST char* str)
  {

    SysDebug1(Misc,"vTextInCmd::SetCmdStr(str:%s)\n",str)
    ::SetDlgItemText(_parentWin->getParent(),_cmdId, str);

  }

//====================>>> vRadioButtonCmd::CmdCallback <<<=======================
  void vTextInCmd::CmdCallback(int /*id*/, WORD codeNotify)
  {
     // Edit strings are noops except for EN_CHANGE
     if (codeNotify == EN_CHANGE)
      {
	HWND myHwnd = GetMyHwnd(_cmdId);
	::SendMessage(myHwnd, WM_GETTEXT, 130, (LPARAM)((LPSTR)_msg));
	if ((dlgCmd->attrs & CA_TextInNotify))	// Notify on each selection?
	    _parentWin->ProcessCmd(_cmdId, M_TextInChange, dlgCmd->cmdType);	
      }
    else if (codeNotify == EN_KILLFOCUS)
      {
	if ((dlgCmd->attrs & CA_TextInNotify))	// Notify on each selection?
	    _parentWin->ProcessCmd(_cmdId, M_TextInLeaveFocus, dlgCmd->cmdType);	
      }

  }
