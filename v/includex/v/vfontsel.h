//===============================================================
// vfontsel.h - font select modal dialog
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VSELFONT_H
#define VSELFONT_H
#include <v/vmodald.h>
#include <v/vfont.h>

    class vApp;

    class vFontSelect : protected vModalDialog
      {
      public:		//---------------------------------------- public
	vFontSelect(vBaseWindow* bw, VCONST char* title = "Select Font") :
	    vModalDialog(bw, title) {_added = 0;}
	vFontSelect(vApp* aw, VCONST char* title = "Select Font") :
	    vModalDialog(aw, title) {_added = 0;}
	~vFontSelect() {}

	int FontSelect(vFont& font, const char* msg = "Select Font",
		int fixedOnly = 0 );

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

	virtual void DialogCommand(ItemVal id, ItemVal val, CmdType ctype);
 	int _added;

      };
#endif
