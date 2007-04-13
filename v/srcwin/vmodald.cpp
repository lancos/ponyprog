//===============================================================
// vdialog.cxx - vdialog class functions - X11R5
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vmodald.h>		// our header
#include <v/vapp.h>

// Define static data of the class
// ... none ...

//=================>>> vModalDialog::vModalDialog <<<=====================
  vModalDialog::vModalDialog(VCONST vBaseWindow* creator, VCONST char* title) :
    vDialog(creator, 1, title)		// constructor
  {
    SysDebug(Constructor,"vModalDialog::vModalDialog(vBaseWindow) constructor\n")
  }

//=================>>> vModalDialog::vModalDialog <<<=====================
  vModalDialog::vModalDialog(VCONST vApp* creator, VCONST char* title) :
    vDialog(creator, 1, title)		// constructor
  {

    SysDebug(Constructor,"vModalDialog::vModalDialog(vApp) constructor\n")

  }

//===============>>> vModalDialog::~vModalDialog <<<=======================
  vModalDialog::~vModalDialog()
  {

    SysDebug(Destructor,"vModalDialog::~vModalDialog() destructor\n")

  }

//==================>>> vModalDialog::CloseDialog <<<=======================
  void vModalDialog::CloseDialog(void)
  {
    vDialog::CloseDialog();
    _curModal = _oldModal;		// V:1.13
    ::EndDialog(_wDialog,1);
  }

//=================>>> vModalDialog::DialogCommand <<<======================
  void vModalDialog::DialogCommand(ItemVal id, ItemVal retval, CmdType ctype)
  {
    // After the user has selected a command from the dialog,
    // this routine is called with the value

    vDialog::DialogCommand(id, retval, ctype);

    _mdItemVal = retval;		// for full modal return
    _mdItemID = id;			// must go after above call
    _mdCmdType = ctype;
  }

//================>>> vModalDialog::ShowModalDialog <<<======================
  ItemVal vModalDialog::ShowModalDialog(VCONST char* msg, ItemVal& retval)
  {
    // This is a fully modal version that will allow the user a simple
    // wait for an answer without needing the DialogCommand method

    // V:1.13 - Windows doesn't seem to handle nested modals very well...
    // I don't know why. So I've added the logic with _oldModal and _curModal
    // to handle nested modals correctly, I hope...

    _oldModal = _curModal;		// V:1.13
    _curModal = this;
    ShowDialog(msg);			// Display the dialog

    retval = _mdItemVal;
    return _mdItemID;			// return the value
  }

