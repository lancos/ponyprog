//===============================================================
// vradiocc.cxx	- RadioButtons
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vxutil.h>	// Motif/Athena mapping
#include <v/vradioc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vapp.h>
#include <v/vctlclrs.h>
#ifdef Athena
#include <v/vicon.h>
#endif

extern "C"
{
#include <X11/StringDefs.h>

#ifndef Athena

#include <Xm/Xm.h>
#include <Xm/ToggleB.h>
#define setLabel(x,y) XmString x = XmStringCreateSimple(y);
#define freeLabel(x) XmStringFree(x);

#else

#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#endif
}

    // Define Static Data

#ifdef Athena			// Athena RB pixels

#define rbon_width 11
#define rbon_height 11
static char rbon_bits[] = {
   0x70, 0x00, 0x8c, 0x01, 0x72, 0x02, 0xfa, 0x02, 0xfd, 0x05, 0xfd, 0x05,
   0xfd, 0x05, 0xfa, 0x02, 0x72, 0x02, 0x8c, 0x01, 0x70, 0x00};

//   0x70, 0x00, 0xfc, 0x01, 0xfe, 0x03, 0xfe, 0x03, 0xff, 0x07, 0xff, 0x07,
//   0xff, 0x07, 0xfe, 0x03, 0xfe, 0x03, 0xfc, 0x01, 0x70, 0x00};
static Pixmap rbon_bitmap = 0;

#define rboff_width 11
#define rboff_height 11
static char rboff_bits[] = {
   0x70, 0x00, 0x8c, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x04, 0x01, 0x04,
   0x01, 0x04, 0x02, 0x02, 0x02, 0x02, 0x8c, 0x01, 0x70, 0x00};
static Pixmap rboff_bitmap = 0;

static int buttonh;		// to track height of button

#define crboff_width 12
#define crboff_height 12
static unsigned char crboff_bits[] =
  {
    4,			// 5 (4+1) colors
    0,0,0,		// 0 -> black
    255,255,255,	// 1 -> white
    255,255,255,	// 2 -> dark
    255,255,255,	// 3 -> light
    190,190,190,	// 4 -> dialog
    4,4,4,4,2,2,2,2,4,4,4,4,
    4,4,4,2,2,2,2,2,2,4,4,4,
    4,4,2,2,1,1,1,1,3,3,4,4,
    4,2,2,1,1,1,1,1,1,3,3,4,
    2,2,1,1,1,1,1,1,1,1,3,3,
    2,2,1,1,1,1,1,1,1,1,3,3,
    2,2,1,1,1,1,1,1,1,1,3,3,
    2,2,1,1,1,1,1,1,1,1,3,3,
    4,2,2,1,1,1,1,1,1,3,3,4,
    4,4,2,2,1,1,1,1,3,3,4,4,
    4,4,4,3,3,3,3,3,3,4,4,4,
    4,4,4,4,3,3,3,3,4,4,4,4
  };

    static vIcon rboffIcon(&crboff_bits[0], crboff_height, crboff_width, 8);

#define crbon_width 12
#define crbon_height 12
static unsigned char crbon_bits[] =
  {
    4,			// 5 (4+1) colors
    0,0,0,		// 0 -> black
    255,255,255,	// 1 -> white
    255,255,255,	// 2 -> dark
    255,255,255,	// 3 -> light
    190,190,190,	// 4 -> dialog
    4,4,4,4,2,2,2,2,4,4,4,4,
    4,4,4,2,2,2,2,2,2,4,4,4,
    4,4,2,2,1,1,1,1,3,3,4,4,
    4,2,2,1,1,1,1,1,1,3,3,4,
    2,2,1,1,1,0,0,1,1,1,3,3,
    2,2,1,1,0,0,0,0,1,1,3,3,
    2,2,1,1,0,0,0,0,1,1,3,3,
    2,2,1,1,1,0,0,1,1,1,3,3,
    4,2,2,1,1,1,1,1,1,3,3,4,
    4,4,2,2,1,1,1,1,3,3,4,4,
    4,4,4,3,3,3,3,3,3,4,4,4,
    4,4,4,4,3,3,3,3,4,4,4,4
  };

    static vIcon rbonIcon(&crbon_bits[0], crbon_height, crbon_width, 8);

    static Pixmap _rbOnPm;	// rb on pixmap
    static Pixmap _rbOffPm;	// rb off pixmap

#endif

    vRadioButtonCmd* vRadioButtonCmd::_RBList = 0;	// empty list to start

