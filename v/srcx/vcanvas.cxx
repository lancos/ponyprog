//===============================================================
// vCanvas - a scrolling window canvas for drawing
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vcanvas.h>		// our header
#include <v/vctlclrs.h>

extern "C"
{
#ifndef Athena
#include <Xm/ScrollBar.h>
#include <Xm/DrawingA.h>
#define sbWidgetClass xmScrollBarWidgetClass
#define sbThickness 13

#else

#include <X11/Xaw/Paned.h>

#ifdef USE3D
#include <v/awscl3d.h>
#define sbWidgetClass scrollbar3dWidgetClass
#define sbThickness 15
#else
#include <X11/Xaw/Scrollbar.h>
#define sbWidgetClass scrollbarWidgetClass
#define sbThickness 10
#endif
#endif

#include <v/canvas.h>		// our own canvas widget

}
#include <v/vapp.h>		// need access to the app
#include <v/vwindow.h>		// we are part of a vWindow
#include <v/vcpdc.h>		// for vCanvasPaneDC

    VCursor vCanvasPane::_currentCursor = -1;	// for cursor
#ifndef Athena
    XEvent* VGlobalButtonPressedEvent;	// nasty but easy
#endif

// Put the X dependent Cursor tables here...

   static unsigned int VtoXmap[] =
      {
	XC_X_cursor,  XC_top_left_arrow, XC_center_ptr, XC_crosshair,
	XC_sb_h_double_arrow, XC_hand2, XC_xterm, XC_icon,
	XC_sb_v_double_arrow, XC_pencil, XC_question_arrow,
	XC_sizing, XC_watch, XC_X_cursor
      };

    static Cursor XCursors[maxCursors+1];

//================>>> vCanvasPane::vCanvasPane <<<========================
  vCanvasPane::vCanvasPane(PaneType pt) : vPane(pt)
  {
    SysDebug(Constructor,"vCanvasPane::vCanvasPane() constructor\n")

    _cpDC = 0;			// no pane DC
    _compressRedraw = 1;	// compress by default

    if (_currentCursor == -1)		// Do we need to create cursors?
      {
	_currentCursor = VC_None;
	for (int i = 0 ; i <= maxCursors ; ++i)  // Create all cursors
	  {
	    XCursors[i] = XCreateFontCursor(theApp->display(),VtoXmap[i]);
	    // Get the fg and bg to make cursor match
	  }
      }

  }

//================>>> vCanvasPane::~vCanvasPane <<<========================
  vCanvasPane::~vCanvasPane()
  {
    SysDebug(Destructor,"vCanvasPane::~vCanvasPane() destructor\n")
    if (_cpDC != 0)
	delete _cpDC;	// free the DC
  }

