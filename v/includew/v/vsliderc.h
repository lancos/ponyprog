//===============================================================
// vslider.h - List Command class - Windows
//
// Copyright (C) 1995-1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VSLIDERC_H
#define VSLIDERC_H

#include <v/vcmd.h>	// We are derived from vcmd

    class V_EXPORT vCmdParent;

    class V_EXPORT vSliderCmd : public vCmd
      {

      public:		//---------------------------------------- public

	vSliderCmd(vCmdParent* dp, CommandObject* co);

	virtual ~vSliderCmd();

	virtual void initialize();

	virtual int GetCmdValue(ItemVal id) VCONST;
	virtual void SetCmdVal(ItemVal val, ItemSetType st);

      protected:	//--------------------------------------- protected



      private:		//--------------------------------------- private

	int _curVal;		// current value

	int _minVal;		// Can scale to min and val
	int _maxVal;

	virtual void CmdCallback(int id, WORD codeNotify);
  };
#endif
