//=========================================================================//
//-------------------------------------------------------------------------//
// filldlg.h -- Header for for FillDialog class                            //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C)  1997-1999  Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lanconel@cs.unibo.it                                           //
//  http://www.cs.unibo.it/~lanconel                                       //
//                                                                         //
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

class FillDialog : public vModalDialog
{
  public:		//---------------------------------------- public
	FillDialog(vBaseWindow* bw, long cfrom = -1, long cto = -1, int cval = -1, char* title = "Fill Buffer");
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
