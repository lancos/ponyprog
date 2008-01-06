//===============================================================
// vchkboxc.cxx	- Checkbox
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vxutil.h>	// Motif/Athena mapping
#include <v/vchkboxc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vapp.h>
#include <v/vicon.h>
#include <v/vctlclrs.h>

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

#ifdef Athena		// Athena
#define boxx_width 12
#define boxx_height 12
static char boxx_bits[] = {
   0xff, 0x0f, 0x03, 0x0c, 0x05, 0x0a, 0x09, 0x09, 0x91, 0x08, 0x61, 0x08,
   0x61, 0x08, 0x91, 0x08, 0x09, 0x09, 0x05, 0x0a, 0x03, 0x0c, 0xff, 0x0f};
static Pixmap boxx_bitmap = 0;

#define boxnox_width 12
#define boxnox_height 12
static char boxnox_bits[] = {
   0xff, 0x0f, 0x01, 0x08, 0x01, 0x08, 0x01, 0x08, 0x01, 0x08, 0x01, 0x08,
   0x01, 0x08, 0x01, 0x08, 0x01, 0x08, 0x01, 0x08, 0x01, 0x08, 0xff, 0x0f};
static Pixmap boxnox_bitmap = 0;

static int buttonh;		// to track height of button

#define cboxnox_width 12
#define cboxnox_height 12
static unsigned char cboxnox_bits[] =
  {
    3,			// 4 (3+1) colors
    0,0,0,		// 0 -> black
    255,255,255,	// 1 -> white
    128,128,128,	// 2 -> dark
    192,192,192,	// 3 -> light
    2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,3,
    2,2,1,1,1,1,1,1,1,1,3,3,
    2,2,1,1,1,1,1,1,1,1,3,3,
    2,2,1,1,1,1,1,1,1,1,3,3,
    2,2,1,1,1,1,1,1,1,1,3,3,
    2,2,1,1,1,1,1,1,1,1,3,3,
    2,2,1,1,1,1,1,1,1,1,3,3,
    2,2,1,1,1,1,1,1,1,1,3,3,
    2,2,1,1,1,1,1,1,1,1,3,3,
    2,2,3,3,3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,3,3,3,3
  };

    static vIcon noxIcon(&cboxnox_bits[0], cboxnox_height, cboxnox_width, 8);
    
#define cboxx_width 12
#define cboxx_height 12
static unsigned char cboxx_bits[] =
  {
    3,			// 4 (3+1) colors
    0,0,0,		// 0 -> black
    255,255,255,	// 1 -> white
    128,128,128,	// 2 -> dark
    192,192,192,	// 3 -> light
    2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,3,
    2,2,1,1,1,1,1,1,1,1,3,3,
    2,2,1,1,1,1,1,0,0,1,3,3,
    2,2,1,1,1,1,1,0,0,1,3,3,
    2,2,1,0,0,1,0,0,1,1,3,3,
    2,2,1,0,0,1,0,0,1,1,3,3,
    2,2,1,1,0,0,0,1,1,1,3,3,
    2,2,1,1,1,0,0,1,1,1,3,3,
    2,2,1,1,1,1,1,1,1,1,3,3,
    2,2,3,3,3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,3,3,3,3
  };

    static vIcon xIcon(&cboxx_bits[0], cboxx_height, cboxx_width, 8);
    
    static Pixmap _Xpm;	// Pixmap for X box
    static Pixmap _noXpm;	// clear checkbox

#endif

//=================>>> vCheckBoxCmd::~vCheckBoxCmd <<<=======================
  vCheckBoxCmd::~vCheckBoxCmd()
  {
    SysDebug(Destructor,"vCheckBoxCmd::~vCheckBoxCmd() destructor\n")
  }

