//===============================================================
// vcomboc.cxx	- vComboBoxCmd
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vxutil.h>	// Motif/Athena mapping
#include <v/vcomboc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vapp.h>
#include <v/vctlclrs.h>
#ifdef USE3D
#include <v/awcmd3d.h>
#endif

extern "C"
{
#include <X11/Xlib.h>

#ifndef Athena

#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/ArrowB.h>
#include <Xm/List.h>

#else

#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>

#endif

#include <X11/Shell.h>
}

#ifndef Athena

#define lblWidgetClass xmLabelWidgetClass
#define cmdWidgetClass xmPushButtonWidgetClass
#define Ncallback XmNactivateCallback
#define Nheight XmNheight
#define Nlabel XmNlabelString
#define NmappedWhenManaged XmNmappedWhenManaged
#define Nwidth XmNwidth
//@@ These use athena parent for now
#define Nresizable(x) // XtNresizable,x,
#define Nsensitive XmNsensitive

#define setLabel(x,y) XmString x = XmStringCreateLocalized(y);
#define freeLabel(x) XmStringFree(x);

#else	//------------------------------------------------------

#define lblWidgetClass labelWidgetClass
#ifdef USE3D
#define cmdWidgetClass command3dWidgetClass
#else
#define cmdWidgetClass commandWidgetClass
#endif
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
#define down_width 8
#define down_height 11
static char down_bits[] = {
   0x00, 0x7e, 0x7e, 0x3c, 0x3c, 0x18, 0x18, 0x00, 0x7e, 0x7e, 0x00};
#else
#define down_width 11
#define down_height 17
static char down_bits[] = {
   0xff, 0x07, 0x01, 0x04, 0x71, 0x04, 0x71, 0x04, 0xfd, 0x05, 0xfd, 0x05,
   0xf9, 0x04, 0xf9, 0x04, 0x71, 0x04, 0x71, 0x04, 0x21, 0x04, 0x21, 0x04,
   0x01, 0x04, 0xfd, 0x05, 0xfd, 0x05, 0x01, 0x04, 0xff, 0x07};
#endif

static Pixmap down_bitmap = 0;
#endif

//=================>>> vComboBoxCmd::vComboBoxCmd <<<=======================
  vComboBoxCmd::vComboBoxCmd(vCmdParent* dp, CommandObject* dc) :
	    vCmd(dp, dc)
  {
    // Create bitmaps if need to
#ifdef Athena
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

    for (int ix = 0 ; ix < MAX_COMBO_LIST ; ++ix)
	_workList[ix] = 0;

    initialize();			// and initialize
  }

//=======================>>> vComboBoxCmd::~vComboBoxCmd <<<=======================
  vComboBoxCmd::~vComboBoxCmd()
  {

    SysDebug(Constructor,"vComboBoxCmd::~vComboBoxCmd() Constructor\n")

  }

