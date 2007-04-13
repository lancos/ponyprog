//===============================================================
// vPopMenu.cpp - vPopupMenu class functions - Windows
//
// Copyright (C) 1995-1999  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vpopmenu.h>		// our header
#include <v/vcmdwin.h>		// we need access to vcmdwin

#include <stdlib.h>

// Define static data of the class

   static char curLbl[100];	// for fixed labels


//==================>>> vPopupMenu::vPopupMenu <<<===========================
  vPopupMenu::vPopupMenu(VCONST vMenu* menu, VCONST vWindow* pWin)
  {
    int i;

    // set up menus -----------------------------------------------

    SysDebug(Constructor,"vPopupMenu::vPopupMenu() constructor\n")

    _pWin = const_cast<vWindow*>(pWin);

    _nextMenuButton = 0;	// no menus defined so far

    for (i = 0 ; i < MAX_PMENU_BUTTONS ; ++i)
      {
	_mb[i].label = 0; // null out menu button array
	_mb[i].hPullDown = 0;
	_mb[i].SubMenu = 0;
        _mb[i].mInfo = 0;
      }

    _mb[0].label = "";
    _mb[0].SubMenu = const_cast<vMenu*>(menu);
    _nextSubMenu =		// we will start submenus here
    _nextMenuButton = 1;		// this many menus defined
    _topLevelMenu = 0;

  }

//==================>>> vPopupMenu::vPopupMenu <<<===========================
  vPopupMenu::vPopupMenu(const vPopupMenu& /* m */)
  {

    vSysError("V Semantics do not allow copy constructors.");
  }

//====================>>> vPopupMenu::~vPopupMenu <<<=======================
  vPopupMenu::~vPopupMenu()			// destructor
  {

    SysDebug(Destructor,"vPopupMenu::~vPopupMenu() destructor\n")

   for (int i = _nextSubMenu - 1 ; i >= 0 ; --i)	// all menus
     {
	::DestroyMenu(_mb[i].hPullDown);
        if (_mb[i].mInfo != 0)
   	    delete _mb[i].mInfo;		// free the space
      }
  }

//======================>>> vPopupMenu::initialize <<<=======================
  void vPopupMenu::initialize()
  {

    if (_topLevelMenu != 0)		// already initialized?
	return;
    _topLevelMenu = (HMENU)::CreatePopupMenu();	// Create a popup menu

    // Now we have to add our menus

    doAddMenu(0, _topLevelMenu);
   }

//======================>>> vPopupMenu::fixLabel <<<========================
  void vPopupMenu::fixLabel(VCONST char* lbl, VCONST char* key) VCONST
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


//======================>>> vPopupMenu::doAddMenu <<<========================
  void vPopupMenu::doAddMenu(int id, HMENU parent)
  {
    // create a button on the menu button bar

    _mb[id].hPullDown = parent;

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
	    ::AppendMenu(_mb[id].hPullDown,MF_SEPARATOR,0,0);
	  }
	else if (item[ix].SubMenu != 0)	// a submenu
	  {
	    if(_nextSubMenu >= MAX_PMENU_BUTTONS)
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
  }

//====================>>> vPopupMenu::ShowMenu <<<======================
  void vPopupMenu::ShowMenu(int x, int y)
  {
    HWND parent = _pWin->winHwnd();

    POINT pt;

    initialize();		// initialize (if not already)

    pt.x = x; pt.y = y;
    ::ClientToScreen(parent ,&pt);
    
    ::TrackPopupMenu(_topLevelMenu,
		TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_LEFTBUTTON,
			pt.x, pt.y,
			0, 
			parent, 
			NULL);

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
	    // see if menuId is the same as the one we are setting
	    if (item[info->ItemIndex].menuId == id)
	      {
		// Ah Ha! We found the value we want
		return item[info->ItemIndex].checked;
	      }
	  }
      }

    return 0;		// assume 0 if not found
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

	      }
	  }
      }
  }
