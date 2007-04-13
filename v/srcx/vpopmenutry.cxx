//===============================================================
// vPopMenu.cxx - vPopupMenu class functions - X
//
// Copyright (C) 1995-1999  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================


#include <v/vpopmenu.h>
#include <v/vwindow.h>


#include <stdlib.h>
#include <v/vctlclrs.h>

extern "C"
{
#include <X11/Core.h>
#include <X11/Shell.h>

#ifndef Athena

#include <Xm/RowColumn.h>
//#include <Xm/CascadeB.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#define setLabel(x,y) XmString x = XmStringCreateLocalized(y);
#define freeLabel(x) XmStringFree(x);
#define Ncallback XmNactivateCallback
#define Nsensitive XmNsensitive
#define Nlabel XmNlabelString

    extern XEvent* VGlobalButtonPressedEvent;	// nasty but easy

#else		// ATHENA
#ifdef AthenaWorks

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

#endif

}


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
#ifdef AthenaWorks
static XtActionsRec submenu_action = { "popupSubmenu",
     (XtActionProc)subMenuAction };
static XtTranslations   SMtrans = 0, SMtrans2 = 0;
#endif
#endif

static char curLbl[100];	// for fixed labels
static char _Mnemonic;

//==================>>> vPopupMenu::vPopupMenu <<<===========================
  vPopupMenu::vPopupMenu(VCONST vMenu* menu, VCONST vWindow* pWin)
  {
    int i;

    // set up menus -----------------------------------------------

    SysDebug(Constructor,"vPopupMenu::vPopupMenu() constructor\n")

    
    _parentWin = pWin;
    initDone = 0;

    _nextMenuButton = 0;	// no menus defined so far

    for (i = 0 ; i < MAX_MENU_BUTTONS ; ++i)
      {
	_mb[i].label = 0;	// null out menu button array
	_mb[i].XMenuButton = 0;
	_mb[i].SubMenu = 0;
	_mb[i].mInfo = 0;
      }

    _mb[0].label = "";
    _mb[0].SubMenu = menu;
    _nextSubMenu =		// we will start submenus here
    _nextMenuButton = 1;	// this many menus defined
    _topLevelMenu = 0;

  }

//==================>>> vPopupMenu::vPopupMenu <<<===========================
  vPopupMenu::vPopupMenu(const vPopupMenu& m)
  {

    vSysError("V Semantics do not allow copy constructors.");
  }

//====================>>> vPopupMenu::~vPopupMenu <<<=======================
  vPopupMenu::~vPopupMenu()			// destructor
  {

    SysDebug(Destructor,"vPopupMenu::~vPopupMenu() destructor\n")

    if (_topLevelMenu != 0)			// delete top + childern
	XtDestroyWidget(_topLevelMenu);
    
    for (int i = 0 ; i < _nextSubMenu ; ++i)	// all menus
      {
	delete _mb[i].mInfo;		// free the space
      }

  }

//======================>>> vPopupMenu::fixLabel <<<========================
  void vPopupMenu::fixLabel(VCONST char* lbl, VCONST char* key) VCONST
  {
    // copy label to global curLbl

    VCONST char* cp;
    int ix = 0;

    _Mnemonic = 0;

#ifndef Athena
    curLbl[0] = ' '; curLbl[1] = ' '; ix = 2;	// to add * checkmark
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
#ifdef AthenaWorks
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
#endif

    curLbl[ix] = 0;		// finish off
  }