//==================>>> vCanvasPane::initialize <<<==========================
  void vCanvasPane::initialize(vWindow* pWindow, Widget pWidget, 
	Widget drawCanvas, Widget drawArea)
  {
    XtCallbackRec HscrollProcList[] =
      {
	{ CHScrollProcCB, this },
	{ (XtCallbackProc) 0, (XtPointer) 0}
      };
    XtCallbackRec VscrollProcList[] =
      {
	{ CVScrollProcCB, this },
	{ (XtCallbackProc) 0, (XtPointer) 0}
      };

#ifdef Athena
    XtCallbackRec HjumpProcList[] =
      {
	{ CHJumpProcCB, this },
	{ (XtCallbackProc) 0, (XtPointer) 0}
      };

    XtCallbackRec VjumpProcList[] =
      {
	{ CVJumpProcCB, this },
	{ (XtCallbackProc) 0, (XtPointer) 0}
      };
#endif

     SysDebug(Constructor,"vCanvasPane::initialize()\n")
    _ResizeDone = 0;
    _HScrlShown = 100;	//@@@@
    _HScrlTop =  0;
    _VScrlShown = 100;
    _VScrlTop =  0;

     // now, build the menu items in the widget provided

    vPane::initialize(pWindow,pWidget);	// initialize base class

    // canvasWidgetClass is V's own Widget that manages a drawing
    // area and scroll bars.

    if (drawCanvas != 0)	// canvas provided by subclass
      {
	_drawCanvas = drawCanvas;
      }
    else
      {
	_drawCanvas = XtVaCreateManagedWidget(
	    "vCanvasPane",
	    canvasWidgetClass,	// widget class 
	    _baseW,			// parent widget
#ifndef Athena
	    XmNallowResize,1,
#else
	    XtNshowGrip, FALSE,
	    XtNallowResize,1,
#endif
	    XtNhSpace, 1,
	    XtNvSpace, 1,
	    XtNheight,(_parentWin->_WinHeight)+16,
	    XtNwidth,(_parentWin->_WinWidth)+16,
	    NULL);			// argument list
      }

    pWindow->setworkArea(_drawCanvas);
     
    // --------------------------------------------------------------
    _hasFocus = 0;	// we don't start out with focus
    _mouseDown = 0;	// mouse is not down

    Dimension curW, curH;

#ifndef Athena

    String translations =
"<BtnMotion>: DrawingAreaInput() \n\
 <BtnDown>: DrawingAreaInput() \n\
 <BtnUp>: DrawingAreaInput() \n\
 <KeyPress>: DrawingAreaInput() \n\
 <KeyRelease>: DrawingAreaInput()";


    if (drawArea != 0)
      {
	_drawWindow = drawArea;
      }
    else
      {
	_drawWindow = XtVaCreateManagedWidget(
	    "vCanvasDraw",		// widget name
	    xmDrawingAreaWidgetClass,	// widget class 
	    _drawCanvas,		// parent widget

	    XmNtranslations, XtParseTranslationTable(translations),
	    XmNheight, _parentWin->_WinHeight,
	    XmNwidth, _parentWin->_WinWidth,

	    XmNresizePolicy, XmRESIZE_ANY,

	    XmNinitialFocus,1,

	    NULL);			// argument list
	  }

	// grab the events now, then setup all the callbacks.
	// V 1.22 - BEW via C R Johnson:
	// XmNuserData did not seem to work in a LessTif/mesa combo,
	// so use client_data instead.

    XtAddCallback(_drawWindow,XmNinputCallback, CinputCB, (XtPointer)this);
    XtAddCallback(_drawWindow,XmNexposeCallback, CexposeCB, (XtPointer)this);
    XtAddCallback(_drawWindow,XmNresizeCallback, CresizeCB, (XtPointer)this);

#else
    if (drawArea != 0)
      {
	_drawWindow = drawArea;
      }
    else
      {
	_drawWindow = XtVaCreateManagedWidget(
	    "vCanvasDraw",		// widget name
	    coreWidgetClass,	// widget class 
	    _drawCanvas,		// parent widget
	    XtNheight, _parentWin->_WinHeight,
	    XtNwidth, _parentWin->_WinWidth,
	    NULL);			// argument list
      }

#endif


    // --------------------------------------------------------------

    // After the drawing widget has been created, we can create
    // the DC for it to use.

    CreateDC();

    // now create scroll bars, add to the _drawCanvas

#ifndef Athena
    _drawVScroll = XtVaCreateManagedWidget(
	"vCanvasVScroll",				// name
	sbWidgetClass,			// class
	_drawCanvas,			// parent

	XmNvalueChangedCallback, VscrollProcList,	// callback for scrolling
	XmNincrementCallback, VscrollProcList,	// callback for scrolling
	XmNdecrementCallback, VscrollProcList,	// callback for scrolling
	XmNpageDecrementCallback, VscrollProcList,	// callback for scrolling
	XmNpageIncrementCallback, VscrollProcList,	// callback for scrolling

	XmNheight, _parentWin->_WinHeight,
	XmNwidth, sbThickness,	// scrollbar param
	XmNorientation,XmVERTICAL,
	XmNmaximum,100,
	NULL);

    _drawHScroll = XtVaCreateManagedWidget(
	"vCanvasHScroll",
	sbWidgetClass,
	_drawCanvas,

	XmNvalueChangedCallback, HscrollProcList,	// callback for scrolling
	XmNdecrementCallback, HscrollProcList,	// callback for scrolling
	XmNincrementCallback, HscrollProcList,	// callback for scrolling
	XmNpageDecrementCallback, HscrollProcList,	// callback for scrolling
	XmNpageIncrementCallback, HscrollProcList,	// callback for scrolling

	XmNorientation,XmHORIZONTAL,
	XmNheight, sbThickness,	// scrollbar param
	XmNwidth, _parentWin->_WinWidth,
	XmNmaximum,100,
	NULL);

#else
    _drawVScroll = XtVaCreateManagedWidget(
	"vCanvasVScroll",				// name
	sbWidgetClass,			// class
	_drawCanvas,			// parent
	XtNscrollProc, VscrollProcList,	// callback for scrolling
	XtNjumpProc, VjumpProcList,	// callback for scrolling
	XtNheight, _parentWin->_WinHeight,
	XtNthickness, sbThickness,	// scrollbar param
#ifdef USE3D
	XtNbackground,_vControlFace,
	XtNscrollbar3dBackground, _vControlBG,
#endif
	NULL);

    _drawHScroll = XtVaCreateManagedWidget(
	"vCanvasHScroll",
	sbWidgetClass,
	_drawCanvas,
	XtNscrollProc, HscrollProcList,	// callback for scrolling
	XtNjumpProc, HjumpProcList,	// callback for scrolling
	XtNorientation,XtorientHorizontal,	// scrollbar param
	XtNthickness, sbThickness,	// scrollbar param
	XtNwidth, _parentWin->_WinWidth,
#ifdef USE3D
	XtNbackground,_vControlFace,
XtNscrollbar3dBackground, _vControlBG,
#endif
	NULL);

#endif


    // handle required events - requires redrawing
#ifdef Athena

    XtAddEventHandler(_drawWindow, ExposureMask, 0,
	(XtEventHandler)CExposeEV,(XtPointer)this);

    // get mouse events
    XtAddEventHandler(_drawWindow, ButtonPressMask, 0,
	(XtEventHandler)CMouseDownEV,(XtPointer)this);
    XtAddEventHandler(_drawWindow, PointerMotionMask, 0,
	(XtEventHandler)CMouseMoveEV,(XtPointer)this);
    XtAddEventHandler(_drawWindow, ButtonReleaseMask, 0,
	(XtEventHandler)CMouseUpEV,(XtPointer)this);

    XtAddCallback(_drawCanvas, XtNcallback, CchangeCB, (XtPointer)this);
#endif

    // These work for Motif and Athena

    XtAddEventHandler(_drawWindow, EnterWindowMask, 0,
	(XtEventHandler)CEnterEV,(XtPointer)this);
    XtAddEventHandler(_drawWindow, LeaveWindowMask, 0,
	(XtEventHandler)CLeaveEV,(XtPointer)this);

    _HOn = _VOn = 0;			// both start out on

    SetHScroll(_HScrlShown, 0);	// initial values for scroll bar
    SetVScroll(_VScrlShown, 0);

    XtUnmanageChild(_drawHScroll);	// Unmanage the widget
    XtUnmanageChild(_drawVScroll);	// Unmanage the widget

    XtVaGetValues(_drawCanvas,		// get values for the draw canvas
	XtNwidth, &curW,		// its new h and w
	XtNheight, &curH,
	NULL);

    _height = curH;	// update the local stuff
    _width = curW;

    // The parent will resize based on stuff the child does.

    XtVaGetValues(_baseW,			// parent widget
	XtNwidth, &curW,		// its new h and w
	XtNheight, &curH,
	NULL);

    _parentWin->_WinWidth = curW;
    _parentWin->_WinHeight = curH;	// update the local stuff

    _compressRedraw = 1;	// compress by default
    _Drawable = 0;
  }

