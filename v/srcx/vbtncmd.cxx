//===============================================================
// vbtncmd.xxx - button Cmd
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vxutil.h>	// Motif/Athena mapping
#include <v/vbtncmd.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vicon.h>	// for icons
#include <v/vctlclrs.h>

extern "C"
{
#ifndef Athena
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#endif

#ifdef USE3D
#include <v/awcmd3d.h>
#include <v/awtog3d.h>
#else
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Toggle.h>
#endif

#include <X11/Xaw/Form.h>

}

#ifndef Athena	// --------------------------------------

#define cmdWidgetClass xmPushButtonWidgetClass
#define togWidgetClass xmToggleButtonWidgetClass
#define Ncallback XmNactivateCallback
#define Nheight XmNheight
#define Nlabel XmNlabelString
#define NmappedWhenManaged XmNmappedWhenManaged
#define Nwidth XmNwidth
//@@ These use athena parent for now
#define Nresizable(x) // XtNresizable,x

#define setLabel(x,y) XmString x = XmStringCreateLocalized(y);
#define freeLabel(x) XmStringFree(x);

#define shadowThick 2

#else	//------------------------------------------------------

#ifdef USE3D
#define cmdWidgetClass command3dWidgetClass
#define togWidgetClass toggle3dWidgetClass
#else
#define cmdWidgetClass commandWidgetClass
#define togWidgetClass toggleWidgetClass
#endif
#define Ncallback XtNcallback
#define Nheight XtNheight
#define Nlabel XtNlabel
#define NmappedWhenManaged XtNmappedWhenManaged
#define Nwidth XtNwidth
#define Nresizable(x) XtNresizable,x,
#define setLabel(x,y) VCONST char* x = y;
#define freeLabel(x)

