//===============================================================
// vreply.h - reply modal dialog - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VREPLY_H
#define VREPLY_H
#include <v/vmodald.h>

    class V_EXPORT vApp;

    class V_EXPORT vReplyDialog : protected vModalDialog
      {
      public:		//---------------------------------------- public
	vReplyDialog(vBaseWindow* bw, VCONST char* title = "Reply") :
	    vModalDialog(bw, title) { added = 0;}
	vReplyDialog(vApp* aw, VCONST char* title = "Reply") :
	    vModalDialog(aw, title) {added = 0;}
	~vReplyDialog() {}

	int Reply(VCONST char* msg, char* reply, int maxlen,
           VCONST char* dflt = "");

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

	virtual void DialogCommand(ItemVal id, ItemVal val, CmdType ctype);
	int added;
	virtual void DialogDisplayed();		// TEH Dec97

      };
#endif
