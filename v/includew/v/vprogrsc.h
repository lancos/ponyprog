//===============================================================
// vprogrsc.h - List Command class - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VPROGRESSC_H
#define VPROGRESSC_H

#include <v/vcmd.h>	// We are derived from vcmd

    class V_EXPORT vCmdParent;

    class V_EXPORT vProgressCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vProgressCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vProgressCmd();

	virtual void initialize();

	virtual int GetCmdValue(ItemVal id) VCONST;
	virtual void SetCmdVal(ItemVal val, ItemSetType st);

	virtual int MEASUREITEM(int id, MEASUREITEMSTRUCT* mis);
	virtual int DRAWITEM(int id, DRAWITEMSTRUCT* dis);


      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private

 	HWND _wLabel;			// For a label

	int _minVal;
	int _maxVal;

	int _curVal;			// current value
	int _isVert;

      };
#endif
