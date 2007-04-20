//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id$
//-------------------------------------------------------------------------//
//                                                                         //
// This program is free software; you can redistribute it and/or           //
// modify it under the terms of the GNU  General Public License            //
// as published by the Free Software Foundation; either version2 of        //
// the License, or (at your option) any later version.                     //
//                                                                         //
// This program is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
// General Public License for more details.                                //
//                                                                         //
// You should have received a copy of the GNU  General Public License      //
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//

#ifndef FillDIALOG_H
#define FillDIALOG_H
#include <v/vmodald.h>

#include "types.h"
#include "string_table.h"

class FillDialog : public vModalDialog
{
  public:		//---------------------------------------- public
	FillDialog(vBaseWindow* bw, long cfrom = -1, long cto = -1, int cval = -1, char* title = STR_LBLFILLBUF);
	virtual ~FillDialog();		// Destructor
	virtual int fillAction(char* msg);

	long GetFrom() const
		{ return mFrom; }
	long GetTo() const
		{ return mTo; }
	int GetValue() const
		{ return mVal; }

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

	long mFrom, mTo;
	int mVal;
};

#endif
