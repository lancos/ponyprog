//===============================================================
// vLayoutPane - a layout window pane for canvases
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


    VCursor vLayoutPane::_currentCursor = -1;	// for cursor

//================>>> vLayoutPane::vLayoutPane <<<========================
  vLayoutPane::vLayoutPane(PaneType pt) : vPane(pt)
  {
    SysDebug(Constructor,"vLayoutPane::vLayoutPane() constructor\n")

    _cpDC = 0;		// no pane DC
    _compressRedraw = 1;
    _currentCursor = VC_Arrow;
  }

//================>>> vLayoutPane::~vLayoutPane <<<========================
  vLayoutPane::~vLayoutPane()
  {
    SysDebug(Destructor,"vLayoutPane::~vLayoutPane() destructor\n")
    if (_cpDC != 0)
	delete _cpDC;	// free the DC
  }

//==================>>> vLayoutPane::initialize <<<==========================
  void vLayoutPane::initialize(vWindow* pWindow, HWND pWidget)
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


    _parentWin->_WinHeight = _height;
    _parentWin->_WinWidth = _width;


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


//====================>>> vLayoutPane::ShowPane <<<======================
  void vLayoutPane::ShowPane(int OnOrOff)
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

//=====================>>> vLayoutPane::SetWidthHeight <<<=========================
  void vLayoutPane::SetWidthHeight(int width, int height)
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

//==================>>> vLayoutPane::SetCursor <<<============================
  void vLayoutPane::SetCursor(VCursor id)
  {
    // Set to current cursor

    if (id < VC_None || id > VC_LAST)
      {
	SysDebug1(BadVals,"vLayoutPane::SetCursor(id=%d)\n",id)
	return;
      }

    if (id == VC_None)		// special case
	UnSetCursor();

    SysDebug1(WindowEvents,"vLayoutPane::SetCursor(id=%d)%d\n",id)

    _currentCursor = id;
    _parentWin->SetWinCursor(_currentCursor);
  }

//===================>>> vLayoutPane::UnSetCursor <<<=========================
  void vLayoutPane::UnSetCursor(void)
  {
    _currentCursor = CA_None;
  }

// ************************************************************************

//----------------------- Scrolling Stuff ---------------------------------

// ************************************************************************

// ************************************************************************
//
// Redraw/resize/focus
//
// ************************************************************************


//=====================>>> vLayoutPane::ExposeEV <<<==========================
  void vLayoutPane::ExposeEV(void)
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

//=====================>>> vLayoutPane::EnterFocus <<<========================
  void vLayoutPane::EnterFocus(void)
  {
    SysDebug(WindowEvents,"vLayoutPane::EnterFocus()\n")
  }

//=====================>>> vLayoutPane::EnterEV <<<==========================
  void vLayoutPane::EnterEV(void)
  {
    if (_hasFocus)		// don't double enter focus
	return;
    _hasFocus = 1;

    EnterFocus();		// call the virtual function
  }

//=====================>>> vLayoutPane::LeaveFocus <<<=======================
  void vLayoutPane::LeaveFocus()
  {
    SysDebug(WindowEvents,"vLayoutPane::LeaveFocus()\n")
  }

//=====================>>> vLayoutPane::LeaveEV <<<==========================
  void vLayoutPane::LeaveEV()
  {
    if (!_hasFocus)		// don't double leave focus
	return;
    _hasFocus = 0;

    LeaveFocus();		// call the virtual function
  }


//=======================>>> vLayoutPane::Redraw <<<==========================
  void vLayoutPane::Redraw(int x, int y, int width, int height)
  {
    // Redraw in vLayoutPane is a no-op.

#ifdef vDEBUG		// Don't have a SysDebug4 macro, so do it by hand
    if (DebugState.WindowEvents && DebugState.System)
	fprintf(stderr,"vLayoutPane::Redraw(x=%d, y=%d, w=%d, h=%d)\n",x,y,width,height);
#endif

  }

//=====================>>> vLayoutPane::Resize <<<============================
  void vLayoutPane::Resize(int newW, int newH)
  {
    // This is the routine the user will override to intercept size changes

    SysDebug2(WindowEvents,"vLayoutPane::Resize(newH:%d, newW:%d)\n",newH,newW)

  }

//==================>>> vLayoutPane::ResizeEV <<<========================
  void vLayoutPane::ResizeEV(int w, int h)
  {
    _parentWin->_WinHeight = _height = h;
    _parentWin->_WinWidth = _width = w;

    Resize((int)w, (int)h);	// Call the virtual function
  }
