//===============================================================
// vfont.cxx - The font class - Windows
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
#include <v/vfont.h>
#include <v/vutil.h>

//=======================>>> vFont::vFont <<<=============================
  vFont::vFont(vFontID fam, int size, vFontID sty, vFontID wt, int und, int ang)
  {
    SysDebug(Constructor,"vFont::vFont() constructor\n")

    _family = fam; _pointSize = size; _style = sty; _weight = wt;
    _underlined = und; _isStockFont = 0; _hFont = 0;
    _fontColor = 0; _angle = ang;
    memset(&_lf, 0, sizeof(LOGFONT));

  }

//=====================>>> vFont::vFont <<<===========================
  vFont::vFont(const vFont& ft)
  {
    // Copy constructor - needs to delete object if already
    // created, and copy the values as needed

    if (_hFont != 0 && !_isStockFont)
      ::DeleteObject(_hFont);
    _isStockFont = 0; _hFont = 0;
    _family = ft._family; _style = ft._style; _weight = ft._weight;
    _pointSize = ft._pointSize; _underlined = ft._underlined;
    _fontColor = ft._fontColor; _angle = ft._angle;
    _lf = ft._lf;

  }

//=====================>>> vFont::= <<<===========================
  vFont& vFont::operator =(const vFont& ft)
  {
    if (this == &ft)     // assigning to self
      {
	return *this;
      }

    // just like the copy constructor

    if (_hFont != 0 && !_isStockFont)
      ::DeleteObject(_hFont);
    _isStockFont = 0; _hFont = 0;
    _family = ft._family; _style = ft._style; _weight = ft._weight;
    _pointSize = ft._pointSize; _underlined = ft._underlined;
    _angle = ft._angle;
    _fontColor = ft._fontColor;
    _lf = ft._lf;

    return *this;
  }

//=====================>>> vFont::SetFontValues <<<===========================
  void vFont::SetFontValues(vFontID fam, int size, vFontID sty, vFontID wt,
    int und)
  {
    if (_hFont != 0 && !_isStockFont)
	::DeleteObject(_hFont);
    _isStockFont = 0; _hFont = 0;
    _family = fam; _style = sty; _weight = wt;
    _pointSize = size; _underlined = und;
    _fontColor = 0;
  }

//=================>>> vFont::GetNativeFontString <<<========================
  void vFont::GetNativeFontString(char* fontStr)
  {
    // This will translate the native font info into a standard
    // string that can be saved to a text file. The SetNativeFontString
    // can be used to set the font to that string.
    // fontStr should be char fontStr[120];

       /*
	* typedef struct tagLOGFONT { // lf 
	*   LONG lfHeight; 			
	*   LONG lfWidth; 
	*   LONG lfEscapement; 
	*   LONG lfOrientation; 
	*   LONG lfWeight; 
	*   BYTE lfItalic; 
	*   BYTE lfUnderline; 
	*   BYTE lfStrikeOut; 
	*   BYTE lfCharSet; 
	*   BYTE lfOutPrecision; 
	*   BYTE lfClipPrecision; 
	*   BYTE lfQuality; 
	*   BYTE lfPitchAndFamily; 
	*   TCHAR lfFaceName[LF_FACESIZE];   LF_FACESIZE = 32
	* } LOGFONT;
	*
	* + COLORREF _fontColor & _pointSize * 10;
	*/
    char val[30];

    LongToStr(_lf.lfHeight,val); strcpy(fontStr,val); strcat(fontStr,";");
    LongToStr(_lf.lfWidth,val); strcat(fontStr,val); strcat(fontStr,";");
    LongToStr(_lf.lfEscapement,val); strcat(fontStr,val); strcat(fontStr,";");
    LongToStr(_lf.lfOrientation,val); strcat(fontStr,val); strcat(fontStr,";");
    LongToStr(_lf.lfWeight,val); strcat(fontStr,val); strcat(fontStr,";");
    IntToStr((int)_lf.lfItalic,val); strcat(fontStr,val); strcat(fontStr,";");
    IntToStr((int)_lf.lfUnderline,val); strcat(fontStr,val); strcat(fontStr,";");
    IntToStr((int)_lf.lfStrikeOut,val); strcat(fontStr,val); strcat(fontStr,";");
    IntToStr((int)_lf.lfCharSet,val); strcat(fontStr,val); strcat(fontStr,";");
    IntToStr((int)_lf.lfOutPrecision,val); strcat(fontStr,val); strcat(fontStr,";");
    IntToStr((int)_lf.lfClipPrecision,val); strcat(fontStr,val); strcat(fontStr,";");
    IntToStr((int)_lf.lfQuality,val); strcat(fontStr,val); strcat(fontStr,";");
    IntToStr((int)_lf.lfPitchAndFamily,val); strcat(fontStr,val); strcat(fontStr,";");
    LongToStr((long)_fontColor,val); strcat(fontStr,val); strcat(fontStr,";");
    IntToStr(_pointSize*10,val); strcat(fontStr,val); strcat(fontStr,";");
    strcat(fontStr,(char*)_lf.lfFaceName);

  }

