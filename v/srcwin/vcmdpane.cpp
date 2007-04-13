//===============================================================
// vCommandPane - the command bar pane class used by the vWindow class
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>           // for Win 32 stuff
#include <v/vcmdpane.h>
#include <v/vwindow.h>          // we are part of a window
#include <v/vthislst.h>         // For this list
#include <v/vutil.h>

#include <commctrl.h>		// for tooltips
static HWND hwndToolTip = 0;

// Define static data of the class
  static BOOL CALLBACK DlgProcCB(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

  static vThisList _thisList;   // hide this in this file

//===================>>> vChkCmdPaneMsgs <<<=======================
  int vChkCmdPaneMsgs(MSG *msg)
  {
    // Need this to check if a message should be processed
    // by the dialog message processor instead of the general loop.

    for (vCommandPane* w = (vCommandPane*) _thisList.GetFirstThis() ; w != 0 ;
                  w = (vCommandPane*) _thisList.GetNextThis())
      {
        if (::IsDialogMessage(w->HwndDialog(),msg))
            return 1;
      }
    return 0;
  }


//===================>>> vCommandPane::vCommandPane <<<====================
  vCommandPane::vCommandPane(CommandObject* cList) :
        vPane(P_Commands), vCmdParent(P_Commands)
  {


    SysDebug(Constructor,"vCommandPane::vCommandPane() constructor\n")

    _origList = cList;          // handle to list of commands
    _dialogType = aCmdBar;      // this is a command pane
    _isDisplayed = 1;           // Command bars displayed by default
    _isShown = 0;               // but not yet shown
    _cmdObj = 0;                // no list
    _cmdObjCount = 0;
  }

//===================>>> vCommandPane::vCommandPane <<<====================
  vCommandPane::vCommandPane(vStatus* s_bar) :
        vPane(P_Status), vCmdParent(P_Status)
  {


    _dialogType = aStatBar;      // this is really a command pane
    _isDisplayed = 1;           // Command bars displayed by default
    _isShown = 0;               // but not yet shown
    char* cp;

    SysDebug(Constructor,"vStatusPane::vStatusPane() constructor\n")


    for (_cmdObjCount = 1 ; s_bar[_cmdObjCount].label ; ++_cmdObjCount)
        ;

    _cmdObj = new CommandObject[_cmdObjCount+1];

    for (int ix = 0 ; ix <= _cmdObjCount ; ++ix)
      {
        if (!s_bar[ix].label)           // end of list
          {
            _cmdObj[ix].cmdType = C_EndOfList;
            _cmdObj[ix].title = 0;
            break;
          }
        _cmdObj[ix].cmdType = C_BoxedLabel;


        _cmdObj[ix].cmdId = s_bar[ix].statId;
        _cmdObj[ix].Sensitive = s_bar[ix].sensitive;
        _cmdObj[ix].retVal = 0;
        cp = new char[strlen(s_bar[ix].label)+1];       // copy string
        strcpy(cp,s_bar[ix].label);
        _cmdObj[ix].title = cp;
        _cmdObj[ix].itemList = 0;
        _cmdObj[ix].attrs = s_bar[ix].attrs;
        _cmdObj[ix].cFrame = 0;
        _cmdObj[ix].cRightOf = 0;
        _cmdObj[ix].cBelow = 0;
        _cmdObj[ix].size = s_bar[ix].width;
	_cmdObj[ix].tip = 0;
      }
    _origList = _cmdObj;                // handle to list of commands

  }

//===================>>> vCommandPane::~vCommandPane <<<====================
  vCommandPane::~vCommandPane()
  {

    SysDebug(Destructor,"vCommandPane::~vCommandPane() destructor\n")

    theApp->unregisterCmdPane(this);

    if (_hTemplate !=0)
      {
        ::GlobalFree(_hTemplate);
        _hTemplate = 0;
      }

    if (_cmdObj != 0)
      {
        for (int ix = 0 ; ix < _cmdObjCount ; ++ix)
          {
              if (_cmdObj[ix].title)
                delete [] _cmdObj[ix].title;    // free strings
          }

        delete [] _cmdObj;                              // free list
      }

    _thisList.Delete((ThisId)_wDialog); // free the this ptr
  }

//==================>>> vCommandPane::ProcessCmd <<<=======================
  void vCommandPane::ProcessCmd(ItemVal id, ItemVal rv, CmdType ct)
  {
    // simple interface between command objects and the
    // inherited vCmdParent ProcessCmd to the CommandObject which does the work

    _parentWin->WindowCommand(id, rv, ct);
  }

//====================>>> vCommandPane::GetPaneValue <<<======================
  int vCommandPane::GetPaneValue(ItemVal id, int& val) VCONST
  {
    if (HasId(id))              // make sure the id is in the pane
      {
        val = GetValue(id);
        return 1;               // assume 0 if not found
      }
    else
        return 0;       // can't find this id
  }

//====================>>> vCommandPane::GetPaneTextIn <<<======================
  int vCommandPane::GetPaneTextIn(ItemVal id, char* str, int maxlen) VCONST
  {
    if (HasId(id))              // make sure the id is in the pane
      {
        return GetTextIn(id,str,maxlen);
      }
    else
        return 0;       // can't find this id
  }

//===================>>> vCommandPane::SetPaneValue <<<========================
  void vCommandPane::SetPaneValue(ItemVal id, int val, ItemSetType setType)
  {
    SetValue(id,val,setType);
  }


//====================>>> vCommandPane::SetPaneString <<<======================
  void vCommandPane::SetPaneString(ItemVal id, VCONST char* str)
  {
    SetString(id,str);
  }

//====================>>> vCommandPane::ShowPane <<<======================
  void vCommandPane::ShowPane(int OnOrOff) VCONST
  {
    if (OnOrOff != _isDisplayed)        // only need to do something if changed
     {
       _isDisplayed = OnOrOff;
       FORWARD_vWM_ResizeMDIClient(theApp->_Frame,SendMessage);
     }
  }

//====================>>> vCommandPane::initialize <<<=======================
  void vCommandPane::initialize(vWindow* pWin, HWND mdiChildWin)
  {
     // now, build the menu items in the widget provided

    DlgCmdList* curCmd;


    vPane::initialize(pWin, mdiChildWin);       // initialize base class

    _associatedMdiWindow = mdiChildWin;

    // Now, build the initial in-memory dialog template using
    // Windows HGLOBAL handle _hTemplate.
    // Using 8,helv makes it ok to use constant 8 in calculating control sizes
    int buildOK = CreateDlgTemplate(
	    WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | DS_SETFONT, 0, 0, 1, 1,
       //	    WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS , 0, 0, 1, 1,
            "", "", "", 8, "Helv" );

    if (!buildOK)
        vSysError("vDialog - Unable to build dialog template");


    int ix;
    // Add each control to the template via AddCmd
    for (ix = 0 ; _origList && (_origList[ix].cmdType != C_EndOfList) ; ++ix)
      {
        curCmd = new DlgCmdList;                // get a new cell

        curCmd->nextDCL = _cmdList;             // add in at front
        _cmdList = curCmd;
        curCmd->cmdP = 0;                       // not added yet
        curCmd->cmdP = AddCmd(&_origList[ix]);
      }

    DoneAddingControls();               // All finished adding controls

    // Now ready to create the dialog

    _wDialog = 0;               // Don't have the window handle yet

    // CreateDialogIndirect will create the dialog, then send a WM_INITDIALOG
    // message to make it be displayed.
 
    const DLGTEMPLATE *ptr = (const DLGTEMPLATE*)GlobalLock(_hTemplate);

    (void) ::CreateDialogIndirectParam((HINSTANCE)theApp->vHandle(),
                                       (DLGTEMPLATE*)ptr,
                                       theApp->_Frame,
                                       (DLGPROC) DlgProcCB, LPARAM(this));
 
    ::GlobalUnlock(_hTemplate); // Unlock the template here
    ::GlobalFree(_hTemplate);

    _hTemplate = 0;
  }

// ---------------------------------------------------------------------
//====================>>> DlgProc <<<=======================
  static BOOL CALLBACK DlgProcCB(HWND hDlg, UINT uMsg,
      WPARAM wParam, LPARAM lParam)
  {
    vCommandPane* useThis;

    if (uMsg == WM_INITDIALOG)  // first time!
      {
        useThis = (vCommandPane*)lParam;
        _thisList.Add((ThisId)hDlg, (void*)useThis);
      }
    else
        useThis = (vCommandPane*)_thisList.GetThis((ThisId)hDlg);
    if (!useThis)
        return 0;

    return useThis->DynamicDlgProc(hDlg, uMsg, wParam, lParam);
  }

//====================>>> vCommandPane::OnInitDialog <<<==================
  int vCommandPane::OnInitDialog(HWND hwnd, HWND /* hwndFocus*/, LPARAM lParam)
  {

    // Changes here must have equivlalents in the vDialog.cpp code
    //create the tooltip window
    if (hwndToolTip == 0)
    	hwndToolTip = CreateWindowEx( 0,
           TOOLTIPS_CLASS,
           NULL,
           WS_POPUP | TTS_ALWAYSTIP,
           CW_USEDEFAULT,
           CW_USEDEFAULT,
           10,
           10,
           hwnd,
           NULL,
           (HINSTANCE)theApp->vHandle(),
           NULL);
 
    for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)
      {
        vCmd* cp = cc->cmdP;

            // BEW - V 1.17 - Shouldn't have to do this, but the dynamic
            // dialog creation somehow replaces characters > 128 with _.
            // This fixes that problem.

	if ((cp->dlgCmd)->cmdType == C_BoxedLabel ||
		(cp->dlgCmd)->cmdType == C_Button ||
		(cp->dlgCmd)->cmdType == C_CheckBox ||
		(cp->dlgCmd)->cmdType == C_Label ||
		(cp->dlgCmd)->cmdType == C_RadioButton ||
		(cp->dlgCmd)->cmdType == C_TextIn
	       )
	  {
	    cp->SetCmdStr((cp->dlgCmd)->title);
	  }
	else if ((cp->dlgCmd)->cmdType == C_Text)
	  {
	    if (!(cp->dlgCmd)->itemList ||
		    (*(char *)(cp->dlgCmd)->itemList == 0))
                cp->SetCmdStr((cp->dlgCmd)->title);
            else
                cp->SetCmdStr((char*)(cp->dlgCmd)->itemList);
	  }
	

        if ((cp->dlgCmd)->cmdType == C_CheckBox && (cp->dlgCmd)->retVal)
            cp->SetCmdVal(1,Checked);

        if ((cp->dlgCmd)->cmdType == C_RadioButton && (cp->dlgCmd)->retVal)
            cp->SetCmdVal(1,Checked);

	if ((cp->dlgCmd)->cmdType == C_Slider || (cp->dlgCmd)->cmdType == C_Spinner)
                   cp->SetCmdVal(cp->GetCmdValue((cp->dlgCmd)->cmdId),Value);

        if (!(cp->dlgCmd)->Sensitive)           // Make insensitive if it was
          {
            cp->SetCmdVal(0,Sensitive);
          }
        if ((cp->dlgCmd)->attrs & CA_Hidden)    // Hide it!
          {
            cp->SetCmdVal(1,Hidden);
          }
        if ((cp->dlgCmd)->cmdType == C_List 
	     || (cp->dlgCmd)->cmdType == C_ComboBox
	     || (cp->dlgCmd)->cmdType == C_SComboBox
	     || (cp->dlgCmd)->cmdType == C_Spinner
	     || (cp->dlgCmd)->cmdType == C_SList )
          {
	    int curval = cp->GetCmdValue((cp->dlgCmd)->cmdId);
            cp->SetCmdVal(curval,ChangeList);
          }
        // This call to fake out using OWNERDRAW for statics
        if ((cp->dlgCmd)->cmdType == C_Icon || (cp->dlgCmd)->cmdType == C_Frame)
          {
            cp->SetCmdVal(0,Sensitive); // Trick to avoid getting focus
          }

        // Now add ToolTip if it has a message
        if ((cp->dlgCmd)->tip && *(cp->dlgCmd)->tip)
          {
            TOOLINFO ti;
            // add the control to the tooltip. TTF_SUBCLASS causes the
	    // tooltip to automatically subclass the window and look for the
	    // messages it is interested in.

	    char *cc = (char*) &ti; 
	    int lim = sizeof (ti);
	    for (int i = 0 ; i++ < lim ; *cc++ = 0)
		;

      	    // ZeroMemory(&ti, sizeof(ti));
            ti.cbSize = sizeof(ti);
      	    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
      	    ti.hwnd = hwnd;
      	    ti.uId = (UINT)GetDlgItem(hwnd,(cp->dlgCmd)->cmdId);
            ti.lpszText = (LPSTR) TEXT((cp->dlgCmd)->tip);
      	    SendMessage(hwndToolTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
          }
      }
    _isShown = 1;                               // We will be shown now
    theApp->registerCmdPane(this);              // add our cmd pane
    theApp->selectCmdPanes(_parentWin);         // turn off cmd panes of other windows
    RECT rc;                                    // to get our size

    ::GetClientRect(_wDialog, &rc);             // the rect of the command bar
    _windowY =
    _windowH = rc.bottom - rc.top;

    FORWARD_vWM_ResizeMDIClient(theApp->_Frame,SendMessage);

    return 1;
  }

//====================>>> vCommandPane::OnEnable <<<==================
  void vCommandPane::OnEnable(HWND hwnd, BOOL fEnable)
  {
   // Make all child windows have the same status as the dialog box.

   for (HWND hwndCtl = ::GetFirstChild(hwnd) ; hwndCtl != NULL ;
        hwndCtl = ::GetNextSibling(hwndCtl))
      {
        ::EnableWindow(hwndCtl, fEnable);
      }
  }

//====================>>> vCommandPane::OnPaint <<<==================
  int vCommandPane::OnPaint(HWND /*hwnd*/)
  {

    // Paint dividing lines around command/status bar

    RECT rc;

    int borderH = ::GetSystemMetrics(SM_CYBORDER);
    HWND useH = _wDialog;

    ::GetClientRect(theApp->_Frame, &rc);

    HDC hdc = ::GetDC(useH);

    if (paneType() == P_Status)         // Paint a status bar
      {
        HPEN oldpen = (HPEN)::SelectObject(hdc, theApp->_ShadowPen);
        ::MoveTo(hdc, 0, 0);
        ::LineTo(hdc, rc.right,0);
        ::SelectObject(hdc, theApp->_LightPen);
        ::MoveTo(hdc, 0, 1);
        ::LineTo(hdc, rc.right, 1);

        ::MoveTo(hdc,10,_windowH - 3);
        ::LineTo(hdc,rc.right - 10, _windowH - 3);
        ::LineTo(hdc,rc.right - 10, 3);

        ::SelectObject(hdc,theApp->_ShadowPen);

        ::LineTo(hdc, 10, 3);
        ::LineTo(hdc, 10, _windowH - 3);
        ::SelectObject(hdc, oldpen);
      }
    else                                // Paint a command bar
      {
        HPEN oldpen = (HPEN)::SelectObject(hdc, theApp->_LightPen);
        ::MoveTo(hdc, 0, 0);
        ::LineTo(hdc, rc.right,0);
        ::SelectObject(hdc, theApp->_ShadowPen);
        ::MoveTo(hdc, 0, _windowH - borderH);
        ::LineTo(hdc, rc.right, _windowH - borderH);
        ::SelectObject(hdc, oldpen);
      }

    ::ReleaseDC(useH, hdc);

    return 0;
  }

//====================>>> vCommandPane::OnCommand <<<==================
  void vCommandPane::OnCommand (HWND /* hwnd*/, int id, HWND /*hwndCtl*/,
   WORD codeNotify)
  {

    vCmd* UseThis = (vCmd *) getThisFromId((ItemVal)id);

    if (UseThis)
      {
	// A very dangerous thing can happen here if the button causes
	// vApp::Exit to be called. This can cause things to be destructed,
	// which means that the command object goes away, which means
	// that UseThis no longer points to something valid, which is of
	// course a problem. Exit can get called in CmdCallback, so we
	// will remember the command type first for the test after the call.

	CmdType ct = (UseThis->dlgCmd)->cmdType;    // will be ok still

	UseThis->CmdCallback( id, codeNotify);

	// Now, the following is ugly, but...
	// We need a SetFocus to get the keyborad back to the
	// canvas so that menu accelerators will work, but for
	// lists and combos, SetFocus breaks them, so we have
	// to check for the special case.
        if ((ct == C_ComboBox || ct == C_SComboBox) &&
		codeNotify != CBN_SELCHANGE)
	    return;
	if ((ct == C_List || ct == C_SList) &&
		codeNotify != LBN_SELCHANGE && codeNotify != LBN_DBLCLK)
	    return;
        if (ct == C_TextIn)
            return;
        ::SetFocus(::GetActiveWindow());
      }
  }

//====================>>> vCommandPane::DynamicDlgProc <<<==================
  int vCommandPane::DynamicDlgProc(HWND hDlg, UINT uMsg,
         WPARAM wParam, LPARAM lParam)
  {
  // This uses Microsoft Message crackers, presumably for enhanced
  // portability to Win32 and Win95
  int lResult = 0;

  switch (uMsg)
    {

    case WM_INITDIALOG:
      // Need to do this here so we can use message crackers
      _wDialog = hDlg;
      (void) HANDLE_WM_INITDIALOG(hDlg, wParam, lParam, OnInitDialog);
      lResult = 0;                      // 0 so won't highlight first control
      break;

    case WM_ENABLE:
      lResult = HANDLE_WM_ENABLE(hDlg, wParam, lParam, OnEnable);
      break;

    case WM_PAINT:
      lResult = HANDLE_WM_PAINT(hDlg, wParam, lParam, OnPaint);
      break;

    case WM_COMMAND:

      switch (GET_WM_COMMAND_ID(wParam, lParam)) // Have to ignore EN messages
        {
        case EN_SETFOCUS:
//        case EN_KILLFOCUS:
        case EN_ERRSPACE:
        case EN_HSCROLL:

        case EN_MAXTEXT:
        case EN_UPDATE:
        case EN_VSCROLL:
          return 0;

        }
      lResult = HANDLE_WM_COMMAND(hDlg, wParam, lParam, OnCommand);

      break;

    case WM_HSCROLL:
    case WM_VSCROLL:
      {

        int id = ::GetWindowID(GET_WM_HSCROLL_HWND(wParam, lParam));

        vCmd* UseThis = (vCmd *) getThisFromId(id);
        int code = GET_WM_HSCROLL_CODE(wParam,lParam);

        if (!UseThis || code == SB_ENDSCROLL)
          {
            return 0;
          }

        UseThis->CmdCallback(code,
                             (WORD)GET_WM_HSCROLL_POS(wParam, lParam));
        return 1;
      }

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORBTN:
          ::SetBkColor(GET_WM_CTLCOLOR_HDC(wParam, lParam, uMsg),
                       ::GetSysColor(COLOR_BTNFACE));
          return (int)(theApp->_BarBrush);

    case WM_CTLCOLORDLG:
        return (int )(theApp->_BarBrush);

    case WM_MEASUREITEM:
      {
        ItemVal mid = (ItemVal)GET_WM_COMMAND_ID(wParam, lParam);
        vCmd* UseThis = (vCmd *) getThisFromId(mid);
        if (!UseThis)
          {
            lResult = 0; break;
          }
        return UseThis->MEASUREITEM((int)mid, (MEASUREITEMSTRUCT*)lParam);
      }

    case WM_DRAWITEM:
      {
        ItemVal did = (ItemVal)GET_WM_COMMAND_ID(wParam, lParam);
        vCmd* UseThis = (vCmd *) getThisFromId(did);
        if (!UseThis)
          {
            lResult = 0; break;
          }
        return UseThis->DRAWITEM((int)did, (DRAWITEMSTRUCT*)lParam);
      }

   }
  return lResult;
}
