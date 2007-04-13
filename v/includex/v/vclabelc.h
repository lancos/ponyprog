//===============================================================
// vclabelc.h - Color Label Command class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VCLABELC_H
#define VCLABELC_H

#include <v/v_defs.h>
#include <v/vapp.h>
#include <v/vlabelc.h>	// We are derived from vbtncmd
#include <v/vcolor.h>

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    class vCmdParent;

    class vColorLabelCmd : public vLabelCmd
      {

      public:		//---------------------------------------- public

	vColorLabelCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vColorLabelCmd();

	virtual void SetCmdVal(ItemVal val, ItemSetType st);


      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

	vColor _color;	// color of this button
	vColor* _origColor;

	Pixel _pixel;

	void initColor(void);

      };
#endif
