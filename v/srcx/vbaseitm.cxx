//===============================================================
// vbaseitm.cpp - the base class that holds handle information - X11R5
//
// Copyright (C) 1995-2000  Bruce E. Wampler
// date: 28 Feb 2000 09:16:09 PM
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Lesser General Public License,
// Version 2.1 This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

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
    _vHandle = b._vHandle;	// X widget
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
	XtDestroyWidget(_vHandle);	// Destroy widget if any
	_vHandle = NULL;
      }
    delete [] _name;		// free this space
  }