//==================>>> vCanvasPane::CreateDC <<<========================
  void vCanvasPane::CreateDC(void)
  {
    if (!_cpDC)
	_cpDC = new vCanvasPaneDC(this);	// create a CanvasPaneDC
  }

//==================>>> vCanvasPane::ChangeCB <<<========================
  void vCanvasPane::ChangeCB(void)
  {
    Dimension curW, curH;

#ifndef Athena
    if (!_ResizeDone)			// problem in lesstif
      {
	_ResizeDone = 1;
	return;
      }
#endif
    XtVaGetValues(_drawWindow,		// get values for the draw canvas
	XtNwidth, &curW,		// its new h and w
	XtNheight, &curH,
	NULL);

    if (curW > 0x7FFF)			// supposed to be unsigned, but
	curW = 0;			// can get confused
    if (curH > 0x7FFF)			// supposed to be unsigned, but
	curH = 0;			// can get confused

    _height = curH;
    _width = curW;

    // The parent will resize based on stuff the child does.

    Dimension pW, pH;
    XtVaGetValues(_baseW,			// parent widget
	XtNwidth, &pW,		// its new h and w
	XtNheight, &pH,
	NULL);

    _parentWin->_WinWidth = pW;
    _parentWin->_WinHeight = pH;	// update the local stuff

    Resize((int)curW, (int)curH);	// Call the virtual function

  }

//====================>>> vCanvasPane::ShowPane <<<======================
  void vCanvasPane::ShowPane(int OnOrOff) VCONST
  {
    if (OnOrOff)
      {
	XtManageChild(_drawCanvas);
      }
    else
      {
	XtUnmanageChild(_drawCanvas);
      }
  }

