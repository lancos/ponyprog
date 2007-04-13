//===============================================================
// vspincc.cxx	- vSpinnerCmd
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vxutil.h>	// Motif/Athena mapping
#include <v/vspinc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vapp.h>
#include <v/vutil.h>
#include <v/vctlclrs.h>

extern "C"
{
#include <X11/Xlib.h>
#include <X11/StringDefs.h>

#ifndef Athena
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/ArrowB.h>
#include <Xm/Label.h>

#else

#ifdef USE3D
#include <v/awcmd3d.h>
#endif

#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
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
#define Nsensitive XmNsensitive

#define setLabel(x,y) XmString x = XmStringCreateSimple(y);
#define freeLabel(x) XmStringFree(x);

#else	//------------------------------------------------------

#ifdef USE3D
#define cmdWidgetClass  command3dWidgetClass
#else
#define cmdWidgetClass  commandWidgetClass
#endif
#define lblWidgetClass labelWidgetClass
#define NborderWidth XtNborderWidth
#define Ncallback XtNcallback
#define Nheight XtNheight
#define Nlabel XtNlabel
#define NmappedWhenManaged XtNmappedWhenManaged
#define Nwidth XtNwidth
#define Nresizable(x) XtNresizable,x,
#define Nsensitive XtNsensitive
#define setLabel(x,y) char* x = y;
#define freeLabel(x)

#endif	// -----------------------------------------------------

#ifdef Athena
#ifdef USE3D
#define up_width 5
#define up_height 3
static char up_bits[] = {
   0x04, 0x0e, 0x1f};
#define down_width 5
#define down_height 3
static char down_bits[] = {
   0x1f, 0x0e, 0x04};
#else
#define up_width 7
#define up_height 8
static char up_bits[] = {
   0x00, 0x08, 0x08, 0x1c, 0x1c, 0x3e, 0x3e, 0x00};

#define down_width 7
#define down_height 8
static char down_bits[] = {
   0x00, 0x3e, 0x3e, 0x1c, 0x1c, 0x08, 0x08, 0x00};
#endif

  static Pixmap up_bitmap = 0;
  static Pixmap down_bitmap = 0;

#endif

//=================>>> vSpinnerCmd::vSpinnerCmd <<<=======================
  vSpinnerCmd::vSpinnerCmd(vCmdParent* dp, CommandObject* co) :
	    vCmd(dp, co)
  {
    // Create bitmaps if need to

#ifdef Athena
    if (up_bitmap == 0)
      {
	up_bitmap = XCreateBitmapFromData(
	    theApp->display(),	// our display
	    DefaultRootWindow(theApp->display()),  // a Drawable
	    up_bits,			// the bitmap data
	    up_width,		// from the file
	    up_height);
      }

    if (down_bitmap == 0)
      {
	down_bitmap = XCreateBitmapFromData(
	    theApp->display(),	// our display
	    DefaultRootWindow(theApp->display()),  // a Drawable
	    down_bits,			// the bitmap data
	    down_width,		// from the file
	    down_height);
      }

#endif
    initialize();			// and initialize
  }

//=======================>>> vSpinnerCmd::~vSpinnerCmd <<<=======================
  vSpinnerCmd::~vSpinnerCmd()
  {

    SysDebug(Constructor,"vSpinnerCmd::~vSpinnerCmd() Destructor\n")

  }

//=====================>>> vSpinnerCmd::initialize <<<=======================
  void vSpinnerCmd::initialize(void)
  {

    Dimension listHeight;
    char *label, intval[20];

    SysDebug(Constructor,"vSpinnerCmd::vSpinnerCmd() constructor\n")

    CopyToLocal();		// make local copies

    // First, setup the list

    SetupList();

    // build a button command for use in a parent window

    Widget WfHoriz = _parentWin->getWidgetFromId(dlgCmd->cRightOf);
    Widget WfVert = _parentWin->getWidgetFromId(dlgCmd->cBelow);

    // We will put a box around the up/down and the label

    int map = !(dlgCmd->attrs & CA_Hidden);

#ifndef Athena
    wBox = XtVaCreateManagedWidget(
	"vSpinner",		// name
	xmFormWidgetClass,	// class
	wParent,		// parent
	XmNshadowThickness,2,
	XmNshadowType,XmSHADOW_IN,
	XmNhorizontalSpacing,0,
	XmNverticalSpacing,0,
	XmNmarginWidth,0,
	XmNmarginHeight,0,
	XmNmappedWhenManaged, map,
//	XmNrubberPositioning,1,
	NULL);
#else
    wBox = XtVaCreateManagedWidget(
	"vSpinner",		// name
	formWidgetClass,	// class
	wParent,		// parent
	NborderWidth,1,		// border around list and scrollbar
	XtNdefaultDistance, 0,
	XtNresizable, True,
	XtNmappedWhenManaged, map,
	NULL);

#endif
    if (WfVert != 0)
	XtVaSetValues(wBox, Nbelow(WfVert) NULL);
    if (WfHoriz != 0)
	XtVaSetValues(wBox, NrightOf(WfHoriz) NULL);
    

    // _maxWidth setup in SetupList
    _maxWidth = (dlgCmd->attrs & CA_Large) ? (_maxWidth * 3)/2 
					      :	_maxWidth;

    if (dlgCmd->size > 0 && dlgCmd->size < 2048)
        _maxWidth = dlgCmd->size;

    if (_TextList)
	label = _fullList[_curSelection];
    else
      {
	IntToStr(_curSelection,intval);
	label = intval;
      }

    setLabel(tmp,label)

    wCmd = XtVaCreateManagedWidget(	// The select button
        "vComboBoxButton",                    // name
        lblWidgetClass,     // class
        wBox,                // parent
	Nwidth, _maxWidth+8,	// widest string
#ifndef Athena
        Nlabel, (XmString)tmp,  // label
	XmNtopAttachment,XmATTACH_FORM,
        XmNtopOffset,2,
	XmNbottomAttachment,XmATTACH_FORM,
        XmNbottomOffset,2,
	XmNleftAttachment,XmATTACH_FORM,
        XmNleftOffset,2,
	XmNrecomputeSize,0,
#else
        Nlabel, tmp,  // label
	XtNjustify, XtJustifyLeft,
	Nresizable(FALSE)
#endif
        NULL);

    freeLabel(tmp)
    
#ifndef Athena
    _wUp = XtVaCreateManagedWidget(
	"vValueUp",		// name
	xmArrowButtonWidgetClass,	// command
	wBox,
	NrightOf(wCmd)
	XmNhighlightThickness,0,
	XmNheight,12,
	XmNwidth,15,
	XmNarrowDirection,XmARROW_UP,
	NULL);

    _wDown = XtVaCreateManagedWidget(
	"vValueDown",		// name
	xmArrowButtonWidgetClass,	// command
	wBox,
	XmNhighlightThickness,0,
	XmNheight,12,
	XmNwidth,15,
	NrightOf(wCmd)
	Nbelow(_wUp)
	XmNarrowDirection,XmARROW_DOWN,
	NULL);
#else
    _wUp = XtVaCreateManagedWidget(
	"vValueUp",		// name
	cmdWidgetClass,	// command
	wBox,
	XtNbitmap,up_bitmap,
	XtNfromVert,NULL,
	XtNvertDistance,0,	// at the top
	XtNfromHoriz, wCmd,	// right of the label
#ifdef USE3D
	XtNbackground,_vControlFace,
	XtNinternalWidth,2,
#else
	XtNinternalHeight,1,
	XtNinternalWidth,1,
#endif
	NULL);

    _wDown = XtVaCreateManagedWidget(
	"vValueDown",		// name
	cmdWidgetClass,	// command
	wBox,
	XtNbitmap,down_bitmap,
	XtNfromVert,_wUp,	// below the up arrow
	XtNfromHoriz, wCmd,	// right of the label
#ifdef USE3D
	XtNbackground,_vControlFace,
	XtNinternalWidth,2,
#else
	XtNinternalHeight,1,
	XtNinternalWidth,1,
#endif
	NULL);

#endif
    // add callback for action
    XtAddCallback(_wUp, Ncallback, CUpCB, (XtPointer)this);
    XtAddCallback(_wDown, Ncallback, CDownCB, (XtPointer)this);

    // Need to set to insensitive if it is set insensitive
    if (!_Sensitive)
      {
	XtVaSetValues(wCmd,		// the widget to set
	    Nsensitive, 0,		// change to insensitive
	    NULL);
	XtVaSetValues(_wUp,		// the widget to set
	    Nsensitive, 0,		// change to insensitive
	    NULL);
	XtVaSetValues(_wDown,		// the widget to set
	    Nsensitive, 0,		// change to insensitive
	    NULL);
      }

  }

//==================>>> vSpinnerCmd::GetCmdValue <<<=========================
  int vSpinnerCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _curSelection;
  }

