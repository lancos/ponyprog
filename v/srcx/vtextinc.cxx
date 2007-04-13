//===============================================================
// vtextinc.xxx	- TextIn Cmd
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
#include <v/vtextinc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent

extern "C"
{
#include <X11/StringDefs.h>


#ifndef Athena
#include <Xm/Xm.h>
#include <Xm/TextF.h>
#else

#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Form.h>

#endif
}

#ifndef Athena
#define Nsensitive XmNsensitive
#else
#define Nsensitive XtNsensitive
#endif

    vTextInCmd* vTextInCmd::_TList = 0;   // initialize static class data

//=====================>>> vTextInCmd::vTextInCmd <<<=======================
  vTextInCmd::vTextInCmd(vCmdParent* dp, CommandObject* co) :
    vCmd(dp, co)
  {
    initialize();
  }

//=====================>>> vTextInCmd::~vTextInCmd <<<=======================
  vTextInCmd::~vTextInCmd()
  {

    SysDebug(Destructor,"vTextInCmd::~vTextInCmd() destructor\n")

    // have to remove ourself from the list of all TextInCmds
    if (_TList == this)		// first one special
      {
	_TList = _nextT;
      }
    else
      {
	for (vTextInCmd* tl = _TList ; tl != 0 ; tl = tl->_nextT)
	  {
	    if (tl->_nextT == this)		// we found ourself!
	      {
		tl->_nextT = _nextT;		// unlink us
		break;
	      }
	  }
      }
  }

//=====================>>> vTextInCmd::initialize <<<=======================
  void vTextInCmd::initialize(void)
  {
    // build a TextIn command for use in a parent window

    SysDebug(Constructor,"vTextInCmd::vTextInCmd() constructor\n")

    CopyToLocal();		// make local copies

    Widget WfHoriz = _parentWin->getWidgetFromId(dlgCmd->cRightOf);
    Widget WfVert = _parentWin->getWidgetFromId(dlgCmd->cBelow);

    static XtActionsRec actions[] =
      {
	{"CtextReturnA",CtextReturnA},
      };

    int defLen = strlen(_title);	// length of supplied string

    int defWidth;

    if (dlgCmd->size <= 0)		// BEW: 7/24/96 - for V 1.13
      {
	defWidth = (dlgCmd->attrs & CA_Large) ? 350 : 200;

	if (dlgCmd->attrs & CA_Small)	// allow small, too.
	    defWidth = 100;
      }
    else
      {
#ifndef Athena
	defWidth = (dlgCmd->size+2) * XTextWidth(theApp->_XDefaultFont, "M",1);
#else
	defWidth = dlgCmd->size * XTextWidth(theApp->_XDefaultFont, "M",1) + 2;
#endif
	if (defWidth > 800)		// Sanity check
	    defWidth = 500;
      }

    _nextT = _TList;
    _TList = this;		// add us in at front of list

    wBox = 0;

    int map = !(dlgCmd->attrs & CA_Hidden);

#ifndef Athena
    wCmd = XtVaCreateManagedWidget(
	"vTextIn",		// name
	xmTextFieldWidgetClass,	// class
	wParent,		// parent
	XtNvalue, _title,
	XmNcursorPosition, defLen,
	XmNwidth, defWidth,
	XmNmappedWhenManaged, map,
	NULL);

    if (dlgCmd->attrs & CA_Password)		// password entry
      {
	Pixel bgcolor;
	XtVaGetValues(wCmd,			// the text's widget
	    XmNbackground, &bgcolor,		// the text's bg color
	    NULL);
	XtVaSetValues(wCmd,			// the text's widget
	    XmNforeground, bgcolor,		// make fg = bg to be invisible
	    NULL);
      }


#else

    wCmd = XtVaCreateManagedWidget(
	"vTextIn",		// name
	asciiTextWidgetClass,	// class
	wParent,		// parent
	XtNstring, _title,
	XtNeditType, XawtextEdit,	// editable
	XtNinsertPosition, defLen,
	XtNresizable, True,
	XtNwidth, defWidth,
	XtNmappedWhenManaged, map,
	XtNecho, !(dlgCmd->attrs & CA_Password),
	NULL);

    // kill off the search and return functions
    XtTranslations txtT =
      XtParseTranslationTable("<Key>Return: CtextReturnA()\n\
	Ctrl<Key>R: no-op(RingBell)\n\
	Ctrl<Key>S: no-op(RingBell)\n");

    XtOverrideTranslations(wCmd, txtT);
    XtAppAddActions(theApp->appContext(), actions, XtNumber(actions));
#endif
    if (WfVert != 0)
	XtVaSetValues(wCmd, Nbelow(WfVert) NULL);
    if (WfHoriz != 0)
	XtVaSetValues(wCmd, NrightOf(WfHoriz) NULL);

    // Need to set to insensitive if it is set insensitive
    if (!_Sensitive)
      {
	XtVaSetValues(wCmd,		// the widget to set
	    Nsensitive, 0,		// change to insensitive
	    NULL);
      }
  }

