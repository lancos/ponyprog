//===============================================================
// vcbtncmd.h - Color Button Command class - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VCBTNCMD_H
#define VCBTNCMD_H

#include <v/v_defs.h>
#include <v/vapp.h>
#include <v/vbtncmd.h>	// We are derived from vbtncmd
#include <v/vcolor.h>

    class V_EXPORT vCmdParent;

    class V_EXPORT vColorButtonCmd : public vButtonCmd
      {

      public:		//---------------------------------------- public

	vColorButtonCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vColorButtonCmd();

	virtual void SetCmdVal(ItemVal val, ItemSetType st);

        virtual int DRAWITEM(int id, DRAWITEMSTRUCT* dis);


      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private
	int   _canChange;
	vColor _color;	// color of this button
	vColor* _origColor;	// ptr to orig color of this button

	void initColor(void);

      };
#endif
