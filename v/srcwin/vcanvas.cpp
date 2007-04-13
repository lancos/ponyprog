//===============================================================
// vCanvas - a scrolling window canvas for drawing
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================
//#include <stdio.h>

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vcanvas.h>		// our header
#include <v/vwindow.h>		// need to use some vwindow stuff


    VCursor vCanvasPane::_currentCursor = -1;	// for cursor

//================>>> vCanvasPane::vCanvasPane <<<========================
  vCanvasPane::vCanvasPane(PaneType pt) : vPane(pt)
  {
    SysDebug(Constructor,"vCanvasPane::vCanvasPane() constructor\n")

    _cpDC = 0;		// no pane DC
    _compressRedraw = 1;
    _currentCursor = VC_Arrow;
  }

//================>>> vCanvasPane::~vCanvasPane <<<========================
  vCanvasPane::~vCanvasPane()
  {
    SysDebug(Destructor,"vCanvasPane::~vCanvasPane() destructor\n")
    if (_cpDC != 0)
	delete _cpDC;	// free the DC
  }

//==================>>> vCanvasPane::initialize <<<==========================
  void vCanvasPane::initialize(vWindow* pWindow, HWND pWidget)
  {

    _HScrlShown = 100;
    _HScrlTop =  0;
    _VScrlShown = 100;
    _VScrlTop =  0;

    // now, build the menu items in the widget provided

    vPane::initialize(pWindow, pWidget);	// initialize base class

    pWindow->_canvasPane = this;		// easy access to/from window
    _drawWindow = pWindow->winHwnd();

    _hasFocus = 0;	// we don't start out with focus

    // set the height and width of our window

    if (theApp->simSDI())
      {
	_height = _parentWin->_WinHeight;
	_width = _parentWin->_WinWidth;
      }
    else
      {
	RECT rc;

	::GetClientRect(_drawWindow,&rc);	// area of the window

	_height = rc.bottom - rc.top;	// update the local stuff
	_width = rc.right - rc.left;
      }


    // After the drawing widget has been created, we can create
    // the V DC for it to use.

    CreateDC();

    _parentWin->_WinHeight = _height;
    _parentWin->_WinWidth = _width;

    SetHScroll(_HScrlShown, 0);	// initial values for scroll bar
    SetVScroll(_VScrlShown, 0);

    _HOn = _VOn = 0;			// both start out off
    _parentWin->SetWinCursor(_currentCursor);

    // Now need to fake out enter focus so the first mouse
    // click is ignored.
    if (!_hasFocus)
      {
	_hasFocus = 1;
	EnterFocus();		// call the virtual function
      }

    if (theApp->simSDI())
    	SetWidthHeight(_width, _height);
  }

//==================>>> vCanvasPane::CreateDC <<<========================
  void vCanvasPane::CreateDC(void)
  {
    if (!_cpDC)
	_cpDC = new vCanvasPaneDC(this);	// create a CanvasPaneDC
  }

//====================>>> vCanvasPane::ShowPane <<<======================
  void vCanvasPane::ShowPane(int OnOrOff) VCONST
  {
    if (OnOrOff)
      {
	::ShowWindow(_drawWindow, SW_RESTORE);
      }
    else
      {
	::ShowWindow(_drawWindow, SW_MINIMIZE);
      }
  }

//=====================>>> vCanvasPane::SetWidthHeight <<<=========================
  void vCanvasPane::SetWidthHeight(int width, int height)
  {
    if (height <= 0 || width <= 0)
	return;
    _parentWin->_WinHeight = _height = height;
    _parentWin->_WinWidth = _width = width;

    RECT rect;
    ::GetWindowRect(_drawWindow,&rect);		// rect of  canvas

    if (theApp->simSDI())
      {
        RECT winRect;
        int cW = rect.right - rect.left;	// Width of  canvas
    	int cH = rect.bottom - rect.top;	// height of canvas
    	::GetWindowRect(theApp->winHwnd(),&winRect);

    	int wW = winRect.right - winRect.left - cW;
    	int wH = winRect.bottom - winRect.top - cH;	// WH of  window

	::MoveWindow(theApp->winHwnd(),winRect.left,winRect.top, width+wW,
	   height + wH,1);
      }
    else
      {
        RECT ar;
    	::GetWindowRect(theApp->winClientHwnd(), &ar);
        int left = (rect.left >= ar.left) ? rect.left - ar.left : rect.left;
        int top = (rect.top >= ar.top) ? rect.top - ar.top : rect.top;
	::MoveWindow(_drawWindow,left, top, width, height,1);
      }
  }


