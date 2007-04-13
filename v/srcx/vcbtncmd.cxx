//===============================================================
// vcbtncmd.xxx	- Color button Cmd
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vxutil.h>	// Motif/Athena mapping
#include <v/vcbtncmd.h>	// our definitions

extern "C"
{
#include <X11/StringDefs.h>
#ifndef Athena
#include <Xm/Xm.h>
#endif
}

#ifndef Athena
#define Nbackground XmNbackground
#else
#define Nbackground XtNbackground
#endif

//=================>>> vColorButtonCmd::vColorButtonCmd <<<=======================
  vColorButtonCmd::vColorButtonCmd(vCmdParent* dp, CommandObject* dc) :
	vButtonCmd(dp, dc)
  {
    initColor();
  }

//====================>>> vColorButtonCmd::~vColorButtonCmd <<<=======================
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

    XtVaSetValues(wCmd, Nbackground, _color.pixel(), NULL);	// change the background pixel
  }

//================>>> vColorButtonCmd::SetCmdVal <<<============================
  void vColorButtonCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vColorButtonCmd::SetCmdVal(val:%d)\n",val)

    switch (st)
      {
	case ChangeColor:		// changed original vColor
	    _color = *_origColor;	// recopy color
	    XtVaSetValues(wCmd, Nbackground, _color.pixel(), NULL);	// change the background pixel
	    break;

	case Red:
	    _color.SetR(val);
	    break;

	case Green:
	    _color.SetG(val);
	    break;

	case Blue:
	    _color.SetB(val);
	    XtVaSetValues(wCmd, Nbackground, _color.pixel(), NULL);	// change the background pixel
	    break;

	default:
	    vButtonCmd::SetCmdVal(val, st);
	    break;
      }
  }
