//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: modaldlg_utilities.cpp,v 1.1 2008/01/06 17:36:22 lancos Exp $
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

// #include <v/vnotice.h>
// #include "modaldlg_utilities.h"

//========================>>> SetCommandObject <<<==============================
int SetCommandObject(const int id, const int val, CommandObject *CmdList)
{
	for (CommandObject *cur = CmdList ; cur->cmdType != C_EndOfList ; ++cur)
	{
		if (cur->cmdId == id)
		{
			cur->retVal = val;
			return 1;
		}
	}

	SysDebug1(BadVals, "SetCommandObject(id:%d...) - No match in list\n", id)

	return 0;
}

//========================>>> SetCBellow <<<==============================
int SetCBelow(const int id, const ItemVal val, CommandObject *CmdList)
{
	int rval = 0;
#ifdef  _WINDOWS

	for (CommandObject *cur = CmdList ; cur->cmdType != C_EndOfList ; ++cur)
	{
		if (cur->cmdId == id)
		{
			cur->cBelow = val;

			rval = 1;
			break;
		}
	}

	if (rval == 0)
	{
		SysDebug1(BadVals, "ClearCBelow(id:%d...) - No match in list\n", id)
	}

#endif
	return rval;
}

//========================>>> SetCommandHidden <<<==============================
int SetCommandHidden(const int id, const bool val, CommandObject *CmdList)
{
	int rval = 0;

	for (CommandObject *cur = CmdList ; cur->cmdType != C_EndOfList ; ++cur)
	{
		if (cur->cmdId == id)
		{
			if (val)
			{
				cur->attrs |= CA_Hidden;
			}
			else
			{
				cur->attrs &= ~CA_Hidden;
			}


			rval = 1;
			break;
		}
	}

	if (rval == 0)
	{
		SysDebug1(BadVals, "SetCommandHidden(id:%d...) - No match in list\n", id)
	}

	return rval;
}

//========================>>> SetCommandArrayHidden <<<==============================
int SetCommandArrayHidden(const int id, const int n, const bool val, CommandObject *CmdList)
{
	int k;
	int rval = 0;

	for (k = 0; k < n; k++)
	{
		for (CommandObject *cur = CmdList ; cur->cmdType != C_EndOfList ; ++cur)
		{
			if (cur->cmdId == id + k)
			{
				if (val)
				{
					cur->attrs |= CA_Hidden;
				}
				else
				{
					cur->attrs &= ~CA_Hidden;
				}

				rval = 1;
				break;
			}
		}

		if (rval == 0)
		{
			SysDebug1(BadVals, "SetCommandArrayHidden(id:%d...) - No match in list\n", id + k)
		}
	}

	return rval;
}

//========================>>> SetCommandSensitive <<<==============================
int SetCommandSensitive(const int id, const int val, CommandObject *CmdList)
{
	for (CommandObject *cur = CmdList ; cur->cmdType != C_EndOfList ; ++cur)
	{
		if (cur->cmdId == id)
		{
			cur->Sensitive = val;
			return 1;
		}
	}

	SysDebug1(BadVals, "SetCommandObject(id:%d...) - No match in list\n", id)

	return 0;
}

//========================>>> SetCommandLabel <<<==============================
int SetCommandLabel(const int id, char *str, CommandObject *CmdList)
{
	for (CommandObject *cur = CmdList ; cur->cmdType != C_EndOfList ; ++cur)
	{
		if (cur->cmdId == id)
		{
			cur->title = str;
			return 1;
		}
	}

	SysDebug1(BadVals, "SetCommandObject(id:%d...) - No match in list\n", id)

	return 0;
}

//========================>>> GetCommandObject <<<==============================
int GetCommandObject(const int id, CommandObject *CmdList)
{
	for (CommandObject *cur = CmdList ; cur->cmdType != C_EndOfList ; ++cur)
	{
		if (cur->cmdId == id)
		{
			return cur->retVal;
		}
	}

	SysDebug1(BadVals, "GetCommandObject(id:%d...) - No match in list\n", id)

	return 0;
}
// ---------------------------------------------------------------------
