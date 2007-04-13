//===============================================================
// vframec.h - Frame Command class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VFRAMEC_H
#define VFRAMEC_H

#include <v/vcmd.h>	// We are derived from vcmd

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    class vCmdParent;

    class vFrameCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vFrameCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vFrameCmd();

	virtual void initialize(void);
  	virtual void SetCmdVal(ItemVal val, ItemSetType st);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

      };
#endif
