//===============================================================
// vclabelc.xxx	- Color label Cmd - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vclabelc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent

//=================>>> vColorLabelCmd::vColorLabelCmd <<<=======================
  vColorLabelCmd::vColorLabelCmd(vCmdParent* dp, CommandObject* dc) :
	vLabelCmd(dp, dc)
  {
    initColor();
  }

//====================>>> vColorLabelCmd::~vColorLabelCmd <<<===================
  vColorLabelCmd::~vColorLabelCmd()
  {

    SysDebug(Destructor,"vColorLabelCmd::~vColorLabelCmd() destructor\n")
  }

//=====================>>> vColorLabelCmd::initColor <<<=======================
  void vColorLabelCmd::initColor(void)
  {
    // wCmd is the widget of this button

    SysDebug(Constructor,"vColorLabelCmd::vColorLabelCmd() constructor\n")

    _origColor = 0;
    
    if (_itemList != 0)				// an RGB provided
      {
	_origColor = (vColor*)_itemList;	// point to the rgb
	_color = *_origColor;
      }
    
  }

//================>>> vColorLabelCmd::SetCmdVal <<<============================
  void vColorLabelCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vColorLabelCmd::SetCmdVal(val:%d)\n",val)

    switch (st)
      {
	case ChangeColor:		// changed original vColor
	  {
	    _color = *_origColor;	// recopy color
	    HWND dH = _parentWin->getParent();
	    char buff[80];
	    ::GetDlgItemText(dH, _cmdId, buff, 80);  // make it redraw
	    ::SetDlgItemText(dH, _cmdId, buff);      // by "changing" text
	    break;
	  }

	case Red:
	    _color.SetR(val);
	    break;

	case Green:
	    _color.SetG(val);
	    break;

	case Blue:
	  {
	    _color.SetB(val);
	    // Now that the colors are set, we have to make the button
	    // redraw. There must be some better way, but this kludge
	    // is all I could figure out to force a redraw of the new color
	    HWND dH = _parentWin->getParent();
	    char buff[80];
	    ::GetDlgItemText(dH, _cmdId, buff, 80);  // make it redraw
	    ::SetDlgItemText(dH, _cmdId, buff);      // by "changing" text
	    break;
	  }

	default:
	    vLabelCmd::SetCmdVal(val, st);
	    break;
      }
  }
