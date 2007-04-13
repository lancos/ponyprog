//===============================================================
// vradioc.h - Radio Button Command class - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VRADIOC_H
#define VRADIOC_H

#include <v/vcmd.h>	// We are derived from vcmd

    class V_EXPORT vCmdParent;

    class V_EXPORT vRadioButtonCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vRadioButtonCmd(vCmdParent* dp, CommandObject* co);
	vRadioButtonCmd(const vRadioButtonCmd&);	// Copy constructor

	virtual ~vRadioButtonCmd();

	virtual void initialize(void);

	virtual void ResetItemValue(void);

	virtual int GetCmdValue(ItemVal id) VCONST;

	virtual void SetCmdVal(ItemVal val, ItemSetType st);

	virtual void SetCmdStr(VCONST char* str);

      protected:	//--------------------------------------- protected

	virtual void CmdCallback(int id, WORD codeNotify);

      private:		//--------------------------------------- private

	static vRadioButtonCmd* _RBList;	// list of all radio buttons
	vRadioButtonCmd* _nextRB;		// ptr to next rb
	int _copied;				// For copy constructor

	HWND _wLabel;				// Widget for button label
      };
#endif
