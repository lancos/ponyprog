//===============================================================
// vselfont.cpp - vSelectFont class functions - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vfontsel.h>           // our header
#include <v/vapp.h>
#include <v/vbasewin.h>

//===================>>> vFontSelect::vFontSelect <<<=======================
  vFontSelect::vFontSelect(vBaseWindow* bw, VCONST char* /*title*/) // constructor
  {
     _parentHWND = bw->winHwnd();	// track parent's HWND
     init();
  }

//===================>>> vFontSelect::vFontSelect <<<=======================
  vFontSelect::vFontSelect(vApp* aw, VCONST char* /*title*/)
  {
     _parentHWND = aw->winHwnd();	// track parent's HWND
     init();
  }

//===================>>> vFontSelect::init <<<=======================
  void vFontSelect::init()
  {
    memset(&_lf,0,sizeof(LOGFONT));	// zap entire thing
    memset(&_cf,0,sizeof(CHOOSEFONT));

    _cf.lStructSize = sizeof(CHOOSEFONT);
    _cf.hwndOwner = _parentHWND;
  }

//======================>>> vFontSelect::SelectFont <<<=======================
  int vFontSelect::FontSelect(vFont& font,const char* /*msg*/, int fixedOnly)
  {
    // Select a font


    // Set to values of the supplied font
    vFontID vfam = font.GetFamily();
    if (vfam == vfOtherFont)
      {
	_cf.rgbColors = font.GetFontColor();
	_lf = font.GetLOGFONT();
      }
    else
      {
	switch (vfam)
	  {
	    case vfSerif:	// Serif font
		_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_ROMAN;
		strcpy(_lf.lfFaceName,"Times New Roman");
		break;

	    case vfSansSerif:	// SansSerif Font
		_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
		strcpy(_lf.lfFaceName,"Arial");
		break;

	    case vfFixed:		// Fixed
		_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_MODERN;
		strcpy(_lf.lfFaceName,"Courier New");
		break;

	    case vfDecorative:	// Decorative
		_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DECORATIVE;
		strcpy(_lf.lfFaceName,"Wingdings");
		break;

	    default:
                 _lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		strcpy(_lf.lfFaceName,"Fixedsys");
		break;
	  }
	 _lf.lfWeight = (font.GetWeight() == vfBold) ? FW_BOLD : FW_REGULAR;
	 _lf.lfHeight = -17;	// Reasonable default
	 _lf.lfItalic = (font.GetStyle() == vfItalic);
	 _lf.lfUnderline = font.GetUnderlined();
      }

    _cf.lpLogFont = &_lf;
    _cf.Flags = CF_SCREENFONTS | CF_EFFECTS |
		CF_INITTOLOGFONTSTRUCT;
    if (fixedOnly)
	_cf.Flags |= CF_FIXEDPITCHONLY;

    _cf.nFontType = SCREEN_FONTTYPE;

    if (!::ChooseFont(&_cf))
	return 0;

    font.SetWinFontValues(_lf, _cf.iPointSize, _cf.rgbColors);

    return 1;
  }

// ---------------------------------------------------------------------
