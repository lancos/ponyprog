//===============================================================
// vfontsel.h - font select modal dialog - Windows
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
#include <v/v_defs.h>
#include <v/vfont.h>
#include <commdlg.h>


    class V_EXPORT vApp;
    class V_EXPORT vBaseWindow;

    class V_EXPORT vFontSelect
      {
      public:		//---------------------------------------- public
	vFontSelect(vBaseWindow* bw, VCONST char* title = "Select Font");
	vFontSelect(vApp* aw, VCONST char* title = "Select Font");
	~vFontSelect() {}

	int FontSelect(vFont& font, const char* msg = "Select Font",
		 int fixedOnly = 0 );

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private
	HWND _parentHWND;
	CHOOSEFONT _cf;
	LOGFONT _lf;

	void init();

      };
#endif
