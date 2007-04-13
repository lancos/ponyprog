//===============================================================
// vStatusPane - the status bar pane class used by the vWindow class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vxutil.h>
#include <v/vstatusp.h>
#include <v/vctlclrs.h>

extern "C"
{
#ifndef Athena

#define lblWidgetClass xmLabelWidgetClass
#define NborderWidth XmNborderWidth
#define Nlabel XmNlabelString
#define Nwidth XmNwidth
#define Nsensitive XmNsensitive

#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#define setLabel(x,y) XmString x = XmStringCreateSimple(y);
#define freeLabel(x) XmStringFree(x);

#else	// *** ATHENA

#define lblWidgetClass labelWidgetClass
#define NborderWidth XtNborderWidth
#define Nlabel XtNlabel
#define Nwidth XtNwidth
#define Nsensitive XtNsensitive

#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#define setLabel(x,y) VCONST char* x = y;
#define freeLabel(x)
#endif
}

#include <v/vwindow.h>		// we are part of a window
#include <v/vfont.h>
#include <v/vapp.h>

#ifdef Athena
#define statbar_width 4
#define statbar_height 2
static char statbar_bits[] = {
   0x01, 0x04};
static Pixmap statbar_bitmap = 0;
#endif

//===================>>> vStatusPane::vStatusPane <<<====================
  vStatusPane::vStatusPane(vStatus* s_bar) : vPane(P_Status)
  {
    // The main job of this constructor is to add the list of
    // status boxes to our internal data structure.  We will not
    // realize the status boxes until the initialize routine is called.

    StatusList* temp;

    SysDebug(Constructor,"vStatusPane::vStatusPane() constructor\n")

    _statusList = 0;			// empty list

    // we are going to reverse these as we realize the list,
    // so work the list in reverse order by finding the end first

    int limit;
    for (limit = 0 ; s_bar[limit].label ; ++limit)
	;

    // now, work the list backwards

    for (int ix = limit - 1 ; ix >= 0 ; --ix)
      {
	temp = new StatusList;		// a new list cell
	temp->statusPtr = &s_bar[ix];   // use the static definition of caller
	temp->nextSL = _statusList;	// link in
	_statusList = temp;		// to front of list
      }
  }

//===================>>> vStatusPane::vStatusPane <<<====================
  vStatusPane::vStatusPane(const vStatusPane& b) : vPane(b)
  {
    vSysError("vStatusPane: V Semantics do not allow copy constructors.");
  }

//===================>>> vStatusPane::~vStatusPane <<<====================
  vStatusPane::~vStatusPane()
  {

    SysDebug(Destructor,"vStatusPane::~vStatusPane() destructor\n")

    StatusList* temp;

    for (StatusList* sl = _statusList ; sl != 0 ;)
      {
	temp = sl;		// now need to free the status list
	sl = sl->nextSL;
	delete temp;
      }
  }

//===================>>> vStatusPane::ShowPane <<<========================
  void vStatusPane::ShowPane(int OnOrOff)
  {
    if (OnOrOff)
      {
	XtManageChild(_statusBar);
      }
    else
      {
	XtUnmanageChild(_statusBar);
      }
  }

//===================>>> vStatusPane::SetPaneValue <<<========================
  void vStatusPane::SetPaneValue(ItemVal id, int val, ItemSetType setType)
  {
    // set any matching status boxes in list to val
 
    
    if (setType != Sensitive)
	return;
    for (StatusList* sl = _statusList ; sl != 0 ; sl = sl->nextSL )
      {
	if ((sl->statusPtr)->statId == id)
	  {
	    (sl->statusPtr)->sensitive = val;

	    XtVaSetValues(sl->w,		// the widget to set
		Nsensitive, val,	// change sensitive
		NULL);
	  }
      }
  }

//====================>>> vStatusPane::SetPaneString <<<======================
  void vStatusPane::SetPaneString(ItemVal id, char* str)
  {
    // change the label in a status
    for (StatusList* sl = _statusList ; sl != 0 ; sl = sl->nextSL )
      {
	if ((sl->statusPtr)->statId == id)
	  {
	    setLabel(tmp,str);
	    XtVaSetValues(sl->w,		// widget
		Nlabel, tmp,			// change label
		NULL);
	    freeLabel(tmp);
	    if ((sl->statusPtr)->width > 0)	// they specified width
	      {
		XtVaSetValues(sl->w,		// the widget to set
		     Nwidth, (sl->statusPtr)->width,
		     NULL);
	      }
	  }
      }
  }

