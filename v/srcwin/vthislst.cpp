//===============================================================
// vThislst.cpp - vThisList class functions - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vthislst.h>

//======================>>> vThisList::vThisList <<<=======================
  vThisList::vThisList()
  {
    // This will keep a list of this pointers accessed by some id
    _cur = 0;		// empty list
    _list = 0;

  }

//======================>>> vThisList::~vThisList <<<=======================
  vThisList::~vThisList()
  {
    // This will keep a list of this pointers accessed by some id
    thisList* next;
    for (thisList* tl = _list ; tl != 0 ; tl = next)
      {
	next = tl->next;	// get next one
	delete tl;
      }
  }

//======================>>> vThisList::Add <<<=======================
  void vThisList::Add(ThisId id, void* This)
  {
    // Add to list
    thisList* newList = new thisList;

    newList->id = id;
    newList->thisPtr = This;
    newList->next = _list;
    _cur = _list = newList;
  }

//======================>>> vThisList::Delete <<<=======================
  void vThisList::Delete(ThisId id)
  {
    // delete from the list
    thisList* next, *prev;

    prev = 0;
    for (thisList* tl = _list ; tl != 0 ; tl = next)
      {
	next = tl->next;
	if (tl->id == id)
	  {
	    if (prev == 0)
		_list = next;
	    else
		prev->next = next;
	    delete tl;
	    _cur = _list;		// keep _cur valid
	    return;
	  }
	prev = tl;
      }
  }

//======================>>> vThisList::GetThis <<<=======================
  void* vThisList::GetThis(ThisId id) VCONST
  {
    for (thisList* tl = _list ; tl != 0 ; tl = tl->next)
      {
	if (tl->id == id)
	  {
	    _cur = tl;		// keep cur valid
	    return tl->thisPtr;
	  }
      }
    return 0;
  }

//======================>>> vThisList::GetFirstThis <<<=======================
  void* vThisList::GetFirstThis() VCONST
  {
    _cur = _list;		// start at front
    if (_cur)
	return _cur->thisPtr;
    else
	return 0;
  }

//======================>>> vThisList::GetNextThis <<<=======================
  void* vThisList::GetNextThis() VCONST
  {
    if (_cur)
      {
	_cur = _cur->next;	// point to next entry
	if (_cur)
	    return _cur->thisPtr;
      }

    return 0;
  }