// ************************************************************************

//------------------------- cursor Stuff ---------------------------------

// ************************************************************************

//==================>>> vCanvasPane::SetCursor <<<============================
  void vCanvasPane::SetCursor(VCursor id)
  {
    // Set to current cursor

    if (id < VC_None || id > VC_LAST)
      {
	SysDebug1(BadVals,"vCanvasPane::SetCursor(id=%d)\n",id)
	return;
      }

    if (id == VC_None)		// special case
	UnSetCursor();

    SysDebug1(WindowEvents,"vCanvasPane::SetCursor(id=%d)%d\n",id)

    _currentCursor = id;
    _parentWin->SetWinCursor(_currentCursor);
  }

//===================>>> vCanvasPane::UnSetCursor <<<=========================
  void vCanvasPane::UnSetCursor(void)
  {
    _currentCursor = CA_None;
  }

// ************************************************************************

//----------------------- Scrolling Stuff ---------------------------------

// ************************************************************************

//=======================>>> vCanvasPane::HPage <<<============================
  void vCanvasPane::HPage(int Shown, int Top)
  {
    // This is the main way a user app gets scrolling events, and
    // will usually override this guy to handle what is actually
    // drawn in the canvas window.
    // This should then be called to actually change the display.

    SysDebug2(WindowEvents,"vCanvasPane::HPage(Shown: %d, Top: %d)\n",Shown, Top);

    SetHScroll(Shown,Top);
  }

//=======================>>> vCanvasPane::HScroll <<<==========================
  void vCanvasPane::HScroll(int step)
  {
    // This is the way the user app gets step events - either page or
    // single step events. The default doesn't have to do anything.

    SysDebug1(WindowEvents,"vCanvasPane::HScroll(%d)\n",step);

    if (step < 0)
      {
	if (_HScrlTop - 1 >= 0)
	    SetHScroll(_HScrlShown, --_HScrlTop);
      }
    else
      {
	if (_HScrlTop + 1 <= 100)
	    SetHScroll(_HScrlShown, ++_HScrlTop);
      }
  }

//=====================>>> vCanvasPane::ShowHScroll <<<=========================
  void vCanvasPane::ShowHScroll(int OnOff)
  {
    if (OnOff)			// Turn on
      {
	if (_HOn)		// Already on
	    return;
	_HOn = 1;
	::ShowScrollBar(_drawWindow, SB_HORZ, 1);
	::SetScrollRange(_drawWindow,SB_HORZ, 0, 100, 0); // set range to 0-100
      }
    else			// Turn off
      {
	if (!_HOn)		// Already off
	    return;
	_HOn = 0;
	::ShowScrollBar(_drawWindow, SB_HORZ, 0);
      }
  }

//=====================>>> vCanvasPane::ShowVScroll <<<=========================
  void vCanvasPane::ShowVScroll(int OnOff)
  {
    if (OnOff)			// Turn on
      {
	if (_VOn)		// Already on
	    return;
	_VOn = 1;
	::ShowScrollBar(_drawWindow, SB_VERT, 1);
	::SetScrollRange(_drawWindow,SB_VERT, 0, 100, 0);
      }
    else			// Turn off
      {
	if (!_VOn)		// Already off
	    return;
	_VOn = 0;
	::ShowScrollBar(_drawWindow, SB_VERT, 0);
      }

  }

//=====================>>> vCanvasPane::SetHScroll <<<=========================
  void vCanvasPane::SetHScroll(int Shown, int Top)
  {
    // Make sure we are setting to valid values, and are changing things!

    if (Shown < 0 || Shown > 100 || Top < 0 || Top > 100)
	return;

    _HScrlShown = Shown;
    _HScrlTop = Top;
#ifndef VWIN16
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS | SIF_PAGE;
    si.nPage = _HScrlShown;
    si.nPos = _HScrlTop;
    ::SetScrollInfo(_drawWindow, SB_HORZ, &si, 1);
#else
    ::SetScrollPos(_drawWindow, SB_HORZ, _HScrlTop, 1);	// Windows just has top
#endif
  }

//========================>>> vCanvasPane::VPage <<<===========================
  void vCanvasPane::VPage(int Shown, int Top)
  {

    SysDebug2(WindowEvents,"vCanvasPane::VPage(Shown: %d, Top: %d)\n",Shown, Top);

    SetVScroll(Shown,Top);

  }