//=====================>>> vCanvasPane::SetWidthHeight <<<============================
  void vCanvasPane::SetWidthHeight(int width, int height)
  {
    Dimension curW = width;
    Dimension curH = height;
    Dimension oldbaseW;
    Dimension oldbaseH;
    Dimension oldcnvW;
    Dimension oldcnvH;

    SysDebug(Constructor,"vCanvasPane::setWidthHeight\n")
    // find out the size difference between the window and the canvas

    XtVaGetValues(_baseW,			// parent widget
	XtNwidth, &oldbaseW,		// its current h and w
	XtNheight, &oldbaseH,
	NULL);

    XtVaGetValues(_drawCanvas,			// parent widget
	XtNwidth, &oldcnvW,		// its current h and w
	XtNheight, &oldcnvH,
	NULL);

    Dimension DiffW = (oldcnvW <= oldbaseW) ? oldbaseW - oldcnvW + 1 : 1 ;
    Dimension DiffH = (oldcnvH <= oldbaseH) ? oldbaseH - oldcnvH + 1 : 1;

    XtVaSetValues(_baseW,		// get values for the draw canvas
	XtNwidth, curW + DiffW,		// its new h and w
	XtNheight, curH + DiffH,	// its new h and w
	NULL);

    // Now set the height of the canvas window. You don't always
    // get what you asked for, however.
    XtVaSetValues(_drawCanvas,	// get values for the draw canvas
	XtNheight, curH,
	XtNwidth, curW,		// its new h and w
	NULL);


    // Get these from X -- the window manager can muck these up.
    XtVaGetValues(_drawCanvas,		// get values for the draw canvas
	XtNwidth, &curW,		// its new h and w
	XtNheight, &curH,
	NULL);

    _height = curH;
    _width = curW;

    XtVaGetValues(_baseW,			// parent widget
	XtNwidth, &curW,		// its new h and w
	XtNheight, &curH,
	NULL);

    _parentWin->_WinWidth = curW;
    _parentWin->_WinHeight = curH;

  }

//------------------------- cursor Stuff ---------------------------------
//=====================>>> vCanvasPane::SetCursor <<<============================
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

    SysDebug1(WindowEvents,"vCanvasPane::SetCursor(id=%d)\n",id)

    _currentCursor = id;

    XDefineCursor(theApp->display(), XtWindow(_drawWindow), XCursors[id]);
  }

//=====================>>> vCanvasPane::UnSetCursor <<<============================
  void vCanvasPane::UnSetCursor(void)
  {
    XUndefineCursor(theApp->display(), XtWindow(_drawWindow));
    _currentCursor = VC_None;
  }

//------------------------- Scrolling Stuff ---------------------------------
//====================>>> vCanvasPane::HJumpProcCB <<<=========================
  void vCanvasPane::HJumpProcCB(float fpercent, int motifValue)
  {
    // This is called with a fraction from 0 to 1.0 indicating
    // how far the user has moved the scroll bar left or right.
    // I can't get the Athena scroll widget to work how I want, so
    // I'll live with how it seems to want to work: the shown
    // fraction only is "correct" when it will all fit. The top
    // will always be right - but 100 is at the bottom with the
    // shown drawn out of the scroll box. Sigh...

    int shown = _HScrlShown;
    int max_top = 100 - shown;
    int top;

#ifndef Athena
    int percent = (int)motifValue;
#else
    int percent = (int)(fpercent * 100.);
#endif

    if (percent > max_top)
      {
	percent = max_top;
      }

    if (max_top == 0)
	top = 0;
    else
	top = (percent * 100) / max_top;

    if (top != _HScrlTop)	// actually changed things
      {
	HPage(shown, top);
      }
  }

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

  }

//=====================>>> vCanvasPane::ShowHScroll <<<=========================
  void vCanvasPane::ShowHScroll(int OnOff)
  {
    if (OnOff)			// Turn on
      {
	if (_HOn)		// Already on
	    return;
	_HOn = 1;
	XtManageChild(_drawHScroll);	// Unmanage the widget
      }
    else			// Turn off
      {
	if (!_HOn)		// Already off
	    return;
	_HOn = 0;
	XtUnmanageChild(_drawHScroll);	// Unmanage the widget
      }

    Dimension curW, curH;

    XtVaGetValues(_drawWindow,		// get values for the draw canvas
	XtNwidth, &curW,		// its new h and w
	XtNheight, &curH,
	NULL);

    _height = curH;	// update the local stuff
    _width = curW;

    Dimension pW, pH;
    XtVaGetValues(_baseW,			// parent widget
	XtNwidth, &pW,		// its new h and w
	XtNheight, &pH,
	NULL);

    _parentWin->_WinWidth = pW;
    _parentWin->_WinHeight = pH;	// update the local stuff

    XClearArea(XtDisplay(_drawWindow), XtWindow(_drawWindow),
	0, 0, _width, _height, 1);

  }

