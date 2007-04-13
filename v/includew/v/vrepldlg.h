//===============================================================
// vrepldlg.h - find/replace modal dialog
//
// Copyright (C) 1995-1999  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef vReplDlg_H
#define vReplDlg_H
#include <v/vmodald.h>

#ifndef V_EXPORT
#define V_EXPORT
#endif
    class V_EXPORT vApp;
    class V_EXPORT vReplaceDialog : protected vModalDialog
      {
      public:		//---------------------------------------- public
	vReplaceDialog(vBaseWindow* bw, VCONST char* title = "Find/Replace") :
		 vModalDialog(bw, title)
	   { added = 0;}
	vReplaceDialog(vApp *aw, VCONST char* title = "Find/Replace") : 
		vModalDialog(aw, title)
	   { added = 0;}
	~vReplaceDialog() {}

	int AskReplacePat(char* findpat, const int maxlen, 
			  char* replpat, const int maxrlen,
			  int& caseSens, int& Down,
			  int& Wrap, int& Prompt, int& All);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

 	int added;
	virtual void DialogCommand(ItemVal id, ItemVal val, CmdType ctype);

      };
#endif
