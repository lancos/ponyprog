//===============================================================
// vMenu.cxx - vMenuPane class functions - X11R5
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vmenu.h>		// our header
#include <v/vcmdwin.h>		// we need access to vcmdwin

#include <stdlib.h>

// Define static data of the class

  static vMenu WindowMenu[] =    // Items for File menu
      {
	{"&Cascade",M_Cascade,isSens,notChk,noKeyLbl,noKey,noSub},
	{"&Tile",M_Tile,isSens,notChk,noKeyLbl,noKey,noSub},
	{"&Arrange icons",M_Arrange,isSens,notChk,noKeyLbl,noKey,noSub},
	{NULL}
      };

  static char curLbl[100];	// for fixed labels

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
	_mb[i].hPullDown = 0;
	_mb[i].SubMenu = 0;
      }

    // copy the menus as defined to our array

    for (i = 0 ; i < MAX_MENU_BUTTONS && &menu[i] != 0 && menu[i].label; ++i)
      {
	_mb[i].label = menu[i].label;
	_mb[i].SubMenu = menu[i].SubMenu;
      }
    if (i < MAX_MENU_BUTTONS && !theApp->simSDI())
      {
	_mb[i].label = "&Window";
	_mb[i].SubMenu = &WindowMenu[0];	// Automatically add window menu
	++i;			// fixup i
      }
    _nextSubMenu =		// we will start submenus here
    _nextMenuButton = i;	// this many menus defined
    _topLevelMenu = 0;

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

    if (::IsMenu(_topLevelMenu))	// v1.22 - from Hilinski
	::DestroyMenu(_topLevelMenu);	// Start with the top

    for (int i = _nextSubMenu - 1 ; i >= 0 ; --i)	// all menus
     {
	if (::IsMenu(_mb[i].hPullDown))
	    ::DestroyMenu(_mb[i].hPullDown);
	delete _mb[i].mInfo;		// free the space
      }
  }

//======================>>> vMenuPane::initialize <<<=======================
  void vMenuPane::initialize(vWindow* pWin, HWND hMenu)
  {
    vPane::initialize(pWin, hMenu);		// initialize these

    _topLevelMenu = (HMENU)hMenu;		// handle to menu bar

    // Now we have to add our menus

    for (int i = 0 ; i < _nextMenuButton ; ++i)
      {
	doAddMenu(i, _topLevelMenu);
      }
    pWin->_WindowMenuIndex = _nextMenuButton - 1; // Window menu
   }

//======================>>> vMenuPane::fixLabel <<<========================
  void vMenuPane::fixLabel(VCONST char* lbl, VCONST char* key) VCONST
  {
    // copy label to global curLbl

    VCONST char* cp;
    int ix = 0;

    for (cp = lbl ; *cp && ix < 99 ; ++cp)	// Scan label
      {
	curLbl[ix++] = *cp;
      }

    if (key && *key)
      {
	curLbl[ix++] = ' '; curLbl[ix++] = ' '; curLbl[ix++] = '\t';
 	for (cp = key ; *cp && ix < 99 ; ++cp)
	  {
	    curLbl[ix++] = *cp;
	  }
      }

    curLbl[ix] = 0;		// finish off
  }