//=====================>>> vCanvasPane::ShowVScroll <<<=========================
  void vCanvasPane::ShowVScroll(int OnOff)
  {
    if (OnOff)			// Turn on
      {
	if (_VOn)		// Already on
	    return;
	_VOn = 1;
	XtManageChild(_drawVScroll);	// Unmanage the widget
      }
    else			// Turn off
      {
	if (!_VOn)		// Already off
	    return;
	_VOn = 0;
	XtUnmanageChild(_drawVScroll);	// Unmanage the widget
      }

    Dimension curW, curH;

    XtVaGetValues(_drawWindow,		// get values for the draw canvas
	XtNwidth, &curW,		// its new h and w
	XtNheight, &curH,
	NULL);

    _height = curH;	// update the local stuff
    _width = curW;

    Dimension pW, pH;
    XtVaGetValues(_baseW,			// parent widget
	XtNwidth, &pW,		// its new h and w
	XtNheight, &pH,
	NULL);

    _parentWin->_WinWidth = pW;
    _parentWin->_WinHeight = pH;	// update the local stuff

    XClearArea(XtDisplay(_drawWindow), XtWindow(_drawWindow),
	0, 0, _width, _height, 1);
  }

//=====================>>> vCanvasPane::SetHScroll <<<=========================
  void vCanvasPane::SetHScroll(int Shown, int Top)
  {
    // Make sure we are setting to valid values, and are changing things!

    if (Shown < 0 || Shown > 100 || Top < 0 || Top > 100)
	return;

    _HScrlShown = Shown;
    if (Shown < 5)		// it must be something that shows
	_HScrlShown = 5;

    if (Shown == 100)		// Map top to space we have available
	_HScrlTop = 0;
    else
	_HScrlTop = Top;

    DrawHScroll(_HScrlShown, _HScrlTop);

  }

//=====================>>> vCanvasPane::DrawHScroll <<<========================
  void vCanvasPane::DrawHScroll(int Shown, int Top)
  {
    Arg args[3];		// Used to pass float value

#ifndef Athena
    float shown = (float) Shown;
    int iTop;

    if (Top == 0)
	iTop = 0;
    else if (Top == 100)
	iTop = 100 - Shown;
    else
	iTop = (int) ((100. - shown)*((float)Top / 100.));

    XtVaSetValues(_drawHScroll,
	XmNsliderSize,Shown,
	XmNvalue,iTop,
	NULL);
#else
    union
      {
	XtArgVal arg_value;
	float float_value;
      } shown_value, top_value;

    float shown = float(Shown) / 100.;
    float top;

    if (Top == 0)
	top = 0.;
    else
	top = float(Top) / 100.;
//1.20	top = (1. - shown)*(float(Top) / 100.);

    shown_value.float_value = shown;
    top_value.float_value = top;

    if (sizeof (float) > sizeof (XtArgVal))
      {
	XtSetArg(args[0], XtNtopOfThumb, &top_value);
	XtSetArg(args[1], XtNshown, &shown_value);
      }
    else
      {
	XtSetArg(args[0], XtNtopOfThumb, top_value.arg_value);
	XtSetArg(args[1], XtNshown, shown_value.arg_value);
      }

    XtSetValues(_drawHScroll, args, 2);		// change it!
#endif

  }

//====================>>> vCanvasPane::VJumpProcCB <<<=======================
  void vCanvasPane::VJumpProcCB(float fpercent, int motifValue)
  {
    // This code must be used with the 3D Widgets since they
    // actually give the correct values -- we need to map
    // the value supplied from the widget to a 0 - 100 range
    // required by the V specs.


    int shown = _VScrlShown;
    int max_top = 100 - shown;
    int top;

#ifndef Athena
    int percent = (int)motifValue;
#else
    int percent = (int)(fpercent * 100.);
#endif

    if (percent > max_top)
      {
	percent = max_top;
      }

    if (max_top == 0)
	top = 0;
    else
	top = (percent * 100) / max_top;

    if (top != _VScrlTop)	// actually changed things
      {
	VPage(shown, top);
      }
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
    // single step events. The default doesn't have to do anything.

    SysDebug1(WindowEvents,"vCanvasPane::VScroll(%d)\n",step);

  }

//=====================>>> vCanvasPane::GetVScroll <<<=========================
  int vCanvasPane::GetVScroll(int& Shown, int& Top) VCONST
  {
    Shown = _VScrlShown; Top = _VScrlTop;
    return _VOn;
  }

//=====================>>> vCanvasPane::GetHScroll <<<=========================
  int vCanvasPane::GetHScroll(int& Shown, int& Top) VCONST
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
    if (Shown < 5)
	_VScrlShown = 5;

    if (Shown == 100)		// Map top to space we have available
	_VScrlTop = 0;
    else
	_VScrlTop = Top;

    DrawVScroll(_VScrlShown, _VScrlTop);
  }