//================>>> vRadioButtonCmd::vRadioButtonCmd <<<=====================
  vRadioButtonCmd::vRadioButtonCmd(vCmdParent* dp, CommandObject* dc) :
	    vCmd(dp, dc)
  {
    // Create bitmaps if need to


    SysDebug(Constructor,"vRadioButtonCmd::vRadioButtonCmd() constructor\n")

#ifdef Athena
    static int colors_set = 0;

    if (!colors_set)
      {
	colors_set = 1;
	// Fixup the colors in the icons
	XColor xc;
	xc.pixel = _vDarkControlShadow;	// the light shadow
	XQueryColor(theApp->display(),theApp->Xcolormap(),&xc);
	crbon_bits[7] = crboff_bits[7] = (unsigned char) (xc.red >> 8);
	crbon_bits[8] = crboff_bits[8] = (unsigned char) (xc.green >> 8);
	crbon_bits[9] = crboff_bits[9] = (unsigned char) (xc.blue >> 8);

	xc.pixel = _vLightControlShadow;	// the light shadow
	XQueryColor(theApp->display(),theApp->Xcolormap(),&xc);
	crbon_bits[10] = crboff_bits[10] = (unsigned char) (xc.red >> 8);
	crbon_bits[11] = crboff_bits[11] = (unsigned char) (xc.green >> 8);
	crbon_bits[12] = crboff_bits[12] = (unsigned char) (xc.blue >> 8);

	xc.pixel = _vDialogBG;	// the dialog background
	XQueryColor(theApp->display(),theApp->Xcolormap(),&xc);
	crbon_bits[13] = crboff_bits[13] = (unsigned char) (xc.red >> 8);
	crbon_bits[14] = crboff_bits[14] = (unsigned char) (xc.green >> 8);
	crbon_bits[15] = crboff_bits[15] = (unsigned char) (xc.blue >> 8);
      }

    if (rbon_bitmap == 0)
      {
	rbon_bitmap = XCreateBitmapFromData(
	    theApp->display(),	// our display
	    DefaultRootWindow(theApp->display()),  // a Drawable
	    rbon_bits,			// the bitmap data
	    rbon_width,		// from the file
	    rbon_height);
      }

    if (rboff_bitmap == 0)
      {
	rboff_bitmap = XCreateBitmapFromData(
	    theApp->display(),	// our display
	    DefaultRootWindow(theApp->display()),  // a Drawable
	    rboff_bits,			// the bitmap data
	    rboff_width,		// from the file
	    rboff_height);
      }

    _rbOnPm = (theApp->Xdepth() > 1) ? rbonIcon.GetXPM() : rbon_bitmap;
    _rbOffPm = (theApp->Xdepth() > 1) ? rboffIcon.GetXPM() : rboff_bitmap;

#endif

    initialize();			// and initialize
  }

//================>>> vRadioButtonCmd::vRadioButtonCmd <<<=====================
  vRadioButtonCmd::vRadioButtonCmd(const vRadioButtonCmd& r) :
	    vCmd(r)
  {
    vSysError("vRadioButtonCmd - V semantics do not support copy constructors!");
  }

//===========>>> vRadioButtonCmd::~vRadioButtonCmd <<<======================
  vRadioButtonCmd::~vRadioButtonCmd()
  {
    // We have to remove ourself from the list of all RBs


    SysDebug(Destructor,"vRadioButtonCmd::~vRadioButtonCmd() destructor\n")

    if (_RBList == this)		// first one special
      {
	_RBList = _nextRB;
      }
    else
      {
	for (vRadioButtonCmd* rbl = _RBList ; rbl != 0 ; rbl = rbl->_nextRB)
	  {
	    if (rbl->_nextRB == this)		// we found ourself!
	      {
		rbl->_nextRB = _nextRB;		// unlink us
		break;
	      }
	  }
      }
  }

