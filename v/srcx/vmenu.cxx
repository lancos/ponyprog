//===============================================================
// vMenu.cxx - vMenuPane class functions - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vmenu.h>		// our header
#include <v/vctlclrs.h>

extern "C"
{
#include <X11/Core.h>
#include <X11/Shell.h>

#ifndef Athena
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#define setLabel(x,y) XmString x = XmStringCreateLocalized(y);
#define freeLabel(x) XmStringFree(x);
#define Ncallback XmNactivateCallback
#define Nsensitive XmNsensitive
#define Nlabel XmNlabelString

#else		// ATHENA

#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Sme.h>

#define setLabel(x,y) char* x = y;
#define freeLabel(x)
#define Ncallback XtNcallback
#define Nsensitive XtNsensitive
#define Nlabel XtNlabel

#endif

}

#include <v/vcmdwin.h>		// we need access to vcmdwin

#include <stdlib.h>

// Define static data of the class

#define menux_width 9
#define menux_height 9
static char menux_bits[] = {
   0xff, 0x01, 0x83, 0x01, 0x45, 0x01, 0x29, 0x01, 0x11, 0x01, 0x29, 0x01,
   0x45, 0x01, 0x83, 0x01, 0xff, 0x01};
static Pixmap menux_bitmap = 0;		// the check for menus

