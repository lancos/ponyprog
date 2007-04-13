//===============================================================
// vcomboc.h - ComboBox - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VCOMBOC_H
#define VCOMOBC_H

#include <v/vcmd.h>	// We are derived from vcmd

    const int MAX_COMBO_LIST = 32;	// put 8 items in the list

    class V_EXPORT vCmdParent;

    class V_EXPORT vComboBoxCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vComboBoxCmd(vCmdParent* dp, CommandObject* dc);

	virtual ~vComboBoxCmd();

	virtual int GetCmdValue(ItemVal id) VCONST;
	virtual void SetCmdVal(ItemVal val, ItemSetType st);

      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private

	char** _fullList;		// alias pointer to full list
	int _maxWidth;			// max width of any item
	int _numItems;			// number of items on list
	int _curSelection;		// index of current selected item

	void SetupComboList(void);

	virtual void CmdCallback(int id, WORD codeNotify);
      };
#endif