//=====================>>> vRadioButtonCmd::initialize <<<=======================
  void vRadioButtonCmd::initialize(void)
  {
    // build a button command for use in a parent window

    CopyToLocal();		// make local copies

    Widget temp;

    Widget WfHoriz = _parentWin->getWidgetFromId(dlgCmd->cRightOf);
    Widget WfVert = _parentWin->getWidgetFromId(dlgCmd->cBelow);

    _nextRB = _RBList;		// add us in to front of list
    _RBList = this;

    int map = !(dlgCmd->attrs & CA_Hidden);

    if (*_title == 0)		// can't have 0 length text...
	_title = " ";

#ifndef Athena

    setLabel(tmp,_title)

    wCmd = XtVaCreateManagedWidget(
	"vRadioButtonBox",			// name
	xmToggleButtonWidgetClass,	// class
	wParent,			// parent
	XmNindicatorType,XmONE_OF_MANY,
	XmNset,_retVal,		// start state
	XmNlabelString,tmp,
	NULL);

    if (WfVert != 0)
	XtVaSetValues(wCmd, Nbelow(WfVert) NULL);
    if (WfHoriz != 0)
	XtVaSetValues(wCmd, NrightOf(WfHoriz) NULL);
    freeLabel(tmp)

    XtAddCallback(wCmd, XmNvalueChangedCallback, CRadioButtonCmdCB, (XtPointer)this);
    // Need to set to insensitive if it is set insensitive
    if (!dlgCmd->Sensitive)
      {
	XtVaSetValues(wCmd,		// the widget to set
	    XmNsensitive, 0,		// change to insensitive
	    NULL);
      }
    wBox = _wLabel = 0;


#else		// Athena -------------------------

    wBox = XtVaCreateManagedWidget(
	"vRadioButtonBox",	// name
	boxWidgetClass,		// class
	wParent,		// parent
	XtNorientation, XtEhorizontal,
	XtNborderWidth,0,		// no border
	XtNhSpace,2,
	XtNvSpace,0,
	XtNresizable, TRUE,
	XtNmappedWhenManaged, map,
	NULL);

    if (WfVert != 0)
	XtVaSetValues(wBox, Nbelow(WfVert) NULL);
    if (WfHoriz != 0)
	XtVaSetValues(wBox, NrightOf(WfHoriz) NULL);

    // calculate height to make the box come out next to the label
    // if possible. We will also try to make these the same height
    // as a button or label

    int asc, des, fh;

    fh = theApp->_XDefaultFont->ascent + theApp->_XDefaultFont->descent;

    buttonh = (rbon_height > fh+6) ? rbon_height : fh+6;

    if (_retVal)			// depends on state
      {
	wCmd = XtVaCreateManagedWidget(
	    "vRadioButtonOn",			// name
	    commandWidgetClass,		// class
	    wBox,			// parent
	    XtNbitmap,_rbOnPm,
	    XtNheight,buttonh,		// calculate the height
	    XtNinternalHeight, 0,
	    XtNinternalWidth, 0,
	    XtNborderWidth,0,
	    XtNhighlightThickness,0,	// no highlight
	    XtNresizable, TRUE,
	    NULL);
      }
    else
      {
	wCmd = XtVaCreateManagedWidget(
	    "vRadioButtonOff",			// name
	    commandWidgetClass,	// class
	    wBox,		// parent
	    XtNbitmap,_rbOffPm,
	    XtNheight,buttonh,		// calculate the height
	    XtNinternalHeight, 0,
	    XtNinternalWidth, 0,
	    XtNborderWidth,0,
	    XtNhighlightThickness,0,	// no highlight
	    XtNresizable, TRUE,
	    NULL);
      }

    _wLabel = XtVaCreateManagedWidget(
	"vRadioButtonLabel",		// name
	commandWidgetClass,		// class
	wBox,				// parent
	XtNlabel, _title,
	XtNinternalHeight, 3,		// same height as button/label
	XtNinternalWidth, 0,
	XtNborderWidth,0,		// label only, no border
	XtNhighlightThickness,0,	// no highlight
	XtNresizable, TRUE,
	NULL);

    XtAddCallback(wCmd, XtNcallback, CRadioButtonCmdCB, (XtPointer)this);
    XtAddCallback(_wLabel, XtNcallback, CRadioButtonCmdCB, (XtPointer)this);

    XtVaSetValues(wBox,         // the widget to set
          XtNbackground, _vDialogBG, NULL);
    XtVaSetValues(wCmd,         // the widget to set
          XtNbackground, _vDialogBG,NULL);
    XtVaSetValues(_wLabel,         // the widget to set
          XtNbackground, _vDialogBG, NULL);

    // Need to set to insensitive if it is set insensitive
    if (!dlgCmd->Sensitive)
      {
	XtVaSetValues(wCmd,		// the widget to set
	    XtNsensitive, 0,		// change to insensitive
	    NULL);
	XtVaSetValues(_wLabel,		// the widget to set
	    XtNsensitive, 0,		// change insensitive
	    NULL);
      }
#endif
  }

//================>>> vRadioButtonCmd::ResetItemValue <<<======================
  void vRadioButtonCmd::ResetItemValue(void)
  {

    // We have to toggle things

    if (_retVal == _origVal)	// No op if no change
	return;

    _retVal = _origVal;		// restore

#ifndef Athena
    XmToggleButtonSetState(wCmd,_retVal,0);
#else
    if (_retVal)			// depends on state
      {
	XtVaSetValues(wCmd, 		// set for this widget
	    XtNbitmap,_rbOnPm,
	    XtNheight,buttonh,		// calculate the height
	    NULL);
      }
    else
      {
	XtVaSetValues(wCmd, 		// set for this widget
	    XtNbitmap,_rbOffPm,	// on bitmap
	    XtNheight,buttonh,		// calculate the height
	    NULL);
      }
#endif

    // let parent window handle now
    _parentWin->ProcessCmd(_cmdId, _retVal, dlgCmd->cmdType);
  }

