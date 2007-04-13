//===============================================================
// vfilesel.h - reply modal dialog - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VFILESEL_H
#define VFILESEL_H
#include <v/v_defs.h>
#include <commdlg.h>


    class V_EXPORT vApp;
    class V_EXPORT vBaseWindow;

    class V_EXPORT vFileSelect
      {
      public:		//---------------------------------------- public
	vFileSelect(vBaseWindow* bw, VCONST char* title = "File Select");

	vFileSelect(vApp* aw, VCONST char* title = "File Select");
	~vFileSelect() {}

	int FileSelect(const char* msg, char* filename,
	    const int maxlen, char** filter, int& filterIndex,
	    const char* initDir = 0);
	int FileSelectSave(const char* msg, char* filename,
	    const int maxlen, char** filter, int& filterIndex,
            const int findDir = 0, const char* initDir = 0);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

	HWND _parentHWND;

	OPENFILENAME ofn;	// Windows struct

	void init();

	void buildFilter(char* fbuff, char** filter, int& fi);

	void strcatEOS(char* to, VCONST char *from);


      };
#endif
