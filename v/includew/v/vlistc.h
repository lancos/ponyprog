//===============================================================
// vlistc.h - List Command class - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VLISTC_H
#define VLISTC_H

#include <v/vcmd.h>	// We are derived from vcmd

    class V_EXPORT vCmdParent;

    class V_EXPORT vListCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vListCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vListCmd();

	virtual int GetCmdValue(ItemVal id) VCONST;
	virtual void SetCmdVal(ItemVal val, ItemSetType st);

      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private

        virtual void CmdCallback(int id, WORD codeNotify);

	char** _fullList;		// alias pointer to full list
	int _maxWidth;			// max width of any item
	int _numItems;			// number of items on list
	int _curSelection;		// index of current selected item
        int _listRows;			// how many rows to display

	void SetupList(void);
      };
#endif
