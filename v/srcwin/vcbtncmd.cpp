//===============================================================
// vcbtncmd.xxx	- Color button Cmd - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vcbtncmd.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent

//=================>>> vColorButtonCmd::vColorButtonCmd <<<=====================
  vColorButtonCmd::vColorButtonCmd(vCmdParent* dp, CommandObject* dc) :
	vButtonCmd(dp, dc)
  {
    initColor();
  }

//====================>>> vColorButtonCmd::~vColorButtonCmd <<<=================
  vColorButtonCmd::~vColorButtonCmd()
  {

    SysDebug(Destructor,"vColorButtonCmd::~vColorButtonCmd() destructor\n")
  }

//=====================>>> vColorButtonCmd::initColor <<<=======================
  void vColorButtonCmd::initColor(void)
  {
    // wCmd is the widget of this button

    SysDebug(Constructor,"vColorButtonCmd::vColorButtonCmd() constructor\n")

    _origColor = 0;
    if (_itemList != 0)				// an RGB provided
      {
	_origColor = (vColor*)_itemList;	// point to the rgb
	_color = *_origColor;
      }
  }

//================>>> vColorButtonCmd::SetCmdVal <<<============================
  void vColorButtonCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vColorButtonCmd::SetCmdVal(val:%d)\n",val)

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
	    vButtonCmd::SetCmdVal(val, st);
	    break;
      }
  }

//================>>> vColorButtonCmd::DRAWITEM <<<============================
  int vColorButtonCmd::DRAWITEM(int id, DRAWITEMSTRUCT* dis)
  {
    switch (dis->itemAction)
      {
	case ODA_DRAWENTIRE:	// redraw entire control
	  {
	DRAW_NORMAL:

	    // Draw a color button
	    HBRUSH brush;
	    HPEN oldpen = static_cast<HPEN>(::SelectObject(dis->hDC, theApp->_ShadowPen));

	    // We will draw  disabled color buttons in gray, otherwise
	    // in the color specified

	    if (dis->itemState & ODS_DISABLED)
		brush = static_cast<HBRUSH> (::GetStockObject(LTGRAY_BRUSH));
	    else
		brush = ::CreateSolidBrush(::GetNearestColor(dis->hDC,_color.pixel()));

	    // Set to the new brush color, remember the old one

	    HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(dis->hDC, brush));

	    // Draw the button with a black border

	    (void)::Rectangle(dis->hDC,dis->rcItem.left, dis->rcItem.top,
		dis->rcItem.right, dis->rcItem.bottom);

	    // Draw text for the color button if it has it

	    if (*_title )
	      {
		int tl = strlen(_title);  		// This little kludge
		int tblanks = 0;                        // prevents drawing
		for (VCONST char* tp = _title ; *tp ; ++tp)    // all-blank titles
		  {                                     // which can leave
		    if (*tp == ' ')                     // an ugly "pure color"
			++tblanks;                      // block in the middle
		  }                                     // of the button.
		if (tblanks != tl)
		  {
		    ::SetBkColor(dis->hDC, ::GetNearestColor(dis->hDC,_color.pixel()));
		    ::DrawText(dis->hDC, _title, strlen(_title),
		      &dis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		  }
	      }

	      // Now draw the shadow around the button
	    ::SelectObject(dis->hDC, theApp->_LightPen);
	    ::MoveTo(dis->hDC, dis->rcItem.left, dis->rcItem.bottom);
	    ::LineTo(dis->hDC, dis->rcItem.left, dis->rcItem.top);
	    ::LineTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.top);
	    ::MoveTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.bottom+1);
	    ::LineTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.top+1);
	    ::LineTo(dis->hDC, dis->rcItem.right-2, dis->rcItem.top+1);


	    ::SelectObject(dis->hDC,theApp->_ShadowPen);
	    ::MoveTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.top+1);
	    ::LineTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.bottom-1);
	    ::LineTo(dis->hDC, dis->rcItem.left, dis->rcItem.bottom-1);
	    ::MoveTo(dis->hDC, dis->rcItem.right-2, dis->rcItem.top+1);
	    ::LineTo(dis->hDC, dis->rcItem.right-2, dis->rcItem.bottom-2);
	    ::LineTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.bottom-2);

	    ::SelectObject(dis->hDC, oldpen);	// restore hdc
	    ::SelectObject(dis->hDC, oldbrush);
	    if (!(dis->itemState & ODS_DISABLED)) // delete the brush if needed
		::DeleteObject(brush);
	    return 1;
	  }

	case ODA_SELECT:	// indicated selected
	  {
	    // nearly the same as regular, but don't worry about insensitive
	    // and draw inverted shadow

	    if (dis->itemState & ODS_SELECTED)  //Draw the icon
	      {
		HPEN oldpen = (HPEN)::SelectObject(dis->hDC, ::GetStockObject(BLACK_PEN));

		HBRUSH brush = static_cast<HBRUSH>(::CreateSolidBrush(::GetNearestColor(dis->hDC,_color.pixel())));

		HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(dis->hDC, brush));

		(void)::Rectangle(dis->hDC,dis->rcItem.left, dis->rcItem.top,
			dis->rcItem.right, dis->rcItem.bottom);

		if (*_title)
		  {
		    RECT rc = dis->rcItem;
		    rc.left += 2;    rc.top += 2;	// shift text
		    ::SetBkColor(dis->hDC, ::GetNearestColor(dis->hDC,_color.pixel()));
		    ::DrawText(dis->hDC, _title, strlen(_title),
		      &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		  }

		::SelectObject(dis->hDC, theApp->_ShadowPen);
		::MoveTo(dis->hDC, dis->rcItem.left, dis->rcItem.bottom);
		::LineTo(dis->hDC, dis->rcItem.left, dis->rcItem.top);
		::LineTo(dis->hDC, dis->rcItem.right, dis->rcItem.top);
		::MoveTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.bottom+1);
		::LineTo(dis->hDC, dis->rcItem.left+1, dis->rcItem.top+1);
	    	::LineTo(dis->hDC, dis->rcItem.right-1, dis->rcItem.top+1);
		::SelectObject(dis->hDC, oldpen);	// restore hdc
		::SelectObject(dis->hDC, oldbrush);
		::DeleteObject(brush);			// free brush
		return 1;
	      }
	    else
		goto DRAW_NORMAL;		// other cases just like normal
	  }

	case ODA_FOCUS:		// gets focus
	  {
	    return 1;		// no op for icon label
	  }
	default:
	    break;
      }
    return 0;
  }
