//=========================================================================//
//-------------------------------------------------------------------------//
// infomdlg.h -- Header for infoModalDialog class                          //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997, 1998  Claudio Lanconelli                           //
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

#ifndef infoMDLG_H
#define infoMDLG_H

#include "types.h"
#include <v/vmodald.h>

#include "string_table.h"

class e24xx_infoModalDialog : public vModalDialog
{
  public:		//---------------------------------------- public
	e24xx_infoModalDialog(vBaseWindow* bw, int rlv, UWORD crc, long size, char* title = STR_MSGDEVINFO);
	virtual ~e24xx_infoModalDialog();		// Destructor
	virtual int infoAction(char* msg = "Device Info");

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private
	char *strptr[4];
};

class other_infoModalDialog : public vModalDialog
{
  public:		//---------------------------------------- public
	other_infoModalDialog(vBaseWindow* bw, long fsize, long esize, UWORD crc, char* title = STR_MSGDEVINFO);
	virtual ~other_infoModalDialog();		// Destructor
	virtual int infoAction(char* msg = "Device Info");

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private
	char *strptr[4];
};

class notesModalDialog : public vModalDialog
{
  public:		//---------------------------------------- public
	notesModalDialog(vBaseWindow* bw, char* id = 0, char* cm = 0, char* title = STR_MSGDEVNOTE);
	virtual ~notesModalDialog();		// Destructor
	virtual int notesAction(char* msg = STR_MSGDEVNOTE);

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

};

class editModalDialog : public vModalDialog
{
  public:		//---------------------------------------- public
	editModalDialog(vBaseWindow* bw, int curval = 0, char* title = STR_MSGEDITBUG);
	virtual ~editModalDialog();		// Destructor
	virtual int editAction(char* msg, int &retval);

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private
	int oldval;
};

class editModalDialog2 : public vModalDialog
{
  public:		//---------------------------------------- public
	editModalDialog2(vBaseWindow* bw, char *curval = "", char* title = STR_MSGEDITBUG);
	virtual ~editModalDialog2();		// Destructor
	virtual int editAction(char* msg, char *text, int len);

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private
};

#endif

