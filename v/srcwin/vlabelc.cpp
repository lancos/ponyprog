//===============================================================
// vlabelc.cxx	- label Cmd - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vlabelc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vutil.h>	// utility
#include <v/vicon.h>	// icons

//=====================>>> vLabelCmd::vLabelCmd <<<=======================
  vLabelCmd::vLabelCmd(vCmdParent* dp, CommandObject* dc):
	    vCmd(dp, dc)
  {
    initialize();
  }

//=====================>>> vLabelCmd::~vLabelCmd <<<=======================
  vLabelCmd::~vLabelCmd()
  {

    SysDebug(Destructor,"vLabelCmd::~vLabelCmd() destructor\n")

  }

//=====================>>> vLabelCmd::initialize <<<=======================
  void vLabelCmd::initialize(void)
  {
    // build a button command for use in a parent window

    CopyToLocal();			// Make local copies of CmdObject

    SysDebug(Constructor,"vLabelCmd::vLabelCmd() constructor\n")

    if (dlgCmd->cmdType == C_Icon && _itemList != 0)	// icon
      {
	initIcon();
	return;
     }

    long style = SS_LEFTNOWORDWRAP;	// default for a label


    if (!(dlgCmd->attrs & CA_Hidden))	// Check for Hidden
	style |= WS_VISIBLE;

    if (dlgCmd->size > 0)		// may reset title!
      {
	_w = dlgCmd->size;
      }
    else if (dlgCmd->attrs & CA_MainMsg && vLblLen(_title) < 20)
      {
	 _w = 88;
      }
    else
	_w = LabelWidth(_title) + 8;		// set my width

    if (_parentWin->paneType() == P_Status
	|| dlgCmd->cmdType == C_Blank)		// icon
      {
	_h = 9;
	_parentWin->SetPosition(_x, _y, _w, _h + 2, dlgCmd->cFrame,
	     dlgCmd->cRightOf, dlgCmd->cBelow);
	_y += 2;	// make look more centered
      }
    else
      {
        _h = 11;				// default height
	_parentWin->SetPosition(_x, _y, _w, _h + 3, dlgCmd->cFrame,
	     dlgCmd->cRightOf, dlgCmd->cBelow);
	_y += 3;	// make look more centered

      }

    _CtrlOffset = _parentWin->AddDlgControl(_x, _y , _w, _h, _cmdId,
	   style, "STATIC", _title, sizeof(vLabelCmd*), (LPBYTE)this);
  }


//=====================>>> vLabelCmd::initIcon <<<=======================
  void vLabelCmd::initIcon(void)
  {
    _ip = (vIcon *) _itemList;	// to access bitmap
    long style = BS_OWNERDRAW;

     if (!(dlgCmd->attrs & CA_Hidden))	// Check for Hidden
	style |= WS_VISIBLE;

    _h = _ip->height / 2 + 6;
#ifdef _WIN32
    _w = _ip->width / 2 + 14;
#else
    _w = _ip->width / 2 + 6;
#endif
   
    _parentWin->SetPosition(_x, _y, _w, _h, dlgCmd->cFrame,
	     dlgCmd->cRightOf, dlgCmd->cBelow);

    _CtrlOffset = _parentWin->AddDlgControl(_x, _y , _w, _h, _cmdId,
	   style, "BUTTON", "", sizeof(vLabelCmd*), (LPBYTE)this);

  }

