//===============================================================
// vmodald.h - general purpose modal dialog class - Windows
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

    class V_EXPORT vApp;

    class V_EXPORT vModalDialog : public vDialog
      {
      public:		//---------------------------------------- public

	vModalDialog(VCONST vBaseWindow* creator, VCONST char* title = "");
	vModalDialog(VCONST vApp* creator, VCONST char* title = "");
	virtual ~vModalDialog();

 	virtual void CloseDialog(void);

	// called when command entered
	virtual void DialogCommand(ItemVal id, ItemVal retval, CmdType ctype);

	virtual ItemVal ShowModalDialog(VCONST char* msg, ItemVal& retval);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

	ItemVal _mdItemVal;
	ItemVal _mdItemID;
	CmdType _mdCmdType;
	char* _mdStrVal;

      };
#endif