//======================>>> vCanvasPane::DrawVScroll <<<=======================
  void vCanvasPane::DrawVScroll(int Shown, int Top)
  {

#ifndef Athena
    float shown = (float) Shown;
    int iTop;

    if (Top == 0)
	iTop = 0;
    else if (Top == 100)
	iTop = 100 - Shown;
    else
	iTop = (int) ((100. - shown)*((float)Top / 100.));
	
    XtVaSetValues(_drawVScroll,
	XmNsliderSize,Shown,
	XmNvalue,iTop,
	NULL);
#else
    Arg args[3];		// Used to pass float value

    union
      {
	XtArgVal arg_value;
	float float_value;
      } shown_value, top_value;

    float shown = float(Shown) / 100.;
    float top;


    if (Top == 0)
	top = 0.;
    else
	top = float(Top) / 100.;
//1.20	 top = (1. - shown)*(float(Top) / 100.);

    shown_value.float_value = shown;
    top_value.float_value = top;

    if (sizeof (float) > sizeof (XtArgVal))
      {
	XtSetArg(args[0], XtNtopOfThumb, &top_value);
	XtSetArg(args[1], XtNshown, &shown_value);
      }
    else
      {
	XtSetArg(args[0], XtNtopOfThumb, top_value.arg_value);
	XtSetArg(args[1], XtNshown, shown_value.arg_value);
      }

    XtSetValues(_drawVScroll, args, 2);		// change it!
#endif

  }

//====================>>> vCanvasPane::HScrollProcCB <<<=======================
  void vCanvasPane::HScrollProcCB(int position)
  {
    if (position  > 0)		// Scroll right a page
	HScroll(1);
    else
	HScroll(-1);		// Scroll left a page
  }

//====================>>> vCanvasPane::VScrollProcCB <<<=======================
  void vCanvasPane::VScrollProcCB(int position)
  {
    if (position  > 0)		// scroll list down one page
	VScroll(1);
    else
	VScroll(-1);		// scroll list up one page
  }

//=====================>>> vCanvasPane::ExposeEV <<<==========================
  void vCanvasPane::ExposeEV(int x, int y, int width, int height, int count)
  {
    // We need to be a little intelligent about handling expose
    // events. Most users don't care about getting all the little
    // rectangles that are generated by dragging a window. Most users
    // just redraw everything on each Redraw, which can be very ugly
    // if you get a bunch of little rectangles in a row. Our solution
    // is a flag that will compress redraws, and will be true by default.
    // Only the last event caueses a Redraw, and it will be for the whole
    // canvas.


    if (_compressRedraw)
      {
	if (count != 0)		// ignore all but last expose event
	    return;
	Redraw(0, 0, _width, _height);
      }
    else
      {
	Redraw(x, y , width, height);
      }
  }

//=====================>>> vCanvasPane::EnterFocus <<<========================
  void vCanvasPane::EnterFocus()
  {
    SysDebug(WindowEvents,"vCanvasPane::EnterFocus()\n")
    if (_currentCursor != VC_None)
	SetCursor(_currentCursor);
  }

