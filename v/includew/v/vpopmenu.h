//===============================================================
// vmenu.h - vmenu class definitions - Windows
//
// Copyright (C) 1995-1999  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VPOPMENU_H
#define VPOPMENU_H

#include <v/v_defs.h>

//	The _menuButton array will point to static arrays of menus.
//	The user will allocate these, e.g.:
//
//	vMenu EditMenu[] =
//	  {
//	    {"Item 1", MENU_1, isSens, notChk, noKeyLbl, noKey, noSub},
//	    {"Item 2", MENU_2, isSens, notChk, noKeyLbl, noKey, noSub},
//	    {NULL}
//	  };
//
// and to define the menu bar
//
//	vMenu myMenu[] = 
//	  {
//	    {"Edit", M_File, isSens, notChk, noKeyLbl, noKey, &EditMenu[0]},
//	    {NULL}
//	  };
//
//	the _menuButton will be done dynamically. The call back
//	will pass the value of an item to a given window.

    typedef struct PMenuInfo	// Implementation dependent stuff
      {
	int ItemIndex;
	int SubMenuIndex;
	PMenuInfo* NxtInfo;
      } PMenuInfo;

    typedef struct PMenuButton
      {
	VCONST char* label;		// Label for button
	vMenu* SubMenu;		// the menu items associated with this button
	PMenuInfo* mInfo;	// dynamically allocated info list

	// System dependent handles
	HMENU hPullDown;	// handle for the pulldown

      } PMenuButton;

    static const int MAX_PMENU_BUTTONS = 20;	// Up to 20 menu buttons + submenus
					// per window
    class V_EXPORT vWindow;

    class V_EXPORT vPopupMenu
      {

      public:		//---------------------------------------- public
	// the constructors
	vPopupMenu(VCONST vMenu* menu, VCONST vWindow* pWin);
	vPopupMenu(const vPopupMenu& m);		// Copy constructor
	virtual ~vPopupMenu();			// Destructor

	// non-virtual methods

	void ShowMenu(int x, int y);

	int isImplemented() { return 1; }

	int GetValue(ItemVal id) VCONST;
	void SetValue(ItemVal id, int val, ItemSetType setType);
	void SetString(ItemVal id, VCONST char* str); // set menu item label

      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private
	// data

	vWindow* _pWin;
	int _nextMenuButton;
	int _nextSubMenu;
	PMenuButton _mb[MAX_PMENU_BUTTONS]; // Up to 10 menu items per window
        HMENU _topLevelMenu;

	// methods
	void initialize();
	void fixLabel(VCONST char* lbl, VCONST char* key) VCONST;
	void doAddMenu(int id, HMENU parent); 	// build the menu
      };

#endif
