//===============================================================
// vnotice.h - notice modal dialog - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VNOTICE_H
#define VNOTICE_H
#include <v/vmodald.h>

    class V_EXPORT vApp;

    class V_EXPORT vNoticeDialog : protected vModalDialog
      {
      public:		//---------------------------------------- public
	vNoticeDialog(VCONST vBaseWindow* bw, VCONST char* title = "Notice") : 
	    vModalDialog(bw,title) { _cmdsAdded = 0; }
	vNoticeDialog(VCONST vApp* aw, VCONST char* title = "Notice") :
	    vModalDialog(aw,title) { _cmdsAdded = 0; }
	~vNoticeDialog() {}

  	void Notice(VCONST char* msg);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

	int _cmdsAdded;
	virtual void DialogCommand(ItemVal id, ItemVal val, CmdType ctype);
	virtual void DialogDisplayed();		// TEH Dec97

      };
#endif