//========================>>> vCanvasPane::VScroll <<<=========================
  void vCanvasPane::VScroll(int step)
  {
    // This is the way the user app gets step events - either page or
    // single step events. Default updates the display

    SysDebug1(WindowEvents,"vCanvasPane::VScroll(%d)\n",step);
    if (step < 0)
      {
	if (_VScrlTop - 1 >= 0)
	    SetVScroll(_VScrlShown, --_VScrlTop);
      }
    else
      {
	if (_VScrlTop + 1 <= 100)
	    SetVScroll(_VScrlShown, ++_VScrlTop);
      }

  }

//=====================>>> vCanvasPane::GetVScroll <<<=========================
  int vCanvasPane::GetVScroll(int& Shown, int& Top) VCONST	// V:1.13
  {
    Shown = _VScrlShown; Top = _VScrlTop;
    return _VOn;
  }

//=====================>>> vCanvasPane::GetHScroll <<<=========================
  int vCanvasPane::GetHScroll(int& Shown, int& Top) VCONST	// V:1.13
  {
    Shown = _HScrlShown; Top = _HScrlTop;
    return _HOn;
  }

//=====================>>> vCanvasPane::SetVScroll <<<=========================
  void vCanvasPane::SetVScroll(int Shown, int Top)
  {
    // Make sure we are setting to valid values, and are changing things!

    if (Shown < 0 || Shown > 100 || Top < 0 || Top > 100)
	return;

    _VScrlShown = Shown;
    _VScrlTop = Top;
#ifndef VWIN16
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS | SIF_PAGE;
    si.nPage = _VScrlShown;
    si.nPos = _VScrlTop;
    ::SetScrollInfo(_drawWindow, SB_VERT, &si, 1);
#else
    ::SetScrollPos(_drawWindow, SB_VERT, _VScrlTop, 1);	// Windows just has top
#endif
  }

//====================>>> vCanvasPane::HScrollEV <<<=======================
  void vCanvasPane::HScrollEV(int code, int pos, HWND /* control */)
  {

    switch (code)
      {
	case SB_BOTTOM:			// Scroll to bottom
	    _HScrlTop = 100;
	    HPage(_HScrlShown, 100);
	    break;

	case SB_LINEDOWN:		// Down one line
	    HScroll(1);
	    break;

	case SB_LINEUP:			// Up one line
	    HScroll(-1);
	    break;

	case SB_PAGEDOWN:		// Page Down - use 10
	    if (_HScrlTop <= 90)
	      {
		_HScrlTop += 10;
		HPage(_HScrlShown, _HScrlTop);
	      }
	    else
	      {
		_HScrlTop = 100;
		HPage(_HScrlShown, 100);
	      }
	    break;

	case SB_PAGEUP:			// Page Up - use -10
	    if (_HScrlTop >= 10)
	      {
		_HScrlTop -= 10;
		HPage(_HScrlShown, _HScrlTop);
	      }
	    else
	      {
		_HScrlTop = 0;
		HPage(_HScrlShown, 0);
	      }
	    break;

	case SB_THUMBPOSITION:		// move to position given
	    _HScrlTop = pos;
	    HPage(_HScrlShown, pos);
	    break;

	case SB_THUMBTRACK:		// for continuous motion - ignore
	    break;

	case SB_TOP:			// top of scroll
	    _HScrlTop = 0;
	    HPage(_HScrlShown, 0);
	    break;
      }
  }

//====================>>> vCanvasPane::VScrollEV <<<=======================
  void vCanvasPane::VScrollEV(int code, int pos, HWND /*control */)
  {
    switch (code)
      {
	case SB_BOTTOM:			// Scroll to bottom
	    _VScrlTop = 100;
	    VPage(_VScrlShown, 100);
	    break;

	case SB_LINEDOWN:		// Down one line
	    VScroll(1);
	    break;

	case SB_LINEUP:			// Up one line
	    VScroll(-1);
	    break;

	case SB_PAGEDOWN:		// Page Down - use 10
	    if (_VScrlTop <= 90)
	      {
		_VScrlTop += 10;
		VPage(_VScrlShown, _VScrlTop);
	      }
	    else
	      {
		_VScrlTop = 100;
		VPage(_VScrlShown, 100);
	      }
	    break;

	case SB_PAGEUP:			// Page Up - use -10
	    if (_VScrlTop >= 10)
	      {
		_VScrlTop -= 10;
		VPage(_VScrlShown, _VScrlTop);
	      }
	    else
	      {
		_VScrlTop = 0;
		VPage(_VScrlShown, 0);
	      }
	    break;

	case SB_THUMBPOSITION:		// move to position given
	    _VScrlTop = pos;
	    VPage(_VScrlShown, pos);
	    break;

	case SB_THUMBTRACK:		// continuous, ignore
	    break;

	case SB_TOP:
	    _VScrlTop = 0;			// top of scroll
	    VPage(_VScrlShown, 0);
	    break;
      }
  }

