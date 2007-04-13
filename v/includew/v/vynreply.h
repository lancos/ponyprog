//===============================================================
// vynreply.h - yes no dialog - Windows
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

    class V_EXPORT vApp;
    class V_EXPORT vYNReplyDialog : protected vModalDialog
      {
      public:		//---------------------------------------- public
	vYNReplyDialog(VCONST vBaseWindow* bw, VCONST char* title = "Yes or No") :
		 vModalDialog(bw,title) {added = 0;}
	vYNReplyDialog(VCONST vApp* aw, VCONST char* title = "Yes or No") :
                 vModalDialog(aw,title) { added = 0; } 
	~vYNReplyDialog() {}

  	int AskYN(VCONST char* msg);
	
      protected:	//--------------------------------------- protected

      	static CommandObject YNDialog[];	// dialog elements

      private:		//--------------------------------------- private

	virtual void DialogCommand(ItemVal id, ItemVal val, CmdType ctype);
	int added;
	virtual void DialogDisplayed();		// TEH Dec97

      };
#endif
