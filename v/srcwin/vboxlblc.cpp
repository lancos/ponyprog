//===============================================================
// vstatlbl.cxx	- boxed label for stat bar - Windows
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
#include <v/vboxlblc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent

//=====================>>> vBoxedLabelCmd::vBoxedLabelCmd <<<================
  vBoxedLabelCmd::vBoxedLabelCmd(vCmdParent* dp, CommandObject* dc) :
    vCmd(dp, dc)
  {
    initialize();
  }

//=================>>> vBoxedLabelCmd::~vBoxedLabelCmd <<<===================
  vBoxedLabelCmd::~vBoxedLabelCmd()
  {

    SysDebug(Destructor,"vBoxedLabelCmd::~vBoxedLabelCmd() destructor\n")

  }

//=====================>>> vBoxedLabelCmd::initialize <<<=====================
  void vBoxedLabelCmd::initialize(void)
  {
    // build a button command for use in a parent window


    SysDebug(Constructor,"vBoxedLabelCmd::vBoxedLabelCmd() constructor\n")

    
    long style = BS_OWNERDRAW | WS_GROUP;	// default for a button

    CopyToLocal();			// Make local copies of CmdObject


    if (!(dlgCmd->attrs & CA_Hidden))		// Check for Hidden
	style |= WS_VISIBLE;

    int tl = strlen(_title);

    if ( tl < 80)
	strcpy(_lbl,_title);
    else
	strcpy(_lbl,"????????");

    if (dlgCmd->size > 0)		// may reset title!
      {
	_w = dlgCmd->size;
      }
     else
	_w = LabelWidth(_lbl) + 4;	// set my width

    _h = 11;			      	// default height

    _parentWin->SetPosition(_x, _y, _w, _h, dlgCmd->cFrame, dlgCmd->cRightOf,
	dlgCmd->cBelow);


    _CtrlOffset = _parentWin->AddDlgControl(_x, _y, _w, _h, _cmdId,
	  style, "BUTTON", _lbl, sizeof(vBoxedLabelCmd), (LPBYTE)this);
  }

//================>>> vBoxedLabelCmd::SetCmdVal <<<===========================
  void vBoxedLabelCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vBoxedLabelCmd::SetCmdVal(val:%d)\n",val)

    HWND myHwnd = GetMyHwnd(_cmdId);
    if (st == Sensitive)		// used so icons won't get
      {                                 // focus -- icons use ownerdraw,
	_Sensitive = val;	// which behave like buttons, not
	::EnableWindow(myHwnd, val);    // static labels -- this fixes that
      }
    else if (st == Hidden)		// hide or unhide
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

//================>>> vBoxedLabelCmd::SetCmdStr <<<===========================
  void vBoxedLabelCmd::SetCmdStr(VCONST char* str)
  {

    SysDebug1(Misc,"vBoxedLabelCmd::SetCmdStr(str:%s)\n",str)

    HWND myHwnd = GetMyHwnd(_cmdId);
    strcpy(_lbl,str);
    _title = _lbl;
    ::RedrawWindow(myHwnd,NULL,NULL,RDW_INVALIDATE);

  }


//================>>> vBoxedLabelCmd::MEASUREITEM <<<=========================
  int vBoxedLabelCmd::MEASUREITEM(int id, MEASUREITEMSTRUCT* mis)
  {
    mis->CtlType = ODT_BUTTON;
    mis->CtlID = id;
    mis->itemWidth = _w;
    mis->itemHeight = _h;
    
    return 1;
  }

//================>>> vBoxedLabelCmd::DRAWITEM <<<============================
  int vBoxedLabelCmd::DRAWITEM(int /* id */, DRAWITEMSTRUCT* dis)
  {
    switch (dis->itemAction)
      {
	case ODA_DRAWENTIRE:	// redraw entire control
	  {
	    // This gets called after the CTLCOLOR message has been
	    // processed in the dialog/commandpane procs.
	    // We are using LTGRAY_BRUSH for dialogs and command bars.
	    // We need to draw the icon properly on the background.
	    // We will use a null pen, and the same background. We have
	    // to use left-1, top-1, right+1, and bottom+1 to have the
	    // interior of the rectangle in the icon's area. We end up
	    // with a white line otherwise. First, fill gray background

	    
	    HPEN oldpen = (HPEN) ::SelectObject(dis->hDC, GetStockObject(NULL_PEN));

	    HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(dis->hDC,
					     theApp->_BarBrush));

	    (void)::Rectangle(dis->hDC,dis->rcItem.left, dis->rcItem.top,
		dis->rcItem.right+1, dis->rcItem.bottom+1);

	    if (!(dlgCmd->attrs & CA_NoBorder))
	      {
	    ::SelectObject(dis->hDC, theApp->_ShadowPen);
	    ::MoveTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.bottom-1);
	    ::LineTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.top);
	    ::LineTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.top);
	   
	    ::SelectObject(dis->hDC,theApp->_LightPen);
	    ::MoveTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.top);
	    ::LineTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.bottom-1);
	    ::LineTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.bottom-1);
              }

	    int oldBkMode = ::GetBkMode(dis->hDC);
	    ::SetBkMode(dis->hDC,TRANSPARENT);
	    ::DrawText(dis->hDC, _lbl, strlen(_lbl),
		      &dis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	    ::SetBkMode(dis->hDC,oldBkMode);


	    ::SelectObject(dis->hDC, oldpen);	// restore hdc
	    ::SelectObject(dis->hDC, oldbrush);

	    return 1;
	  }

	case ODA_SELECT:	// indicated selected
	  {
	    return 1;		// no op for icon Frame
	  }

	case ODA_FOCUS:		// gets focus
	  {
	    return 1;		// no op for icon Frame
	  }
      }
    return 0;
  }

//===================>>> vBoxedLabelCmd::vCmdCallback <<<=======================
  void vBoxedLabelCmd::CmdCallback(int /* id */, WORD codeNotify)
  {
   // No Op for a label
  }
  
