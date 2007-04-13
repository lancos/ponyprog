//===============================================================
// vtextinc.h - Button Command class - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VTEXTIN_H
#define VTEXTIN_H

#include <v/vcmd.h>	// We are derived from vcmd

    class V_EXPORT vCmdParent;

    class V_EXPORT vTextInCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vTextInCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vTextInCmd();

	virtual void initialize(void);

	int GetTextIn(ItemVal id, char* str, int maxlen) VCONST; // to get the text back

	virtual void SetCmdVal(ItemVal val, ItemSetType st);

	virtual void SetCmdStr(VCONST char* str);

        virtual void CmdCallback(int id, WORD codeNotify);


      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private
        char _msg[132];

      };
#endif
