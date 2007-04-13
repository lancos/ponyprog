//===============================================================
// vdialog.h - general purpose dialog class - Windows
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

    class V_EXPORT vCmd;		// Dialogs have Cmds
    class V_EXPORT vApp;

    class V_EXPORT vDialog : public vBaseWindow, public vCmdParent
      {
	friend int vChkWinDlgMsgs(MSG *msg);
      public:		//---------------------------------------- public

	// One constructor for windows, one for the app

	vDialog(VCONST vBaseWindow* creator, int modal = 0, 
		VCONST char* title = "");
	vDialog(VCONST vApp* creator, int modal = 0, VCONST char* title = "");
	vDialog(const vDialog&);		// Copy constructor
	~vDialog();

  	virtual void AddDialogCmds(CommandObject* cList);  // Add a list of Cmds
	virtual void AddDialogCmdObj(CommandObject* Cmd, vCmd* CmdInstance);
	virtual void CancelDialog(void);		// Cancel selected
 	virtual void CloseDialog(void);			// Close  dialog
	virtual void SetDialogTitle(VCONST char * title);
        void GetDialogPosition(int& left, int& top, int& width, 
		int& height) VCONST;
        void SetDialogPosition(int left, int top);

	// Dialog action selected
	virtual void DialogCommand(ItemVal id, ItemVal retval, CmdType ctype);

        virtual void DialogDisplayed();

	int IsDisplayed(void) VCONST { return _IsDisplayed; }  // If displayed or not
	virtual void ShowDialog(VCONST char* msg);	// called to invoke dialog

	// Windows utility

	int DynamicDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);


	// functions from vCmdParent to override

	virtual void ProcessCmd(ItemVal id, ItemVal rv, CmdType ct);


      protected:	//--------------------------------------- protected

        void init(int modal, VCONST char* title);

	int _IsDisplayed;		// if popped up or not

	virtual void showBaseWindow(void) { };	// no-op for dialog
	HWND winHwnd() VCONST { return _parentHWND; }
	HWND myHwnd() VCONST { return _myHWND; }	// V:1.13

        // V:1.13
	vDialog* _oldModal;		// pointer of previous Modal this
	static vDialog* _curModal;	// this of current modal


      private:		//--------------------------------------- private

	HWND _parentHWND;
	HWND _myHWND; 			// V:1.13
	
       	HWND hwndToolTip;		// handle to my tip hwnd

	int _modal;			// if modal or modeless

	vCmd* _DefaultButton;		// for easy Warp to default button
	vCmd* _FirstTextIn;		// for SetKeyBoardFocus
    };
#endif