//======================>>> vMenuPane::doAddMenu <<<========================
  void vMenuPane::doAddMenu(int id, HMENU parent)
  {
    // create a button on the menu button bar

    _mb[id].hPullDown = ::CreateMenu();	// create submenu

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
	    ::AppendMenu(_mb[id].hPullDown,MF_SEPARATOR,0,0);
	  }
	else if (item[ix].SubMenu != 0)	// a submenu
	  {
	    if(_nextSubMenu >= MAX_MENU_BUTTONS)
	      {
		SysDebug(BadVals,"Too many submenus!\n");
		continue;
	      }

	    // we will create a submenu
	    // copy the definitions, track the new index

	    int sub = _nextSubMenu++;	// get our index, bump for next time

	    _mb[sub].label = item[ix].label;
	    _mb[sub].SubMenu = item[ix].SubMenu;
	    info->SubMenuIndex = sub;		// no submenu normally

	    // We create a dangling menu handle here. Windows seems
	    // to delete submenus of a menu, so we need to remember
	    // the parent menu, not the submenu. The recursive call
	    // replaces the parent menu handle with the submenu.

	    HMENU subParent = ::CreateMenu();	// a submenu

	    // Now, recursively call doAddMenu with the submenu
	    doAddMenu(sub, subParent);

	    ::AppendMenu(_mb[id].hPullDown,MF_POPUP,(UINT)_mb[sub].hPullDown,
		item[ix].label);
	    _mb[sub].hPullDown = subParent;
	  }
	else
	  {
	    // create the item

	    fixLabel(item[ix].label,item[ix].keyLabel);
	    ::AppendMenu(_mb[id].hPullDown,MF_STRING,(UINT)item[ix].menuId,
		curLbl);
	    //	item[ix].label);
	    if (item[ix].checked)	// was it checked?
	      {
		::CheckMenuItem(_mb[id].hPullDown,
		   item[ix].menuId, MF_BYCOMMAND | MF_CHECKED);
	      }
	    if (!item[ix].sensitive)
	      {
		::EnableMenuItem(_mb[id].hPullDown,
		   item[ix].menuId, MF_BYCOMMAND | MF_GRAYED);
	      }
	  }
      }
    ::AppendMenu(parent,MF_POPUP,(int) (UINT)_mb[id].hPullDown, _mb[id].label);
  }

//====================>>> vMenuPane::CheckAccel <<<======================
  int vMenuPane::CheckAccel(vKey vkey, unsigned int shift, ItemVal& id) VCONST
  {
    // scan all menus in this window to see if this keystroke
    // matches an accelerator key

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
	    if (item[info->ItemIndex].accel == vkey &&
		 item[info->ItemIndex].kShift == shift &&
		 item[info->ItemIndex].sensitive)	// V 1.25, 6/1/00
	      {
		id = item[info->ItemIndex].menuId;
		return 1;
	      }
	  }
      }

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
		      {
			item[info->ItemIndex].checked = val;
			UINT action = val ? MF_BYCOMMAND | MF_CHECKED :
					    MF_BYCOMMAND | MF_UNCHECKED;
			::CheckMenuItem(_mb[ix].hPullDown,
				item[info->ItemIndex].menuId,action);
			break;
		      }

		    case Sensitive:			// sensitive
		      {
			item[info->ItemIndex].sensitive = val;
			UINT action = val ? MF_BYCOMMAND | MF_ENABLED :
					    MF_BYCOMMAND | MF_GRAYED;
			::EnableMenuItem(_mb[ix].hPullDown,
				item[info->ItemIndex].menuId,action);
			break;
		      }
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

	      item[info->ItemIndex].label = str;
	       ::ModifyMenu(_mb[ix].hPullDown,	// the menu
		    item[info->ItemIndex].menuId,  // the id
		    MF_BYCOMMAND | MF_STRING,	// change string
		    item[info->ItemIndex].menuId,  // same id
		    item[info->ItemIndex].label);  // new label

		if (item[info->ItemIndex].checked) // Changing string wipes out
		  {                                // checked and sensitive state
		    ::CheckMenuItem(_mb[ix].hPullDown,
			item[info->ItemIndex].menuId,MF_BYCOMMAND | MF_CHECKED);
		  }
		if (!item[info->ItemIndex].sensitive)
		  {
		    ::EnableMenuItem(_mb[ix].hPullDown,
		      item[info->ItemIndex].menuId,MF_BYCOMMAND | MF_GRAYED);
		  }

		::DrawMenuBar(theApp->_Frame);
	      }
	  }
      }
  }
