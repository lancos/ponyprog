//===============================================================
// vlistc.cxx	- ListCmd
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vxutil.h>	// Motif/Athena mapping
#include <v/vlistc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vapp.h>
#include <v/vctlclrs.h>

extern "C"
{
#include <X11/Xlib.h>

#ifndef Athena
#include <Xm/List.h>
#include <Xm/Form.h>

#else	// ### ATHENA

#include <X11/Xaw/Form.h>
#ifndef DONTUSEx3D
#include <v/awscl3d.h>
#define sbWidgetClass scrollbar3dWidgetClass
#define sbThickness 15
#else
#include <X11/Xaw/Scrollbar.h>
#define sbWidgetClass scrollbarWidgetClass
#define sbThickness 10
#endif
#include <X11/Xaw/List.h>

#endif

}

#define DefaultWidth 32

#ifndef Athena
#define Nsensitive XmNsensitive
#else
#define Nsensitive XtNsensitive
#endif

    static char* EmptyItem = "";	// null item for short lists

//=================>>> vListCmd::vListCmd <<<=======================
  vListCmd::vListCmd(vCmdParent* dp, CommandObject* dc) :
	    vCmd(dp, dc)
  {
    // Create bitmaps if need to

#ifdef Athena
    _fscale = 0.0;
    _scale = 0;
#endif
    _maxWidth = DefaultWidth;
    initialize();			// and initialize
  }

//=======================>>> vListCmd::~vListCmd <<<=======================
  vListCmd::~vListCmd()
  {

    SysDebug(Constructor,"vListCmd::~vListCmd() Destructor\n")
#ifndef Athena
    // now, free the XmString list we created
    for (int iy = 0 ; iy < _listItems ; ++iy)
	XmStringFree(_strList[iy]);
    XtFree((char*)_strList);
#endif

  }

