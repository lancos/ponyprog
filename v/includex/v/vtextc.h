//===============================================================
// vtextc.h - Text Output Command Class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VTEXTC_H
#define VTEXTC_H

#include <v/vcmd.h>	// We are derived from vcmd

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    class vCmdParent;

    class vTextCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vTextCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vTextCmd();

	virtual void initialize(void);

	virtual void SetCmdVal(ItemVal val, ItemSetType st);

	virtual void SetCmdStr(VCONST char* str);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

      };
#endif
