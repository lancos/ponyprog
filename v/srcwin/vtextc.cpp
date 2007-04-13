//===============================================================
// vtextc.cxx	- Text output cmd - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vapp.h>
#include <v/vtextc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vutil.h>

//=====================>>> vTextCmd::vTextCmd <<<=======================
  vTextCmd::vTextCmd(vCmdParent* dp, CommandObject* dc) :
	    vCmd(dp, dc)
  {
    initialize();
  }

//=====================>>> vTextCmd::~vTextCmd <<<=======================
  vTextCmd::~vTextCmd()
  {

    SysDebug(Destructor,"vTextCmd::~vTextCmd() destructor\n")

  }

//=====================>>> vTextCmd::initialize <<<=======================
  void vTextCmd::initialize(void)
  {
    // multiline, boxed text output
    SysDebug(Constructor,"vTextCmd::vTextCmd() constructor\n")

    int numLines;
    char* useText;
    char* winCopy = 0;

    CopyToLocal();			// Make local copies of CmdObject
    char* theText;

    long style = ES_READONLY;

    if (!_itemList || (*(char*)_itemList == 0))
	theText = (char*) _title;
    else
	theText = (char*)_itemList;

    if (!(dlgCmd->attrs & CA_NoBorder))
	style |= WS_BORDER;

    if (!(dlgCmd->attrs & CA_Hidden))	// Check for Hidden
	style |= WS_VISIBLE;


    _w = vTextLen(theText, numLines)*5+4; // set my width AND get numLines

    if (dlgCmd->size > 0)		// may reset title!
      {
	_w = dlgCmd->size;
      }

    style |= ES_LEFT;
    if (_parentWin->paneType() == P_Status)
      {
	_h = (11 * numLines) + 1;	// calc height
      }
    else
      {
	_h = (11 * numLines) + 1;	// calc height

      }

    useText = theText;
    if (numLines > 1)		// have to convert \n to \r\n
      {
	style |= ES_MULTILINE;
	winCopy = new char[(strlen(theText) + numLines + 1)];
	char * cp;
	for (cp = winCopy ; *useText ; )
	  {
	    if (*useText == '\n')     	// add extra \r
		*cp++ = '\r';

	    *cp++ = *useText++;
	  }
	*cp = 0;			// terminate string
	useText = winCopy;
      }

    _parentWin->SetPosition(_x, _y, _w, _h, dlgCmd->cFrame, dlgCmd->cRightOf,
        dlgCmd->cBelow);

    _CtrlOffset = _parentWin->AddDlgControl(_x, _y , _w, _h, _cmdId,
	   style, "EDIT", useText, sizeof(vTextCmd*), (LPBYTE)this);
    if (winCopy != 0)
	delete [] winCopy;
  }

//================>>> vTextCmd::SetCmdVal <<<============================
  void vTextCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {
    SysDebug1(Misc,"vTextCmd::SetCmdVal(val:%d)\n",val)

    HWND myHwnd = GetMyHwnd(_cmdId);
    if (st == Hidden)		// hide or unhide
      {
	if (val)
	  {
	    ::ShowWindow(myHwnd,SW_HIDE);
	  }
	else
	  {
	    ::ShowWindow(myHwnd,SW_SHOW);
	  }
      }
  }

//================>>> vTextCmd::SetCmdStr <<<============================
  void vTextCmd::SetCmdStr(VCONST char* str)
  {

    SysDebug1(Misc,"vTextCmd::SetCmdStr(str:%s)\n",str)

    int numLines;
    VCONST char* useText = str;
    char* winCopy = 0;

    (void) vTextLen(str, numLines);
    if (numLines > 1)		// have to convert \n to \r\n
      {
	winCopy = new char[(strlen(str) + numLines + 1)];
	char * cp;
	for (cp = winCopy ; *useText ; )
	  {
	    if (*useText == '\n')     	// add extra \r
		*cp++ = '\r';

	    *cp++ = *useText++;
	  }
	*cp = 0;			// terminate string
	useText = winCopy;
      }

    ::SetDlgItemText(_parentWin->getParent(),_cmdId, useText);
    if (winCopy != 0)
	delete [] winCopy;
    _title = str;
  }
