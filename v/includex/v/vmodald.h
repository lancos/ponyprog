//===============================================================
// vmodald.h - general purpose modal dialog class - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VMODALD_H
#define VMODALD_H

#include <v/vdialog.h>		// derived from vDialog

    class vApp;
    class vModalDialog : public vDialog
      {
      public:		//---------------------------------------- public

	vModalDialog(vBaseWindow* creator, VCONST char* title = "");
	vModalDialog(vApp* creator, VCONST char* title = "");
	virtual ~vModalDialog();

 	virtual void CloseDialog(void);

	// called when command entered
	virtual void DialogCommand(ItemVal id, ItemVal retval, CmdType ctype);

	virtual void ShowDialog(VCONST char* msg);	// called to invoke the dialog

	virtual ItemVal ShowModalDialog(VCONST char* msg, ItemVal& retval);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

	void WaitForX(int* flag);
	int _mdDone;		// for returning modally
	ItemVal _mdItemVal;
	ItemVal _mdItemID;
	CmdType _mdCmdType;

      };
#endif