#endif	// -----------------------------------------------------

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
    Pixmap pixmap = 0;
    Pixmap inspixmap = 0;

    CopyToLocal();		// make local copies

    VCONST char* label = _title;

    SysDebug(Constructor,"vButtonCmd::vButtonCmd() constructor\n")

    Widget WfHoriz = _parentWin->getWidgetFromId(dlgCmd->cRightOf);
    Widget WfVert = _parentWin->getWidgetFromId(dlgCmd->cBelow);

    int map = !(dlgCmd->attrs & CA_Hidden);

    if (dlgCmd->cmdType == C_IconButton && _itemList != 0)	// icon
      {
	vIcon* ip = (vIcon *) _itemList;	// to access bitmap

#ifndef Athena
	unsigned long wbg;
	XtVaGetValues(wParent, XmNbackground, &wbg, 0);	// get bg

	pixmap = ip->GetXPM(3, wbg);
	inspixmap = ip->GetInsensXPM(3, wbg);
	wCmd = XtVaCreateManagedWidget(
	    "ButtonCmd",			// name
	    xmPushButtonWidgetClass,	// class
	    wParent,		// parent
	    XmNlabelType,XmPIXMAP,
	    XmNlabelPixmap,pixmap,
	    XmNlabelInsensitivePixmap,inspixmap,

	    XmNmarginTop,0,
	    XmNmarginBottom,0,
//	    XmNmarginLeft,2,		// 1.24
//	    XmNmarginRight,2,
	    XmNmarginLeft,1,		// 1.24
	    XmNmarginRight,1,
	    XmNalignment, XmALIGNMENT_CENTER,
	    XmNborderWidth,0,
	    XmNshadowThickness, shadowThick,

	    NmappedWhenManaged, map,
	    Nresizable(TRUE)
	    NULL);
#else
	pixmap = ip->GetXPM(2);
	wCmd = XtVaCreateManagedWidget(
	    "IconButtonCmd",		// name
	    cmdWidgetClass,		// class
	    wParent,			// parent
	    XtNbitmap, pixmap,		// the pixmap
            XtNbackground, _vControlFace,
	    XtNinternalWidth,2,
	    NmappedWhenManaged, map,
	    Nresizable(TRUE)
	    NULL);
#endif
	if (WfVert != 0)
	    XtVaSetValues(wCmd, Nbelow(WfVert) NULL);
	if (WfHoriz != 0)
	    XtVaSetValues(wCmd, NrightOf(WfHoriz) NULL);
      }
    else if (dlgCmd->cmdType == C_ToggleIconButton && _itemList != 0)	// icon
      {
	vIcon* ip = (vIcon *) _itemList;	// to access bitmap


#ifndef Athena
	unsigned long wbg;
	XtVaGetValues(wParent, XmNbackground, &wbg, 0);	// get bg

	pixmap = ip->GetXPM(3, wbg);
	inspixmap = ip->GetInsensXPM(3, wbg);
	wCmd = XtVaCreateManagedWidget(
	    "ToggleIconButtonCmd",		// name
	    togWidgetClass,		// class
	    wParent,			// parent
	    XmNlabelType,XmPIXMAP,
	    XmNlabelPixmap, pixmap,		// the pixmap
	    XmNlabelInsensitivePixmap,inspixmap,
	    XmNset,_retVal,

	    XmNmarginTop,2,
	    XmNmarginBottom,2,
	    XmNmarginLeft,2,
	    XmNmarginRight,2,
	    XmNalignment, XmALIGNMENT_CENTER,
	    XmNborderWidth,0,
	    XmNshadowThickness, shadowThick,

	    XmNfillOnSelect,0,		// alternate method suggested
	    XmNindicatorOn,0,		// by promayon@imag.fr

	    NmappedWhenManaged, map,
	    Nresizable(TRUE)
	    NULL);
#else

	pixmap = ip->GetXPM(2);
	wCmd = XtVaCreateManagedWidget(
	    "ToggleIconButtonCmd",		// name
	    togWidgetClass,		// class
	    wParent,			// parent
	    XtNbitmap, pixmap,		// the pixmap
            XtNbackground, _vControlFace,
	    XtNinternalWidth,2,
	    XtNstate,_retVal,
#ifdef USE3D
	    XtNhighlightThickness,0,
#endif
	    NmappedWhenManaged, map,
	    Nresizable(TRUE)
	    NULL);
#endif

	if (WfVert != 0)
	    XtVaSetValues(wCmd, Nbelow(WfVert) NULL);
	if (WfHoriz != 0)
	    XtVaSetValues(wCmd, NrightOf(WfHoriz) NULL);

      }
    else if (dlgCmd->cmdType == C_ToggleButton)	// normal toggle button
      {
	setLabel(lbl,label)
	wCmd = XtVaCreateManagedWidget(
	    "ToggleButtonCmd",		// name
	    togWidgetClass,		// class
	    wParent,			// parent
	    NmappedWhenManaged, map,
	    Nlabel, lbl,		// label
#ifndef Athena
	    XmNmarginTop,0,
	    XmNmarginBottom,0,
	    XmNmarginLeft,3,
	    XmNmarginRight,3,
	    XmNalignment, XmALIGNMENT_CENTER,
	    XmNborderWidth,0,
	    XmNshadowThickness, shadowThick,
	    XmNset,_retVal,
	    XmNfillOnSelect,1,
	    XmNindicatorOn,0,
#else
	    XtNstate,_retVal,
            XtNbackground, _vControlFace,
#ifdef USE3D
	    XtNhighlightThickness,0,
#endif
#endif
	    Nresizable(TRUE)
	    NULL);
	if (WfVert != 0)
	    XtVaSetValues(wCmd, Nbelow(WfVert) NULL);
	if (WfHoriz != 0)
	    XtVaSetValues(wCmd, NrightOf(WfHoriz) NULL);

	freeLabel(lbl)
      }
    else			// regular command button
      {
	setLabel(lbl,label)

#ifdef Athena
	if ((dlgCmd->attrs & CA_DefaultButton))	// default button?
	  {
	    wCmd = XtVaCreateManagedWidget(
		"ButtonCmd",		// name
		cmdWidgetClass,		// class type
		wParent,			// parent widget
		NmappedWhenManaged, map,
		Nlabel, lbl,		// label
		XtNinternalHeight,1,
		XtNborderWidth, 2,
		Nresizable(TRUE)
		NULL);
	  }
	else
#endif
	  {
	    wCmd = XtVaCreateManagedWidget(
		"ButtonCmd",		// name
		cmdWidgetClass,		// class type
		wParent,			// parent widget
		NmappedWhenManaged, map,
		Nlabel, lbl,		// label
#ifndef Athena
		XmNmarginTop,0,
		XmNmarginBottom,0,
		XmNmarginLeft,3,
		XmNmarginRight,3,
	  	XmNalignment, XmALIGNMENT_CENTER,
		XmNborderWidth,0,
		XmNshadowThickness, shadowThick,
#endif
		Nresizable(TRUE)
		NULL);
	  }

	if (WfVert != 0)
	    XtVaSetValues(wCmd, Nbelow(WfVert) NULL);
	if (WfHoriz != 0)
	    XtVaSetValues(wCmd, NrightOf(WfHoriz) NULL);

	freeLabel(lbl)
      }

#ifdef Athena
    if (dlgCmd->cmdType == C_Button)
      {
	XtVaSetValues(wCmd,         // the widget to set
            XtNbackground, _vControlFace,
	    NULL);
      }
