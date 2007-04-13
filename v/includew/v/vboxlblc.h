//===============================================================
// vboxlblc.h - boxed label class - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VBOXLBLC_H
#define VBOXLBLC_H

#include <v/vcmd.h>	// We are derived from vcmd

    class V_EXPORT vCmdParent;

    class V_EXPORT vBoxedLabelCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vBoxedLabelCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vBoxedLabelCmd();

	virtual void initialize(void);
	virtual void SetCmdVal(ItemVal val, ItemSetType st);
	virtual void SetCmdStr(VCONST char* str);

	virtual void CmdCallback(int id, WORD codeNotify);

	virtual int MEASUREITEM(int id, MEASUREITEMSTRUCT* mis);
	virtual int DRAWITEM(int id, DRAWITEMSTRUCT* dis);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private
          char _lbl[80];

      };
#endif
