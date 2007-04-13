//===============================================================
// vprogrsc.cxx	- Progress Bar - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vprogrsc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vapp.h>
#include <v/vutil.h>


//=================>>> vProgressCmd::vProgressCmd <<<=======================
  vProgressCmd::vProgressCmd(vCmdParent* dp, CommandObject* dc) :
	    vCmd(dp, dc)
  {
    initialize();			// and initialize
  }

//=======================>>> vProgressCmd::~vProgressCmd <<<===================
  vProgressCmd::~vProgressCmd()
  {

    SysDebug(Constructor,"vProgressCmd::~vProgressCmd() Destructor\n")

  }

//=====================>>> vProgressCmd::initialize <<<=======================
  void vProgressCmd::initialize()
  {

    SysDebug(Constructor,"vProgressCmd::vProgressCmd() constructor\n")

    CopyToLocal();

    long style;

    style = WS_GROUP;		 // Make local copies of CmdObject

     if (!(dlgCmd->attrs & CA_Hidden))	// Check for Hidden
	style |= WS_VISIBLE;

     style |= BS_OWNERDRAW;		// We are drawing this

    _isVert = (dlgCmd->attrs & CA_Vertical);

    int ProgressHeight = (dlgCmd->attrs & CA_Large) ? 150 : 100;
    if ((dlgCmd->attrs) & CA_Small)
	ProgressHeight = 50;
    if (dlgCmd->size > 0)
	ProgressHeight = dlgCmd->size;		// they gave size

    int* minMax = (int *)_itemList;	// giving range?
    _minVal = 0; _maxVal = 100;			// default min/max
    if (minMax != 0)				// They gave a range list
      {
	_minVal = minMax[0];
	_maxVal = minMax[1];
      }
    if (_minVal > _maxVal)
      {
	SysDebug2(BadVals,"vProgressCmd:vProgressCmd - bad range %d to %d\n",_minVal,_maxVal)
	_minVal = 0; _maxVal = 100;	// make some sense!
      }

    if (_retVal < _minVal)	// set a legal value for the top
	_curVal = _minVal;
    else if (_retVal > _maxVal)
	_curVal = _maxVal;
    else
	_curVal = _retVal;

    if (_isVert)		// set sizes for V or H
      {
	_h = ProgressHeight;
	_w = 10;
      }
    else
      {
	_h = 14;
	_w = ProgressHeight;
      }

    _parentWin->SetPosition(_x, _y, _w, _h, dlgCmd->cFrame, dlgCmd->cRightOf,
	dlgCmd->cBelow);
    _CtrlOffset = _parentWin->AddDlgControl(_x, _y, _w, _h, _cmdId,
	style, "BUTTON", _title, sizeof(vProgressCmd*), (LPBYTE)this);
  }

//==================>>> vProgressCmd::GetCmdValue <<<=========================
  int vProgressCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _curVal;
  }