#define menuclr_width 9
#define menuclr_height 9
static char menuclr_bits[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
static Pixmap menuclr_bitmap = 0;	// the no-check for menus

#define rtarrow_width 9
#define rtarrow_height 9
static char rtarrow_bits[] = {
   0x03, 0x00, 0x0f, 0x00, 0x3f, 0x00, 0xff, 0x00, 0xff, 0x01, 0xff, 0x00,
   0x3f, 0x00, 0x0f, 0x00, 0x03, 0x00};
static Pixmap rtarrow_bitmap = 0;	// right arrow for submenus


static void subMenuAction(Widget, XEvent *, String *, Cardinal *);

#ifdef Athena
static XtActionsRec submenu_action = { "popupSubmenu",
     (XtActionProc)subMenuAction };
static XtTranslations   SMtrans = 0, SMtrans2 = 0;
#endif

static char curLbl[100];	// for fixed labels
static char _Mnemonic;

//==================>>> vMenuPane::vMenuPane <<<===========================
  vMenuPane::vMenuPane(VCONST vMenu* menu) : vPane(P_Menu)
  {
    int i;

    // set up menus -----------------------------------------------

    SysDebug(Constructor,"vMenuPane::vMenuPane() constructor\n")

    
    _nextMenuButton = 0;	// no menus defined so far

    for (i = 0 ; i < MAX_MENU_BUTTONS ; ++i)
      {
	_mb[i].label = 0; // null out menu button array
	_mb[i].XMenuButton = 0;
	_mb[i].SubMenu = 0;
      }

    // copy the menus as defined to our array

    for (i = 0 ; i < MAX_MENU_BUTTONS && &menu[i] != 0 && menu[i].label; ++i)
      {
	_mb[i].label = menu[i].label;
	_mb[i].SubMenu = menu[i].SubMenu;
      }
    _nextSubMenu =		// we will start submenus here
    _nextMenuButton = i;	// this many menus defined

  }

//==================>>> vMenuPane::vMenuPane <<<===========================
  vMenuPane::vMenuPane(const vMenuPane& m) : vPane(m)
  {

    vSysError("V Semantics do not allow copy constructors.");
  }

//====================>>> vMenuPane::~vMenuPane <<<=======================
  vMenuPane::~vMenuPane()			// destructor
  {

    SysDebug(Destructor,"vMenuPane::~vMenuPane() destructor\n")

    for (int i = 0 ; i < _nextSubMenu ; ++i)	// all menus
      {
	delete _mb[i].mInfo;		// free the space
      }

  }

//======================>>> vMenuPane::fixLabel <<<========================
  void vMenuPane::fixLabel(VCONST char* lbl, VCONST char* key,
	 int isMainMenu) VCONST
  {
    // copy label to global curLbl

    VCONST char* cp;
    int ix = 0;

    _Mnemonic = 0;

#ifndef Athena
    if (!isMainMenu)
      {
	curLbl[0] = ' '; curLbl[1] = ' '; ix = 2;
      }
#endif
    for (cp = lbl ; *cp && ix < 99 ; ++cp)	// Scan label
      {
 	if (*cp != '&')
	  {
	    curLbl[ix++] = *cp;
	  }
	else
	    _Mnemonic = *(cp+1);
      }

#ifdef Athena		// ATHENA ONLY
    if (key && *key)
      {
	curLbl[ix++] = ' '; curLbl[ix++] = ' '; curLbl[ix++] = '<';
 	for (cp = key ; *cp && ix < 99 ; ++cp)
	  {
	    if (*cp != '&')
	      {
		curLbl[ix++] = *cp;
	      }
	  }
	curLbl[ix++] = '>';
      }
#endif

    curLbl[ix] = 0;		// finish off
  }

//======================>>> vMenuPane::doAddMenu <<<========================
  void vMenuPane::doAddMenu(int id, Widget parent)
  {
    // create a button on the menu button bar


    fixLabel(_mb[id].label,0,1);	// copy the label
    setLabel(tmp,curLbl);

#ifndef Athena

    // Motif does things in the reverse order of Athena --
    // create a menu popup off the menubar, then add
    // a cascade buttong to that

    if (parent == 0)
      {
	_mb[id].XPopUp = 
	  XmCreatePulldownMenu(_baseW,"pulldown",0,0);
//#	XtManageChild(_mb[id].XPopUp);
      }
    else
      {
	_mb[id].XPopUp = parent;		// a submenu
      }

    // Create the cascade button

    if (parent == 0)
      {
	_mb[id].XMenuButton = XtVaCreateManagedWidget(
            "vMenuButton",				// name for widget
	    xmCascadeButtonWidgetClass,			// widget class 
	    _baseW,					// parent widget
	    Nlabel, tmp,				// display name
	    XmNsubMenuId,_mb[id].XPopUp,
	    XmNmnemonic, _Mnemonic,
	    NULL);					// argument list
      }
    else
      {
	_mb[id].XMenuButton = parent;		// a submenu
      }

#else

    if (parent == 0)			// adding a top level menu
      {
	_mb[id].XMenuButton = XtVaCreateManagedWidget(
	    "vMenuButton",				// name for widget
	    menuButtonWidgetClass,			// widget class 
	    _baseW,					// parent widget
	    XtNlabel, tmp,				// display name
	    XtNmenuName, _mb[id].label,
	    XtNbackground, _vMenuBarBG,
	    XtNborderWidth,0,	// label only, no border
	    NULL);					// argument list
      }
    else
      {
	_mb[id].XMenuButton = parent;		// a submenu
      }

    // Create the Popup Shell needed for this menu
    if (parent == 0)
      {
	_mb[id].XPopUp = XtVaCreatePopupShell(
	  _mb[id].label,
	  simpleMenuWidgetClass,			// menu
	  _mb[id].XMenuButton,			// parent
	  XtNbackground, _vMenuBarBG,
	  NULL);
      }
    else
      {
	_mb[id].XPopUp = XtVaCreatePopupShell(
	  _mb[id].label,				// name
	  simpleMenuWidgetClass,			// menu
	  _mb[id].XMenuButton,			// parent
	  XtNtranslations, SMtrans2,		// need these to work right
	  XtNbackground, _vMenuBarBG,
	  NULL);
      }
#endif

    freeLabel(tmp);

    // loop through the list

    _mb[id].mInfo = 0;			// empty list so far

    vMenu* item = _mb[id].SubMenu;	// The first item in list

    MenuInfo* info;			// for current info
    for (int ix = 0 ; item[ix].label != 0 ; ++ix)
      {
	info = new MenuInfo;		// new space or current tem
	info->NxtInfo = _mb[id].mInfo;	// add to front of list
	_mb[id].mInfo = info;		// fix front pointer
	info->ItemIndex = ix;		// index to item list
	info->SubMenuIndex = 0;		// no submenu normally

	if (item[ix].menuId == M_Line)
	  {
#ifndef Athena
	    info->WItem = XtVaCreateManagedWidget(
	        "-",			// widget name
	        xmSeparatorWidgetClass,	// widget class 
	        _mb[id].XPopUp,		// parent widget
	        NULL);			// argument list
#else
	    info->WItem = XtVaCreateManagedWidget(
	        "-",			// widget name
	        smeLineObjectClass,	// widget class 
	        _mb[id].XPopUp,		// parent widget
		XtNvertSpace, 0,
		XtNleftMargin,15,	// room for check-mark
		XtNrightMargin,10,
		XtNbackground, _vMenuBarBG,
	        NULL);			// argument list
#endif
	  }
	else if (item[ix].SubMenu != 0)	// a submenu
	  {
	    if(_nextSubMenu >= MAX_MENU_BUTTONS)
	      {
		SysDebug(BadVals,"Too many submenus!\n");
		continue;
	      }
	    fixLabel(item[ix].label);
	    setLabel(tmp,curLbl);

#ifndef Athena
	    // Create a pulldown for pullright

	    info->WItem =  XmCreatePulldownMenu(_mb[id].XPopUp,
		"vPullRight",0,0);
//#	    XtManageChild(info->WItem);
	
	    // Create the cascade button

	    _mb[id].XMenuButton = XtVaCreateManagedWidget(
	    	"vMenuButton",				// name for widget
		xmCascadeButtonWidgetClass,			// widget class 
	        _mb[id].XPopUp,		// parent widget
		Nlabel, tmp,				// display name
		XmNsubMenuId,info->WItem,
		XmNmnemonic, _Mnemonic,
		NULL);					// argument list

#else	// ATHENA

	    info->WItem = XtVaCreateManagedWidget(
		item[ix].label,
	        smeBSBObjectClass,	// widget class 
	        _mb[id].XPopUp,		// parent widget
		XtNlabel,tmp,	// item[ix].label, // the label on the item
		Nsensitive,item[ix].sensitive,  // if sensitive
		XtNvertSpace, 25,
		XtNleftMargin,15,	// room for check-mark
		XtNrightMargin,10,
		XtNrightBitmap,		// set the left side
		rtarrow_bitmap,		// to the X checkbox
	        NULL);			// argument list

	    // Override the PARENT to grab motion, which will cause
	    // the popup menu to come up

	    XtOverrideTranslations(_mb[id].XPopUp, SMtrans);
#endif
	    freeLabel(tmp);

	    // we will create a submenu
	    // copy the definitions, track the new index

	    int sub = _nextSubMenu++;	// get our index, bump for next time

	    _mb[sub].label = item[ix].label;
	    _mb[sub].XMenuButton = 0;
	    _mb[sub].SubMenu = item[ix].SubMenu;
	    info->SubMenuIndex = sub;		// no submenu normally
	    // Now, recursively call doAddMenu with the submenu
#ifndef Athena
	    doAddMenu(sub,info->WItem);
#else
	    doAddMenu(sub,_mb[id].XPopUp);
#endif
	  }
	else
	  {
	    // create the item widget

	    fixLabel(item[ix].label, item[ix].keyLabel);
#ifndef Athena
//
//@@@ Note: we don't yet support accelerators on Motif -- we
// have to convert the X keycode to a Motif "Alt<key>X" format...
//
	    if (item[ix].checked)	// was it checked?
		curLbl[0] = '*';	// flaky checks

	    setLabel(tmp,curLbl);

	    info->WItem = XtVaCreateManagedWidget(
		item[ix].label,
	        xmPushButtonWidgetClass,	// widget class 
	        _mb[id].XPopUp,		// parent widget
		Nlabel,tmp,	// item[ix].label, // the label on the item
		Nsensitive,item[ix].sensitive,  // if sensitive
		XmNfillOnSelect,1,
		XmNindicatorOn,0,
		XmNshadowThickness,1,
		XmNindicatorType,XmN_OF_MANY,
	        NULL);			// argument list
	
#else
	    setLabel(tmp,curLbl);
	    info->WItem = XtVaCreateManagedWidget(
		item[ix].label,
	        smeBSBObjectClass,	// widget class 
	        _mb[id].XPopUp,		// parent widget
		XtNlabel,tmp,	// item[ix].label, // the label on the item
		Nsensitive,item[ix].sensitive,  // if sensitive
		XtNvertSpace, 25,
		XtNleftMargin,15,	// room for check-mark
		XtNrightMargin,10,
		XtNbackground, _vMenuBarBG,
	        NULL);			// argument list

	    if (item[ix].checked)	// was it checked?
	      {
		XtVaSetValues(info->WItem,	// to this menu item
			XtNleftBitmap,		// set the left side
			menux_bitmap,		// to the X checkbox
			NULL);
	      }
#endif


	    // and add the call back for each item
	    freeLabel(tmp);

	    XtAddCallback(info->WItem,
	        Ncallback, CmenuCB, (XtPointer)this);
	  }
      }
  }

//====================>>> vMenuPane::CheckAccel <<<======================
  int vMenuPane::CheckAccel(vKey vkey, unsigned int shift, ItemVal& id) VCONST
  {
    // scan all menus in this window to see if this keystroke
    // matches an accelerator key

#ifdef Athena
    vMenu* item;

    // Search all menus in this list
    for (int ix = 0 ; ix < _nextSubMenu ; ++ix)
      {
	MenuInfo* info;			// for current info

	// scan the list of info for each menu entry

	for (info = _mb[ix].mInfo ; info != 0 ; info = info->NxtInfo)
	  {
	    item = _mb[ix].SubMenu;	// The current item in list
	    // see if its menuId is the same as the one we are setting
	    if (item[info->ItemIndex].accel == vkey
		 && item[info->ItemIndex].kShift == shift
		 && item[info->ItemIndex].sensitive)		// V 1.25, 6/1/00
	      {
		id = item[info->ItemIndex].menuId;
		return 1;
	      }
	  }
      }
#endif

    id = 0;
    return 0;		// assume 0 if not found
  }

//====================>>> vMenuPane::GetPaneValue <<<======================
  int vMenuPane::GetPaneValue(ItemVal id, int& val) VCONST
  {
    // scan all menus in this window to retrieve the what value
    // then scan button bar if not found

    vMenu* item;

    // Search all menus in this list
    for (int ix = 0 ; ix < _nextSubMenu ; ++ix)
      {
	MenuInfo* info;			// for current info

	// scan the list of info for each menu entry

	for (info = _mb[ix].mInfo ; info != 0 ; info = info->NxtInfo)
	  {
	    item = _mb[ix].SubMenu;	// The current item in list
	    // see if its menuId is the same as the one we are setting
	    if (item[info->ItemIndex].menuId == id)
	      {
		// Ah Ha! We found the value we want
		val = item[info->ItemIndex].checked;
		return 1;
	      }
	  }
      }

    val = 0;
    return 0;		// assume 0 if not found
  }

//====================>>> vMenuPane::GetMenuThis <<<======================
  vMenuPane* vMenuPane::GetMenuThis(Widget w)
  {
    for (int ix = 0 ; ix < _nextSubMenu ; ++ix)
      {
	if (_mb[ix].XPopUp == w)
	    return this;
      }
    return 0;
  }

//======================>>> vMenuPane::initialize <<<=======================
  void vMenuPane::initialize(vWindow* pWin, Widget pWidget,
	Widget drawCanvas, Widget drawArea)
  {
    // First, create the Pixmaps, but only once!

    pWin->vAddMenuBar();				// Set up menubar

    vPane::initialize(pWin, pWidget);		// initialize these

#ifdef Athena
    if (menux_bitmap == 0)			// need to create stuff
      {
	// The bitmaps

	menux_bitmap = XCreateBitmapFromData(
	    XtDisplay(_parentWin->vHandle()),	// our display
	    DefaultRootWindow(XtDisplay(_parentWin->vHandle() )),  // a Drawable
	    menux_bits,			// the menux bitmap data
	    menux_width,		// from the file
	    menux_height);

	menuclr_bitmap = XCreateBitmapFromData(
	    XtDisplay(_parentWin->vHandle()),	// our display
	    DefaultRootWindow(XtDisplay(_parentWin->vHandle())),  // a Drawable
	    menuclr_bits,			// the menux bitmap data
	    menuclr_width,		// from the file
	    menuclr_height);

	rtarrow_bitmap = XCreateBitmapFromData(
	    XtDisplay(_parentWin->vHandle()),	// our display
	    DefaultRootWindow(XtDisplay(_parentWin->vHandle())),  // a Drawable
	    rtarrow_bits,		// the rtarrow bitmap data
	    rtarrow_width,		// from the file
	    rtarrow_height);

	// the action for submenus

	XtAppAddActions(XtWidgetToApplicationContext(_parentWin->vHandle()),
		 &submenu_action, 1);
      }

    if (SMtrans == 0)			// The translations needed
      {
	SMtrans = XtParseTranslationTable(
	  "<BtnMotion>: highlight() popupSubmenu()");
	SMtrans2 = XtParseTranslationTable(
	  "<LeaveWindow>: unhighlight() MenuPopdown()\n<BtnUp>: notify()\
 unhighlight() MenuPopdown()\n<BtnMotion>: highlight()");
      }
#endif

    // Now we have to add our menus

    for (int i = 0 ; i < _nextMenuButton ; ++i)
      {
	doAddMenu(i, 0);
      }
  }

//============================>>> vMenuPane::menuCB <<<==========================
  void vMenuPane::menuCB(Widget w)
  {
    //	menu button class callback
    //
    // We will do some work here
    // scan all menus until we find the widget

    vMenu* item;
    ItemVal menu_val = 0;

    // Scan all menus in this window looking for widget corresponding
    // to the widget that generated the callback
    for (int ix = 0 ; ix < _nextSubMenu ; ++ix)
      {
	MenuInfo* info;			// for current info

	for (info = _mb[ix].mInfo ; info != 0 ; info = info->NxtInfo)
	  {
	    if (info->WItem == w)	// found it!
	      {
		item = _mb[ix].SubMenu;	// The first item in list
		menu_val = item[info->ItemIndex].menuId;
		goto FoundIt;
	      }
	  }
      }
FoundIt:
    _parentWin->MenuCommand(menu_val);
  }

extern "C"
{
//============================>>> CmenuCB <<<==========================
  void CmenuCB(Widget w, XtPointer ItmPtr, XtPointer call_data)
  { 
    //	menu button "C" Call Back
    // ItmPtr will have the this pointer of our object

   ((vMenuPane*)ItmPtr)->menuCB(w);
  }
}

//==================>>> vMenuPane::SetPaneValue <<<========================
  void vMenuPane::SetPaneValue(ItemVal id, int val, ItemSetType setType)
  {
    // Set the given item on or off

    vMenu* item;

    // Search all menus in this list
    for (int ix = 0 ; ix < _nextSubMenu ; ++ix)
      {
	MenuInfo* info;			// for current info

	// scan the list of info for each menu entry
	for (info = _mb[ix].mInfo ; info != 0 ; info = info->NxtInfo)
	  {
	    item = _mb[ix].SubMenu;	// The current item in list
	    // see if its menuId is the same as the one we are setting
	    if (item[info->ItemIndex].menuId == id)
	      {
		// Ah Ha! We found the value we want
		switch (setType)
		  {
		    case Value:			// check box
		    case Checked:		// check box
			item[info->ItemIndex].checked = val;
			if (val)		// turning it on
			  {
// OK, I know this isn't the best, but I tried and couldn't
// get ToggleButtons to work like I wanted, so I'm faking
// check marks with an '*'. If Motif had nice lefthand
// Bitmaps like Athena....
#ifndef Athena
			    fixLabel(item[info->ItemIndex].label);
			    curLbl[0] = '*';
			    setLabel(tmp,curLbl);
			    XtVaSetValues(info->WItem,	// to this menu item
			        Nlabel,tmp,
				NULL);
			    freeLabel(tmp);
#else
			    XtVaSetValues(info->WItem,	// to this menu item
			        XtNleftBitmap,		// set the left side
				menux_bitmap,		// to the X checkbox
				NULL);
#endif
			  }
			else
			  {
#ifndef Athena
			    fixLabel(item[info->ItemIndex].label);
			    setLabel(tmp,curLbl);
			    XtVaSetValues(info->WItem,	// to this menu item
			        Nlabel,tmp,
				NULL);
			    freeLabel(tmp);
#else
			    XtVaSetValues(info->WItem,	// to this menu item
			        XtNleftBitmap,		// set the left side
				menuclr_bitmap,		// to the X clearbox
				NULL);
#endif
			  }
			break;

		    case Sensitive:			// sensitive
			item[info->ItemIndex].sensitive = val;
			XtVaSetValues(info->WItem,	// set sens or not
			    Nsensitive, val,		// how to set
			    NULL);
			break;
		  }	// end switch
	      }
	  }
      }
  }

//================>>> vMenuPane::SetPaneString <<<========================
  void vMenuPane::SetPaneString(ItemVal id, VCONST char* str) 
  {
    // Set the given item on or off

    vMenu* item;

    // Search all menus in this window 
    for (int ix = 0 ; ix < _nextSubMenu ; ++ix)
      {
	MenuInfo* info;			// for current info

	// scan the list of info for each menu entry
	for (info = _mb[ix].mInfo ; info != 0 ; info = info->NxtInfo)
	  {
	    item = _mb[ix].SubMenu;	// The current item in list

	    if (item[info->ItemIndex].menuId == id)
	      {
		// Ah Ha! We found the value we want
		fixLabel(str);
#ifndef Athena
		item[info->ItemIndex].label = str;
		// This is flakey, but you have to check for check here
		if (item[info->ItemIndex].checked)
		    curLbl[0] = '*';
#endif
		setLabel(tmp,curLbl);
	  
		XtVaSetValues(info->WItem,	// change label
		    Nlabel, tmp,
		    NULL);

		freeLabel(tmp);
	      }
	  }
      }
  }

//==================>>> vMenuPane::doSubMenu <<<========================
  void vMenuPane::doSubMenu(Widget parent, Widget subitem, int x, int y)
  {
    // Maybe popup a submenu

    // Scan all menus in this window looking for widget corresponding
    // to the widget that generated the callback
    for (int ix = 0 ; ix < _nextSubMenu ; ++ix)
      {
	if (_mb[ix].XPopUp == parent)
	  {
	    MenuInfo* info;			// for current info
	    for (info = _mb[ix].mInfo ; info != 0 ; info = info->NxtInfo)
	      {
		if (info->WItem == subitem)
		  {
		    vMenu* item = _mb[ix].SubMenu;  // The first item in list

		    // look at static definition to see if this one had a
		    // submenu. If so, then it is the one we are looking for.
		    if (item[info->ItemIndex].SubMenu != 0)
		      {
			// get the popup shell for the submenu and pop it up.
			Widget submenu = _mb[info->SubMenuIndex].XPopUp;
			XtVaSetValues(submenu, XtNx, x, XtNy, y, NULL);
			XtPopup(submenu, XtGrabExclusive);
		 	theApp->XWaitUntilMapped(submenu);
			return;
		      }
		  }
	      }
	    break;
	  }
      }
    // we didn't find a value
  }

//================>>> subMenuAction <<<========================
  static void subMenuAction(Widget w, XEvent *event, 
	String *params, Cardinal *nparams)
  {
#ifdef Athena
    Dimension    width, height;
    int		x, y;
    Widget	cw = XawSimpleMenuGetActiveEntry(w);

    if (cw == 0 || event->type != MotionNotify)
	return;

    x = event->xmotion.x;
    y = event->xmotion.y;

    XtVaGetValues(w, XtNwidth, &width, XtNheight, &height, NULL);

    if (x < 0 || x >= width || y < 0 || y >= height)
	return;

    /*
    **  Only the second half of the menu is sensitve to pulls
    */
    if (x < (width - (width / 3)))
	return;

    vMenuPane* mypane = theApp->GetMenuThis(w);

    if (mypane == 0)
	return;

    // Don't mess with this too much -- the sub menu seems to need
    // to popup over the cursor, or it loses track of things.
    x = event->xmotion.x_root - 10 ;	// Make sure cursor shows
    y = event->xmotion.y_root - 10 ;

    mypane->doSubMenu(w, cw, x, y);

#ifdef ITWORKS
    for (cur = rightHead; cur != NULL && cur->widget != cw; cur = cur->next)
	;

    if (cur == NULL)
	return;

    XtVaSetValues(cur->shell, XtNx, x, XtNy, y, NULL);

    XtPopup(cur->shell, XtGrabExclusive);
#endif
#endif
  }
