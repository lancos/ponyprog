//===============================================================
// vynreply.h - yes no dialog
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VYNREPLY_H
#define VYNREPLY_H
#include <v/vmodald.h>

    class vApp;
    class vYNReplyDialog : protected vModalDialog
      {
      public:		//---------------------------------------- public
	vYNReplyDialog(vBaseWindow* bw, VCONST char* title = "Yes or No") :
		 vModalDialog(bw,title) { _cmdsAdded = 0; }
	vYNReplyDialog(vApp* aw, VCONST char* title = "Yes or No") :
		 vModalDialog(aw,title) { _cmdsAdded = 0; }
	~vYNReplyDialog() {}

  	int AskYN(VCONST char* msg);
	
      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

	int _cmdsAdded;
	virtual void DialogCommand(ItemVal id, ItemVal val, CmdType ctype);

      };
#endif
