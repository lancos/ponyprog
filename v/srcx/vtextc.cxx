//===============================================================
// vtextc.cxx	- Text output cmd
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
#include <v/vtextc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vctlclrs.h>

extern "C"
{
#include <X11/StringDefs.h>

#ifndef Athena
#include <Xm/Xm.h>
#include <Xm/Label.h>
#else

#include <X11/Xaw/Label.h>
#include <X11/Xaw/Form.h>
#endif
}

#ifndef Athena

#define lblWidgetClass xmLabelWidgetClass
#define NborderWidth XmNborderWidth
#define Ncallback XmNactivateCallback
#define Nheight XmNheight
#define Nlabel XmNlabelString
#define NmappedWhenManaged XmNmappedWhenManaged
#define Nwidth XmNwidth
//@@ These use athena parent for now
#define Nresizable(x) // XtNresizable,x,

#define setLabel(x,y) XmString x = XmStringCreateLtoR(y,XmFONTLIST_DEFAULT_TAG);
#define freeLabel(x) XmStringFree(x);

#else	//------------------------------------------------------

#define lblWidgetClass labelWidgetClass
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


//=====================>>> vTextCmd::vTextCmd <<<=======================
  vTextCmd::vTextCmd(vCmdParent* dp, CommandObject* dc) :
	    vCmd(dp, dc)
  {
    initialize();
  }

//=====================>>> vTextCmd::~vTextCmd <<<=======================
  vTextCmd::~vTextCmd()
  {

    SysDebug(Destructor,"vTextCmd::~vTextCmd() destructor\n")

  }

//=====================>>> vTextCmd::initialize <<<=======================
  void vTextCmd::initialize(void)
  {
    // build a button command for use in a parent window


    SysDebug(Constructor,"vTextCmd::vTextCmd() constructor\n")

    CopyToLocal();		// make local copies

    Widget WfHoriz = _parentWin->getWidgetFromId(dlgCmd->cRightOf);
    Widget WfVert = _parentWin->getWidgetFromId(dlgCmd->cBelow);

    char* theText = (_itemList && *(char*)_itemList) ? (char *) _itemList
						   : _title;
    int border = (dlgCmd->attrs & CA_NoBorder) ? 0 : 1;    // border?

    int map = !(dlgCmd->attrs & CA_Hidden);

    setLabel(tmp,theText)
#ifndef Athena
    wCmd = XtVaCreateManagedWidget(
        "vText",		// name
	lblWidgetClass,		// class
	wParent,		// parent
	Nlabel, tmp, 	// label
	NborderWidth,border,	// A border!
	NmappedWhenManaged, map,
	NULL);
#else
    wCmd = XtVaCreateManagedWidget(
        "vText",		// name
	lblWidgetClass,		// class
	wParent,		// parent
	Nlabel, tmp, // label
	NborderWidth,border,	// A border!
	Nresizable(TRUE)
	NmappedWhenManaged, map,
	XtNbackground, _vControlBG,
	NULL);
#endif
    if (WfVert != 0)
	XtVaSetValues(wCmd, Nbelow(WfVert) NULL);
    if (WfHoriz != 0)
	XtVaSetValues(wCmd, NrightOf(WfHoriz) NULL);
    freeLabel(tmp)

    wBox = 0;		// no Box, but make copy for placement

  }

//================>>> vTextCmd::SetCmdVal <<<============================
  void vTextCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    if (st == Hidden)		// hide or unhide
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
  }

//================>>> vTextCmd::SetCmdStr <<<============================
  void vTextCmd::SetCmdStr(VCONST char* str)
  {

    SysDebug1(Misc,"vTextCmd::SetCmdStr(str:%s)\n",str)

    _title = str;
    setLabel(tmp,str)		// allocate label
    XtVaSetValues(wCmd,		// the widget to set
	Nlabel, tmp, // label
	NULL);
    freeLabel(tmp)


  }