//=================>>> vFont::SetNativeFontString <<<========================
  void vFont::SetNativeFontString(char* fontStr)
  {
    LOGFONT lf;
    int ptSize;
    long rgb;

    char* cp = fontStr;		// a copy to use
    char str[32];

    // translate to native format

    cp = getSubstring(cp, str);
    lf.lfHeight = StrToLong(str);
    cp = getSubstring(cp, str); lf.lfWidth = StrToLong(str);
    cp = getSubstring(cp, str); lf.lfEscapement = StrToLong(str);
    cp = getSubstring(cp, str); lf.lfOrientation = StrToLong(str);
    cp = getSubstring(cp, str); lf.lfWeight = StrToLong(str);

    cp = getSubstring(cp, str); lf.lfItalic = (BYTE) StrToLong(str);
    cp = getSubstring(cp, str); lf.lfUnderline = (BYTE) StrToLong(str);
    cp = getSubstring(cp, str); lf.lfStrikeOut = (BYTE) StrToLong(str);
    cp = getSubstring(cp, str); lf.lfCharSet = (BYTE) StrToLong(str);
    cp = getSubstring(cp, str); lf.lfOutPrecision = (BYTE) StrToLong(str);
    cp = getSubstring(cp, str); lf.lfClipPrecision = (BYTE) StrToLong(str);
    cp = getSubstring(cp, str); lf.lfQuality = (BYTE) StrToLong(str);
    cp = getSubstring(cp, str); lf.lfPitchAndFamily = (BYTE) StrToLong(str);

    cp = getSubstring(cp, str); rgb = StrToLong(str);
    cp = getSubstring(cp, str); ptSize = (int) StrToLong(str);

    strcpy(lf.lfFaceName, cp);

    SetWinFontValues(lf, ptSize, (DWORD)rgb);	// and set font
  }

//=================>>> vFont::getSubstring <<<========================
  char* vFont::getSubstring(char* str, char* substr)
  {
    while (*str != ';' && *str != 0)	// copy to ; or nul
	*substr++ = *str++;
    *substr = 0;			// terminate
    if (*str != 0)
	return ++str;			// skip the ;
    else
	return 0;
  }

//=====================>>> vFont::SetWinFontValues <<<========================
  void vFont::SetWinFontValues(VCONST LOGFONT& lf, 
	int iPointSize, DWORD rgbColors)
  {
    if (_hFont != 0 && !_isStockFont)
	::DeleteObject(_hFont);
    _lf = lf;

    _isStockFont = 0; _hFont = 0;

    // Map to whatever

    // First, set what will be true no matter what

    _pointSize = iPointSize / 10;
    _style = lf.lfItalic ? vfItalic : vfNormal;
    _underlined = lf.lfUnderline;
    _fontColor = rgbColors;
    _angle = lf.lfEscapement / 10;

    // Now set things that might make it NOT a V font.

    if (lf.lfWeight == FW_NORMAL)
	_weight = vfNormal;
    else if (lf.lfWeight == FW_BOLD)
	_weight = vfBold;
    else
      {
	_weight = vfNormal;
	goto WinNative;
      }
    if (lf.lfStrikeOut)
	goto WinNative;

    // Now, determine if it is a V family

    if (lf.lfPitchAndFamily == (DEFAULT_PITCH | FF_ROMAN) &&
	strcmp(lf.lfFaceName,"Times New Roman") == 0)
      {
	_family = vfSerif;
	return;
      }
    else if (lf.lfPitchAndFamily == (DEFAULT_PITCH | FF_SWISS) &&
	strcmp(lf.lfFaceName,"Arial") == 0)
      {
	_family = vfSansSerif;
	return;
      }
    else if (lf.lfPitchAndFamily == (DEFAULT_PITCH | FF_MODERN) &&
	strcmp(lf.lfFaceName,"Courier New") == 0)
      {
	_family = vfFixed;
	return;
      }
    else if (lf.lfPitchAndFamily == (DEFAULT_PITCH | FF_DECORATIVE) &&
	strcmp(lf.lfFaceName,"Wingdings") == 0 && lf.lfCharSet == SYMBOL_CHARSET)
      {
	_family = vfDecorative;
	return;
      }

  WinNative:			// a Native Windows font
      _family = vfOtherFont;
      return;

  }