//=====================>>> vComboBoxCmd::initialize <<<=======================
  void vComboBoxCmd::initialize(void)
  {


    SysDebug(Constructor,"vComboBoxCmd::vComboBoxCmd() constructor\n")

    CopyToLocal();

    // First, setup the list

    SetupComboList();

    // build a button command for use in a parent window

    Widget WfHoriz = _parentWin->getWidgetFromId(dlgCmd->cRightOf);
    Widget WfVert = _parentWin->getWidgetFromId(dlgCmd->cBelow);

    // We will put a box around the list and the scrollbar

    int map = !(dlgCmd->attrs & CA_Hidden);

#ifndef Athena
    wBox = XtVaCreateManagedWidget(
	"ComboBox",		// name
	xmFormWidgetClass,	// class
	wParent,		// parent
	XmNshadowThickness,2,
	XmNshadowType,XmSHADOW_IN,
	XmNhorizontalSpacing,0,
	XmNverticalSpacing,0,
	XmNmarginWidth,0,
	XmNmarginHeight,0,
	XmNmappedWhenManaged, map,
	XmNrubberPositioning,1,
	NULL);
#else
    wBox = XtVaCreateManagedWidget(
	"ComboBox",		// name
	boxWidgetClass,	// class
	wParent,		// parent
	XtNorientation, XtEhorizontal,
	XtNborderWidth,1,	// border around list and scrollbar
	XtNhSpace,1,
	XtNvSpace,1,
	XtNdefaultDistance, 0,
	XtNmappedWhenManaged, map,
	XtNresizable, TRUE,
	XtNbackground,_vControlFace,
	NULL);
#endif
    if (WfVert != 0)
	XtVaSetValues(wBox, Nbelow(WfVert) NULL);
    if (WfHoriz != 0)
	XtVaSetValues(wBox, NrightOf(WfHoriz) NULL);

    // figure out the label

    wCmd = XtVaCreateManagedWidget(	// The select button
        "vComboBoxButton",                    // name
        lblWidgetClass,     // class
        wBox,                // parent
	Nwidth, _maxWidth+8,	// widest string
#ifndef Athena
        Nlabel, (XmString)_workList[_curSelection],  // label
	XmNtopAttachment,XmATTACH_FORM,
        XmNtopOffset,2,
	XmNbottomAttachment,XmATTACH_FORM,
        XmNbottomOffset,2,
	XmNleftAttachment,XmATTACH_FORM,
        XmNleftOffset,2,
	XmNrecomputeSize,0,
#else
        Nlabel, _workList[_curSelection],  // label
	XtNjustify, XtJustifyLeft,
	Nresizable(TRUE)
#endif
        NULL);

#ifndef Athena
    _downButton = XtVaCreateManagedWidget(	// the down button
	"vComboBoxDown",                     // name
	xmArrowButtonWidgetClass,         // class
	wBox,                       // parent
	XmNtopAttachment,XmATTACH_FORM,
        XmNtopOffset,2,
	XmNbottomAttachment,XmATTACH_FORM,
        XmNbottomOffset,2,
	XmNrightAttachment,XmATTACH_FORM,
        XmNrightOffset,2,
	NrightOf(wCmd)
	XmNarrowDirection,XmARROW_DOWN,
	NULL);          
#else
    _downButton = XtVaCreateManagedWidget(	// the down button
	"vComboBoxDown",                     // name
	cmdWidgetClass,         // class
	wBox,                       // parent
	XtNbitmap,down_bitmap,
	XtNbackground,_vControlFace,
#ifdef USE3D
	XtNinternalWidth, 2,
#else
	XtNinternalHeight, 0,
	XtNinternalWidth, 0,
	XtNborderWidth,0,
#endif
	NULL);          
#endif

    _popup = XtVaCreatePopupShell(	// This is a popup for the list
	"vComboPopup",
	overrideShellWidgetClass,
	wParent,			// parent
	XtNoverrideRedirect,1,
	XtNallowShellResize,1,
	NULL);
	
    PoppedUp = 0;

#ifndef Athena
    int maxShow = (_numItems > 8) ? 8 : _numItems;

    _popupList = XtVaCreateManagedWidget(
	"vComboBoxList",			// name
	xmListWidgetClass,	// class
	_popup,
	XmNitemCount, _numItems,	// this many
	XmNitems, (XmString)_workList,		// the work list
	XmNwidth, _maxWidth + 8,		// widest string
	XmNlistSizePolicy, XmCONSTANT,
	XmNvisibleItemCount, maxShow,
	NULL);
    XtAddCallback(_popupList, XmNsingleSelectionCallback, CComboBoxCmdCB, (XtPointer)this);
    XtAddCallback(_popupList, XmNbrowseSelectionCallback, CComboBoxCmdCB, (XtPointer)this);
#else
    _popupList = XtVaCreateManagedWidget(
	"vComboBoxList",			// name
	listWidgetClass,	// class
	_popup,
	XtNdefaultColumns, 1,	// 1 column
	XtNforceColumns, True,	// force columns
	XtNlist, _workList,	// the work list
	XtNlongest, _maxWidth,	// widest string
	XtNnumberStrings, _numItems,	// always MAX_LIST strings
	XtNverticalList, TRUE,	// we are a vertical list
	XtNrowSpacing, 1,
	XtNresizable, TRUE,
	NULL);
#endif

    // add callback for actions
    XtAddCallback(_downButton, Ncallback, CComboDownCB, (XtPointer)this);
#ifdef Athena
    XtAddCallback(_popupList, XtNcallback, CComboBoxCmdCB, (XtPointer)this);
#endif

    if (!_Sensitive)
      {
	XtVaSetValues(wCmd,             // the widget to set
	    Nsensitive, 0,            // change to insensitive
	    NULL);
	XtVaSetValues(_downButton,	// the widget to set
	    Nsensitive, 0,		// change to val
	    NULL);
      }  
  }