//=====================>>> vTextInCmd::GetTextIn <<<=======================
  int vTextInCmd::GetTextIn(ItemVal /* id */, char* str, int maxlen) VCONST
  {
    // recover the data from a TextInCmd - return text + length

    String text;			// X type

    XtVaGetValues(wCmd,			// the text's widget
#ifndef Athena
	XmNvalue, &text,		// the text's address
#else
	XtNstring, &text,		// the text's address
#endif
	NULL);

    // now, copy the string from the widget to the user
    // we will take account for length, too.
    char* toP = str;
    int l = 0;				// track the length

    for (char* fromP = (char*)text ; *fromP && l < maxlen ; ++l)
      {
	*toP++ = *fromP++;
      }
    *toP = 0;			// terminate the string
    return l;
  }

//================>>> vTextInCmd::SetCmdVal <<<============================
  void vTextInCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vTextInCmd::SetCmdVal(val:%d)\n",val)

    if (st == Sensitive)
      {
	_Sensitive = val;		// set
	XtVaSetValues(wCmd,		// the widget to set
	    Nsensitive, val,		// change sensitive
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
    else
      {
	SysDebug2(BadVals,"vTextInCmd::SetCmdVal(val:%d,type:%d) - BAD TYPE\n",val,st)
      }

  }

//================>>> vTextInCmd::SetCmdStr <<<============================
  void vTextInCmd::SetCmdStr(VCONST char* str)
  {

    SysDebug1(Misc,"vTextInCmd::SetCmdStr(str:%s)\n",str)

    int defLen = strlen(str);	// length of supplied string

    XtVaSetValues(wCmd,		// the widget to set
#ifndef Athena
	XmNvalue, str,
#else
	XtNstring, str,
#endif
	NULL);

    XtVaSetValues(wCmd,		// the widget to set
	XtNinsertPosition, defLen,
	NULL);
  }

extern "C"
{
//==========================>>> CtextReturnA <<<=============================
  void CtextReturnA(Widget w, XEvent* ev, String* pars, Cardinal* np)
  {
    // Make the Return key return from the dialog if there is a default cmd.

    // _TList is a static, so we can get the this back by looking
    // for the proper widget.

    for (vTextInCmd* tl = vTextInCmd::_TList ; tl != 0 ; tl = tl->_nextT)
      {
	if (tl->wCmd == w)		// we've found our widget!
	  {
	    tl->textReturn();
	    break;
	  }
      }

  }
}

//=====================>>> vTextInCmd::textReturn <<<=======================
  void vTextInCmd::textReturn(void)
  {
    //@@@ This will interact with the Dialog to return if there
    //	was a default button.
    SysDebug(Misc,"vTextInCmd::textReturn\n")

    _parentWin->ProcessCmd(_cmdId, _retVal, C_TextIn);

  }