//=====================>>> vListCmd::initialize <<<=======================
  void vListCmd::initialize(void)
  {

    static char empty[] = " ";
#ifdef Athena
    XtCallbackRec scrollProcList[] =
      {
	{ CScrollProcCB, this },
	{ (XtCallbackProc) 0, (XtPointer) 0}
      };

    XtCallbackRec jumpProcList[] =
      {
	{ CJumpProcCB, this },
	{ (XtCallbackProc) 0, (XtPointer) 0}
      };
#endif

    Dimension listHeight;

    SysDebug(Constructor,"vListCmd::vListCmd() constructor\n")

    CopyToLocal();		// make local copies

    // First, setup the list

    _listRows = 8;
    if (dlgCmd->attrs & CA_Size)	// specifying size
      {
	if (dlgCmd->size > 0 && dlgCmd->size <= MAX_LIST)
	    _listRows = dlgCmd->size;
      }

#ifdef Athena
    for (int ix = 0 ; ix < _listRows ; ++ix)
	_workList[ix] = 0;
#endif

    SetupList();

    // determine positions

    Widget WfHoriz = _parentWin->getWidgetFromId(dlgCmd->cRightOf);
    Widget WfVert = _parentWin->getWidgetFromId(dlgCmd->cBelow);

    // We will put a box around the list and the scrollbar in Athena

    int map = !(dlgCmd->attrs & CA_Hidden);

    if (dlgCmd->attrs & CA_ListWidth && dlgCmd->retVal > 0)	// Specifying width
	_maxWidth = dlgCmd->retVal;
    else if (dlgCmd->attrs & CA_Large)
      {
	if (_maxWidth > DefaultWidth)
		_maxWidth = (_maxWidth * 3) / 2;	// 150%
	else
	    _maxWidth = DefaultWidth * 2;
      }

#ifndef Athena

    wBox = XtVaCreateManagedWidget(
	"vListFrame",		// name
	xmFormWidgetClass,	// class
	wParent,		// parent
	XmNborderWidth,0,	// no border around list and scrollbar
	XtNmappedWhenManaged, map,
	NULL);

    // convert list to XmStrings 

    if (_numItems == 0)			// gotta fake it out for initial
      {
	++_numItems;
	_strList = (XmStringTable) XtMalloc((_numItems+1) * sizeof (XmString*));
	_listItems = _numItems;
	_strList[0] = XmStringCreateLocalized(empty);
	_strList[_numItems] = 0;	// 1.24
      }
    else
      {
	_strList = (XmStringTable) XtMalloc((_numItems+1) * sizeof (XmString*));
	_listItems = _numItems;
	for (int ix = 0 ; ix < _numItems ; ++ix)
	    _strList[ix] = XmStringCreateLocalized(_fullList[ix]);
	_strList[_numItems] = 0;	// 1.24
      }

    // Create a scrolled list 
    // We need to use the Arg list to set values because some
    // can't be reset after the list is created.

    Arg args[12];
    int argc;
    argc = 0;
    XtSetArg(args[argc], XmNscrollBarDisplayPolicy,XmSTATIC);	argc++;
    XtSetArg(args[argc], XmNvisualPolicy,XmCONSTANT);		argc++;
    XtSetArg(args[argc], XmNscrollingPolicy,XmAUTOMATIC);	argc++;
    XtSetArg(args[argc], XmNitems, (XmString)_strList);		argc++;
    XtSetArg(args[argc], XmNitemCount, _listItems);		argc++;
    XtSetArg(args[argc], XmNwidth, _maxWidth + 8);		argc++;
    XtSetArg(args[argc], XmNvisibleItemCount,_listRows);	argc++;
    XtSetArg(args[argc], XmNlistSizePolicy,XmCONSTANT);		argc++;

    wCmd = XmCreateScrolledList(wBox,"vList",args,argc);

    // add callback for action
    XtAddCallback(wCmd, XmNsingleSelectionCallback, CListCmdCB, (XtPointer)this);
    XtAddCallback(wCmd, XmNbrowseSelectionCallback, CListCmdCB, (XtPointer)this);

    // place in Frame

    if (WfVert != 0)
	XtVaSetValues(wBox, Nbelow(WfVert) NULL);
    if (WfHoriz != 0)
	XtVaSetValues(wBox, NrightOf(WfHoriz) NULL);

    XtManageChild(wCmd);		// have to manage the list child!

#else	// ################ ATHENA

    wBox = XtVaCreateManagedWidget(
	"vListFrame",		// name
	formWidgetClass,	// class
	wParent,		// parent
	XtNorientation, XtEhorizontal,
	XtNborderWidth,1,	// border around list and scrollbar
	XtNdefaultDistance, 1,
	XtNresizable, True,
	XtNmappedWhenManaged, map,
	XtNbackground, _vControlFace,
	NULL);

    if (WfVert != 0)
	XtVaSetValues(wBox, Nbelow(WfVert) NULL);
    if (WfHoriz != 0)
	XtVaSetValues(wBox, NrightOf(WfHoriz) NULL);

    wCmd = XtVaCreateManagedWidget(
	"vList",			// name
	listWidgetClass,	// class
	wBox,			// parent
	XtNdefaultColumns, 1,	// 1 column
	XtNforceColumns, True,	// force columns
	XtNlist, _workList,	// the work list
	XtNlongest, _maxWidth,	// widest string
	XtNnumberStrings, _listRows, // _listRows strings
	XtNverticalList, TRUE,	// we are a vertical list
	XtNrowSpacing, 1,
	XtNresizable, True,
	NULL);

    // add callback for action
    XtAddCallback(wCmd, XtNcallback, CListCmdCB, (XtPointer)this);
    
    // Create the scroll bar

    XtVaGetValues(wCmd, XtNheight, &listHeight, NULL);

    Dimension min_thumb = 10;

    _wScroll = XtVaCreateManagedWidget(
	"vListScroll",		// name
	sbWidgetClass,	// class
	wBox,			// parent
	XtNscrollProc, scrollProcList,	// callback for scrolling
	XtNjumpProc, jumpProcList,	// callback for scrolling
	XtNminimumThumb,min_thumb,
	XtNthickness, sbThickness,	// how wide
	XtNlength,listHeight,	// high as list
	XtNfromHoriz, wCmd,
	XtNresizable, TRUE,
#ifndef DONTUSEx3D
        XtNbackground, _vControlFace, 
        XtNscrollbar3dBackground, _vControlBG,
#endif  
	NULL);

    SetScroll(_ScrlShown, _ScrlTop);

#endif	// End of Motif/Athena sections


    // Need to set to insensitive if it is set insensitive
    if (!_Sensitive)
      {
	XtVaSetValues(wCmd,		// the widget to set
	    Nsensitive, 0,		// change to insensitive
	    NULL);
#ifdef Athena
	XtVaSetValues(_wScroll,		// the widget to set
	    Nsensitive, 0,		// change to insensitive
	    NULL);
#endif
      }

  }