//====================>>> vStatusPane::initialize <<<=======================
  void vStatusPane::initialize(vWindow* pWin, Widget pWidget,
	Widget drawCanvas,  Widget drawArea)
  {
     // now, build the status bar items in the widget provided

    vPane::initialize(pWin, pWidget);	// initialize base class

#ifdef Athena
    // Create pixmap
    if (statbar_bitmap == 0)		// haven't built the pixmap yet
      {
	statbar_bitmap = XCreatePixmapFromBitmapData(
	    theApp->display(),				// our display
	    DefaultRootWindow(theApp->display()),	// a Drawable
	    statbar_bits,		// the statbar bitmap data
	    statbar_width,		// from the file
	    statbar_height,
	    (unsigned long) theApp->Xfg(),	// Forground color
	    (unsigned long) theApp->Xbg(),	// Background color
	    DefaultDepth(theApp->display(),DefaultScreen(theApp->display())));
      }
#endif

    // Now the status bar that will have status boxes
#ifndef Athena
    _Shadow = XtVaCreateManagedWidget(
	"vStatusShadow",			// name
	xmFrameWidgetClass,		// class
	_baseW,				// parent
	XmNshadowType,XmSHADOW_IN,

	// PanedWindow constrants
	XmNallowResize,0,
	XmNskipAdjust,1,

	NULL);

    pWin->setmsgArea(_Shadow);		// set our widget

    _statusBar = XtVaCreateManagedWidget(
	"vStatusBar",
	xmRowColumnWidgetClass,		// widget class
	_Shadow,			// parent widget
	// PanedWindow constrants
//	XmNallowResize,1,
//	XmNskipAdjust,1,
	// RowColumn values
	XmNspacing,2,
	XmNmarginHeight,1,
	XmNmarginWidth,1,
	XmNnumColumns,1,
	XmNadjustMargin,0,
	XmNadjustLast,0,
	XmNorientation,XmHORIZONTAL,
	XmNpacking,XmPACK_TIGHT,
	XmNisAligned,0,

	// PanedWindow constrants
	XmNallowResize,0,
	XmNskipAdjust,1,

	NULL);			// argument list
#else
    _statusBar = XtVaCreateManagedWidget(
	"vStatusBar",
	boxWidgetClass,		// widget class
	_baseW,			// parent widget
	XtNvSpace,2,

	XtNshowGrip,FALSE,
	XtNallowResize,1,	// We can resize
	XtNresizeToPreferred,1, // To our preferred size
	XtNskipAdjust,TRUE,	// Change only if have to
	NULL);			// argument list

    // set the background pattern
    if (theApp->Xdepth() <= 1)
        XtVaSetValues(_statusBar,         // the widget to set
          XtNbackgroundPixmap, statbar_bitmap, NULL);
    else
        XtVaSetValues(_statusBar,         // the widget to set
          XtNbackground, _vStatusBarBG,NULL);
#endif

    for (StatusList* sl = _statusList ; sl != 0 ; sl = sl->nextSL )
      {
	int border = ((sl->statusPtr)->attrs & CA_NoBorder) ? 0 : 1;	// border?

	setLabel(tmp, (sl->statusPtr)->label);	// label on the status

#ifndef Athena
	// For Motif, make a shadowed border for bordered text
	Widget tmpW;			// A throwaway widget...

	if (border > 0)
	  {
	    tmpW = XtVaCreateManagedWidget(
		"vStatusTextShadow",			// name
		xmFrameWidgetClass,		// class
		_statusBar,				// parent
		XmNshadowType,XmSHADOW_ETCHED_IN,
		NULL);
	    sl->w = XtVaCreateManagedWidget(
		"vStatusLabel",		// name
		lblWidgetClass,		// type of widget
		tmpW,			// status's parent
		Nlabel, tmp,		// label on the status
		XmNalignment,XmALIGNMENT_CENTER,
		Nsensitive, (sl->statusPtr)->sensitive,  // sensitive?
	    NULL);
	  }
	else		// Athena and noborder Motif
#endif
	sl->w = XtVaCreateManagedWidget(
	    "vStatusLabel",		// name
	    lblWidgetClass,		// type of widget
	    _statusBar,			// status's parent
	    Nlabel, tmp,		// label on the status
	    Nsensitive, (sl->statusPtr)->sensitive,  // sensitive?
	    NborderWidth, border,
#ifdef Athena
	    XtNbackground, _vStatusBarBG,	// ATHENA ONLY
#else
	    XmNalignment,XmALIGNMENT_BEGINNING,
#endif
	    NULL);

	freeLabel(tmp);

	if ((sl->statusPtr)->width > 0)		// they specified width
	  {
	    XtVaSetValues(sl->w,		// the widget to set
	        XtNwidth, (sl->statusPtr)->width,
		NULL);
          }
      }
  }
