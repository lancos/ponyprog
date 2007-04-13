//===============================================================
// vlablec.h - Lable Command Class - Windows
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

    class V_EXPORT vCmdParent;
    class V_EXPORT vIcon;

    class V_EXPORT vLabelCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vLabelCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vLabelCmd();

	virtual void initialize(void);

	virtual void SetCmdStr(VCONST char* str);
	virtual void SetCmdVal(ItemVal val, ItemSetType st);
	virtual void CmdCallback(int id, WORD codeNotify);

	virtual int MEASUREITEM(int id, MEASUREITEMSTRUCT* mis);
	virtual int DRAWITEM(int id, DRAWITEMSTRUCT* dis);

      protected:	//--------------------------------------- protected
        void initIcon(void);
      private:		//--------------------------------------- private
	vIcon* _ip;

      };
#endif
