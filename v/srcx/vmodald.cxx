//===============================================================
// vdialog.cxx - vdialog class functions - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vmodald.h>		// our header
#include <v/vapp.h>

// Define static data of the class
// ... none ...

//=================>>> vModalDialog::vModalDialog <<<=====================
  vModalDialog::vModalDialog(vBaseWindow* creator, VCONST char* title) :
    vDialog(creator, 1, title)		// constructor
  {

    SysDebug(Constructor,"vModalDialog::vModalDialog() constructor\n")

  }

//=================>>> vModalDialog::vModalDialog <<<=====================
  vModalDialog::vModalDialog(vApp* creator, VCONST char* title) :
    vDialog(creator, 1, title)		// constructor
  {

    SysDebug(Constructor,"vModalDialog::vModalDialog(vApp)) constructor\n")

  }

//===============>>> vModalDialog::~vModalDialog <<<=======================
  vModalDialog::~vModalDialog()
  {

    SysDebug(Destructor,"vModalDialog::~vModalDialog() destructor\n")

  }

//==================>>> vModalDialog::CloseDialog <<<=======================
  void vModalDialog::CloseDialog(void)
  {
    _mdDone = 1;			// We are done now, so save values
    vDialog::CloseDialog();
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

    if (id == M_Cancel || id == M_Done || id == M_OK)
      {
	_mdDone = 1;			// We are done now, so save values
      }
  }

//================>>> vModalDialog::ShowModalDialog <<<======================
  ItemVal vModalDialog::ShowModalDialog(VCONST char* msg, ItemVal& retval)
  {
    // This is a fully modal version that will allow the user a simple
    // wait for an answer without needing the DialogCommand method


    ShowDialog(msg);			// Display the dialog

    WaitForX(&_mdDone);			// Wait till we're done

    retval = _mdItemVal;
    return _mdItemID;			// return the value
  }

//===================>>> vModalDialog::ShowDialog <<<=======================
  void vModalDialog::ShowDialog(VCONST char* msg)
  {
    //	Show the dialog with the default message

    _mdDone = 0;		// not done with the dialog

    vDialog::ShowDialog(msg);	// now just use our superclass
  }

//=========================>>> vModalDialog::WaitForX <<<========================
  void vModalDialog::WaitForX(int* flag)
  {
    //	Sometimes we just need to get synchronous. We probably could
    //	define a bunch of extra, awkward callbacks, but sometimes
    //	it is more sensible just to wait.
    //
    //	The only obvious way to wait in X is to take over the
    //	event loop while waiting for some state variable to change.
    //	One of the events will cause the variable to change to 1,
    //	and we can then exit from here. Note that the XtAppNextEvent
    //	will block if there aren't any events, so this isn't just
    //	a busy loop - the system will be free to do useful things.
   
    XEvent an_event;		// need this variable

    *flag = 0;			// The caller provides the flag

    while (! *flag)		// Wait for an event to set the flag
      {
	XtAppProcessEvent(theApp->appContext(),XtIMAll); // handle events
      }
    // After here, the regular event loop will take over
  }
