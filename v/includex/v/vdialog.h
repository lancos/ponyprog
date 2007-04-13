//===============================================================
// vdialog.h - general purpose dialog class - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VDIALOG_H
#define VDIALOG_H
#include <v/vbasewin.h>		// We inherit form vBaseWindow
#include <v/vcmdprnt.h>		// We also inherit from vCmdParent

    class vCmd;		// Dialogs have Cmds
    class vApp;

    class vDialog : public vBaseWindow, public vCmdParent
      {

      public:		//---------------------------------------- public

	vDialog(vBaseWindow* creator, int modal = 0, 
		VCONST char* title = "");  // The construtor
	vDialog(vApp* creator, int modal = 0, 
		VCONST char* title = "");  // The construtor
	vDialog(const vDialog&);		// Copy constructor
	~vDialog();

	void init(int modal, VCONST char* title);	// finish constructor
  	virtual void AddDialogCmds(CommandObject* cList);  // Add a list of Cmds
 	virtual void CancelDialog(void);		// Cancel selected
 	virtual void CloseDialog(void);			// Close  dialog
	virtual void SetDialogTitle(VCONST char * title);

	// Dialog action selected
	virtual void DialogCommand(ItemVal id, ItemVal retval, CmdType ctype);
	virtual void DialogDisplayed();

	int IsDisplayed(void) VCONST { return _IsDisplayed; }  // If displayed or not
	virtual void GetDialogPosition(int& left, int& top, int& width, int& height) VCONST;
	virtual void SetDialogPosition(int left, int top);
	virtual void ShowDialog(VCONST char* msg);	// called to invoke dialog


	// functions from vCmdParent to override

	virtual void ProcessCmd(ItemVal id, ItemVal rv, CmdType ct);


      protected:	//--------------------------------------- protected

	virtual void showBaseWindow(void) { };	// no-op for dialog

      private:		//--------------------------------------- private

	int _modal;			// if modal or modeless

	Widget _parentHandle;

	Widget _dialogFrame;

	int _IsDisplayed;		// if popped up or not
	int _addedWDW;


	vCmd* _DefaultButton;		// for easy Warp to default button
	vCmd* _FirstTextIn;		// for SetKeyBoardFocus

      };
#endif
