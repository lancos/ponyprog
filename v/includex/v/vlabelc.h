//===============================================================
// vlablec.h - Lable Command Class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VLABELC_H
#define VLABELC_H

#include <v/vapp.h>
#include <v/vcmd.h>	// We are derived from vcmd

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    class vCmdParent;

    class vLabelCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vLabelCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vLabelCmd();

	virtual void initialize(void);

	virtual void SetCmdStr(VCONST char* str);
	virtual void SetCmdVal(ItemVal val, ItemSetType st);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

      };
#endif