//==================>>> vRadioButtonCmd::GetCmdValue <<<=========================
  int vRadioButtonCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _retVal;
  }

//================>>> vRadioButtonCmd::SetCmdVal <<<========================
  void vRadioButtonCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vRadioButtonCmd::SetCmdVal(val:%d)\n",val)

    if (st == Sensitive)
      {
	dlgCmd->Sensitive = val;	// set
#ifndef Athena
	XtVaSetValues(wCmd,		// the widget to set
	    XmNsensitive, val,		// change sensitive
	    NULL);
#else
	XtVaSetValues(wCmd,		// the widget to set
	    XtNsensitive, val,		// change sensitive
	    NULL);
	XtVaSetValues(_wLabel,		// the widget to set
	    XtNsensitive, val,		// change sensitive
	    NULL);
#endif
      }
    else if (st == Hidden)		// hide or unhide
      {
	if (val)
	  {
#ifndef Athena
	    XtUnmapWidget(wCmd);	// unmap this widget
#else
	    XtUnmapWidget(wBox);	// unmap this widget
#endif
	  }
	else
	  {
#ifndef Athena
	    XtMapWidget(wCmd);	// unmap this widget
#else
	    XtMapWidget(wBox);	// unmap this widget
#endif
	  }
      }
    else if (st == Value)
      {
	if (!val)			// turning it off
	  {
	    _retVal = 0;
#ifndef Athena
	    XmToggleButtonSetState(wCmd,_retVal,0);
#else
	    XtVaSetValues(wCmd, 	// set for this widget
		XtNbitmap,_rbOffPm,	// off bitmap
		XtNheight,buttonh,		// calculate the height
		NULL);
#endif
	  }
	else
	  {
	    RadioButtonCmdCB(0);	// just like we clicked it, but no notify
	  }
      }
    else
      {
	SysDebug2(BadVals,"vRadioButtonCmd::SetCmdVal(val:%d, type:%d) - BAD TYPE\n",val,st)
      }

  }

//================>>> vRadioButtonCmd::SetCmdStr <<<=========================
  void vRadioButtonCmd::SetCmdStr(VCONST char *str)
  {

    SysDebug1(Misc,"vRadioButtonCmd::SetCmdStr(str:%s)\n",str)

    _title = str;
#ifndef Athena
    setLabel(tmp,str)
    XtVaSetValues(wCmd,		// the widget to set
	XmNlabelString,tmp,
	NULL);
    freeLabel(tmp)
#else
    XtVaSetValues(_wLabel,		// the widget to set
	XtNlabel, str, 			// label
	NULL);
#endif
  }

//====================>>> vRadioButtonCmd::RadioButtonCmdCB <<<=======================
  void vRadioButtonCmd::RadioButtonCmdCB(int NotifyParent)
  {

    // We have to toggle things, so scan the list of all radio buttons,
    // searching for other buttons in the same frame

    if (_retVal)			// We are already on, so ignore
      {
#ifndef Athena
	XmToggleButtonSetState(wCmd,1,0);   // Don't allow off
#endif
	return;
      }

    ItemVal ourFrame = dlgCmd->cFrame;		// remember our frame

    for (vRadioButtonCmd* rbl = _RBList ; rbl != 0 ; rbl = rbl->_nextRB)
      {
	CommandObject* dc = rbl->dlgCmd;	// shorthand

	if (dc->cFrame == ourFrame)		// Radio Button in same frame
	  {
	    if (rbl->_retVal)			// This one was ON, turn off
	      {
		rbl->_retVal = 0;			// toggle
#ifndef Athena
		XmToggleButtonSetState(rbl->wCmd,0,0);
#else
		XtVaSetValues(rbl->wCmd, 	// set for this widget
		    XtNbitmap,_rbOffPm,	// off bitmap
		    XtNheight,buttonh,		// calculate the height
		    NULL);
#endif
		// inform parent of change
		if (NotifyParent)
		    _parentWin->ProcessCmd(dc->cmdId, rbl->_retVal, dc->cmdType);
		break;
	      }
	  }
      }

    // Now, we need to turn this RadioButton ON
    _retVal = 1;			// toggle
#ifndef Athena
    XmToggleButtonSetState(wCmd,1,0);
#else
    XtVaSetValues(wCmd, 		// set for this widget
    XtNbitmap,_rbOnPm,
	XtNheight,buttonh,		// calculate the height
	NULL);
#endif
    // and let the parent window know about the change
    if (NotifyParent)
	_parentWin->ProcessCmd(_cmdId, _retVal, dlgCmd->cmdType);
  }

extern "C"
{
//============================>>> CRadioButtonCB <<<=============================
  void CRadioButtonCmdCB(Widget w, XtPointer This, XtPointer call_data)
  {
    ((vRadioButtonCmd*)This)->RadioButtonCmdCB(1);
  }
}
