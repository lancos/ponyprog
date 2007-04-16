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

#include "eeprom.h"

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

typedef enum {
	verboseNo,
	verboseErr,
	verboseAll
} VerboseType;

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

	void SetTitle();

	const char *GetFileName() const;

	e2ProgressDialog *GetProgressDlg() const
		{ return e2Prg; }

	//All commands
	int CmdOpen(int type = ALL_TYPE, char *file = 0, long relocation = 0);
	int CmdSave(int type = ALL_TYPE, char *file = 0, long relocation = 0);
	int CmdSaveAs(int type = ALL_TYPE, long relocation = 0);
	int CmdLastFile1();
	int CmdLastFile2();
	int CmdReload();
	int CmdPrint();
	int CmdRead(int type = ALL_TYPE);
	int CmdWrite(int type = ALL_TYPE, bool verify = true);
	int CmdVerify(int type = ALL_TYPE);
	int CmdErase(int type = ALL_TYPE);
	int CmdGetInfo();
	int CmdReset();
	int CmdReadLock();
	int CmdWriteLock();
	int CmdReadSpecial();
	int CmdWriteSpecial();
	int CmdReadSecurity(int display_dialog = 0);
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
	int CmdProgram();
	int CmdSetSerialNumber();
	int CmdReadCalibration(int idx);
	int CmdRunScript(bool test_mode = false);

	int SpecialBits(int readonly = 0);
	int ProgramOptions();
	int SerialNumberOptions();
	int OscCalibOption();

  protected:
//	void CmdRemoteMode();

  private:		//--------------------------------------- private

	int NextPage();
	int PrevPage();
	void FirstPage();
	void LastPage();

	int SaveFile(int force_select = 0);
	int OpenFile(char const *file = 0);
	void UpdateStrFromBuf();
	void UpdateStrFromStr(char const *s1, char const *s2 = 0);
	void UpdateStatusBar();
//	void UpdateChipType(int pritype = -1, int subtype = -1);
//	void SetChipSubType(int pritype, int subtype = 0);
	void UpdateMenuType(long new_type = 0);
	void UpdateFileMenu();
	int TypeToMenuId(long type);
	long MenuIdToType(int id);
	void MenuIdToCbxId(int id, int &idx1, int &idx2);
	void TypeToCbxId(long type, int &idx1, int &idx2);
	void CbxMenuInit();
	long CbxIdToType(int idx1, int idx2);
	int OpenScript(char const *file);

	int OnError(int err_no, char const *msgerr = 0);
	int PlaySoundMsg(bool val);

	long GetDevSize() const;

	int CoordToIndex(int row, int col);
	void IndexToCoord(int index, int &row, int &col);

	int ScriptError(int line_number, int arg_index, char *arg, char *msg = 0);

	// Standard elements
	vMenuPane* e2Menu;				// For the menu bar
	e2TextCanvasPane* e2Canvas;		// For the canvas
	vCommandPane* e2CmdPane;		// for the first command pane
	vCommandPane* e2CmdPane2;		// for the second command pane
	vStatusPane* e2Status;			// For the status bar
//	e2Timer* _timer;				// Timer for Date/Time

	// Dialogs associated with CmdWindow
//	e2Dialog* e2Dlg;
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

	//Verbose mode
	VerboseType verbose;
};
#endif
