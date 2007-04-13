//===============================================================
// vpopmenu.h - vpopmenu class definitions - X
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
#include <v/vmenu.h>

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
	Widget WItem;		// Widget for each menu item
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
	Widget XMenuButton;	// an X widget for the button
	Widget XPopUp;		// and X widget for the popup

      } PMenuButton;

    static const int MAX_PMENU_BUTTONS = 20;	// Up to 20 menu buttons + submenus
	
    extern "C"
      {
	void CPmenuCB(Widget w, XtPointer ItmPtr, XtPointer call_data);
      };
				// per window
    class vPopupMenu
      {

      public:		//---------------------------------------- public
	friend void CPmenuCB(Widget w, XtPointer ItmPtr, XtPointer call_data);
	// the constructors
	vPopupMenu(VCONST vMenu* menu, VCONST vWindow* pWin);
	vPopupMenu(const vPopupMenu& m);		// Copy constructor
	virtual ~vPopupMenu();			// Destructor

	// non-virtual methods
	void initialize();

#ifdef VX_HASPOPUPS
	int isImplemented() { return 1; }
#else
	int isImplemented() { return 0; }
#endif

	int GetValue(ItemVal id) VCONST;
	void SetValue(ItemVal id, int val, ItemSetType setType);
	void SetString(ItemVal id, VCONST char* str);

	void ShowMenu(int x, int y);

      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private

	// data
#ifdef VX_HASPOPUPS
	int initDone;

	vWindow* _parentWin;

	Widget _topLevelMenu;

	int _nextMenuButton;
	int _nextSubMenu;
	PMenuButton _mb[MAX_PMENU_BUTTONS]; // Up to 10 menu items per window

	// methods
	void doAddMenu(int id, Widget parent); 	// build the menu
	void PmenuCB(Widget w);			// callback for menu selection
	void doSubMenu(Widget parent, Widget subitem, int x, int y);

	void fixLabel(VCONST char* lbl, VCONST char* key = 0) VCONST;
#ifdef Athena
	vPopupMenu* GetMenuThis(Widget w);
#endif
	int CheckAccel(vKey vkey, unsigned int shift, ItemVal& id) VCONST;
	void menuCB(Widget w);
#endif	// VX_HASPOPUPS
      };

#endif
