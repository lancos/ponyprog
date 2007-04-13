//===============================================================
// vclabelc.xxx	- Color label Cmd
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vxutil.h>	// Motif/Athena mapping
#include <v/vclabelc.h>	// our definitions

extern "C"
{
#include <X11/StringDefs.h>
#ifndef Athena
#include <Xm/Xm.h>
#endif
}

#ifndef Athena
#define Nforeground XmNforeground
#else
#define Nforeground XtNforeground
#endif

//=================>>> vColorLabelCmd::vColorLabelCmd <<<=======================
  vColorLabelCmd::vColorLabelCmd(vCmdParent* dp, CommandObject* dc) :
	vLabelCmd(dp, dc)
  {
    initColor();
  }

//====================>>> vColorLabelCmd::~vColorLabelCmd <<<=======================
  vColorLabelCmd::~vColorLabelCmd()
  {

    SysDebug(Destructor,"vColorLabelCmd::~vColorLabelCmd() destructor\n")
  }

//=====================>>> vColorLabelCmd::initColor <<<=======================
  void vColorLabelCmd::initColor(void)
  {
    // wCmd is the widget of this button

    SysDebug(Constructor,"vColorLabelCmd::vColorLabelCmd() constructor\n")

    if (_itemList != 0)				// an RGB provided
      {
	_origColor = (vColor*)_itemList;	// point to the rgb
	_color = *_origColor;			// copy the color
      }
    XtVaSetValues(wCmd, Nforeground, _color.pixel(), NULL);	// change the background pixel
  }

//================>>> vColorLabelCmd::SetCmdVal <<<============================
  void vColorLabelCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug1(Misc,"vColorLabelCmd::SetCmdVal(val:%d)\n",val)

    switch (st)
      {
	case ChangeColor:
	    _color = *_origColor;	// recopy color
	    XtVaSetValues(wCmd, Nforeground, _color.pixel(), NULL);	// change the background pixel
	    break;
	    
	case Red:
	    _color.SetR(val);
	    break;

	case Green:
	    _color.SetG(val);
	    break;

	case Blue:
	    _color.SetB(val);
	    XtVaSetValues(wCmd, Nforeground, _color.pixel(), NULL);
	    break;

	default:
	    vLabelCmd::SetCmdVal(val, st);
	    break;
      }
  }