//=====================>>> vSpinnerCmd::SetCmdVal <<<=========================
  void vSpinnerCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug2(Misc,"vSpinnerCmd::SetCmdVal(val:%d, type:%d)\n",val,st)

    if (st == ChangeListPtr || st == ChangeList)
      {
	if (st == ChangeListPtr)
	    _itemList = dlgCmd->itemList;
	int oldMax = _maxWidth;		// track current max width

	SetupList();			// resetup the list

	if (oldMax > _maxWidth)
	    _maxWidth = oldMax;		// don't let it get narrower

	goto SetValue;
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
    else if (st == Value)
      {
SetValue:
	_curSelection = val;

	if (_TextList)          // handle like this for text list
	  {
	    if (_curSelection < 0)
		_curSelection = 0;
	    if (_curSelection >= _numItems)
		_curSelection = _numItems - 1;

	    int newSel = _curSelection;

	    if (newSel >= 0 && newSel < _numItems)
	      {
		// Now show the new current selection

		setLabel(tstr,_fullList[_curSelection])
		XtVaSetValues(wCmd,		// the widget to set
		    Nlabel,tstr,	// the current value
		    Nwidth, _maxWidth+4,	// widest string
		    NULL);
		freeLabel(tstr)
	      }
	  }
	else                    // integer values
	  {
	    if (_itemList)
	      {
		if (_curSelection < _minVal)
		    _curSelection = _minVal;
		if (_curSelection >= _maxVal)
		    _curSelection = _maxVal;
	      }
	    char intval[30];
	    IntToStr(_curSelection, intval);
	    setLabel(tmp,intval)
	    XtVaSetValues(wCmd,		// the widget to set
		Nlabel,tmp,	// the current value
		Nwidth, _maxWidth+4,	// widest string
		NULL);
	    freeLabel(tmp)
	  }

	// Need to set explicitly if not yet realized
	if (!XtIsRealized(wCmd))
	  {
	    XtVaSetValues(wCmd, 
		Nwidth,_maxWidth,	// widest string
		NULL);
	  }
      }
    else if (st == Sensitive)
      {
	_Sensitive = val;
	XtVaSetValues(wCmd,		// the widget to set
	    Nsensitive, val,		// change to val
	    NULL);
	XtVaSetValues(_wUp,		// the widget to set
	    Nsensitive, val,		// change to val
	    NULL);
	XtVaSetValues(_wDown,		// the widget to set
	    Nsensitive, val,		// change to val
	    NULL);
      }
  }

