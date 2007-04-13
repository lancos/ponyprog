//===============================================================
// vtipwin.cxx - vTipWindow class functions - X11R5
//
// Copyright (C) 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vapp.h>		// we are a friend of App
#include <v/vtipwin.h>		// our header
#include <v/vctlclrs.h>
#include <v/vxutil.h>		// Athena/Motif mapping
#include <v/vcolor.h>		// for yellow bg

extern "C"
{
#include <X11/Xlib.h>
#include <X11/Shell.h>

#ifndef Athena
#include <Xm/Xm.h>
#include <Xm/Label.h>
#else
#include <X11/Xaw/Label.h>
#endif
}


#ifndef Athena
#define setLabel(x,y) XmString x = XmStringCreateSimple(y);
#define freeLabel(x) XmStringFree(x);
#define lblWidgetClass xmLabelWidgetClass
#define Nlabel XmNlabelString
//@@ These use athena parent for now
#else
#define setLabel(x,y) char* x = y;
#define freeLabel(x)
#define lblWidgetClass labelWidgetClass
#define Nlabel XtNlabel

#endif

//======================>>> vTipWindow::vTipWindow <<<=======================
  vTipWindow::vTipWindow()
  {

    SysDebug(Constructor,"vTipWindow::vTipWindow")

    _popup = 0;
    _wLbl = 0;
  }

//======================>>> vTipWindow::vTipWindow <<<=======================
  vTipWindow::vTipWindow(const vTipWindow& w) 		// Copy Constructor
  {
    // Copy is shallow, but we track that it is a copy
    vSysError("vTipWindow - V semantics do not support copy constructors!");
  }

//======================>>> vTipWindow::~vTipWindow <<<=======================
  vTipWindow::~vTipWindow()			// destructor
  {

    SysDebug(Destructor,"vTipWindow::~vTipWindow destructor\n")

    if (_popup == 0)
	return;
    if (PoppedUp)
	XtPopdown(_popup);		// another way to get it down
  }

//======================>>> vTipWindow::initialize <<<=======================
  void vTipWindow::initialize(void)
  {
    _popup = XtVaCreatePopupShell(
	"vTipWindow",				// use name supplied
	overrideShellWidgetClass,		// a toplevel shell
	theApp->vHandle(),		// based on App window
	XtNoverrideRedirect,1,
	XtNallowShellResize,1,
	NULL);

    PoppedUp = 0;

    setLabel(tmp,"   ")
    _wLbl = XtVaCreateManagedWidget(
	"vTipLabel",			// name
	lblWidgetClass,		// class
	_popup,			// parent
	Nlabel, tmp,		// label
#ifndef Athena
	XmNbackground, vStdColors[vC_Cyan].pixel(),
	XmNmarginTop,2,
	XmNmarginBottom,2,
	XmNmarginLeft,2,
	XmNmarginRight,2,
#else
	XtNinternalWidth,2,
	XtNinternalHeight,3,  // adjust to make same height as button
#endif
	NULL);
    freeLabel(tmp)
  }

//============================>>> vTipWindow::CloseTipWindow <<<==========================
  void vTipWindow::CloseTipWindow(void)
  {
    SysDebug(Build,"vTipWindow::CloseTipWin()")
    if (!PoppedUp)
	return;
    XtPopdown(_popup);		// another way to get it down
    PoppedUp = 0;
  }

//======================>>> vTipWindow::ShowTipWindow <<<=======================
  void vTipWindow::ShowTipWindow(int x, int y, VCONST char* tip)
  {

    if (PoppedUp)
	return;

    if (_popup == 0)		// have to create popups
	initialize();

    XtVaSetValues(_popup,	// set loc of popup
	XtNx, x,
	XtNy, y,
	NULL);

    setLabel(tmp,tip)
    XtVaSetValues(_wLbl,	// the widget to set
	Nlabel, tmp,		// label
	NULL);
    freeLabel(tmp)

    XtPopup(_popup,XtGrabNone);
    PoppedUp = 1;
  }

//================>>> vTipWindow::GetPosition <<<========================
  void vTipWindow::GetPosition(int& left, int& top, int& width, int& height)
  {
    Dimension w,h,b;
    Position l,t;

    XtVaGetValues(_popup, XtNwidth, &w, XtNheight, &h,
		  XtNx, &l, XtNy, &t, XtNborderWidth, &b, NULL);
    
    left = l; top = t; width = w+b+b; height = h+b+b;

  }

//================>>> vTipWindow::SetPosition <<<========================
  void vTipWindow::SetPosition(int left, int top)
  {
   Position l = left;
   Position t = top;
   XtVaSetValues(_popup,XtNx,l, XtNy,t,NULL);
  } 

//================>>> vTipWindow::RaiseWindow <<<========================
  void vTipWindow::RaiseWindow(void)
  {
    // raise this window to the top of all other windows

    XRaiseWindow(XtDisplay(_popup),	// our display
	    XtWindow(_popup));	// our window

  }
