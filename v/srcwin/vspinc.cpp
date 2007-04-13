//===============================================================
// vspinc.cxx	- vSpinnerCmd - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vspinc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vapp.h>
#include <v/vutil.h>

  int vSpinnerCmd::_textIdPool = (int)M_WindowsReserved5;

//=================>>> vSpinnerCmd::vSpinnerCmd <<<=======================
  vSpinnerCmd::vSpinnerCmd(vCmdParent* dp, CommandObject* co) :
	    vCmd(dp, co)
  {
    initialize();			// and initialize
  }

//=======================>>> vSpinnerCmd::~vSpinnerCmd <<<=====================
  vSpinnerCmd::~vSpinnerCmd()
  {

    SysDebug(Constructor,"vSpinnerCmd::~vSpinnerCmd() Destructor\n")

  }

//=====================>>> vSpinnerCmd::initialize <<<=======================
  void vSpinnerCmd::initialize(void)
  {
    char *theText, intval[20];
    long style;

    SysDebug(Constructor,"vSpinnerCmd::vSpinnerCmd() constructor\n")

    CopyToLocal();			// Make local copies of CmdObject

    // First, build the Text box

    SetupList();

    // BEW - v1.20 - If we leave this ES_READONLY, then the spinner
    // has a gray background, which is confusing. It can be edited
    // now, but that doesn't seem to hurt anything.

    style = /* ES_READONLY |*/ WS_BORDER | ES_CENTER; // Bordered readonly text box

    if (!(dlgCmd->attrs & CA_Hidden))	// Check for Hidden
	style |= WS_VISIBLE;

    if (_TextList)
	theText = _fullList[_curSelection];
    else
      {
        style |= (ES_MULTILINE | ES_RIGHT);
	IntToStr(_curSelection,intval);
	theText = intval;
      }

    _maxWidth = _maxWidth*4 + 2;	// change chars to dialog pixels

    _maxWidth = (dlgCmd->attrs & CA_Large) ? (_maxWidth * 3)/2
                                              : _maxWidth;

    if (dlgCmd->size > 0 && dlgCmd->size < 2048)
        _maxWidth = dlgCmd->size;

    _w = _maxWidth;

    _h = 14;

    _textId = _textIdPool++;	// get an id for the text control

    _parentWin->SetPosition(_x, _y, _w, _h, dlgCmd->cFrame, dlgCmd->cRightOf,
        dlgCmd->cBelow);

    _CtrlOffset = _parentWin->AddDlgControl(_x, _y , _w, _h, _textId,
	   style, "EDIT", theText, sizeof(vSpinnerCmd*), (LPBYTE)this);


    // Now, build the up/down arrows out of a scroll bar

    // Cannot make it a WS_TABSTOP or it shows the blinking thumb...
    style = WS_GROUP;

    int tx = _x + _w;
    int ty = _y;
    int th = _h;

    int tw = 10;

    style |= SBS_VERT;
    if (!(dlgCmd->attrs & CA_Hidden))	// Check for Hidden
	style |= WS_VISIBLE;

    // Some kludgy stuff here. We will call SetPosition, but only
    // to make the total size and frame stuff come out right.
    // We know where the control goes already.

    _parentWin->SetPosition(tx, ty, tw, th, dlgCmd->cFrame, _textId,
	dlgCmd->cBelow,1);
    (void)/*_CtrlOffset = */_parentWin->AddDlgControl(tx, ty, tw, th, _cmdId,
	style, "SCROLLBAR", _title, sizeof(vSpinnerCmd*), (LPBYTE)this);

    _w += 10;		// add in width of scroll arrows
  }

//==================>>> vSpinnerCmd::GetCmdValue <<<=========================
  int vSpinnerCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _curSelection;
  }