//=================>>> vCheckBoxCmd::vCheckBoxCmd <<<=======================
  vCheckBoxCmd::vCheckBoxCmd(vCmdParent* dp, CommandObject* dc) :
	    vCmd(dp, dc)
  {
    // Create bitmaps if need to


    SysDebug(Constructor,"vCheckBoxCmd::vCheckBoxCmd() constructor\n")

#ifdef Athena
    // Fixup the colors in the icons
    static int colors_set = 0;

    if (!colors_set)
      {
  	colors_set = 1;

	XColor xc;
	xc.pixel = _vDarkControlShadow;	// the light shadow
	XQueryColor(theApp->display(),theApp->Xcolormap(),&xc);
	cboxnox_bits[7] = cboxx_bits[7] = (unsigned char) (xc.red >> 8);
	cboxnox_bits[8] = cboxx_bits[8] = (unsigned char) (xc.green >> 8);
	cboxnox_bits[9] = cboxx_bits[9] = (unsigned char) (xc.blue >> 8);

	xc.pixel = _vLightControlShadow;	// the light shadow
	XQueryColor(theApp->display(),theApp->Xcolormap(),&xc);
	cboxnox_bits[10] = cboxx_bits[10] = (unsigned char) (xc.red >> 8);
	cboxnox_bits[11] = cboxx_bits[11] = (unsigned char) (xc.green >> 8);
	cboxnox_bits[12] = cboxx_bits[12] = (unsigned char) (xc.blue >> 8);
      }

    if (boxx_bitmap == 0)
      {
	boxx_bitmap = XCreateBitmapFromData(
	    theApp->display(),	// our display
	    DefaultRootWindow(theApp->display()),  // a Drawable
	    boxx_bits,			// the boxx bitmap data
	    boxx_width,		// from the file
	    boxx_height);
      }
    if (boxnox_bitmap == 0)
      {
	boxnox_bitmap = XCreateBitmapFromData(
	    theApp->display(),	// our display
	    DefaultRootWindow(theApp->display()),  // a Drawable
	    boxnox_bits,			// the menux bitmap data
	    boxnox_width,		// from the file
	    boxnox_height);
      }
    _Xpm = (theApp->Xdepth() > 1) ? xIcon.GetXPM() : boxx_bitmap;
    _noXpm = (theApp->Xdepth() > 1) ? noxIcon.GetXPM() : boxnox_bitmap;

#endif
    initialize();			// and initialize
  }

//=====================>>> vCheckBoxCmd::initialize <<<=======================
  void vCheckBoxCmd::initialize(void)
  {
    // build a button command for use in a parent window

    CopyToLocal();		// make local copies

    Widget WfHoriz = _parentWin->getWidgetFromId(dlgCmd->cRightOf);
    Widget WfVert = _parentWin->getWidgetFromId(dlgCmd->cBelow);

    int map = !(dlgCmd->attrs & CA_Hidden);

#ifndef Athena
    setLabel(tmp,_title)

    wCmd = XtVaCreateManagedWidget(
	"vCheckBox",			// name
	xmToggleButtonWidgetClass,	// class
	wParent,			// parent
	XmNindicatorType,XmN_OF_MANY,
	XmNset,_retVal,		// start state
	XmNlabelString,tmp,
	NULL);
    if (WfVert != 0)
      {
	XtVaSetValues(wCmd, Nbelow(WfVert) NULL);
      }

    if (WfHoriz != 0)
      {
	XtVaSetValues(wCmd, NrightOf(WfHoriz) NULL);
      }

    freeLabel(tmp)

    XtAddCallback(wCmd, XmNvalueChangedCallback, CCheckBoxCmdCB, (XtPointer)this);
    // Need to set to insensitive if it is set insensitive
    if (!_Sensitive)
      {
	XtVaSetValues(wCmd,		// the widget to set
	    XmNsensitive, 0,		// change to insensitive
	    NULL);
      }
    wBox = _wLabel = 0;

#else		// Athena -------------------------

    wBox = XtVaCreateManagedWidget(
	"vCheckBox",		// name
	boxWidgetClass,		// class
	wParent,		// parent
	XtNorientation, XtEhorizontal,
	XtNborderWidth,0,		// no border
	XtNhSpace,2,
	XtNvSpace,0,
	XtNmappedWhenManaged, map,
	XtNresizable, TRUE,
	NULL);

    if (WfVert != 0)
	XtVaSetValues(wBox, Nbelow(WfVert) NULL);
    if (WfHoriz != 0)
	XtVaSetValues(wBox, NrightOf(WfHoriz) NULL);

    // calculate height to make the box come out next to the label
    // if possible. We will also try to make these the same height
    // as a button or label

    int fh;

    fh = theApp->_XDefaultFont->ascent + theApp->_XDefaultFont->descent;

    buttonh = (boxx_height > fh+6) ? boxx_height : fh+6;

    if (_retVal)			// depends on state
      {
	wCmd = XtVaCreateManagedWidget(
	    "vCheckBoxCheck",			// name
	    commandWidgetClass,		// class
	    wBox,			// parent
	    XtNbitmap,_Xpm,
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
	    "vCheckBoxCheck",			// name
	    commandWidgetClass,	// class
	    wBox,		// parent
	    XtNbitmap,_noXpm,
	    XtNheight,buttonh,		// calculate the height
	    XtNinternalHeight, 0,
	    XtNinternalWidth, 0,
	    XtNborderWidth,0,
	    XtNhighlightThickness,0,	// no highlight
	    XtNresizable, TRUE,
	    NULL);
      }

    _wLabel = XtVaCreateManagedWidget(
	"vCheckBoxLabel",		// name
	commandWidgetClass,		// class
	wBox,				// parent
	XtNlabel, _title,
	XtNinternalHeight, 3,		// same height as button/label
	XtNinternalWidth, 0,
	XtNborderWidth,0,		// label only, no border
	XtNhighlightThickness,0,	// No highlight
	XtNresizable, TRUE,
	NULL);

    XtAddCallback(wCmd, XtNcallback, CCheckBoxCmdCB, (XtPointer)this);
    XtAddCallback(_wLabel, XtNcallback, CCheckBoxCmdCB, (XtPointer)this);

    XtVaSetValues(wBox,         // the widget to set
          XtNbackground, _vDialogBG, NULL);
    XtVaSetValues(wCmd,         // the widget to set
          XtNbackground, _vDialogBG, NULL);
    XtVaSetValues(_wLabel,         // the widget to set
          XtNbackground, _vDialogBG, NULL);

    // Need to set to insensitive if it is set insensitive
    if (!_Sensitive)
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

//================>>> vCheckBoxCmd::ResetItemValue <<<======================
  void vCheckBoxCmd::ResetItemValue(void)
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
	    XtNbitmap,_Xpm,		// on bitmap
	    XtNheight,buttonh,		// calculate the height
	    NULL);
      }
    else
      {
	XtVaSetValues(wCmd, 		// set for this widget
	    XtNbitmap,_noXpm,		// off bitmap
	    XtNheight,buttonh,		// calculate the height
	    NULL);
      }