//=====================>>> vProgressCmd::SetCmdVal <<<=========================
  void vProgressCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug2(Misc,"vProgressCmd::SetCmdVal(val:%d, type:%d)\n",val,st)
    HWND myHwnd = GetMyHwnd(_cmdId);
    if (st == Value)
      {
	if (val > _maxVal || val < _minVal )
	    return;
	_curVal = val;
	// Let Windows force the redraw
	::RedrawWindow(myHwnd,NULL,NULL,RDW_INVALIDATE);
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

//================>>> vProgressCmd::MEASUREITEM <<<============================
  int vProgressCmd::MEASUREITEM(int id, MEASUREITEMSTRUCT* mis)
  {
    mis->CtlType = ODT_BUTTON;
    mis->CtlID = id;
    mis->itemWidth = _w;
    mis->itemHeight = _h;

    return 1;
  }

//================>>> vProgressCmd::DRAWITEM <<<============================
  int vProgressCmd::DRAWITEM(int /*id*/, DRAWITEMSTRUCT* dis)
  {
    switch (dis->itemAction)
      {
	case ODA_DRAWENTIRE:	// redraw entire control
	  {
	    HBRUSH pBrush;
	    if (::GetSysColor(COLOR_SCROLLBAR) == ::GetSysColor(COLOR_BTNFACE))
		pBrush = ::CreateSolidBrush(RGB(128,128,128));
	    else
		pBrush = ::CreateSolidBrush(::GetSysColor(COLOR_SCROLLBAR));

	    HPEN oldpen = (HPEN) ::SelectObject(dis->hDC, GetStockObject(NULL_PEN));

	    HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(dis->hDC, pBrush));

	    (void)::Rectangle(dis->hDC,dis->rcItem.left, dis->rcItem.top,
		dis->rcItem.right, dis->rcItem.bottom);

	    ::SelectObject(dis->hDC, theApp->_ShadowPen);
	    ::MoveTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.bottom-1);
	    ::LineTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.top+1);
	    ::LineTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.top+1);
	   
	    ::SelectObject(dis->hDC,theApp->_LightPen);

	    ::LineTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.bottom-1);
	    ::LineTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.bottom-1);

	    // Now Draw the Progress part inside
	    ::SelectObject(dis->hDC, theApp->_LightPen);

	    ::SelectObject(dis->hDC, theApp->_BarBrush);

	    long screensize = (_isVert) ?
			    dis->rcItem.bottom - dis->rcItem.top - 2 :
			    dis->rcItem.right - dis->rcItem.left - 2;
	    long ctrlsize = _maxVal - _minVal;
	    int delta = (int) (screensize - ((screensize * (_curVal-_minVal)) / ctrlsize));

	    if (_curVal == _minVal)
		delta = screensize;
	    else if (_curVal == _maxVal)
		delta = 0;

	    RECT pr;
	    if (_isVert)
	      {
		pr.top = dis->rcItem.top + delta + 2;
		pr.right = dis->rcItem.right - 2;
		pr.bottom = dis->rcItem.bottom - 2;
		pr.left = dis->rcItem.left + 2;
	      }
	    else
	      {
		pr.top = dis->rcItem.top + 2;
		pr.right = dis->rcItem.right - 2 - delta;
		pr.bottom = dis->rcItem.bottom - 2;
		pr.left = dis->rcItem.left + 2;
	      }


            if (delta != screensize)	// clean look for 0
	      {
                (void)::Rectangle(dis->hDC,pr.left, pr.top,
		    pr.right, pr.bottom);
		::MoveTo(dis->hDC, pr.left+1, pr.bottom-1);
		::LineTo(dis->hDC, pr.left+1, pr.top+1);
		::LineTo(dis->hDC, pr.right-1, pr.top+1);

		::SelectObject(dis->hDC, theApp->_ShadowPen);
		if (_isVert)
		    ::LineTo(dis->hDC, pr.right-1, pr.bottom-1);
		else
		    ::MoveTo(dis->hDC, pr.right-1, pr.bottom-1);
		::LineTo(dis->hDC, pr.left+1, pr.bottom-1);
	      }

	    // Draw a Label now?
	    if (!(dlgCmd->attrs & CA_NoLabel))
	      {
                char buff[20];
		IntToStr(_curVal,buff);	// string representation
		if (dlgCmd->attrs & CA_Percent)
		    strcat(buff,"%");

		//::SetBkColor(dis->hDC, _color.pixel());
		int oldBkMode = ::GetBkMode(dis->hDC);
		::SetBkMode(dis->hDC,TRANSPARENT);
		::DrawText(dis->hDC, buff, strlen(buff),
		      &dis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                ::SetBkMode(dis->hDC,oldBkMode);
	      }

	    ::SelectObject(dis->hDC, oldpen);	// restore hdc
	    ::SelectObject(dis->hDC, oldbrush);
	    ::DeleteObject(pBrush);
	  }

	default:
	    break;
      }
    return 0;
  }