// ************************************************************************
//
// Redraw/resize/focus
//
// ************************************************************************


//=====================>>> vCanvasPane::ExposeEV <<<==========================
  void vCanvasPane::ExposeEV(void)
  {
    int x = 0, y = 0, height = 0, width = 0;
    RECT rect;
    if (::GetUpdateRect(_drawWindow, &rect, 0))
      {
  	x = rect.left ; y = rect.top;
  	height = rect.bottom - rect.top;
  	width = rect.right - rect.left;
     	_cpDC->ClearRect(x, y, width, height);
      }

    Redraw(x, y , width, height);
  }

//=====================>>> vCanvasPane::EnterFocus <<<========================
  void vCanvasPane::EnterFocus(void)
  {
    SysDebug(WindowEvents,"vCanvasPane::EnterFocus()\n")
  }

//=====================>>> vCanvasPane::EnterEV <<<==========================
  void vCanvasPane::EnterEV(void)
  {
    if (_hasFocus)		// don't double enter focus
	return;
    _hasFocus = 1;

    EnterFocus();		// call the virtual function
  }

//======================>>> vCanvasPane::FontChanged <<<===========================
  void vCanvasPane::FontChanged(VCONST vFont& vf)
  {
    // Called when the font is changed.

    SysDebug1(WindowEvents,"vCanvasPane::FontChanged(%d)\n",vf)
  }

//=====================>>> vCanvasPane::LeaveFocus <<<=======================
  void vCanvasPane::LeaveFocus()
  {
    SysDebug(WindowEvents,"vCanvasPane::LeaveFocus()\n")
  }

//=====================>>> vCanvasPane::LeaveEV <<<==========================
  void vCanvasPane::LeaveEV()
  {
    if (!_hasFocus)		// don't double leave focus
	return;
    _hasFocus = 0;

    LeaveFocus();		// call the virtual function
  }

//=====================>>> vCanvasPane::MouseDown <<<==========================
  void vCanvasPane::MouseDown(int x, int y, int button)
  {
    // Mouse events in vCanvasPane are no-ops.

#ifdef vDEBUG		// Don't have a SysDebug4 macro, so do it by hand
    char msg[10];
    sprintf(msg,"Button = %d",button);
#endif

    SysDebug3(MouseEvents,"vCanvasPane::MouseDown(x:%d,y:%d,btn:%d)\n",x,y,button)
  }

//=====================>>> vCanvasPane::MouseUp <<<============================
  void vCanvasPane::MouseUp(int x, int y, int button)
  {
    // Mouse events in vCanvasPane are no-ops.

    SysDebug3(MouseEvents,"vCanvasPane::MouseUp(x:%d,y:%d,btn:%d)\n",x,y,button)
  }

//=====================>>> vCanvasPane::MouseMove <<<==========================
  void vCanvasPane::MouseMove(int x, int y, int button)
  {
    // Mouse events in vCanvasPane are no-ops.

    SysDebug3(MouseEvents,"vCanvasPane::MouseMove(x:%d,y:%d,btn:%d)\n",x,y,button)
  }

//=======================>>> vCanvasPane::Redraw <<<==========================
  void vCanvasPane::Redraw(int x, int y, int width, int height)
  {
    // Redraw in vCanvasPane is a no-op.

#ifdef vDEBUG		// Don't have a SysDebug4 macro, so do it by hand
    if (DebugState.WindowEvents && DebugState.System)
	fprintf(stderr,"vCanvasPane::Redraw(x=%d, y=%d, w=%d, h=%d)\n",x,y,width,height);
#endif

  }

//=====================>>> vCanvasPane::Resize <<<============================
  void vCanvasPane::Resize(int newW, int newH)
  {
    // This is the routine the user will override to intercept size changes

    SysDebug2(WindowEvents,"vCanvasPane::Resize(newH:%d, newW:%d)\n",newH,newW)

  }

//==================>>> vCanvasPane::ResizeEV <<<========================
  void vCanvasPane::ResizeEV(int w, int h)
  {
    _parentWin->_WinHeight = _height = h;
    _parentWin->_WinWidth = _width = w;

    Resize((int)w, (int)h);	// Call the virtual function
  }