#endif

    // let parent window handle now
    _parentWin->ProcessCmd(_cmdId, _retVal, dlgCmd->cmdType);
  }

//==================>>> vCheckBoxCmd::GetCmdValue <<<=========================
  int vCheckBoxCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _retVal;
  }

//================>>> vCheckBoxCmd::SetCmdVal <<<============================
  void vCheckBoxCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vCheckBoxCmd::SetCmdVal(val:%d)\n",val)

    if (st == Value || st == Checked)
      {
	_retVal = val;			// set

#ifndef Athena
	XmToggleButtonSetState(wCmd,_retVal,0);
#else
	if (_retVal)			// depends on state
	  {
	    XtVaSetValues(wCmd, 		// set for this widget
	        XtNbitmap,_Xpm,
		XtNheight,buttonh,		// calculate the height
		NULL);
	  }
	else
	  {
	    XtVaSetValues(wCmd, 		// set for this widget
		XtNbitmap,_noXpm,
		XtNheight,buttonh,		// calculate the height
		NULL);
	  }
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
    else if (st == Sensitive)
      {
	_Sensitive = val;		// set
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
    else
      {
	SysDebug2(BadVals,"vCheckBoxCmd::SetCmdVal(val:%d,type:%d) - BAD TYPE\n",val,st)
      }

  }


//================>>> vCheckBoxCmd::SetCmdStr <<<============================
  void vCheckBoxCmd::SetCmdStr(VCONST char* str)
  {

    SysDebug1(Misc,"vCheckBoxCmd::SetCmdStr(str:%s)\n",str)

#ifndef Athena
    setLabel(tmp,str)
    XtVaSetValues(wCmd,		// the widget to set
	XmNlabelString,tmp,
	NULL);
    freeLabel(tmp)
#else
    XtVaSetValues(wCmd,		// the widget to set
	XtNlabel, str, 		// label
	NULL);
#endif

  }
//====================>>> vCheckBoxCmd::CheckBoxCmdC <<<=======================
  void vCheckBoxCmd::CheckBoxCmdCB(void)
  {

    // We have to toggle bitmap for Athena

    if (_retVal)			// depends on state
      {
	_retVal = 0;		// toggle
#ifdef Athena
	XtVaSetValues(wCmd, 		// set for this widget
	    XtNbitmap,_noXpm,
	    XtNheight,buttonh,		// calculate the height
	    NULL);
#endif
      }
    else
      {
	_retVal = 1;		// toggle
#ifdef Athena
	XtVaSetValues(wCmd, 		// set for this widget
	    XtNbitmap,_Xpm,
	    XtNheight,buttonh,		// calculate the height
	    NULL);
#endif
      }

    // let parent window handle now
    _parentWin->ProcessCmd(_cmdId, _retVal, dlgCmd->cmdType);
  }

extern "C"
{
//============================>>> CCheckBoxCB <<<=============================
  void CCheckBoxCmdCB(Widget w, XtPointer This, XtPointer call_data)
  {
    ((vCheckBoxCmd*)This)->CheckBoxCmdCB();
  }
}