//==================>>> vComboBoxCmd::GetCmdValue <<<=========================
  int vComboBoxCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _curSelection;
  }

//=====================>>> vComboBoxCmd::SetCmdVal <<<=========================
  void vComboBoxCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug2(Misc,"vComboBoxCmd::SetCmdVal(val:%d, type:%d)\n",val,st)


    if (st == ChangeList || st == ChangeListPtr)
      {
	if (st == ChangeListPtr)
	    _itemList = dlgCmd->itemList;
	int oldMax = _maxWidth;		// track current max width

	SetupComboList();			// resetup the list
	if (oldMax > _maxWidth)
	    _maxWidth = oldMax;		// don't let it get narrower

	// turn them all off!
#ifndef Athena
	XmListReplaceItemsPosUnselected(_popupList, (XmString*)_workList, _numItems, 1);
#else
	XawListChange(_popupList, _workList, _numItems, _maxWidth, True);
#endif
	if (val < 0)		// unselect
	    val = 0;
	else if (val >= _numItems )
	    val = _numItems - 1;

	_retVal =
	_curSelection = val;	// change the current value

	XtVaSetValues(wCmd,		// the widget to set
#ifndef Athena
	    Nlabel, (XmString)_workList[_curSelection],  // label
#else
	    Nlabel, _workList[_curSelection],  // label
#endif
	    Nwidth, _maxWidth+8,	// widest string
	    NULL);
	return;
      }
    else if (st == Hidden)		// hide or unhide
      {
	if (val)
	  {
	    XtUnmapWidget(wBox);	// unmap this widget
	  }
	else
	  {
	    XtMapWidget(wBox);	// unmap this widget
	  }
      }
    else if (st == Checked)	// fake out to pop down
      {
	if (PoppedUp)
	  {
	    XtPopdown(_popup);		// another way to get it down
	    PoppedUp = 0;
	  }
      }
    else if (st == Value)
      {
	if (val < 0)		// unselect
	    val = 0;
	else if (val >= _numItems )
	    val = _numItems - 1;

	// Now set appropriate _curSelection

	_retVal =
	_curSelection = val;	// change the current value
	XtVaSetValues(wCmd,		// the widget to set
#ifndef Athena
	    Nlabel, (XmString)_workList[_curSelection],  // label
#else
	    Nlabel, _workList[_curSelection],  // label
#endif
	    Nwidth, _maxWidth+8,	// widest string
	    NULL);
      }
    else if (st == Sensitive)
      {
	_Sensitive = val;        // set
	XtVaSetValues(wCmd,		// the widget to set
	    Nsensitive, val,		// change to val
	    NULL);
	XtVaSetValues(_downButton,	// the widget to set
	    Nsensitive, val,		// change to val
	    NULL);
      }
  }

#ifdef USECMDBUTTON
//====================>>> vComboBoxCmd::ComboAcceptCB <<<=======================
  void vComboBoxCmd::ComboAcceptCB(void)
  {

    if (PoppedUp)
      {
	XtPopdown(_popup);		// another way to get it down
	PoppedUp = 0;
      }
    else				// ok - the user has selected it
      {
	_parentWin->ProcessCmd(_cmdId,
	    (ItemVal)_curSelection, dlgCmd->cmdType);
      }
  }

extern "C"
{
//============================>>> CComboAcceptCB <<<=============================
  void CComboAcceptCB(Widget w, XtPointer This, XtPointer lr)
  {
    // call the C++ version
    ((vComboBoxCmd*)This)->ComboAcceptCB();
  }
}
#endif


