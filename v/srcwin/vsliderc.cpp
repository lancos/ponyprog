//===============================================================
// vsliderc.cxx	- SliderCmd - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vsliderc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vapp.h>

//=================>>> vSliderCmd::vSliderCmd <<<=======================
  vSliderCmd::vSliderCmd(vCmdParent* dp, CommandObject* dc) :
	    vCmd(dp, dc)
  {
    initialize();			// and initialize
  }

//=======================>>> vSliderCmd::~vSliderCmd <<<=======================
  vSliderCmd::~vSliderCmd()
  {

    SysDebug(Constructor,"vSliderCmd::~vSliderCmd() Destructor\n")

  }

//=====================>>> vSliderCmd::initialize <<<=======================
  void vSliderCmd::initialize()
  {

    SysDebug(Constructor,"vSliderCmd::vSliderCmd() constructor\n")

    long style;

    if (_parentWin->_dialogType != aCmdBar)
	style = WS_TABSTOP | WS_GROUP;	// default for a button
    else
	style = WS_GROUP;

    CopyToLocal();			// Make local copies of CmdObject

    if (!(dlgCmd->attrs & CA_Hidden))	// Check for Hidden
	style |= WS_VISIBLE;

    // Set the size of the slider
    int SliderHeight = (dlgCmd->attrs & CA_Large) ? 170 : 110;
    if (dlgCmd->attrs & CA_Small)
	SliderHeight = 60;
    if (dlgCmd->size > 0 && dlgCmd->size < 2048)
	SliderHeight = dlgCmd->size;

    if (dlgCmd->attrs & CA_Vertical)
      {
	style |= SBS_VERT;
	 _w = 12;		// set my width
	 _h = SliderHeight;				// default height
      }
    else
      {
	style |= SBS_HORZ;
	 _w = SliderHeight;		// set my width
	 _h = 14;				// default height
      }


    int* minMax = (int *)_itemList;	// giving range?
    _minVal = 0; _maxVal = 100;			// default min/max
    if (minMax != 0)				// They gave a range list
      {
	_minVal = minMax[0];
	_maxVal = minMax[1];
      }
    
    if (_minVal > _maxVal)
      {
	SysDebug2(BadVals,"vSliderCmd:vSliderCmd - bad range %d to %d\n",_minVal,_maxVal)
	_minVal = 0; _maxVal = 100;	// make some sense!
      }

    
    if (_retVal < _minVal)	// set a legal value for the top
	_curVal = _minVal;
    else if (_retVal > _maxVal)
	_curVal = _maxVal;
    else
	_curVal = _retVal;



    _parentWin->SetPosition(_x, _y, _w, _h, dlgCmd->cFrame, dlgCmd->cRightOf,
	dlgCmd->cBelow);
    _CtrlOffset = _parentWin->AddDlgControl(_x, _y, _w, _h, _cmdId,
	style, "SCROLLBAR", _title, sizeof(vSliderCmd*), (LPBYTE)this);
  }

//==================>>> vSliderCmd::GetCmdValue <<<=========================
  int vSliderCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _curVal;
  }

//=====================>>> vSliderCmd::SetCmdVal <<<=========================
  void vSliderCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug2(Misc,"vSliderCmd::SetCmdVal(val:%d, type:%d)\n",val,st)
    HWND myHwnd = GetMyHwnd(_cmdId);
    if (st == Value)
      {
	if (val < _minVal || val > _maxVal )
	    return;
	_curVal = val;
	// Now set appropriate _curVal, scroll
	::SetScrollRange(myHwnd, SB_CTL, _minVal, _maxVal, 0);
	::SetScrollPos(myHwnd, SB_CTL, _curVal, 1);
      }
    else if (st == Sensitive)
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

//===================>>> vSliderCmd::vCmdCallback <<<=======================
  void vSliderCmd::CmdCallback(int id, WORD codeNotify)
  {
    // id is the SB_ code, and codeNotify is the position

    short newVal = _curVal;

    switch (id)
      {
	case SB_BOTTOM:			// Scroll to bottom
	    newVal = _maxVal;
	    break;

	case SB_LINEDOWN:		// Down one line
	    if (newVal < _maxVal)
		++newVal;
	    break;

	case SB_LINEUP:			// Up one line
	    if (newVal > _minVal)
		--newVal;
	    break;

	case SB_PAGEDOWN:		// Page Down - use 10
	    if (newVal + 10 < _maxVal)
		newVal += 10;
	    else
		newVal = _maxVal;
	    break;

	case SB_PAGEUP:			// Page Up - use -10
	    if (newVal - 10 > _minVal)
		newVal -= 10;
	    else
		newVal = _minVal;
	    break;

	case SB_THUMBPOSITION:		// move to position given
	case SB_THUMBTRACK:
	    newVal = (short) codeNotify;
	    break;

	case SB_TOP:			// top of scroll
	    newVal = _minVal;
	    break;
      }

    if (newVal != _curVal)		// only notify if changed
      {
	_curVal = newVal;
	HWND myHwnd = GetMyHwnd(_cmdId);		// get my HWND
	::SetScrollPos(myHwnd, SB_CTL, _curVal, 1);	// update control
	_parentWin->ProcessCmd(_cmdId, _curVal, dlgCmd->cmdType);
      }
  }
