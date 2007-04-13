//===============================================================
// vbtncmd.cpp - button Cmd - Windows
//
// Copyright (C) 1995-1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vbtncmd.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vutil.h>
#include <v/vicon.h>
#include <v/vapp.h>

//====================>>> vButtonCmd::vButtonCmd <<<=======================
  vButtonCmd::vButtonCmd(vCmdParent* dp, CommandObject* dc) :
    vCmd(dp, dc)
  {
    initialize();
  }

//====================>>> vButtonCmd::~vButtonCmd <<<=======================
  vButtonCmd::~vButtonCmd()
  {

    SysDebug(Destructor,"vButtonCmd::~vButtonCmd() destructor\n")

  }

//=====================>>> vButtonCmd::initialize <<<=======================
  void vButtonCmd::initialize()
  {
    // build a button command for use in a parent window

    SysDebug(Constructor,"vButtonCmd::vButtonCmd() constructor\n")

    long style;

    if (_parentWin->_dialogType != aCmdBar)
	style = WS_TABSTOP | WS_GROUP;	// default for a button
    else
	style = WS_GROUP;

    CopyToLocal();			// Make local copies of CmdObject

    if (!(dlgCmd->attrs & CA_Hidden))	// Check for Hidden
	style |= WS_VISIBLE;

    if ((dlgCmd->cmdType == C_IconButton || dlgCmd->cmdType == C_ToggleIconButton)
        && _itemList != 0) // icon
      {
	initIconButton(style);		// Special init if icon
	return;
     }

    if (dlgCmd->cmdType == C_ColorButton) // need owner draw if color
	style |= BS_OWNERDRAW;
    else if (dlgCmd->cmdType == C_ToggleButton) // need owner draw if toggle
	style |= BS_OWNERDRAW;
    else if ((dlgCmd->attrs & CA_DefaultButton))   // default button?
	style |= BS_DEFPUSHBUTTON;	// Only regular buttons can be default
    else
	style |= BS_PUSHBUTTON;		// Plain old button

    _w = vLblLen(_title)*4+10;		// set my width
    _h = 14;				// default height

    if ((dlgCmd->cmdType == C_Button || dlgCmd->cmdType == C_ToggleButton)
        && dlgCmd->size > 0) // size - width
      {
	int w = vLblLen(_title)*4+10;
	if (w < dlgCmd->size)
	  {
            _w = dlgCmd->size;
	  }
      }
    else if (dlgCmd->size > 0)	// size - square (icons, color buttons)
      {
	_w = _h = dlgCmd->size;
      }

    _parentWin->SetPosition(_x, _y, _w, _h, dlgCmd->cFrame, dlgCmd->cRightOf,
	dlgCmd->cBelow);
    _CtrlOffset = _parentWin->AddDlgControl(_x, _y, _w, _h, _cmdId,
	style, "BUTTON", _title, sizeof(vButtonCmd*), (LPBYTE)this);

  }

//=====================>>> vButtonCmd::initIconButton <<<====================
  void vButtonCmd::initIconButton(long style)
  {
    _ip = (vIcon *) _itemList;	// to access bitmap

    style |= BS_OWNERDRAW;

    _h = (_ip->height / 2) + 5;
    _w = (_ip->width / 2) + 5;

    _parentWin->SetPosition(_x, _y, _w, _h, dlgCmd->cFrame,
	     dlgCmd->cRightOf, dlgCmd->cBelow);

    _CtrlOffset = _parentWin->AddDlgControl(_x, _y , _w, _h, _cmdId,
	   style, "BUTTON", "", sizeof(vButtonCmd*), (LPBYTE)this);

  }

//==================>>> vButtonCmd::GetCmdValue <<<=========================
  int vButtonCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _retVal;
  }

//================>>> vButtonCmd::ResetItemValue <<<======================
  void vButtonCmd::ResetItemValue(void)
  {

    // We have to toggle things
    if (dlgCmd->cmdType != C_ToggleIconButton &&
      dlgCmd->cmdType != C_ToggleButton)
	return;

    if (_retVal == _origVal)	// No op if no change
	return;

    HWND myHwnd = GetMyHwnd(_cmdId);
    _retVal = _origVal;		// restore

    ::ShowWindow(myHwnd,SW_HIDE); // trick into
    ::ShowWindow(myHwnd,SW_SHOW); // repaint the window

    // let parent window handle now
    _parentWin->ProcessCmd(_cmdId, _retVal, dlgCmd->cmdType);
  }


