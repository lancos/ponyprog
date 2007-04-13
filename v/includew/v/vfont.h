//===============================================================
// vfont.h - The font class - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VFONT_H
#define VFONT_H

#include <windows.h>
#include <commdlg.h>	// For CHOOSEFONT

    class V_EXPORT vCanvasPane;	// the vCanvasPane needs to the the X font
    class V_EXPORT vApp;		// the vApp needs Xfont, too
    class V_EXPORT vListCmd;
    class V_EXPORT vComboBoxCmd;

    enum vFontID		// various font related ids
      {
        vfDefaultSystem,	// system default (fixed for Windows)
	vfDefaultFixed,		// the system default fixed font
	vfDefaultVariable,	// the system default variable font
	vfSerif,		// serifed font - TimesRoman
	vfSansSerif,		// SansSerif - Swiss or Helvetica
	vfFixed,		// fixed font - Courier
	vfDecorative,		// decorative - dingbat
	vfOtherFont,		// for all other fonts
	vfNormal,		// normal style, weight
	vfBold,			// boldface
	vfItalic,		// italic style
	vfEndOfList
      };


    class V_EXPORT vFont		// make the font stuff a class to make it portable
      {
	friend class V_EXPORT vCanvasPane;
	friend class V_EXPORT vApp;

      public:		//---------------------------------------- public
	vFont(vFontID fam = vfDefaultFixed, int size = 10,
	   vFontID sty = vfNormal, vFontID wt = vfNormal,
           int und = 0, int ang = 0);

	vFont(const vFont &ft);		// copy constructor

	~vFont();

	vFont& operator =(const vFont& ft);  // vFont = vFont
	int operator ==(const vFont& ft) VCONST
	  { return (_family == ft._family && _style == ft._style &&
	    _weight == ft._weight && _pointSize == ft._pointSize  &&
	    _underlined == ft._underlined && _angle == ft._angle); }

	int operator !=(const vFont& ft) VCONST
	  { return !(_family == ft._family && _style == ft._style &&
	    _weight == ft._weight && _pointSize == ft._pointSize  &&
	    _underlined == ft._underlined && _angle == ft._angle); }


	vFontID GetFamily() VCONST { return _family; }

	int GetPointSize() VCONST { return _pointSize; }

	vFontID GetStyle() VCONST { return _style; }

	vFontID GetWeight() VCONST { return _weight; }

	int GetUnderlined() VCONST { return _underlined; }

        int GetAngle() VCONST { return _angle; }


	void SetFontValues(vFontID fam = vfDefaultFixed, int size = 10,
	   vFontID sty = vfNormal, vFontID wt = vfNormal, int und = 0);

	void GetNativeFontString(char* fontStr);
	void SetNativeFontString(char* fontStr);
        void SetWinFontValues(VCONST LOGFONT& lf,  int pt, DWORD rgb);

	void LoadFont(HDC dc, int isPrinterDC);	// make sure font with given id is loaded

	HFONT GetHFONT() VCONST { return _hFont; }
	LOGFONT GetLOGFONT() VCONST { return _lf; }
	COLORREF GetFontColor() VCONST { return _fontColor; }

      protected:	//--------------------------------------- protected
	char* getSubstring(char* str, char* substr);

      private:		//--------------------------------------- private
	vFontID _family;	// family - vfSerif, etc.
	vFontID _style; 	// style - nomal, italic, slant
	vFontID _weight;	// weight - normal, light, bold
	int _pointSize;		// point size of font
	int _underlined;	// if underlined or not
        int _angle;		// angle to print at

	int _isStockFont;	// if a stock font

	COLORREF _fontColor;

	HFONT _hFont;		// windows handle to font
	LOGFONT _lf;		// Window full font inf
  };

#endif
