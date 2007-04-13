//===============================================================
// vbaseitm.cpp - the base class that holds handle information - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vbaseitm.h>		// my defines
#include <string.h>

//=====================>>> vBaseItem::vBaseItem <<<====================
  vBaseItem::vBaseItem(VCONST char* name)		// constructor
  {
    SysDebug1(Constructor,"\nvBaseItem::vBaseItem(%s) constructor\n",name)

    _vHandle = NULL;			// No widget yet
    _name = new char[strlen(name)+1];	// build a copy
    strcpy(_name,name);
    _copied = 0;			// not copied
  }

//=====================>>> vBaseItem::vBaseItem <<<====================
  vBaseItem::vBaseItem(const vBaseItem& b)		// Copy constructor
  {
    SysDebug(Constructor,"\nvBaseItem::vBaseItem(const vBaseItem) Copy constructor\n")
	
    _name = b._name;		// name of item
    _vHandle = b._vHandle;	// Windows HANDLE
    _copied = 1;		// This instance IS a copy

  }

//=====================>>> vBaseItem::~vBaseItem <<<====================
  vBaseItem::~vBaseItem()				// destructor
  {

    if (_copied)			// don't destruct if a copy
      {
	SysDebug1(Destructor,"vBaseItem::~vBaseItem: NOT destroying copy %s\n\n",_name)
	return;
      }
    SysDebug1(Destructor,"vBaseItem::~vBaseItem: destroying %s\n\n",_name)

    if (_vHandle)
      {
	_vHandle = NULL;
      }
    delete [] _name;		// free this space
  }