//================>>> vButtonCmd::SetCmdVal <<<============================
  void vButtonCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vButtonCmd::SetCmdVal(val:%d)\n",val)
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
	::ShowWindow(myHwnd,SW_HIDE);	// this tricks it into always
	::ShowWindow(myHwnd,SW_SHOW);   // repainting.
      }
  
  }

//================>>> vButtonCmd::SetCmdStr <<<============================
  void vButtonCmd::SetCmdStr(VCONST char* str)
  {
    // Set the string value of button

    SysDebug1(Misc,"vButtonCmd::SetCmdStr(str:%s)\n",str)
    ::SetDlgItemText(_parentWin->getParent(),_cmdId, str);
    _title = str;

  }

//================>>> vButtonCmd::MEASUREITEM <<<============================
  int vButtonCmd::MEASUREITEM(int id, MEASUREITEMSTRUCT* mis)
  {
    mis->CtlType = ODT_BUTTON;
    mis->CtlID = id;
    mis->itemWidth = _w;
    mis->itemHeight = _h;
    
    return 1;
  }

//================>>> vButtonCmd::DRAWITEM <<<============================
  int vButtonCmd::DRAWITEM(int /* id */, DRAWITEMSTRUCT* dis)
  {

    HBITMAP hbm;
    switch (dis->itemAction)
      {
	case ODA_DRAWENTIRE:	// redraw entire control
	  {
	DRAW_NORMAL:
	    if (dlgCmd->cmdType == C_IconButton ||
		dlgCmd->cmdType == C_ToggleIconButton)  //Draw the icon
	      {
		POINT ptSize, ptOrg;

		// Draw the button rectangle in LTGRAY with a BLACK border

		HPEN oldpen = (HPEN) ::SelectObject(dis->hDC, ::GetStockObject(BLACK_PEN));
		HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(dis->hDC, theApp->_BarBrush));
		if (!(dlgCmd->attrs & CA_Flat))	// Check for flat
		    (void)::Rectangle(dis->hDC,dis->rcItem.left, dis->rcItem.top,
			dis->rcItem.right, dis->rcItem.bottom);

		// Draw the shadows - WHITE left, top, GRAY right, bottom
		if (dlgCmd->cmdType == C_IconButton)
		    ::SelectObject(dis->hDC, theApp->_LightPen);
		else if (_retVal == 0)
		    ::SelectObject(dis->hDC, theApp->_LightPen);
		else
		    ::SelectObject(dis->hDC,theApp->_ShadowPen);

		if (!(dlgCmd->attrs & CA_Flat))	// Check for flat
		  {
		    ::MoveTo(dis->hDC, dis->rcItem.left, dis->rcItem.bottom);
		    ::LineTo(dis->hDC, dis->rcItem.left, dis->rcItem.top);
		    ::LineTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.top);
		    ::MoveTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.bottom+1);
		    ::LineTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.top+1);
		    ::LineTo(dis->hDC, dis->rcItem.right-2, dis->rcItem.top+1);
		  }

		if (dlgCmd->cmdType == C_IconButton || _retVal == 0)
		    ::SelectObject(dis->hDC,theApp->_ShadowPen);
		else
		    ::SelectObject(dis->hDC, theApp->_LightPen);

		if (!(dlgCmd->attrs & CA_Flat))	// Check for flat
		  {
		    ::MoveTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.top+1);
		    ::LineTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.bottom-1);
		    ::LineTo(dis->hDC, dis->rcItem.left, dis->rcItem.bottom-1);
		    ::MoveTo(dis->hDC, dis->rcItem.right-2, dis->rcItem.top+1);
		    ::LineTo(dis->hDC, dis->rcItem.right-2, dis->rcItem.bottom-2);
		    ::LineTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.bottom-2);
		  }

		// Get color to draw icon - GRAY or BLACK
		HBRUSH dbrush = 0;
		if (dis->itemState & ODS_DISABLED)
		  {
		    // This stuff needed to be sure it shows
		    if (::GetSysColor(COLOR_BTNSHADOW) == ::GetSysColor(COLOR_BTNFACE))
		      {
			::SelectObject(dis->hDC, ::GetStockObject(GRAY_BRUSH));
		      }
		    else
		      {
			dbrush = ::CreateSolidBrush(::GetSysColor(COLOR_BTNSHADOW));
			::SelectObject(dis->hDC, dbrush);
		      }
		  }
		else		// ODS_SELECTED or ODS_FOCUS - draw 2 ways!
		  {
		    if (_ip->iType == Transparent)
		      {
			// Prepare for BitBlt with raster operation of PSDPxax, i.e.
			// paint with the brush where the color of the pixel on the
			// destination bitmap is zero
			dbrush = ::CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
			::SelectObject(dis->hDC, dbrush);
		      }
		    else
		      {
			::SelectObject(dis->hDC, ::GetStockObject(BLACK_BRUSH));
		      }
		  }

		// Now setup coordinate to draw icon

		HDC hdcMem = ::CreateCompatibleDC(dis->hDC);
		::SetMapMode(hdcMem, GetMapMode(dis->hDC));
		hbm = _ip->GetIconHBM(dis->hDC);	// get icon HBITMAP
		HBITMAP oldbm = static_cast<HBITMAP>(::SelectObject(hdcMem,hbm));
		ptSize.x = _ip->width;
		ptSize.y = _ip->height;
		::DPtoLP(dis->hDC, &ptSize, 1);
		ptOrg.x = 0;
		ptOrg.y = 0;
		::DPtoLP(hdcMem, &ptOrg, 1);

                int l = (dis->rcItem.right - _ip->width) / 2;
		int t = (dis->rcItem.bottom - _ip->height) / 2;

                // Now BitBlt the icon BMP into the drawing canvas, leaving
		// background untouched, and the pen color to draw 1 bits
		// (that's what the E20746L does) for monochrome

		if (_ip->depth == 1)
		  {
		    ::SetBkColor(dis->hDC, RGB(255,255,255)); // white bg for icon
                    ::BitBlt(dis->hDC, l, t, ptSize.x,
			ptSize.y, hdcMem, ptOrg.x, ptOrg.y, 0xE20746L);
		  }
		else
		  {
                    ::BitBlt(dis->hDC, l, t, ptSize.x,
		    ptSize.y, hdcMem, ptOrg.x, ptOrg.y, SRCCOPY);
		    if (_ip->iType == Transparent)
		      {
			// Now restore area from the button where the mask is zero
			HDC hdcMemMask = ::CreateCompatibleDC( dis->hDC );
			HBITMAP hmsk = static_cast<HBITMAP>(_ip->GetIconMask( dis->hDC)); // get icon mask HBITMAP
			HBITMAP oldBM = static_cast<HBITMAP>(::SelectObject( hdcMemMask, hmsk ));
			::BitBlt( dis->hDC, l, t, ptSize.x,
			ptSize.y, hdcMemMask, ptOrg.x, ptOrg.y, 0xB8074A); //PSDPxax
			::SelectObject( hdcMemMask, oldBM );
			::DeleteDC( hdcMemMask );
		      }
		  }
                ::SelectObject(hdcMem,oldbm);
		::DeleteDC(hdcMem);
		::SelectObject(dis->hDC, oldpen);	// restore hdc
		::SelectObject(dis->hDC, oldbrush);
		if (dbrush) ::DeleteObject(dbrush);
	      }
	    else if (dlgCmd->cmdType == C_ToggleButton)
	      {
		// Draw a toggle button

		HPEN pen;
		if (dis->itemState & ODS_DISABLED)
		    pen = (HPEN) ::GetStockObject(WHITE_PEN);
		else
		    pen = (HPEN) ::GetStockObject(BLACK_PEN);

		// Set to the new brush color, remember the old one

		HPEN oldpen = (HPEN)::SelectObject(dis->hDC, pen);

		// Draw the button with a black border

		if (!(dlgCmd->attrs & CA_Flat))	// Check for flat
		    (void)::Rectangle(dis->hDC,dis->rcItem.left, dis->rcItem.top,
			dis->rcItem.right, dis->rcItem.bottom);

		// Draw text for the Toggle button if it has it

		if (*_title )
		  {
		    int tl = strlen(_title);  		// This little kludge
		    int tblanks = 0;                        // prevents drawing
		    for (VCONST char* tp = _title ; *tp ; ++tp)    // all-blank titles
		      {                                     // which can leave
			if (*tp == ' ')                     // an ugly "pure color"
			    ++tblanks;                      // block in the middle
		      }                                     // of the button.
		    if (tblanks != tl)
		      {
			RECT rc = dis->rcItem;
			if (_retVal)
			  {
			    rc.left += 2;    rc.top += 2;	// shift text
			  }
			::DrawText(dis->hDC, _title, strlen(_title),
			  &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		      }
		  }

		// Now draw the shadow around the button
		if (_retVal == 0)
		    ::SelectObject(dis->hDC, theApp->_LightPen);
		else
		    ::SelectObject(dis->hDC,theApp->_ShadowPen);

		if (!(dlgCmd->attrs & CA_Flat))	// Check for flat
		  {
		    ::MoveTo(dis->hDC, dis->rcItem.left, dis->rcItem.bottom);
		    ::LineTo(dis->hDC, dis->rcItem.left, dis->rcItem.top);
		    ::LineTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.top);
		    ::MoveTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.bottom+1);
		    ::LineTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.top+1);
		    ::LineTo(dis->hDC, dis->rcItem.right-2, dis->rcItem.top+1);
		  }


		if (_retVal == 0)
                    ::SelectObject(dis->hDC,theApp->_ShadowPen);
		else
		    ::SelectObject(dis->hDC, theApp->_LightPen);

		if (!(dlgCmd->attrs & CA_Flat))	// Check for flat
		  {
		    ::MoveTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.top+1);
		    ::LineTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.bottom-1);
		    ::LineTo(dis->hDC, dis->rcItem.left, dis->rcItem.bottom-1);
		    ::MoveTo(dis->hDC, dis->rcItem.right-2, dis->rcItem.top+1);
		    ::LineTo(dis->hDC, dis->rcItem.right-2, dis->rcItem.bottom-2);
		    ::LineTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.bottom-2);
		  }

		::SelectObject(dis->hDC, oldpen);	// restore hdc
	      }

	    return 1;
	  }

	case ODA_SELECT:	// indicated selected
	  {
	    if (dlgCmd->cmdType == C_IconButton ||
		dlgCmd->cmdType == C_ToggleIconButton)
	      {
		if (dis->itemState & ODS_SELECTED)  //Draw the icon
		  {
		    POINT ptSize, ptOrg;

		    // Draw the button rectangle in LTGRAY with a BLACK border
		    HBRUSH fbrush = ::CreateSolidBrush(
				::GetSysColor(COLOR_BTNFACE));
		    HPEN oldpen = (HPEN) ::SelectObject(dis->hDC,
			::GetStockObject(BLACK_PEN));
		    HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(dis->hDC, fbrush));
		    if (!(dlgCmd->attrs & CA_Flat))	// Check for flat
			(void)::Rectangle(dis->hDC,dis->rcItem.left,
		           dis->rcItem.top, dis->rcItem.right, dis->rcItem.bottom);


		     // select pen, even if we don't draw...
		     ::SelectObject(dis->hDC, theApp->_ShadowPen);
		     if (!(dlgCmd->attrs & CA_Flat))	// Check for flat
		        {
			  // Draw the shadows - GRAY left, top, No right, bottom
			  ::MoveTo(dis->hDC, dis->rcItem.left, dis->rcItem.bottom);
			  ::LineTo(dis->hDC, dis->rcItem.left, dis->rcItem.top);
			  ::LineTo(dis->hDC, dis->rcItem.right, dis->rcItem.top);
			  ::MoveTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.bottom-1);
			  ::LineTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.top+1);
			  ::LineTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.top+1);
			}

		    // Now setup coordinate to draw icon

		    HDC hdcMem = ::CreateCompatibleDC(dis->hDC);
		    ::SetMapMode(hdcMem, GetMapMode(dis->hDC));
		    hbm = _ip->GetIconHBM(hdcMem);	// get icon HBITMAP
		    HBITMAP oldbm = static_cast<HBITMAP>(::SelectObject(hdcMem,hbm));
		    ptSize.x = _ip->width;
		    ptSize.y = _ip->height;
		    DPtoLP(dis->hDC, &ptSize, 1);
		    ptOrg.x = 0;
		    ptOrg.y = 0;
		    DPtoLP(hdcMem, &ptOrg, 1);
		    ::SetBkColor(dis->hDC, RGB(255,255,255)); // white bg for icon
		    ::SelectObject(dis->hDC, ::GetStockObject(BLACK_BRUSH));

		    // Now BitBlt the icon BMP into the drawing canvas, leaving
		    // background untouched, and the pen color to draw 1 bits
		    // (that's what the E20746L does) (shifted 1 from normal icon)

		    int l = (dis->rcItem.right - _ip->width) / 2;
		    int t = (dis->rcItem.bottom - _ip->height) / 2;
		    ::BitBlt(dis->hDC, l+1, t+1, ptSize.x,
		        ptSize.y, hdcMem, ptOrg.x, ptOrg.y, 0xE20746L);
                    ::SelectObject(hdcMem,oldbm);
		    ::DeleteDC(hdcMem);
		    ::SelectObject(dis->hDC, oldpen);	// restore hdc
		    ::SelectObject(dis->hDC, oldbrush);
		    ::DeleteObject(fbrush);
		    return 1;
		  }
		else
		  {
		    goto DRAW_NORMAL;
		  }
	      }
	    else if (dlgCmd->cmdType == C_ToggleButton)
	      {
		// nearly the same as regular, but don't worry about
		// insensitive and draw inverted shadow

		if (dis->itemState & ODS_SELECTED)  //Draw the icon
		  {
                    // Draw the button rectangle in LTGRAY with a BLACK border
		    HBRUSH fbrush = ::CreateSolidBrush(
				::GetSysColor(COLOR_BTNFACE));
		    HPEN oldpen = (HPEN) ::SelectObject(dis->hDC,
			::GetStockObject(BLACK_PEN));
		    HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(dis->hDC, fbrush));
		    if (!(dlgCmd->attrs & CA_Flat))	// Check for flat
		        (void)::Rectangle(dis->hDC,dis->rcItem.left,
		          dis->rcItem.top, dis->rcItem.right, dis->rcItem.bottom);

		    if (*_title)
		      {
			RECT rc = dis->rcItem;
			rc.left += 2;    rc.top += 2;	// shift text
			::DrawText(dis->hDC, _title, strlen(_title),
			  &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		      }

		    ::SelectObject(dis->hDC, theApp->_ShadowPen);
		    if (!(dlgCmd->attrs & CA_Flat))	// Don't draw if flat
		      {
			::MoveTo(dis->hDC, dis->rcItem.left, dis->rcItem.bottom);
			::LineTo(dis->hDC, dis->rcItem.left, dis->rcItem.top);
			::LineTo(dis->hDC, dis->rcItem.right, dis->rcItem.top);
			::MoveTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.bottom+1);
			::LineTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.top+1);
			::LineTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.top+1);
		      }
		    ::SelectObject(dis->hDC, oldpen);	// restore hdc
		    ::SelectObject(dis->hDC, oldbrush);
		    ::DeleteObject(fbrush);
		    return 1;
		  }
		else
		  {
		    goto DRAW_NORMAL;
		  }
	      }
	    return 1;		// no op for icon label
	  }

	case ODA_FOCUS:		// gets focus
	  {
	    // need to draw something if CA_Flat
//		::MessageBeep(MB_OK);
		return 1;		// no op for icon label
	  }
	default:
	    break;
      }
    return 0;
  }

//====================>>> vButtondCmd::CmdCallback <<<=======================
  void vButtonCmd::CmdCallback(int /* id */, WORD /* codeNotify */)
  {

    if (dlgCmd->cmdType == C_ToggleIconButton ||	// depends on state
        dlgCmd->cmdType == C_ToggleButton)
      {
	SetCmdVal(!_retVal, Checked);
      }
    
    // let parent window handle now
    _parentWin->ProcessCmd(_cmdId, _retVal, dlgCmd->cmdType);
  }