//=======================>>> vFont::~vFont <<<=============================
  vFont::~vFont()
  {
    SysDebug(Destructor,"vFont::~vFont() destructor\n")
    if (_hFont != 0 && !_isStockFont)
      ::DeleteObject(_hFont);		// free the font resource
  }

//=======================>>> vFont::LoadFont <<<=============================
  void vFont::LoadFont(HDC dc, int isPrinterDC)
  {
    // Create the stuff needed for interal font stuff

    if (_hFont != 0)
	return;

    SysDebug(Misc,"Loading font\n");

    int ff_weight = 0;
    int ff_family = 0;
    char *ff_face = NULL;
    BYTE chrSet = ANSI_CHARSET;

    _isStockFont = 0;	// assume not a stock font


    switch (_family)
      {
	case vfDefaultVariable:	// default font
	    if (isPrinterDC)
	      {
		ff_family = FF_SWISS;
		ff_face = "Arial";
	      }
	    else
	      {
		_isStockFont = 1;
		_hFont = (HFONT) ::GetStockObject(SYSTEM_FONT);
	      }
	    break;

	// The rest of the fonts will be Windows TrueType fonts

	case vfSerif:	// Serif font
	    ff_family = FF_ROMAN;
	    ff_face = "Times New Roman" ;
	    break;

	case vfSansSerif:	// SansSerif Font
	      ff_family = FF_SWISS;
	      ff_face = "Arial";
	      break;

	case vfFixed:		// Fixed
	     ff_family = FF_MODERN;
	     ff_face = "Courier New";
	     break;

	case vfDecorative:	// Decorative
	     ff_family = FF_DECORATIVE;
	     ff_face = "Wingdings";
	     chrSet = SYMBOL_CHARSET;
	     break;

	case vfOtherFont:	// set by font picker dialog
	  {
	    if (isPrinterDC)	// Need to override this to be same as
		_lf.lfHeight = _pointSize*12/10; // other "V" fonts
            _hFont = ::CreateFontIndirect(&_lf);
            if (_hFont == NULL)		// just in case
	      {
		_hFont = (HFONT) ::GetStockObject(SYSTEM_FONT);
		_isStockFont = 1;
                _angle = 0;
	      }
	    return;
	  }

	case vfDefaultFixed:	// default font
	case vfDefaultSystem:
	default:
            if (isPrinterDC)
	      {
		ff_family = FF_MODERN;
	        ff_face = "Courier New" ;
	      }
	    else
	      {
		_isStockFont = 1;
		_hFont = (HFONT) ::GetStockObject(SYSTEM_FIXED_FONT);
                _angle = 0;
	      }
	    break;
      }

    // Now, set the rest of the font attributes

    BYTE ff_italic = (_style == vfItalic) ? 1 : 0;
					      ;
    if (_weight == vfNormal)
      ff_weight = FW_NORMAL;
    else if (_weight == vfBold)
      ff_weight = FW_BOLD;

    if (!_isStockFont)		// load non stock fonts
      {
	int nHeight;
	// This is a bit of a kludge, but these values give fonts that
	// are very close to the same physical print size as the PostScript
        // version. It doesn't look too bad!
	if (isPrinterDC)
	    nHeight = _pointSize*12/10;
	else
	    nHeight = _pointSize*GetDeviceCaps(dc, LOGPIXELSY)/72;

	_hFont = ::CreateFont(nHeight, 0, _angle * 10, 0,ff_weight,ff_italic,
		(BYTE)_underlined,
                0, chrSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | ff_family, ff_face);
      }
    if (_hFont == NULL)		// just in case
      {
	_hFont = (HFONT) ::GetStockObject(SYSTEM_FONT);
	_isStockFont = 1;
      }
  }


//########################################################################