//================>>> vLabelCmd::SetCmdVal <<<============================
  void vLabelCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {
    SysDebug1(Misc,"vLabelCmd::SetCmdVal(val:%d)\n",val)

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

//================>>> vLabelCmd::SetCmdStr <<<============================
  void vLabelCmd::SetCmdStr(VCONST char* str)
  {

    SysDebug1(Misc,"vLabelCmd::SetCmdStr(str:%s)\n",str)

    ::SetDlgItemText(_parentWin->getParent(),_cmdId, str);
    _title = str;
  }

//================>>> vLabelCmd::MEASUREITEM <<<============================
  int vLabelCmd::MEASUREITEM(int id, MEASUREITEMSTRUCT* mis)
  {
    mis->CtlType = ODT_BUTTON;
    mis->CtlID = id;
    mis->itemWidth = _w;
    mis->itemHeight = _h;
    
    return 1;
  }

//================>>> vLabelCmd::DRAWITEM <<<============================
  int vLabelCmd::DRAWITEM(int /* id */, DRAWITEMSTRUCT* dis)
  {
    switch (dis->itemAction)
      {
	case ODA_DRAWENTIRE:	// redraw entire control
	  {

	    POINT ptSize, ptOrg;

		// Draw the button rectangle in LTGRAY with a BLACK border

            HPEN oldpen = (HPEN) ::SelectObject(dis->hDC, ::GetStockObject(BLACK_PEN));
            HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(dis->hDC, ::GetStockObject(BLACK_BRUSH)));


		// Now setup coordinate to draw icon

            HDC hdcMem = ::CreateCompatibleDC(dis->hDC);
            ::SetMapMode(hdcMem, GetMapMode(dis->hDC));
            HBITMAP hbm = _ip->GetIconHBM(dis->hDC);	// get icon HBITMAP
            HBITMAP oldbm = static_cast<HBITMAP>(::SelectObject(hdcMem,hbm));
            ptSize.x = _ip->width;
            ptSize.y = _ip->height;
            ::DPtoLP(dis->hDC, &ptSize, 1);
            ptOrg.x = 0;
            ptOrg.y = 0;
            ::DPtoLP(hdcMem, &ptOrg, 1);

            int l = 0;		// looks strange, but left over from icon button
            int t = 0;

                // Now BitBlt the icon BMP into the drawing canvas, leaving
		// background untouched, and the pen color to draw 1 bits
		// (that's what the E20746L does) for monochrome

            if (_ip->depth == 1)
              {
                ::SetBkColor(dis->hDC, RGB(255,255,255)); // white bg for icon
                ::BitBlt(dis->hDC, l, t, ptSize.x,
                  ptSize.y, hdcMem, ptOrg.x, ptOrg.y, 0xE20746L);
              }
            else
              {
                ::BitBlt(dis->hDC, l, t, ptSize.x,
		    ptSize.y, hdcMem, ptOrg.x, ptOrg.y, SRCCOPY);
		if (_ip->iType == Transparent)
		  {
		    HBRUSH dbrush, oldbrush;

		    // Prepare for BitBlt with raster operation of PSDPxax, i.e.
		    // paint with the brush where the color of the pixel on the
		    // destination bitmap is zero
		    dbrush = ::CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
		    oldbrush = static_cast<HBRUSH>(::SelectObject(dis->hDC, dbrush));

		    // Now restore area from the button where the mask is zero
		    HDC hdcMemMask = ::CreateCompatibleDC( dis->hDC );
		    HBITMAP hmsk = _ip->GetIconMask( dis->hDC); // get icon mask HBITMAP
		    HBITMAP oldBM = static_cast<HBITMAP>(::SelectObject( hdcMemMask, hmsk));

		    ::BitBlt( dis->hDC, l, t, ptSize.x,
                  	ptSize.y, hdcMemMask, ptOrg.x, 
			ptOrg.y, 0xB8074A); //PSDPxax

		    // Clean up
		    ::SelectObject( hdcMemMask, oldBM );
		    ::DeleteDC( hdcMemMask );
		    ::SelectObject( dis->hDC, oldbrush);
		    ::DeleteObject( dbrush );
		  }
              }
            ::SelectObject(hdcMem,oldbm);
            ::DeleteDC(hdcMem);
            ::SelectObject(dis->hDC, oldpen);	// restore hdc
            ::SelectObject(dis->hDC, oldbrush);

            return 1;
	  }

	case ODA_SELECT:	// indicated selected
	  {
	    return 1;		// no op for icon label
	  }

	case ODA_FOCUS:		// gets focus
          {
	    return 1;		// no op for icon label
	  }
      }
    return 0;
  }

//===================>>> vLabelCmd::vCmdCallback <<<=======================
  void vLabelCmd::CmdCallback(int /*id*/, WORD codeNotify)
  {
   // No Op for a label - the icon button may generate one of these!
  }