//=====================>>> vCanvasPane::EnterEV <<<==========================
  void vCanvasPane::EnterEV()
  {
    if (_hasFocus)		// don't double enter focus
	return;
    _hasFocus = 1;

#ifndef Athena
    XmProcessTraversal(_drawWindow,XmTRAVERSE_CURRENT);	// Xm bug, call twice!
    XmProcessTraversal(_drawWindow,XmTRAVERSE_CURRENT);
#endif

    theApp->XCheckSelection();
    EnterFocus();		// call the virtual function
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

//=====================>>> vCanvasPane::MouseDownEV <<<=======================
  void vCanvasPane::MouseDownEV(int x, int y, int button)
  {
    // We track mouse down internally - X should do it, but
    // sometimes it seems to get the mouse down withoud sending
    // the event, making MouseMove not work as advertised.
    if (_mouseDown)
	return;
    _mouseDown = 1;
    MouseDown(x,y,button);
  }

//=====================>>> vCanvasPane::MouseUpEV <<<=========================
  void vCanvasPane::MouseUpEV(int x, int y, int button)
  {
    if (!_mouseDown)
	return;
    _mouseDown = 0;
    MouseUp(x,y,button);
  }

//=====================>>> vCanvasPane::MouseMoveEV <<<=======================
  void vCanvasPane::MouseMoveEV(int x, int y, int button)
  {
    if (_mouseDown && button != 0)		// only if down
	MouseMove(x,y,button);
    else
      {
	MouseMotion(x,y);
      }
  }

//=====================>>> vCanvasPane::MouseDown <<<==========================
  void vCanvasPane::MouseDown(int x, int y, int button)
  {
    // Mouse events in vCanvasPane are no-ops.

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

//======================>>> vCanvasPane::FontChanged <<<===========================
  void vCanvasPane::FontChanged(VCONST vFont& vf)
  {
    // Called when the font is changed.

    SysDebug(WindowEvents,"vCanvasPane::FontChanged()\n")
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

    SysDebug2(WindowEvents,"vCanvasPane::Resize(newW:%d, newH:%d)\n",newW,newH)

  }

//=====================>>> vCanvasPane::CnvKeyInEv <<<============================
  void vCanvasPane::CnvKeyInEv(unsigned int keycode, unsigned int state)
  {
    _parentWin->KeyInEV(keycode, state);
  }

extern "C"
{
#ifndef Athena

//==============================>>> CinputCB <<<==============================
  void CinputCB(Widget w, XtPointer client_data, XtPointer call_data)
  { 
    //	input callback
    //
    // call_data will point to a structure that has data

    XmDrawingAreaCallbackStruct *cbs = 		// get callback struct
	(XmDrawingAreaCallbackStruct *)call_data;

    XEvent* xep = cbs->event;  // the event struct

    XtPointer xtp = client_data;	// v1.22: this

    switch (xep->xany.type)		// react depending on type
      {
	case KeyPress:
	  {
	    XKeyPressedEvent *kp = (XKeyPressedEvent*)xep;
	    ((vCanvasPane*)xtp)->CnvKeyInEv(kp->keycode, kp->state);
	    break;
	  }

	case KeyRelease:
	  {
	    // ignore key releases
	    break;
	  }

	case ButtonPress:
	  {
	    VGlobalButtonPressedEvent = xep;	// needed for right popup
	    XButtonEvent* bpxp = (XButtonEvent*)xep;
	    ((vCanvasPane*)xtp)->MouseDownEV(bpxp->x, bpxp->y, bpxp->button);
	    break;
	  }

	case ButtonRelease:
	  {
	    XButtonEvent* brxp = (XButtonEvent*)xep;
	    ((vCanvasPane*)xtp)->MouseUpEV(brxp->x, brxp->y, brxp->button);
	    break;
	  }

	case MotionNotify:
	  {
	    XMotionEvent* xp = (XMotionEvent*)xep;
	    int button = 0;		// assume button 1

	    if (xp->state & Button1Mask)
		button = 1;
	    else if (xp->state & Button2Mask)
		button = 2;
	    else if (xp->state & Button3Mask)
		button = 3;

	    ((vCanvasPane*)xtp)->MouseMoveEV(xp->x, xp->y, button);
	    break;
	  }

	default:
	    break;


      }
  }

//==============================>>> CexposeCB <<<==============================
  void CexposeCB(Widget w, XtPointer client_data, XtPointer call_data)
  { 
    //	expose callback
    //
    // client_data will have the this pointer of our object
    // call_data will point to a structure that handles
    // resize, scrollbars, and exposure

    XmDrawingAreaCallbackStruct *cbs = 		// get callback struct
	(XmDrawingAreaCallbackStruct *)call_data;

    XExposeEvent* xp = (XExposeEvent*) cbs->event;  // the event struct

    // v1.22 - client_data has this
    ((vCanvasPane*)client_data)->ExposeEV(xp->x, xp->y, xp->width, xp->height, xp->count);

  }

//==============================>>> CresizeCB <<<==============================
  void CresizeCB(Widget w, XtPointer client_data, XtPointer call_data)
  { 
    //	change callback
    //
    // client_data will have the this pointer of our object
    // call_data will point to a structure that handles
    // resize, scrollbars, and exposure

    ((vCanvasPane*)client_data)->ChangeCB();
  }


#else		// ###### Athena

//==============================>>> CchangeCB <<<==============================
  void CchangeCB(Widget w, XtPointer client_data, XtPointer call_data)
  { 
    //	change callback
    //
    // client_data will have the this pointer of our object
    // call_data will point to a structure that handles
    // resize, scrollbars, and exposure

    ((vCanvasPane*)client_data)->ChangeCB();
  }

//==============================>>> CExposeEV <<<==============================
  void CExposeEV(Widget w, XtPointer client_data, XEvent* event, char *x)
  { 

#ifdef Athena

    //	Exposure event
    //
    // client_data will have the this pointer of our object

    XExposeEvent* xp;

    xp = (XExposeEvent*)event;

    ((vCanvasPane*)client_data)->ExposeEV(xp->x, xp->y, xp->width, xp->height, xp->count);
#endif
  }

//=============================>>> CMouseDownEV <<<============================
  void CMouseDownEV(Widget w, XtPointer client_data, XEvent* event, char *x)
  { 
    // Mouse down event
#ifdef Athena
    XButtonEvent* xp;

    xp = (XButtonEvent*)event;

    ((vCanvasPane*)client_data)->MouseDownEV(xp->x, xp->y, xp->button);
#endif
  }

//==============================>>> CMouseUpEV <<<===========================
  void CMouseUpEV(Widget w, XtPointer client_data, XEvent* event, char *x)
  { 
#ifdef Athena
    // Mouse up event
    XButtonEvent* xp;

    xp = (XButtonEvent*)event;

    ((vCanvasPane*)client_data)->MouseUpEV(xp->x, xp->y, xp->button);
#endif
  }

//============================>>> CMouseMoveEV <<<==========================
  void CMouseMoveEV(Widget w, XtPointer client_data, XEvent* event, char *x)
  { 
#ifdef Athena
    // Mouse down event
    XMotionEvent* xp;

    xp = (XMotionEvent*)event;

    int button = 0;		// assume button 1

    if (xp->state & Button1Mask)
	button = 1;
    else if (xp->state & Button2Mask)
	button = 2;
    else if (xp->state & Button3Mask)
	button = 3;

    ((vCanvasPane*)client_data)->MouseMoveEV(xp->x, xp->y, button);
#endif
  }
#endif		// ### Motif or Athena

// common to both Motif and Athena

//==============================>>> CEnterEV <<<=============================
  void CEnterEV(Widget w, XtPointer client_data, XEvent* event, char *x)
  { 
    //	Enter event
    //
    // client_data will have the this pointer of our object

    ((vCanvasPane*)client_data)->EnterEV();
  }

//==============================>>> CLeaveEV <<<==============================
  void CLeaveEV(Widget w, XtPointer client_data, XEvent* event, char *x)
  { 
    //	Leave event
    //
    // client_data will have the this pointer of our object

    ((vCanvasPane*)client_data)->LeaveEV();
  }

//============================>>> CHJumpProcCB <<<=============================
  void CHJumpProcCB(Widget w, XtPointer This, XtPointer pc_ptr)
  {
    float percent = *(float*)pc_ptr;	// get the percent back

    ((vCanvasPane*)This)->HJumpProcCB(percent,0);
  }

//============================>>> CHScollProcCB <<<=============================
  void CHScrollProcCB(Widget w, XtPointer This, XtPointer position)
  {
#ifndef Athena
    XmScrollBarCallbackStruct* cbp = (XmScrollBarCallbackStruct*)position;
    switch (cbp->reason)
      {
	case XmCR_DECREMENT:
	case XmCR_PAGE_DECREMENT:
	  {
	    ((vCanvasPane*)This)->HScrollProcCB(-1);
	    break;
	  }
	case XmCR_INCREMENT:
	case XmCR_PAGE_INCREMENT:
	  {
	    ((vCanvasPane*)This)->HScrollProcCB(1);
	    break;
	  }
	case XmCR_VALUE_CHANGED:
	  {
	    ((vCanvasPane*)This)->HJumpProcCB(0.0, cbp->value);
	    break;
	  }
      }
#else
    int pos = (int)position;
    ((vCanvasPane*)This)->HScrollProcCB(pos);
#endif
  }

//============================>>> CJumpProcCB <<<=============================
  void CVJumpProcCB(Widget w, XtPointer This, XtPointer pc_ptr)
  {
    float percent = *(float*)pc_ptr;	// get the percent back

    ((vCanvasPane*)This)->VJumpProcCB(percent,0);
  }

//============================>>> CVScollProcCB <<<=============================
  void CVScrollProcCB(Widget w, XtPointer This, XtPointer position)
  {
#ifndef Athena
    XmScrollBarCallbackStruct* cbp = (XmScrollBarCallbackStruct*)position;
    switch (cbp->reason)
      {
	case XmCR_DECREMENT:
	case XmCR_PAGE_DECREMENT:
	  {
	    ((vCanvasPane*)This)->VScrollProcCB(-1);
	    break;
	  }
	case XmCR_INCREMENT:
	case XmCR_PAGE_INCREMENT:
	  {
	    ((vCanvasPane*)This)->VScrollProcCB(1);
	    break;
	  }
	case XmCR_VALUE_CHANGED:
	  {
	    ((vCanvasPane*)This)->VJumpProcCB(0.0, cbp->value);
	    break;
	  }
      }
#else
    int pos = (int)position;
    ((vCanvasPane*)This)->VScrollProcCB(pos);
#endif
  }

}