//====================>>> vSpinnerCmd::DownUpCB <<<=======================
  void vSpinnerCmd::DownUpCB(int down)
  {

    // Change the value, either in the list or the value.
    // The increment will depend on various things

    if (_TextList)
      {
	if (down)
	  {
	    if (_TextList && _curSelection < (_numItems-1))
		_curSelection += _step;
	  }
	else 
	  {
	    if (_TextList && _curSelection > 0)
		_curSelection -= _step;
	  }
      }
    else			// up/down mean opposite for numbers
      {
	if (down)
	  {
	    if ((_maxVal == 0 && _minVal == 0) || (_curSelection - _step) >= _minVal)
		_curSelection -= _step;
	  }
	else 
	  {
	    if ((_maxVal == 0 && _minVal == 0) || (_curSelection + _step) <= _maxVal)
		_curSelection += _step;
	  }
      }
    
    // Now show the new current selection

    if (_TextList)
      {
	setLabel(tmp,_fullList[_curSelection])
	XtVaSetValues(wCmd,		// the widget to set
	    Nlabel,tmp,	// the current value
	    Nwidth, _maxWidth+4,	// widest string
	    NULL);
	freeLabel(tmp)
      }
    else
      {
	char intval[30];
	IntToStr(_curSelection, intval);
	setLabel(tmp,intval)
	XtVaSetValues(wCmd,		// the widget to set
	    Nlabel,tmp,	// the current value
	    Nwidth, _maxWidth+4,	// widest string
	    NULL);
	freeLabel(tmp)
      }

    if (!(dlgCmd->attrs & CA_NoNotify))	// Notify on each selection?
	_parentWin->ProcessCmd(_cmdId, _curSelection, dlgCmd->cmdType);
  }

