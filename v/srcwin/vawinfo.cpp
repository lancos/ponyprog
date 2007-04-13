//===============================================================
// vawinfo.cxx - vAppWinInfo utility class for tracking windows
//
// Copyright (C) 1995.1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vawinfo.h>
#include <string.h>

//=======================>>> vAppWinInfo::vAppWinInfo <<<===================
  vAppWinInfo::vAppWinInfo(VCONST char* name, VCONST void* ptr)
  {
    SysDebug1(Constructor,"vAppWinInfo::vAppWinInfo(%s) - constructor\n",name);

    _infoName = new char[strlen(name)+1];
    strcpy(_infoName, name);
    _ptr = ptr;
  }

//=======================>>> vAppWinInfo::~vAppWinInfo <<<===================
  vAppWinInfo::~vAppWinInfo()
  {
    SysDebug(Destructor,"vAppWinInfo::~vAppWinInfo() - desstructor\n");
    delete [] _infoName;
  }
