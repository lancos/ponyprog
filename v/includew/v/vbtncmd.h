//===============================================================
// vbtncmd.h - Button Command class -- Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VBTNCMD_H
#define VBTNCMD_H

#include <v/vapp.h>
#include <v/vcmd.h>	// We are derived from vcmd

    class V_EXPORT vCmdParent;
    class V_EXPORT vIcon;

    class V_EXPORT vButtonCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vButtonCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vButtonCmd(void);

	virtual void initialize(void);

        virtual int GetCmdValue(ItemVal id) VCONST;
        virtual void ResetItemValue(void);
	virtual void SetCmdVal(ItemVal val, ItemSetType st);

	virtual void SetCmdStr(VCONST char* str);

	virtual int MEASUREITEM(int id, MEASUREITEMSTRUCT* mis);
	virtual int DRAWITEM(int id, DRAWITEMSTRUCT* dis);

      protected:	//--------------------------------------- protected
        virtual void CmdCallback(int id, WORD codeNotify);

      private:		//--------------------------------------- private
	void initIconButton(long style);
	vIcon* _ip;

      };
#endif
