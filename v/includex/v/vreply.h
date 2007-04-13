//===============================================================
// vreply.h - reply modal dialog
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

    class vApp;
    class vReplyDialog : protected vModalDialog
      {
      public:		//---------------------------------------- public
	vReplyDialog(vBaseWindow* bw, VCONST char* title = "Reply") : vModalDialog(bw, title)
	  {added = 0; df = 0;}
	vReplyDialog(vApp *aw, char* VCONST title = "Reply") : vModalDialog(aw, title)
	  {added = 0; df = 0;}
	~vReplyDialog() { if (df != 0) delete [] df;}

	int Reply(VCONST char* msg, char* reply, int maxlen, VCONST char* dflt="");

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

	virtual void DialogDisplayed();
	virtual void DialogCommand(ItemVal id, ItemVal val, CmdType ctype);
	int added;
	char* df;

      };
#endif
