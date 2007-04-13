//===============================================================
// vspinc.h - spinner- Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VSPINC_H
#define VSPINC_H

#include <v/vcmd.h>	// We are derived from vcmd

    class V_EXPORT vCmdParent;

    class V_EXPORT vSpinnerCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vSpinnerCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vSpinnerCmd();

	virtual void initialize(void);

	virtual int GetCmdValue(ItemVal id) VCONST;
	virtual void SetCmdVal(ItemVal val, ItemSetType st);

      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private

	char** _fullList;		// alias pointer to full list
	int _maxWidth;			// max width of any item
	int _numItems;			// number of items on list
	int _maxVal;
	int _minVal;
	int _step;
	int _curSelection;		// index of current selected item
	int _TextList;
	int _textId;			// my text id

	static int _textIdPool; // For text portion of spinner

	void SetupList(void);
	virtual void CmdCallback(int id, WORD codeNotify);

      };
#endif
