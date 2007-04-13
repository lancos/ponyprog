//===============================================================
// vCommandPane - the button bar pane class used by the vWindow class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vcmdpane.h>
#include <v/vctlclrs.h>

extern "C"
{
#ifndef Athena
#include <Xm/PanedW.h>
#include <Xm/RowColumn.h>
#else		// *** ATHENA

#include <X11/Xaw/Box.h>
#include <X11/Xaw/Paned.h>
#endif
}

#include <v/vwindow.h>		// we are part of a window
#include <v/vfont.h>

#ifdef Athena
#define gray_width 2
#define gray_height 2
static char gray_bits[] = {
   0x01, 0x02};
static Pixmap gray_bitmap = 0;
#endif

//===================>>> vCommandPane::vCommandPane <<<====================
  vCommandPane::vCommandPane(CommandObject* cList) :
	vPane(P_Commands), vCmdParent()
  {
    SysDebug(Constructor,"vCommandPane::vCommandPane() constructor\n")

    _origList = cList;		// handle to list of commands
  }

//===================>>> vCommandPane::~vCommandPane <<<====================
  vCommandPane::~vCommandPane()
  {
    SysDebug(Destructor,"vCommandPane::~vCommandPane() destructor\n")
  }

//==================>>> vCommandPane::ProcessCmd <<<=======================
  void vCommandPane::ProcessCmd(ItemVal id, ItemVal rv, CmdType ct)
  {
    // simple interface between command objects and the
    // inherited vCmdParent ProcessCmd to the CommandObject which does the work

    _parentWin->WindowCommand(id, rv, ct);
  }

//====================>>> vCommandPane::GetPaneValue <<<======================
  int vCommandPane::GetPaneValue(ItemVal id, int& val) VCONST
  {
    if (HasId(id))		// make sure the id is in the pane
      {
	val = GetValue(id);
	return 1;		// assume 0 if not found
      }
    else
	return 0;	// can't find this id
  }

//====================>>> vCommandPane::GetPaneTextIn <<<======================
  int vCommandPane::GetPaneTextIn(ItemVal id, char* str, int maxlen) VCONST
  {
    if (HasId(id))              // make sure the id is in the pane
      {
        return GetTextIn(id,str,maxlen);
      }
    else
        return 0;       // can't find this id
  }

//===================>>> vCommandPane::SetPaneValue <<<========================
  void vCommandPane::SetPaneValue(ItemVal id, int val, ItemSetType setType)
  {
    SetValue(id,val,setType);
  }

//====================>>> vCommandPane::SetPaneString <<<======================
  void vCommandPane::SetPaneString(ItemVal id, VCONST char* str)
  {
    SetString(id,str);
  }

//====================>>> vCommandPane::ShowPane <<<======================
  void vCommandPane::ShowPane(int OnOrOff) VCONST
  {
    if (OnOrOff)
      {
	XtManageChild(_wDialog);
      }
    else
      {
	XtUnmanageChild(_wDialog);
      }
  }

//====================>>> vCommandPane::initialize <<<=======================
  void vCommandPane::initialize(vWindow* pWin, Widget pWidget,
	Widget drawCanvas, Widget drawArea)
  {
     // now, build the menu items in the widget provided

    DlgCmdList*	curCmd;

#ifdef Athena
    if (gray_bitmap == 0)		// haven't built the pixmap yet
      {
	gray_bitmap = XCreatePixmapFromBitmapData(
	    theApp->display(),				// our display
	    DefaultRootWindow(theApp->display()),	// a Drawable
	    gray_bits,		// the gray bitmap data
	    gray_width,		// from the file
	    gray_height,
	    (unsigned long) theApp->Xfg(),	// Forground color
	    (unsigned long) theApp->Xbg(),	// Background color
	    DefaultDepth(theApp->display(),DefaultScreen(theApp->display())));

      }
#endif

    vPane::initialize(pWin, pWidget);	// initialize base class

    // Now the Command bar that will have Commands

#ifndef Athena

    Widget wrap = pWin->getcmdArea();	// have we defined enclosing area?

    if (wrap == 0)			// don't have a pane yet
      {
	wrap = XtVaCreateManagedWidget(
	    "vCommandPaneWrap",
	    xmPanedWindowWidgetClass,		// widget class
	    _baseW,			// parent widget
	    XmNmarginHeight,2,
	    XmNmarginWidth,2,
	    XmNsashHeight,10,
	    XmNsashWidth,0,
	    XmNsashShadowThickness,0,
	    XmNseparatorOn,1,
	    XmNallowResize,1,
	    NULL);			// argument list
	pWin->setcmdArea(wrap);
      }

    /* NOTE: POSSIBLE LessTif difference...
     * At one point, LessTif had a bug that made the resizing
     * not work correctly, and my workaround was to only have
     * one of XmNresizeHeight or XmNresizeWidth true. e.g.,
     *	XmNresizeHeight,1,
     *	XmNresizeWidth,0,
     *
     *  Real Motif barfed on that, and wanted both to be 1.
     *  This is fixed, at least as of LessTif 0.88
     *  
     */

    _wDialog = XtVaCreateManagedWidget(
	"vCommandPane",
	xmRowColumnWidgetClass,		// widget class
	wrap,			// parent widget
	// PanedWindow constrants
	XmNallowResize,1,

	// RowColumn values
	XmNspacing,2,

	XmNisAligned,1,
	XmNentryVerticalAlignment,XmALIGNMENT_CENTER,
	XmNentryAlignment,XmALIGNMENT_CENTER,
	XmNrowColumnType,XmWORK_AREA,
	XmNadjustLast,0,

	XmNpacking,XmPACK_TIGHT,

	XmNisHomogeneous,0,

	XmNresizeHeight,1,	// LessTif bug??? couldn't have
	XmNresizeWidth,1,	// both 1 at some point - need to test

	XmNmarginHeight,1,
	XmNmarginWidth,1,
	XmNnumColumns,1,
	XmNadjustMargin,0,
	XmNadjustLast,0,
	XmNorientation,XmHORIZONTAL,
	NULL);			// argument list

#else
    _wDialog = XtVaCreateManagedWidget(
	"vCommandPane",
	boxWidgetClass,		// widget class
	_baseW,			// parent widget
	XtNshowGrip,FALSE,
	XtNallowResize,1,
	XtNresizeToPreferred,1,
	XtNskipAdjust,TRUE,
	XtNvSpace,2,
	NULL);			// argument list

    // set the background pattern
    if (theApp->Xdepth() <= 1)
        XtVaSetValues(_wDialog,         // the widget to set
          XtNbackgroundPixmap, gray_bitmap, NULL);
    else
        XtVaSetValues(_wDialog,         // the widget to set
          XtNbackground, _vDialogBG, NULL);  
#endif

    for (int ix = 0 ; _origList && (_origList[ix].cmdType != C_EndOfList) ;
		 ++ix)
      {
	curCmd = new DlgCmdList;		// get a new cell

	curCmd->nextDCL = _cmdList;		// add in at front
	_cmdList = curCmd;	

	curCmd->cmdP = AddCmd(&_origList[ix]);

      }
  }
