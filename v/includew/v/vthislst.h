//===============================================================
// vThislst.h - vTimer class functions - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================
#ifndef VTHISLST_H
#define VTHISLST_H
#include <v/v_defs.h>

  typedef unsigned long ThisId;
  typedef struct thisList
    {
      ThisId id;
      void* thisPtr;
      thisList* next;
    } thisList;

  class V_EXPORT vThisList
    {
    public:		//---------------------------------------- public
      vThisList();
      ~vThisList();

      void Add(ThisId id, void* This);
      void* GetThis(ThisId id) VCONST;
      void Delete(ThisId id);
      void* GetFirstThis() VCONST;
      void* GetNextThis() VCONST;

    protected:		//--------------------------------------- protected
    private:		//--------------------------------------- private
      thisList* _list;
      VMUTABLE thisList* _cur;
    };
#endif