#endif

    if (dlgCmd->cmdType == C_Button && dlgCmd->size > 0) // size - width
      {
	int w = XTextWidth(theApp->_XDefaultFont, label,strlen(label));
	if (w < dlgCmd->size)
	  {
	    XtVaSetValues(wCmd,
#ifndef Athena
	        Nwidth,dlgCmd->size+5,
#else
#ifdef USE3D
	        Nwidth,dlgCmd->size+3,
#else
	        Nwidth,dlgCmd->size,
#endif
#endif
		NULL);	// the border
	  }
      }
    else if (dlgCmd->size > 0)	// size - square (icons, color buttons)
      {
	XtVaSetValues(wCmd,
#ifndef Athena
	    Nwidth, dlgCmd->size+6,
	    Nheight, dlgCmd->size+5,
	    XmNrecomputeSize,1,
#else
#ifdef USE3D
	    Nwidth,dlgCmd->size+3,
	    Nheight, dlgCmd->size+3,
#else
	    Nwidth, dlgCmd->size,
	    Nheight, dlgCmd->size,
#endif
#endif
	    NULL);	// the border
      }

    wBox = 0;		// no Box, but make copy for placement

    if ((dlgCmd->attrs & CA_DefaultButton))	// default button?
      {
#ifdef Athena
	// make the Return key work for this button
	XtAccelerators button = XtParseAcceleratorTable(
	    "<Key>Return: set() notify() unset()\n");

	XtVaSetValues(wCmd,
	    XtNaccelerators, button,	// the accelerators
	    NULL);	// the border
#else
//	XtVaSetValues(wCmd,
//	    XmNshowAsDefault,1,
//	    NULL);	// the border
#endif
      }

#ifndef Athena
    if (dlgCmd->cmdType == C_ToggleIconButton && _itemList ||
	dlgCmd->cmdType == C_ToggleButton)	// A toggle button
      {
	XtAddCallback(wCmd, XmNvalueChangedCallback, CButtonCmdCB, (XtPointer)this);
      }
    else
	XtAddCallback(wCmd, Ncallback, CButtonCmdCB, (XtPointer)this);

#else
    XtAddCallback(wCmd, Ncallback, CButtonCmdCB, (XtPointer)this);
#endif

    // Need to set to insensitive if it is set insensitive
    if (!_Sensitive)
      {
	XtVaSetValues(wCmd,		// the widget to set
	    XtNsensitive, 0,		// change to insensitive
	    NULL);
      }
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

    _retVal = _origVal;		// restore

#ifndef Athena
    XtVaSetValues(wCmd, 		// set for this widget
	XmNset,_retVal,
	NULL);
#else
    XtVaSetValues(wCmd, 		// set for this widget
	XtNstate,_retVal,
	NULL);
#endif

    // let parent window handle now
    _parentWin->ProcessCmd(_cmdId, _retVal, dlgCmd->cmdType);
  }

//================>>> vButtonCmd::SetCmdVal <<<============================
  void vButtonCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vButtonCmd::SetCmdVal(val:%d)\n",val)

    if (st == Sensitive)
      {
	_Sensitive = val;		// set
	XtVaSetValues(wCmd,		// the widget to set
	    XtNsensitive, val,		// change sensitive
	    NULL);
      }
    else if (st == Hidden)		// hide or unhide
      {
	if (val)
	  {
	    XtUnmapWidget(wCmd);	// unmap this widget
	  }
	else
	  {
	    XtMapWidget(wCmd);	// unmap this widget
	  }
      }
    else if (st == Value || st == Checked)
      {
	_retVal = val;			// set

	XtVaSetValues(wCmd, 		// set for this widget
#ifndef Athena
	  XmNset,_retVal,
#else
	  XtNstate,_retVal,
#endif
	  NULL);
      }
  }

//================>>> vButtonCmd::SetCmdStr <<<============================
  void vButtonCmd::SetCmdStr(char* str)
  {
    // Set the string value of button

    SysDebug1(Misc,"vButtonCmd::SetCmdStr(str:%s)\n",str)

    _title = str;

    setLabel(tmp,str)

    XtVaSetValues(wCmd,		// the widget to set
	Nlabel, tmp, 		// label
	NULL);

    freeLabel(tmp)

  }

//====================>>> vButtonCmd::ButtonCmdCB <<<=======================
  void vButtonCmd::ButtonCmdCB(void)
  {
    if (dlgCmd->cmdType == C_ToggleIconButton || 
	dlgCmd->cmdType == C_ToggleButton)
	_retVal = !_retVal;

    _parentWin->ProcessCmd(_cmdId, _retVal, dlgCmd->cmdType);
  }

extern "C"
{
//============================>>> CButtonCB <<<=============================
  void CButtonCmdCB(Widget w, XtPointer This, XtPointer call_data)
  {
    ((vButtonCmd*)This)->ButtonCmdCB();
  }
}
