//===============================================================
// vlistc.cxx	- ListCmd - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vlistc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vapp.h>
  
//=================>>> vListCmd::vListCmd <<<=======================
  vListCmd::vListCmd(vCmdParent* dp, CommandObject* dc) :
	    vCmd(dp, dc)
  {
    _maxWidth = 10;

    SysDebug(Constructor,"vListCmd::vListCmd() constructor\n")

    CopyToLocal();			// Make local copies of CmdObject

    // First, setup the list

    _listRows = 8;
    if (dlgCmd->attrs & CA_Size)	// Size specified
      {
        if (dlgCmd->size > 0 && dlgCmd->size <= 32)
            _listRows = dlgCmd->size;
      }

    SetupList();

    _maxWidth = (dlgCmd->attrs & CA_Large) ? (_maxWidth * 3)/2
					      :	_maxWidth;
    long style = (_parentWin->_dialogType != aCmdBar)
		 ? WS_TABSTOP | WS_GROUP
		 : WS_GROUP;

    if (!(dlgCmd->attrs & CA_Hidden))	// Check for Hidden
	style |= WS_VISIBLE;

    style |= LBS_STANDARD | LBS_NOINTEGRALHEIGHT;

    _w = _maxWidth*4+10;		// set my width

    if ((dlgCmd->attrs & CA_ListWidth) && dlgCmd->retVal > _w)
    	_w = dlgCmd->retVal; 		// CA_ListWidth V 1.18 BEW

    _h = (8 * _listRows) + 4;		// default height (listRows * 8 hi each)

    _parentWin->SetPosition(_x, _y, _w, _h, dlgCmd->cFrame, dlgCmd->cRightOf,
	 dlgCmd->cBelow);
    _CtrlOffset = _parentWin->AddDlgControl(_x, _y, _w, _h, _cmdId,
	style, "LISTBOX", _title, sizeof(vListCmd*), (LPBYTE)this);
  }

//=======================>>> vListCmd::~vListCmd <<<=======================
  vListCmd::~vListCmd()
  {

    SysDebug(Constructor,"vListCmd::~vListCmd() Destructor\n")

  }

//==================>>> vListCmd::GetCmdValue <<<=========================
  int vListCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _curSelection;     	// return currently selected item
  }

//=====================>>> vListCmd::SetCmdVal <<<=========================
  void vListCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug2(Misc,"vListCmd::SetCmdVal(val:%d, type:%d)\n",val,st)
    HWND myHwnd = GetMyHwnd(_cmdId);

    if (st == ChangeList || st == ChangeListPtr)
      {
	if (st == ChangeListPtr)
	    _itemList = dlgCmd->itemList;

	// Change list is used both for initial display,
	// and when user changes the list.
	::SendDlgItemMessage(_parentWin->getParent(),
		_cmdId, LB_RESETCONTENT,0,0); // Clear current

	int oldMax = _maxWidth;		// track current max width

	SetupList();			// resetup the list
	if (oldMax > _maxWidth)
	    _maxWidth = oldMax;		// don't let it get narrower

	for (int ixx = 0 ; ixx < _numItems ; ++ixx)
	  {
	    // Add each item to the list
	    ::SendDlgItemMessage(_parentWin->getParent(), _cmdId,
		LB_INSERTSTRING,ixx,(LPARAM)_fullList[ixx]);
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
		_cmdId, LB_SETCURSEL,_curSelection,0);
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
    else if (st == Value)	// select a given item
      {
	if (val >= _numItems )
	    return;

	_curSelection = val;	// change the current value
	if (val < 0)		// unselect
	  {
	    _curSelection = -1;
	  }

	// Now set appropriate _curSelection
	// in windows, -1 means unselect, so this works for both

	::SendDlgItemMessage(_parentWin->getParent(),
		_cmdId, LB_SETCURSEL,_curSelection,0);

      }
    else if (st == Sensitive)
      {
	_Sensitive = val;		// set
	::EnableWindow(myHwnd, val);
      }
  }


//====================>>> vListCmd::SetupList <<<=======================
  void vListCmd::SetupList(void)
  {
    // Set up the list for use

    int len;

    _curSelection = -1;

    _fullList = (char**)_itemList;	// private copy of list

    // Calculate widest item and number of items
    for ( _numItems = 0 ; _fullList && _fullList[_numItems] != 0 ; ++_numItems)
      {
	len = strlen(_fullList[_numItems]);	// strlen
	if (len > _maxWidth)
	    _maxWidth = len;		// track largest so far
      }

    if (_curSelection < 0)		// make a safe default choice
	_curSelection = 0;
    else if (_curSelection >= _numItems)
	_curSelection = _numItems - 1;

    // Note that at this point _numItems is how many items are
    // in the user supplied list.

  }

//===================>>> vListCmd::CmdCallback <<<=======================
  void vListCmd::CmdCallback(int /*  id */, WORD codeNotify)
  {
    // See if we are getting a message we care about

    if (codeNotify != LBN_SELCHANGE && codeNotify != LBN_DBLCLK)
	return;

    // Retrieve the current selection
    if ((_curSelection = ::SendDlgItemMessage(_parentWin->getParent(),
		_cmdId, LB_GETCURSEL,0,0)) ==  LB_ERR)
      {
	_curSelection = -1;
	return;
      }

    if (_curSelection >= _numItems )	// Safety check
	_curSelection = -1;

    if (!(dlgCmd->attrs & CA_NoNotify))	// Notify on each selection?
	_parentWin->ProcessCmd(_cmdId, _curSelection, dlgCmd->cmdType);
  }
