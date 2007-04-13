//===============================================================
// vcmdpar.h - A parent class for Cmds -- Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================
//
//	This class is used by vDialog and CmdPane classes to
//	allow the vCmd objects to have access to the ProcessCmd

#ifndef VCMDPAR_H
#define VCMDPAR_H

#include <v/v_defs.h>
#include <v/vcmd.h>

enum DialogType  { aDialog, aCmdBar,aStatBar };

    typedef struct DlgCmdList
      {
	vCmd* cmdP;		// pointer to a command object
	DlgCmdList* nextDCL;	// next button
      } DlgCmdList;

    class V_EXPORT vCmdParent
      {
      public:		//---------------------------------------- public

	vCmdParent(PaneType pt = P_Dialog);

	virtual ~vCmdParent();

	virtual void ProcessCmd(ItemVal id, ItemVal rv, CmdType ct) {}

  	virtual int GetTextIn(ItemVal id, char* strout, int maxlen);
	virtual int GetValue(ItemVal id) VCONST;
	int HasId(ItemVal id) VCONST;
	virtual void SetValue(ItemVal id, ItemVal val, ItemSetType setType);
	virtual void SetString(ItemVal id, VCONST char* str);
	virtual void SetFrameChildren(ItemVal frameId, int frameVal);

	PaneType paneType() { return _pt; }

	// These are platform dependent...
	vCmd* getThisFromId(ItemVal find_id);

	void SetPosition(int& x, int& y, int w, int h, ItemVal frame = 0,
	 ItemVal rightOf = 0, ItemVal below = 0, int isSpinner = 0);

	int CreateDlgTemplate(LONG dtStyle,int dtX, int dtY,
	    int dtCX, int dtCY, LPSTR dtMenuName, LPSTR dtClassName,
	    LPSTR dtCaption, short int PointSize, LPSTR szTypeFace);

	int AddDlgControl (int dtilX, int dtilY, int dtilCX, int dtilCY,
		int dtilID, long dtilStyle, LPSTR dtilClass,
		LPCSTR dtilText,BYTE dtilInfo, LPBYTE dtilData);

	void DoneAddingControls(void);

	HWND getParent(void) VCONST;		// to get parent widget
	DialogType _dialogType;
	HWND HwndDialog() { return _wDialog; }

      protected:	//--------------------------------------- protected

	DlgCmdList* _cmdList;		// list of command items for this dialog

	HWND _wDialog;			// handle of dialog
	HGLOBAL _hTemplate;		// handle for template
        int _callbackID;		// index to callback array
	int maxX, maxY;			// for calculating size of dialog
	int _windowH;			// height of window
	int _windowY;			// where a command bar is actually drawn

	virtual vCmd* AddCmd(CommandObject* cmd);	// Add a Cmd to the dialog
	void FixFrame(int cxr, int cyb, int margin, vCmd* frameCmd);

      private:		//--------------------------------------- private
	PaneType _pt;

      };
#endif
