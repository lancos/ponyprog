//===============================================================
// vframec.h - Frame Command class - Windows
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

    class V_EXPORT vCmdParent;

    class V_EXPORT vFrameCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vFrameCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vFrameCmd();

	virtual void initialize(void);
	virtual void SetCmdVal(ItemVal val, ItemSetType st);

	virtual void CmdCallback(int id, WORD codeNotify);

	virtual int MEASUREITEM(int id, MEASUREITEMSTRUCT* mis);
	virtual int DRAWITEM(int id, DRAWITEMSTRUCT* dis);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

      };
#endif
