//=========================================================================//
//-------------------------------------------------------------------------//
// sernumdlg.h -- Header for for SerNumDialog class                        //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C)  1997-2001  Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lancos@libero.it                                               //
//  http://www.LancOS.com                                                  //
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

#ifndef SerNumDIALOG_H
#define SerNumDIALOG_H

#include <v/vmodald.h>

#include "types.h"
#include "e2profil.h"
#include "e2awinfo.h"

#include "string_table.h"

class SerNumDialog : public vModalDialog
{
  public:		//---------------------------------------- public
	SerNumDialog(vBaseWindow* bw, char* title = STR_MSGSERNUMCFG);
	virtual ~SerNumDialog();		// Destructor
	virtual int SerNumAction(long &cLoc, bool &cMemType, bool &cAutoInc, FmtEndian &cFmt, int &cLen, DWORD &cVal);

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

};

class OscCalibDialog : public vModalDialog
{
  public:		//---------------------------------------- public
	OscCalibDialog(vBaseWindow* bw, e2AppWinInfo* aw, char* title = STR_MSGOSCCALIBCFG);
	virtual ~OscCalibDialog();		// Destructor
	virtual void DialogCommand(ItemVal,ItemVal,CmdType); // action selected
	virtual int OscCalibAction(long &cLoc, bool &cMemType, BYTE &cVal);

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

	e2AppWinInfo *awip;
};

#endif
