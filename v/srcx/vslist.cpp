//===============================================================
// vSList Class for working with C_List char string lists
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/v_defs.h>

#include <v/vslist.h>

#include <v/vutil.h>

//===========================>>> vSList::vSList <<<=======================
  vSList::vSList(int maxsize)
  {
    list = new char*[maxsize];
    max = maxsize;
    init();
  }

//===========================>>> vSList::~vSList <<<=======================
  vSList::~vSList()
  {
    erase();		// delete contents
    delete [] list;	// delete the array itself
  }

//=============================>>> vSList::= <<<==========================
//  vSList& vSList::operator =(const vSList& slist)
//  {
//    if (this == &slist)
//	return *this;
//
//  }

//=============================>>> vSList::init <<<==========================
  void vSList::init()
  {
    for (int ix = 0 ; ix < max ; ++ix)
	list[ix] = 0;
  }

//=============================>>> vsList::erase <<<==========================
  void vSList::erase()
  {
    for (int ix = 0 ; ix < max ; ++ix)
      {
	if (list[ix] != 0)
	    delete [] list[ix];
	list[ix] = 0;
      }
  }

//========================>>> vSList::size <<<==========================
  int vSList::size() const
  {
    int ix;
    for (ix = 0 ; ix < max && list[ix] != 0 ; ++ix)
	;
    return ix;
  }

//========================>>> vSList::insert <<<==========================
  int vSList::insert(int insAt, const char* strn) const
  {
    int items = size();
    int iWhere = insAt;

    if (items >= max)
	return -1;

    if (iWhere < 0 || iWhere >= max)	// append to end
	iWhere = items;

    // First, shift the list down by one

    for (int ix = items + 1 ; ix > iWhere ; --ix)
	list[ix] = list[ix-1];

    list[iWhere] = new char[strlen(strn)+1];
    strcpy(list[iWhere],strn);
    return items + 1;
  }

//========================>>> vSList::replace <<<==========================
  int vSList::replace(int repAt, const char* strn) const
  {
    int items = size();
    int iWhere = repAt;

    if (items >= max || iWhere < 0 || iWhere >= items)
	return -1;

    if (list[iWhere] == 0)
	return -1;

    delete [] list[iWhere];
    list[iWhere] = new char[strlen(strn)+1];
    strcpy(list[iWhere],strn);
    return items;
  }

//========================>>> vSList::deleteItem <<<==========================
  int vSList::deleteItem(int delAt) const
  {
    int items = size();
    int iWhere = delAt;

    if (iWhere < 0 || iWhere > max)	// delete last item
	iWhere = items - 1;

    if (iWhere >= items)
	return -1;

    // First, delete the given item

    if (list[iWhere] == 0)	// oops!
	return -1;

    delete [] list[iWhere];	// free the space!

    // Now, shift the list up by one

    for (int ix = iWhere ; ix < items ; ++ix)
	list[ix] = list[ix+1];

    --items;
    list[items] = 0;

    return items;
  }