//==================>>> vListCmd::GetCmdValue <<<=========================
  int vListCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _curSelection;
  }

//=====================>>> vListCmd::SetCmdVal <<<=========================
  void vListCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug2(Misc,"vListCmd::SetCmdVal(val:%d, type:%d)\n",val,st)

    static char empty[] = " ";

    if (st == ChangeList || st == ChangeListPtr)
      {
	if (st == ChangeListPtr)
	    _itemList = dlgCmd->itemList;

	int oldMax = _maxWidth;		// track current max width

	SetupList();			// resetup the list

	if (oldMax > _maxWidth)
	    _maxWidth = oldMax;		// don't let it get narrower

#ifndef Athena
	XmListDeleteAllItems(wCmd);	// clear the list

	// cleanup old copy of the list
	if (_strList != 0)
	  {
	    // now, free the XmString list we created
	    for (int iy = 0 ; iy < _listItems ; ++iy)
		XmStringFree(_strList[iy]);
	    XtFree((char*)_strList);
	  }

	// reallocate
	_strList = (XmStringTable) XtMalloc((_numItems+1) * sizeof (XmString*));

	for (int ix = 0 ; ix < _numItems ; ++ix)
          {
	    if (!*_fullList[ix])
		_strList[ix] = XmStringCreateLocalized(empty);
	    else
		_strList[ix] = XmStringCreateLocalized(_fullList[ix]);
	  }
	_strList[_numItems] = 0;	// 1.24
	_listItems = _numItems;

//1.24	XmListAddItemsUnselected(wCmd, _strList, _numItems, 1);  // add the new list

	if (_numItems > 0)
	    XtVaSetValues(wCmd, 
		XmNitemCount, _numItems,	// 1.24
		XmNitems, _strList,		// 1.24
		XmNwidth, _maxWidth+20,	// widest string
		NULL);
#else
	// turn them all off!
	XawListChange(wCmd, _workList, _listRows, _maxWidth, True);
	SetScroll(_ScrlShown, _ScrlTop);
#endif
	SetCmdVal(val,Value);
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
    else if (st == Value)
      {
	if (val >= _numItems )
	    return;

	if (val < 0)		// unselect
	  {
	    _curSelection = -1;
	    // turn them all off!
#ifndef Athena
	    XmListDeselectAllItems(wCmd);
#else
	    XawListChange(wCmd, _workList, _listRows, _maxWidth, True);
#endif
	    return;
	  }

	// Now set appropriate _curSelection, scroll, and highlight

	_curSelection = val;	// change the current value

	// if the selection is already on the screen, then highlight it
	// See if the currently selected item is on the list, and rehighlight

#ifndef Athena
	int newSel = _curSelection;
	if (newSel >= 0 && newSel < _listRows)
	  {
	    XmListDeselectAllItems(wCmd);
	    XmListSelectPos(wCmd, newSel+1, 0);
	  }
	// Need to set explicitly if not yet realized
	if (!XtIsRealized(wCmd))
	    XtVaSetValues(wCmd, 
		XmNwidth, _maxWidth+20,	// widest string
		NULL);

#else	// ##### ATHENA

	int newSel = _curSelection - _workMapsTo;

	if (newSel >= 0 && newSel < _listRows)
	  {
	    XawListChange(wCmd, _workList, _listRows, _maxWidth, True);
	    XawListHighlight(wCmd, newSel);
	  }
	else			// have to scroll
	  {
	    DoScroll(_curSelection - _workMapsTo);  // change what is displayed
	  }

	// Need to set explicitly if not yet realized
	if (!XtIsRealized(wCmd))
	  {
	    XtVaSetValues(wCmd, 
		XtNlongest, _maxWidth,	// widest string
		NULL);
	  }
#endif
      }
    else if (st == Sensitive)
      {
	_Sensitive = val;
#ifndef Athena
	XtVaSetValues(wBox,		// the widget to set
	    Nsensitive, val,		// change to val
	    NULL);
#else
	XtVaSetValues(wCmd,		// the widget to set
	    Nsensitive, val,		// change to val
	    NULL);
	XtVaSetValues(_wScroll,		// the widget to set
	    Nsensitive, val,		// change to val
	    NULL);
#endif
      }
  }