//======================>>> vPopupMenu::doAddMenu <<<========================
  void vPopupMenu::doAddMenu(int id, Widget parent)
  {
    // create a button on the menu button bar


    // fixLabel(_mb[id].label,0);	// copy the label
    // setLabel(tmp,curLbl);


    _mb[id].XPopUp = parent;		// a submenu
    _mb[id].XMenuButton = parent;		// a submenu

//@@ need something different here for Athena - this part is
//   mostly why Athena isn't supported at the moment...

    // freeLabel(tmp);

    // loop through the list

    _mb[id].mInfo = 0;			// empty list so far

    vMenu* item = _mb[id].SubMenu;	// The first item in list

    PMenuInfo* info;			// for current info
    for (int ix = 0 ; item[ix].label != 0 ; ++ix)
      {
	info = new PMenuInfo;		// new space or current tem
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
#ifdef AthenaWorks
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
//		xmCascadeButtonWidgetClass,			// widget class 
		xmPushButtonWidgetClass,			// widget class 
	        _mb[id].XPopUp,		// parent widget
		Nlabel, tmp,				// display name
		XmNsubMenuId,info->WItem,
		XmNmnemonic, _Mnemonic,
		NULL);					// argument list

#else	// ATHENA
#ifdef AthenaWorks

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
#ifdef AthenaWorks
	    doAddMenu(sub,_mb[id].XPopUp);
#endif
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
	
	    // and add the call back for each item
	    freeLabel(tmp);

	    XtAddCallback(info->WItem,
	        Ncallback, CPmenuCB, (XtPointer)this);
#else
#ifdef AthenaWorks
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
	    // and add the call back for each item
	    freeLabel(tmp);

	    XtAddCallback(info->WItem,
	        Ncallback, CPmenuCB, (XtPointer)this);
#endif
#endif

	  }
      }
  }

//====================>>> vPopupMenu::GetValue <<<======================
  int vPopupMenu::GetValue(ItemVal id) VCONST
  {
    // scan all menus in this window to retrieve the what value
    // then scan button bar if not found

    vMenu* item;

    // Search all menus in this list
    for (int ix = 0 ; ix < _nextSubMenu ; ++ix)
      {
	PMenuInfo* info;			// for current info

	// scan the list of info for each menu entry

	for (info = _mb[ix].mInfo ; info != 0 ; info = info->NxtInfo)
	  {
	    item = _mb[ix].SubMenu;	// The current item in list
	    // see if its menuId is the same as the one we are setting
	    if (item[info->ItemIndex].menuId == id)
	      {
		// Ah Ha! We found the value we want
		return item[info->ItemIndex].checked;
	      }
	  }
      }

    return 0;		// assume 0 if not found
  }

#ifdef Athena
//====================>>> vPopupMenu::GetMenuThis <<<======================
  vPopupMenu* vPopupMenu::GetMenuThis(Widget w)
  {
    for (int ix = 0 ; ix < _nextSubMenu ; ++ix)
      {
	if (_mb[ix].XPopUp == w)
	    return this;
      }
    return 0;
  }
#endif

//======================>>> vPopupMenu::initialize <<<=======================
  void vPopupMenu::initialize()
  {
    // First, create the Pixmaps, but only once!

    if (_topLevelMenu != 0)			// already done!
	return;

#ifdef Athena
#ifdef AthenaWorks
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


//    _topLevelMenu = XtVaCreatePopupShell(
//	  _mb[id].label,
//	  simpleMenuWidgetClass,			// menu
//	  _mb[id].XMenuButton,			// parent
//	  XtNbackground, _vMenuBarBG,
//	  NULL);

    // Now we have to add our menus

    doAddMenu(0, _topLevelMenu);
#endif
#else	// Motif

#define TESTIT
#ifdef TESTIT
    XmString line, square, circle, exit, exit_acc;

    line = XmStringCreateLocalized ("Line");
    square = XmStringCreateLocalized ("Square");
    circle = XmStringCreateLocalized ("Circle");
    exit = XmStringCreateLocalized ("Exit");
    exit_acc = XmStringCreateLocalized ("Ctrl+C");
    _topLevelMenu = XmVaCreateSimplePopupMenu (_parentWin->vHandle(),
		 "popup", NULL, // popup_cb,
        XmVaPUSHBUTTON, line, 'L', NULL, NULL,
        XmVaPUSHBUTTON, square, 'S', NULL, NULL,
        XmVaPUSHBUTTON, circle, 'C', NULL, NULL,
        XmVaSEPARATOR,
        XmVaPUSHBUTTON, exit, 'x', "Ctrl<Key>c", exit_acc,
        NULL);
    XmStringFree (line);
    XmStringFree (square);
    XmStringFree (circle);
    XmStringFree (exit);
    XmStringFree (exit_acc);

#else
    _topLevelMenu = XmCreatePopupMenu(_parentWin->vHandle(),"popupmenu",0,0);
    // Now we have to add our menus


    (void) XtVaCreateManagedWidget(
		"TestMe",
	        xmPushButtonWidgetClass,	// widget class 
	        _topLevelMenu,		// parent widget
	        NULL);			// argument list
	
//    doAddMenu(0, _topLevelMenu);

#endif
#endif

  }

