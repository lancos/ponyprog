//===============================================================
// vframec.cxx	- a frame for holding commands
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vxutil.h>	// Motif/Athena mapping
#include <v/vapp.h>
#include <v/vframec.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vctlclrs.h>

extern "C"
{
#include <X11/StringDefs.h>

#ifndef Athena
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#else
#include <X11/Xaw/Form.h>
#endif

}

#ifndef Athena	// --------------------------------------

#define formWidgetC xmFormWidgetClass
#define Ncallback XmNactivateCallback
#define Nheight XmNheight
#define Nlabel XmNlabelString
#define NmappedWhenManaged XmNmappedWhenManaged
#define Nwidth XmNwidth
#define NborderWidth XmNborderWidth

//@@ These use athena parent for now
#define Nresizable(x) // XtNresizable,x,

#define setLabel(x,y) XmString x = XmStringCreateSimple(y);
#define freeLabel(x) XmStringFree(x);

#else	//------------------------------------------------------

#define formWidgetC formWidgetClass
#define NborderWidth XtNborderWidth
#define Ncallback XtNcallback
#define Nheight XtNheight
#define Nlabel XtNlabel
#define NmappedWhenManaged XtNmappedWhenManaged
#define Nwidth XtNwidth
#define Nresizable(x) XtNresizable,x,
#define setLabel(x,y) char* x = y;
#define freeLabel(x)

#endif	// -----------------------------------------------------


//=====================>>> vFrameCmd::vFrameCmd <<<=======================
  vFrameCmd::vFrameCmd(vCmdParent* dp, CommandObject* dc) :
    vCmd(dp, dc)
  {
    initialize();
  }

//=====================>>> vFrameCmd::~vFrameCmd <<<=======================
  vFrameCmd::~vFrameCmd()
  {

    SysDebug(Destructor,"vFrameCmd::~vFrameCmd() destructor\n")

  }

//=====================>>> vFrameCmd::initialize <<<=======================
  void vFrameCmd::initialize(void)
  {
    // build a button command for use in a parent window


    SysDebug(Constructor,"vFrameCmd::vFrameCmd() constructor\n")

    CopyToLocal();

    Widget WfHoriz = _parentWin->getWidgetFromId(dlgCmd->cRightOf);
    Widget WfVert = _parentWin->getWidgetFromId(dlgCmd->cBelow);

#ifndef Athena
    int border = (dlgCmd->attrs & CA_NoBorder) ? 0 : 3;    // border?
#else
    int border = (dlgCmd->attrs & CA_NoBorder) ? 0 : 1;    // border?
#endif

    int map = !(dlgCmd->attrs & CA_Hidden);

#ifndef Athena
    
    int margin, spacing;

    if (dlgCmd->attrs & CA_NoSpace)	// Tight spacing?
      {
	margin = 0;
	spacing = 0;
      }
    else
      {
	margin = 3;
	spacing = 2;
      }
    wBox = XtVaCreateManagedWidget(
	"vFrameWrap",			// name
	xmFrameWidgetClass,		// class
	wParent,			// parent
	Nresizable(TRUE)
	NmappedWhenManaged, map,
	XmNmarginHeight,margin,
	XmNmarginWidth,margin,
	XmNshadowThickness,border,
	XmNshadowType,XmSHADOW_ETCHED_IN,
	NULL);

    wCmd = XtVaCreateManagedWidget(
	"vFrame",			// name
	formWidgetC,			// class
	wBox,				// parent
	XmNverticalSpacing,spacing,
	XmNhorizontalSpacing,spacing,
	Nresizable(TRUE)
	NULL);

#else
    wBox = XtVaCreateManagedWidget(
	"vFrame",			// name
	formWidgetC,			// class
	wParent,			// parent
	Nresizable(TRUE)
	NmappedWhenManaged, map,
	NborderWidth, border,
	NULL);
#endif

    if (WfVert != 0)
	XtVaSetValues(wBox, Nbelow(WfVert) NULL);
    if (WfHoriz != 0)
	XtVaSetValues(wBox, NrightOf(WfHoriz) NULL);


#ifdef Athena
    if (dlgCmd->attrs & CA_NoSpace)	// Tight spacing?
      {
	XtVaSetValues(wBox,		// the widget to set
	    XtNdefaultDistance,0,
	    NULL);
      }

    XtVaSetValues(wBox,         // the widget to set
          XtNbackground, _vDialogBG, NULL);  

    wCmd = 0;		// duplicate to simplify SetCmdVal
#endif
  }

//================>>> vFrameCmd::SetCmdVal <<<============================
  void vFrameCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vFrameCmd::SetCmdVal(val:%d)\n",val)

    if (st == Hidden)		// hide or unhide
      {
	if (val)
	  {
	    ::XtUnmapWidget(wBox);	// unmap this widget
	  }
	else
	  {
	    ::XtMapWidget(wBox);	// unmap this widget
	  }
      }
    else if (st == Value && dlgCmd->cmdType == C_ToggleFrame)   // Toggle?
      {
	if (val)                // have to hide/show me first
	  {
	    ::XtMapWidget(wBox);	// unmap this widget
	  }
	else
	  {
	    ::XtUnmapWidget(wBox);	// unmap this widget
	  }
	_parentWin->SetFrameChildren(_cmdId,val); // and now set value of childen
      }
 }
