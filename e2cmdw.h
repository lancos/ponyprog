//=========================================================================//
//-------------------------------------------------------------------------//
// e2cmdw.h -- Header for for e2CmdWindow class                            //
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

#ifndef e2CMDW_H
#define e2CMDW_H

#include <v/vcmdwin.h>	// So we can use vCmdWindow
#include <v/vmenu.h>	// For the menu pane
#include <v/vutil.h>	// For V Utilities
#include <v/vcmdpane.h> // command pane
#include <v/vstatusp.h>	// For the status pane
//#include <v/vtimer.h>	// Timer

#ifdef vDEBUG
#include <v/vdebug.h>
#endif

#include "e2cnv.h"	// e2TextCanvasPane
#include "e2dlg.h"	// e2Dialog

/**
class e2CmdWindow;

class e2Timer : public vTimer
{
      public:		//---------------------------------------- public
	e2Timer(e2CmdWindow* cw) { cmdw = cw; }
	~e2Timer() {}
	virtual void TimerTick();
      private:		//--------------------------------------- private
	e2CmdWindow* cmdw;
};
**/

class e2AppWinInfo;

class e2CmdWindow : public vCmdWindow
{
	friend int AppMain(int, char**);	// allow AppMain access

  public:		//---------------------------------------- public
	e2CmdWindow(char*, int, int);
	virtual ~e2CmdWindow();
	virtual void WindowCommand(ItemVal id, ItemVal val, CmdType cType);
	virtual void KeyIn(vKey keysym, unsigned int shift);

	void SetAWInfo(e2AppWinInfo* awi)
		{ awip = awi; }
	e2AppWinInfo* GetAWInfo() const
		{ return awip; }

	void About();
	void Draw(int rows = 0, int cols = 0);
	void Print();
	int NextLine();
	int PrevLine();
	void ScrollV(int top);

	void PostInit();
	void CharEdit(int row = -1, int col = -1);

	int IsBufChanged() const;
	void SaveFile(int force_select = 0);

	void SetTitle();

	const char *GetFileName() const;

	e2ProgressDialog *GetProgressDlg() const
		{ return e2Prg; }

	//All commands
	int CmdOpen();
	int CmdSave();
	int CmdLastFile1();
	int CmdLastFile2();
	int CmdReload();
	int CmdPrint();
	int CmdRead();
	int CmdWrite();
	int CmdVerify();
	int CmdGetInfo();
	int CmdReset();
	int CmdReadLock();
	int CmdWriteLock();
	int CmdReadSpecial();
	int CmdWriteSpecial();
	int CmdReadSecurity();
	int CmdWriteSecurity();
	int CmdDoubleSize();
	int CmdClearBuf();
	int CmdFillBuf();
	int CmdByteSwap();
	int CmdCalibration();
	int CmdHelp();
	int CmdEditNote();
	int CmdSelectDevice(long new_type);
	int CmdSetDeviceType(ItemVal val);
	int CmdSetDeviceSubType(ItemVal val);

	int SpecialBits(int readonly = 0);

  protected:
	void CmdRemoteMode();

  private:		//--------------------------------------- private

	int NextPage();
	int PrevPage();
	void FirstPage();
	void LastPage();

	void OpenFile(char const *file = 0);
	void UpdateStrFromBuf();
	void UpdateStrFromStr(char const *s1, char const *s2 = 0);
	void UpdateStatusBar();
	void UpdateChipType(int pritype = -1, int subtype = -1);
	void SetChipSubType(int pritype, int subtype = 0);
	void UpdateMenuType(long new_type = 0);
	void UpdateFileMenu();
	int TypeToMenuId(long type);
	long MenuIdToType(int id);
	int OnError(int err, char const *msg = 0);

	long GetDevSize() const;

	int CoordToIndex(int row, int col);
	void IndexToCoord(int index, int &row, int &col);

	// Standard elements
	vMenuPane* e2Menu;				// For the menu bar
	e2TextCanvasPane* e2Canvas;		// For the canvas
	vCommandPane* e2CmdPane;		// for the first command pane
	vCommandPane* e2CmdPane2;		// for the second command pane
	vStatusPane* e2Status;			// For the status bar
//	e2Timer* _timer;				// Timer for Date/Time

	// Dialogs associated with CmdWindow
	e2Dialog* e2Dlg;
	e2ProgressDialog *e2Prg;

	// AppWinInfo associated with this window
	e2AppWinInfo* awip;

	// Info for the TextCanvas
	int first_line;

	// Index for checking type menu
	int type_index;

	// Edit buffer enabled (checked the menu)
	int editbuf_enabled;

	int curIndex;

	//Remote mode flag
	int remote;
};
#endif