extern "C"
{
//============================>>> CUpCB <<<=============================
  void CUpCB(Widget w, XtPointer This, XtPointer lr)
  {
    ((vSpinnerCmd*)This)->DownUpCB( 0 );
  }

//============================>>> CDownCB <<<=============================
  void CDownCB(Widget w, XtPointer This, XtPointer lr)
  {
    ((vSpinnerCmd*)This)->DownUpCB( 1 );
  }
}

//====================>>> vSpinnerCmd::SetupList <<<=======================
  void vSpinnerCmd::SetupList(void)
  {
    // Set up the list for use

    int width, len;

    // First, count how many items are in the supplied list

    _fullList = (char**)_itemList;		// list

    _maxWidth = 0;			// no items yet

    if (dlgCmd->attrs & CA_Text)	// A text list of values
      {
	_TextList = 1;			// This is a text list
	_step = 1;
	for ( _numItems = 0 ; _fullList[_numItems] != 0 ; ++_numItems)
	  {

	    len = strlen(_fullList[_numItems]);	// strlen
	    width = XTextWidth(theApp->_XDefaultFont,
		_fullList[_numItems], len);
	    if (width > _maxWidth)
		_maxWidth = width;		// track largest so far
	  }

	if (_numItems <= 0)
	    SysDebug(BadVals,"Bad list provided for Spinner\n");

	// Set to default value

	if (_retVal >= 0 && _retVal < _numItems)
	    _curSelection = _retVal;
	else
	    _curSelection = 0;		// item 0 by default
      }
    else				// an integer list
      {
	_TextList = 0;			// This is an integer list

	int* minMaxList = (int *) _itemList;		// list
	_minVal = 0; _maxVal = 0; _step = 1;
	if (minMaxList != 0)		// provided a range list
	  {
	    _minVal = minMaxList[0];
	    _maxVal = minMaxList[1];
	    _step = minMaxList[2];

	    // Try to make the width pretty

	    char buff[20];
	    IntToStr(_minVal,buff);
	    _maxWidth = XTextWidth(theApp->_XDefaultFont,
	   	buff, strlen(buff));
	    IntToStr(_maxVal,buff);
	    int maxW = XTextWidth(theApp->_XDefaultFont,
	   	buff, strlen(buff));
	    if (maxW > _maxWidth)
		_maxWidth = maxW;
	  }
	else
	    _maxWidth = XTextWidth(theApp->_XDefaultFont,
	   	"1234567", 10);
	_curSelection = _retVal;
      }
    _maxWidth += 2;		// just a little margin

  }