//====================>>> vListCmd::ListCmdCB <<<=======================
  void vListCmd::ListCmdCB(int index)
  {
    _curSelection = index + _workMapsTo;  // change the current selection

    if (_curSelection >= _numItems )
	_curSelection = -1;

    if (!(dlgCmd->attrs & CA_NoNotify))	// Notify on each selection?
	_parentWin->ProcessCmd(_cmdId, _curSelection, dlgCmd->cmdType);
  }

#ifdef Athena
//====================>>> vListCmd::DoScroll <<<==========================
  void vListCmd::DoScroll(int cnt)
  {
    // Scroll the list by cnt lines

    int newTop;
    int maxMap = _numItems - _listRows;

    if (_numItems <= _listRows)	// not big enough to scroll
	return;

    newTop = _workMapsTo + cnt;	// where we would go
    if (cnt > 0)		// scrolling up
      {
	if (_workMapsTo == maxMap)	// already at bottom
	    return;
	_workMapsTo = (newTop > maxMap) ? maxMap : newTop;
      }
    else
      {
	if (_workMapsTo == 0)		// already at top
	    return;
	_workMapsTo = (newTop < 0) ? 0 : newTop;
      }

    // Now redraw list
    for (int ix = 0 ; ix < _listRows ; ++ix)
      {
	_workList[ix] = _fullList[ix + _workMapsTo];
      }

    XawListChange(wCmd, _workList, _listRows, _maxWidth, True);

    // See if the currently selected item is on the list, and rehighlight
    int newSel = _curSelection - _workMapsTo;

    if (newSel >= 0 && newSel < _listRows)
      {
	XawListHighlight(wCmd, newSel);
      }
    // _workMapsTo varies from 0 to _numItems - _listRows
    // _ScrlTop varies from 0 to 100 - _ScrlShown

    // Use _scale to transform the current _workMapsTo to a _ScrlTop

    if (_workMapsTo == (_numItems - _listRows))	// Account for integer scaling
	_ScrlTop = 100 - _ScrlShown;

    if (_fscale != 0.0)
	_ScrlTop = (int)((float)_workMapsTo * _fscale);
    else if (_scale != 0)
	_ScrlTop = _workMapsTo * _scale;
    else
	_ScrlTop = (int) ((float)_workMapsTo * _scale);

    SetScroll(_ScrlShown, _ScrlTop);
  }

//====================>>> vListCmd::JumpProcCB <<<=======================
  void vListCmd::JumpProcCB(int percent)
  {
    int shown = _ScrlShown;
    int min_top;
    int newMap;

    min_top = 100 - shown;

    if (percent > min_top)	// have to fix it
      {
	SetScroll(shown, min_top);
	percent = min_top;
      }

    // See if we have changed anything
    if (_fscale != 0.0)
      {
	newMap = (int)((float)percent / _fscale) + 1;
	if (newMap == 1)	// Kludge to get to 0
	    newMap = 0;
      }
    else if (_scale == 0)		// nothing to change
      {
	return;
      }
    else
	newMap = (int)((float)percent / _scale);

    if (newMap != _workMapsTo)		// we've changed the screen
      {
	DoScroll(newMap - _workMapsTo);  // change what is displayed
      }
  }

