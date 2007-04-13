//===============================================================
// vFind.h - reply modal dialog - common Win/X
//
// Copyright (C) 1995-1999  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef vFind_H
#define vFind_H
#include <v/vmodald.h>

#ifndef V_EXPORT
#define V_EXPORT
#endif

    class V_EXPORT vApp;
    class V_EXPORT vFindDialog : protected vModalDialog
      {
      public:		//---------------------------------------- public
	vFindDialog(vBaseWindow* bw, VCONST char* title = "Find") :
		 vModalDialog(bw, title)
	   { added = 0;}
	vFindDialog(vApp *aw, VCONST char* title = "Find") : 
		vModalDialog(aw, title)
	   { added = 0;}
	~vFindDialog() {}

	int AskFindPat(char* reply, const int maxlen, int& caseSens,
		int& Down, int& Wrap);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

 	int added;
	virtual void DialogCommand(ItemVal id, ItemVal val, CmdType ctype);

      };
#endif