//============================>>> vPopupMenu::menuCB <<<==========================
  void vPopupMenu::menuCB(Widget w)
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
	PMenuInfo* info;			// for current info

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
FoundIt: return;
//@@@@@    _parentWin->MenuCommand(menu_val);
  }

extern "C"
{
//============================>>> CPmenuCB <<<==========================
  void CPmenuCB(Widget w, XtPointer ItmPtr, XtPointer call_data)
  { 
    //	menu button "C" Call Back
    // ItmPtr will have the this pointer of our object

   ((vPopupMenu*)ItmPtr)->menuCB(w);
  }
}

//==================>>> vPopupMenu::SetValue <<<========================
  void vPopupMenu::SetValue(ItemVal id, int val, ItemSetType setType)
  {
    // Set the given item on or off

    vMenu* item;

    // Search all menus in this list
    for (int ix = 0 ; ix < _nextSubMenu ; ++ix)
      {
	PMenuInfo* info;			// for current info

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
#ifdef AthenaWorks
			    XtVaSetValues(info->WItem,	// to this menu item
			        XtNleftBitmap,		// set the left side
				menux_bitmap,		// to the X checkbox
				NULL);
#endif
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
#ifdef AthenaWorks
			    XtVaSetValues(info->WItem,	// to this menu item
			        XtNleftBitmap,		// set the left side
				menuclr_bitmap,		// to the X clearbox
				NULL);
#endif
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

//================>>> vPopupMenu::SetString <<<========================
  void vPopupMenu::SetString(ItemVal id, VCONST char* str) 
  {
    // Set the given item on or off

    vMenu* item;

    // Search all menus in this window 
    for (int ix = 0 ; ix < _nextSubMenu ; ++ix)
      {
	PMenuInfo* info;			// for current info

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

//==================>>> vPopupMenu::doSubMenu <<<========================
  void vPopupMenu::doSubMenu(Widget parent, Widget subitem, int x, int y)
  {
    // Maybe popup a submenu

    // Scan all menus in this window looking for widget corresponding
    // to the widget that generated the callback
    for (int ix = 0 ; ix < _nextSubMenu ; ++ix)
      {
	if (_mb[ix].XPopUp == parent)
	  {
	    PMenuInfo* info;			// for current info
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
#ifdef AthenaWorks
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

    vPopupMenu* mypane = theApp->GetMenuThis(w);

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
#endif
  }

//====================>>> vPopupMenu::ShowMenu <<<======================
  void vPopupMenu::ShowMenu(int x, int y)
  {
    if (!initDone)
      {
	initDone = 1;
	initialize();
      }

    XButtonPressedEvent bp;


    bp.type = ButtonPress;
    bp.display = theApp->display();
    bp.window = 0;
    bp.root = 0;
    bp.subwindow = 0;
    bp.time = 0;
    bp.x = x;
    bp.y = y;
    bp.x_root = x;
    bp.x_root = y;
    bp.state = 0;
    bp.same_screen = 1;
    bp.button = 0;

    XmMenuPosition(_topLevelMenu,
	(XButtonPressedEvent *)VGlobalButtonPressedEvent);
    XtManageChild(_topLevelMenu);
//    XtPopup(XtParent(_topLevelMenu),XtGrabNonexclusive);
//    XtMapWidget(XtParent(_topLevelMenu));
//    XtMapWidget(_topLevelMenu);

  }