//====================>>> vListCmd::SetScroll <<<=======================
  void vListCmd::SetScroll(int Shown, int Top)
  {
    Arg args[3];		// Used to pass float value

    union
      {
	XtArgVal arg_value;
	float float_value;
      } shown_value, top_value;

    shown_value.float_value = float (Shown / 100.0);
    top_value.float_value = float (Top / 100.0);

    if (sizeof (float) > sizeof (XtArgVal))
      {
	XtSetArg(args[0], XtNtopOfThumb, &top_value);
	XtSetArg(args[1], XtNshown, &shown_value);
      }
    else
      {
	XtSetArg(args[0], XtNtopOfThumb, top_value.arg_value);
	XtSetArg(args[1], XtNshown, shown_value.arg_value);
      }

    XtSetValues(_wScroll, args, 2);		// change it!
  }

//====================>>> vListCmd::ScrollProcCB <<<=======================
  void vListCmd::ScrollProcCB(int position)
  {
    if (position  > 0)		// scroll list up one line
	DoScroll(1);
    else
	DoScroll(-1);		// scroll list down one line

  }
#endif	// Athena scrolling stuff

//====================>>> vListCmd::SetupList <<<=======================
  void vListCmd::SetupList(void)
  {
    // Set up the list for use

    int width, len;

    // First, count how many items are in the supplied list

    _workMapsTo = 0;
    _curSelection = -1;

    _fullList = (char**)_itemList;		// list

    for ( _numItems = 0 ; _fullList && _fullList[_numItems] != 0 ; ++_numItems)
      {
#ifdef Athena
	if (_numItems < _listRows)	// copy only first _listRows
	  {
	    _workList[_numItems] = _fullList[_numItems];
	  }
#endif

	len = strlen(_fullList[_numItems]);	// strlen
	width = XTextWidth(theApp->_XDefaultFont,
		_fullList[_numItems], len);
	if (width > _maxWidth)
	    _maxWidth = width;		// track largest so far
      }

#ifdef Athena
    // Note that at this point _numItems is how many items are
    // in the user supplied list. We can use this number to null
    // out the rest of the displayed list.
    if (_numItems < _listRows)
      {
	for (int ix = _numItems ; ix < _listRows ; ++ix )
	  {
	    _workList[ix] = EmptyItem;
	  }
      }

    // Now set the scrollbar size limits
    if (_numItems > _listRows)
      {
	_ScrlShown = (_listRows * 100) / _numItems;
      }
    else
	_ScrlShown = 100;

    if (_numItems > _listRows)
      {
	_fscale = 0.0;
	_scale = (100 - _ScrlShown) / (_numItems - _listRows);
	if (_scale == 0)
	  {
	    _fscale = ((100. - (float)_ScrlShown) / ((float)_numItems - (float)_listRows));
  	  }
      }
    else
	_scale = 0;		// will leave top at 0

    _ScrlTop =  0;
#endif

  }

extern "C"
{
//============================>>> CListCmdCB <<<=============================
  void CListCmdCB(Widget w, XtPointer This, XtPointer lr)
  {
#ifndef Athena
    // need to adjust Motif's indexing from 1 to our indexing from 0
    XmListCallbackStruct* xml = (XmListCallbackStruct*) lr;
    ((vListCmd*)This)->ListCmdCB(xml->item_position - 1);
#else
    ((vListCmd*)This)->ListCmdCB(((XawListReturnStruct*)lr)->list_index);
#endif
  }

#ifdef Athena
//============================>>> CJumpProcCB <<<=============================
  void CJumpProcCB(Widget w, XtPointer This, XtPointer pc_ptr)
  {

    float percent = *(float*)pc_ptr;	// get the percent back

    ((vListCmd*)This)->JumpProcCB((int)(percent*100.));
  }

//============================>>> CScollProcCB <<<=============================
  void CScrollProcCB(Widget w, XtPointer This, XtPointer position)
  {
    int pos = (int)position;
    ((vListCmd*)This)->ScrollProcCB(pos);
  }
#endif	// Athena only
}
