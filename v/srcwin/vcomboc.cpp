//===============================================================
// vcomboc.cxx	- vComboBoxCmd - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vcomboc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vapp.h>
#include <v/vutil.h>

  
//=================>>> vComboBoxCmd::vComboBoxCmd <<<=======================
  vComboBoxCmd::vComboBoxCmd(vCmdParent* dp, CommandObject* dc) :
	    vCmd(dp, dc)
  {
    SysDebug(Constructor,"vComboBoxCmd::vComboBoxCmd() constructor\n")

    CopyToLocal();			// Make local copies of CmdObject

    // First, setup the list
    _maxWidth = 10;

    SetupComboList();

    _maxWidth = (dlgCmd->attrs & CA_Large) ? (_maxWidth * 3)/2
					      :	_maxWidth;

    long style = (_parentWin->_dialogType != aCmdBar)
		 ? WS_TABSTOP | WS_GROUP
		 : WS_GROUP;

    
    if (!(dlgCmd->attrs & CA_Hidden))	// Check for Hidden
	style |= WS_VISIBLE;

    style |=  CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL;

     _w = _maxWidth*4+10;		// set my width


    _h = 14;        		// 14 is std height for combo without list


    _parentWin->SetPosition(_x, _y, _w, _h, dlgCmd->cFrame, dlgCmd->cRightOf,
	dlgCmd->cBelow);

    _CtrlOffset = _parentWin->AddDlgControl(_x, _y, _w, _h, _cmdId,
	style, "COMBOBOX", _title, sizeof(vCmd*), (LPBYTE)this);
  }

//=======================>>> vComboBoxCmd::~vComboBoxCmd <<<=======================
  vComboBoxCmd::~vComboBoxCmd()
  {

    SysDebug(Constructor,"vComboBoxCmd::~vComboBoxCmd() Constructor\n")

  }

//==================>>> vComboBoxCmd::GetCmdValue <<<=========================
  int vComboBoxCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _curSelection;
  }

//=====================>>> vComboBoxCmd::SetCmdVal <<<=========================
  void vComboBoxCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug2(Misc,"vComboBoxCmd::SetCmdVal(val:%d, type:%d)\n",val,st)
    HWND myHwnd = GetMyHwnd(_cmdId);

    if (st == ChangeList || st == ChangeListPtr)
      {
	if (st == ChangeListPtr)
	    _itemList = dlgCmd->itemList;
	// Change list is used both for initial display,
	// and when user changes the list.
	::SendDlgItemMessage(_parentWin->getParent(),
		_cmdId, CB_RESETCONTENT,0,0); // Clear current

	int oldMax = _maxWidth;		// track current max width

	SetupComboList();			// resetup the list
	if (oldMax > _maxWidth)
	    _maxWidth = oldMax;		// don't let it get narrower
	for (int ixx = 0 ; ixx < _numItems ; ++ixx)
	  {
	    // Add each item to the list
	    ::SendDlgItemMessage(_parentWin->getParent(),_cmdId,
		CB_INSERTSTRING,ixx,(LPARAM)_fullList[ixx]);
	  }
	// Need to set to insensitive if it is set insensitive
	if (!_Sensitive)
	  {
	    ::EnableWindow(myHwnd, 0);
	  }
	if (dlgCmd->attrs & CA_Hidden)
	  {
	    ::ShowWindow(myHwnd,SW_HIDE);
	  }
	_curSelection = val;
	::SendDlgItemMessage(_parentWin->getParent(),
		_cmdId, CB_SETCURSEL,_curSelection,0);

	return;
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
    else if (st == Value)
      {
	if (val >= _numItems )
	    return;

	_curSelection = val;	// change the current value
	if (val < 0)		// unselect
	  {
	    _curSelection = -1;
	    return;
	  }
	// Now set appropriate _curSelection
	// in windows, -1 means unselect, so this works for both
	_retVal = _curSelection;

	::SendDlgItemMessage(_parentWin->getParent(),
		_cmdId, CB_SETCURSEL,_curSelection,0);
      }
    else if (st == Sensitive)
      {
	_Sensitive = val;        // set
	::EnableWindow(myHwnd, val);
      }
  }

//===================>>> vComboBoxCmd::CmdCallback <<<=======================
  void vComboBoxCmd::CmdCallback(int /* id */, WORD codeNotify)
  {
    // See if we are getting a message we care about

    if (codeNotify != CBN_SELCHANGE)
	return;

    // Retrieve the current selection
    if ((_curSelection = ::SendDlgItemMessage(_parentWin->getParent(),
		_cmdId, CB_GETCURSEL,0,0)) ==  LB_ERR)
      {
	_curSelection = -1;
	return;
      }

    if (_curSelection >= _numItems )	// Safety check
	_curSelection = -1;

    if (!(dlgCmd->attrs & CA_NoNotify))	// Notify on each selection?
	_parentWin->ProcessCmd(_cmdId, _curSelection, dlgCmd->cmdType);
  }

//====================>>> vComboBoxCmd::SetupComboList <<<=======================
  void vComboBoxCmd::SetupComboList(void)
  {
    // Set up the list for use

    int len;

    _curSelection = -1;

    _fullList = (char**)_itemList;	// private copy of list

    // Calculate widest item and number of items
    for ( _numItems = 0 ; _fullList[_numItems] != 0 ; ++_numItems)
      {
	len = strlen(_fullList[_numItems]);	// strlen
	if (len > _maxWidth)
	    _maxWidth = len;		// track largest so far
      }

    _curSelection = _retVal;	// the default

    if (_curSelection < 0)		// make a safe default choice
	_curSelection = 0;
    else if (_curSelection >= _numItems)
	_curSelection = _numItems - 1;

    // Note that at this point _numItems is how many items are
    // in the user supplied list.

  }