//====================>>> vComboBoxCmd::ComboDownCB <<<=======================
  void vComboBoxCmd::ComboDownCB(void)
  {
    Position x, y;
    Dimension height;

    if (!PoppedUp)
      {
	// We have to calculate a bit to find where to but the combobox
	XtTranslateCoords(wBox,		// Get where the ComboBox wbox is
	    (Position) 0, (Position) 0, &x, &y);

	XtVaGetValues(wBox,		// and the size
	    XtNheight,&height,
	    NULL);

	XtVaSetValues(_popup,		// set loc of popup
	    XtNx, x,
	    XtNy, y+height,
	    NULL);

  	// I can't quite get this to work just how I want. Ideally,
	// nothing will happen until they pop down the combobox.
	// Unfortunately, GrabExclusive seems to work ok for MODAL
	// dialogs, but not for modeless or the command bar.
	// GrabNone lets the combo hang down while they select
	// other things in the dialog. This seems less messy
	// than inconsistent operation, and they really want to
	// press the arrow to unpop this thing.

//	XtPopup(_popup,XtGrabExclusive);	// If we want modal popup
	XtPopup(_popup,XtGrabNone);
	theApp->XWaitUntilMapped(_popup);
	PoppedUp = 1;
      }
    else
      {
	XtPopdown(_popup);		// another way to get it down
	PoppedUp = 0;
      }
  }

extern "C"
{
//============================>>> CComboDownCB <<<=============================
  void CComboDownCB(Widget w, XtPointer This, XtPointer lr)
  {
    // call the C++ version
    ((vComboBoxCmd*)This)->ComboDownCB();
  }
}


//====================>>> vComboBoxCmd::ComboBoxCmdCB <<<=======================
  void vComboBoxCmd::ComboBoxCmdCB(int index)
  {
    _curSelection = index;	// change the current selection

    XtPopdown(_popup);		// take down the combo part
    PoppedUp = 0;
    XtVaSetValues(wCmd,
#ifndef Athena
        Nlabel, (XmString)_workList[_curSelection],  // change to current item
#else
        Nlabel, _workList[_curSelection],  // change to current item
#endif
	Nwidth, _maxWidth+8,	// widest string
	NULL);

    // We will dynamically notify the program that the selection
    // changed -- this may not be the best behavior, and
    // we might change it.

    if (!(dlgCmd->attrs & CA_NoNotify))
      {
	_parentWin->ProcessCmd(_cmdId,
	    (ItemVal)_curSelection, dlgCmd->cmdType);
      }
  }

extern "C"
{
//============================>>> CComboBoxCmdCB <<<=============================
  void CComboBoxCmdCB(Widget w, XtPointer This, XtPointer lr)
  {
#ifndef Athena
    // need to adjust Motif's indexing from 1 to our indexing from 0
    XmListCallbackStruct* xml = (XmListCallbackStruct*) lr;
    ((vComboBoxCmd*)This)->ComboBoxCmdCB(xml->item_position - 1); 
#else
    ((vComboBoxCmd*)This)->
	ComboBoxCmdCB(((XawListReturnStruct*)lr)->list_index);
#endif
  }
}


//====================>>> vComboBoxCmd::SetupComboList <<<=======================
  void vComboBoxCmd::SetupComboList(void)
  {
    // Set up the list for use

    int width, len;

    // First, count how many items are in the supplied list


    _fullList = (char**)_itemList;		// list

    _maxWidth = 0;			// no items yet

    for ( _numItems = 0 ; _fullList[_numItems] != 0 ; ++_numItems)
      {
	if (_numItems < MAX_COMBO_LIST)	// only allow MAX_LIST items
	  {
#ifndef Athena
	    if (_workList[_numItems] != 0)
		freeLabel((XmString)_workList[_numItems])
	    _workList[_numItems] = (char*)XmStringCreateLocalized(_fullList[_numItems]);
#else
	    _workList[_numItems] = _fullList[_numItems];
#endif
	  }
	else
	  {
	    SysDebug(BadVals,"Too many items for a combo box");
	  }

	len = strlen(_fullList[_numItems]);	// strlen
	width = XTextWidth(theApp->_XDefaultFont,
		_fullList[_numItems], len);
	if (width > _maxWidth)
	    _maxWidth = width;		// track largest so far
      }

    _workList[_numItems] = NULL;	// zap end of the list

    if (_numItems <= 0)
      {
	SysDebug(BadVals,"No Items supplied for ComboBox -- need 1!\n")
      }

    _curSelection = _retVal;	// the default

    if (_curSelection < 0)		// make a safe default choice
	_curSelection = 0;
    else if (_curSelection >= _numItems)
	_curSelection = _numItems - 1;

    // Note that at this point _numItems is how many items are
    // in the user supplied list. 

  }
