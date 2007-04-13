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

#include <v/vpopmenu.h>		// our header
#include <v/vcmdwin.h>		// we need access to vcmdwin

//==================>>> vPopupMenu::vPopupMenu <<<===========================
  vPopupMenu::vPopupMenu(VCONST vMenu* menu, VCONST vWindow* pWin)
  {

    // set up menus -----------------------------------------------

    SysDebug(Constructor,"vPopupMenu::vPopupMenu() constructor\n")

  }

//==================>>> vPopupMenu::vPopupMenu <<<===========================
  vPopupMenu::vPopupMenu(const vPopupMenu& /* m */)
  {

    vSysError("V Semantics do not allow copy constructors.");
  }

//====================>>> vPopupMenu::~vPopupMenu <<<=======================
  vPopupMenu::~vPopupMenu()			// destructor
  {

  }

//====================>>> vPopupMenu::ShowMenu <<<======================
  void vPopupMenu::ShowMenu(int x, int y)
  {
  }

//====================>>> vPopupMenu::GetValue <<<======================
  int vPopupMenu::GetValue(ItemVal id) VCONST
  {
    // scan all menus in this window to retrieve the what value
    // then scan button bar if not found

    return 0;		// assume 0 if not found
  }

//==================>>> vPopupMenu::SetValue <<<========================
  void vPopupMenu::SetValue(ItemVal id, int val, ItemSetType setType)
  {
    // Set the given item on or off

  }

//================>>> vPopupMenu::SetString <<<========================
  void vPopupMenu::SetString(ItemVal id, VCONST char* str) 
  {
    // Set the given item on or off

  }