//=====================>>> vSpinnerCmd::SetCmdVal <<<=========================
  void vSpinnerCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug2(Misc,"vSpinnerCmd::SetCmdVal(val:%d, type:%d)\n",val,st)

    if (st == ChangeList || st == ChangeListPtr)
      {
	if (st == ChangeListPtr)
	    _itemList = dlgCmd->itemList;

	int oldMax = _maxWidth;		// track current max width

	SetupList();			// resetup the list

	if (oldMax > _maxWidth)
	    _maxWidth = oldMax;		// don't let it get narrower
	// redisplay text
	_curSelection = val;
	SetCmdVal(_curSelection,Value);
	return;
      }
    else if (st == Hidden)		// hide or unhide
      {
	HWND myHwnd = GetMyHwnd(_cmdId);
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

	_curSelection = val;	// change the current value

	// See if the currently selected item is in the list

	if (_TextList)		// handle like this for text list
	  {
	    if (_itemList)
	      {
		if (_curSelection < 0)
		    _curSelection = 0;
		if (_curSelection >= _numItems)
		    _curSelection = _numItems - 1;
	      }

	    // Now show the new current selection
	    ::SetDlgItemText(_parentWin->getParent(),_textId,
		   _fullList[_curSelection]);
	  }
	else			// integer values
	  {
	    char intval[32];
	    if (_curSelection < _minVal)
		_curSelection = _minVal;
	    if (_curSelection >= _maxVal)
		_curSelection = _maxVal;

	    IntToStr(_curSelection, intval);

	    ::SetDlgItemText(_parentWin->getParent(),_textId,intval);
	  }
      }
    else if (st == Sensitive)
      {
	HWND myHwnd = GetMyHwnd(_cmdId);
        _Sensitive = val;		// set
	::EnableWindow(myHwnd, val);
      }
  }

//===================>>> vSpinnerCmd::vCmdCallback <<<=======================
  void vSpinnerCmd::CmdCallback(int id, WORD codeNotify)
  {
    // id is the SB_ code, and codeNotify is the position

    int down = 1;

    switch (id)
      {
	case SB_LINEDOWN:		// Down one line
	    down = 1;
	    break;

	case SB_LINEUP:			// Up one line
	    down = 0;
	    break;
      }

    // Change the value, either in the list or the value.

    // The increment will depend on various things

    if (_TextList)
      {
	if (down)
	  {
	    if (_TextList && _curSelection < (_numItems-1))
		_curSelection += _step;
	  }
	else 
	  {
	    if (_TextList && _curSelection > 0)
		_curSelection -= _step;
	  }
      }
    else			// up/down mean opposite for numbers
      {
	if (down)
	  {
	    if ((_maxVal == 0 && _minVal == 0) || (_curSelection - _step) >= _minVal)
		_curSelection -= _step;
	  }
	else 
	  {
	    if ((_maxVal == 0 && _minVal == 0) || (_curSelection + _step) <= _maxVal)
		_curSelection += _step;
	  }
      }
    
    // Now show the new current selection

    SetCmdVal(_curSelection,Value);

    if (!(dlgCmd->attrs & CA_NoNotify))	// Notify on each selection?
	_parentWin->ProcessCmd(_cmdId, _curSelection, dlgCmd->cmdType);
  }

//====================>>> vSpinnerCmd::SetupList <<<=======================
  void vSpinnerCmd::SetupList(void)
  {
    // Set up the list for use

    int width;

    // First, count how many items are in the supplied list

    _fullList = (char**)_itemList;		// list

    _maxWidth = 0;			// no items yet

    if (dlgCmd->attrs & CA_Text)	// A text list of values
      {
	_TextList = 1;			// This is a text list
	_step = 1;
	for ( _numItems = 0 ; _fullList[_numItems] != 0 ; ++_numItems)
	  {

	    width = strlen(_fullList[_numItems]);	// strlen
	    if (width > _maxWidth)
		_maxWidth = width;		// track largest so far
	  }

	if (_numItems <= 0)
	  {
	    SysDebug(BadVals,"Bad list provided for ValueBox\n");
	  }

	// Set to default value

	if (_retVal >= 0 && _retVal < _numItems)
	    _curSelection = _retVal;
	else
	    _curSelection = 0;		// item 0 by default
      }
    else				// an integer list
      {
	_TextList = 0;			// This is an integer list

	int* minMaxList = (int *) _itemList;		// list
	_minVal = 0; _maxVal = 0x7fff; _step = 1;
	if (minMaxList != 0)		// provided a range list
	  {
	    _minVal = minMaxList[0];
	    _maxVal = minMaxList[1];
	    _step = minMaxList[2];

	    // Try to make the width pretty

	    char buff[20];
	    IntToStr(_minVal,buff);
	    _maxWidth = strlen(buff) + 1;

	    IntToStr(_maxVal,buff);
	    int maxW = strlen(buff) + 1;
	    if (maxW > _maxWidth)
		_maxWidth = maxW;
	  }
	else
	    _maxWidth = 6;
	_curSelection = _retVal;
      }

  }
