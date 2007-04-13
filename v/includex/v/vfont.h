//===============================================================
// vfont.h - The font class - X11R5
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

#include <X11/Xlib.h>

    class vCanvasPane;	// the vCanvasPane needs to the the X font
    class vApp;		// the vApp needs Xfont, too
    class vListCmd;
    class vComboBoxCmd;

    enum vFontID		// various font related ids
      {
	vfDefaultSystem,	// the default system font
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
	vfSerif2,		// alternative fonts...
	vfSansSerif2,
	vfFixed2,
	vfDecorative2,
	vfEndOfList
      };


    class vFont		// make the font stuff a class to make it portable
      {
	friend class vCanvasPane;
	friend class vApp;
	friend class vFontList;

      public:		//---------------------------------------- public
	vFont(vFontID fam = vfDefaultFixed, int size = 10,
	   vFontID sty = vfNormal, vFontID wt = vfNormal,
	    int und = 0, int ang = 0);

	vFont(const vFont &ft);		// copy constructor

	~vFont();

	vFont& operator =(const vFont& ft) VCONST;  // vFont = vFont
	int operator ==(const vFont& ft) VCONST
	  { return (_family == ft._family && _style == ft._style &&
	    _weight == ft._weight && _pointSize == ft._pointSize  &&
	    _underlined == ft._underlined && _angle == ft._angle); }

	int operator !=(const vFont& ft) VCONST
	  { return !(_family == ft._family && _style == ft._style &&
	    _weight == ft._weight && _pointSize == ft._pointSize  &&
	    _underlined == ft._underlined && _angle == ft._angle); }

	void SetNativeFontString(char* fontStr);
	void GetNativeFontString(char* fontStr);


	vFontID GetFamily() VCONST { return _family; }

	int GetPointSize() VCONST { return _pointSize; }

	vFontID GetStyle() VCONST { return _style; }

	vFontID GetWeight() VCONST { return _weight; }

	int GetUnderlined() VCONST { return _underlined; }

	int GetAngle() VCONST { return _angle; }

	void SetFontValues(vFontID fam = vfDefaultFixed, int size = 10,
	   vFontID sty = vfNormal, vFontID wt = vfNormal, int und = 0, int ang = 0);

	// Following should be used only internally

	void LoadFont() VCONST;	// make sure font with given id is loaded

	int XTextW(VCONST char* str) VCONST;
	int XFontH(int& asc, int& des) VCONST;

	XFontStruct* GetXFont() { return _XFont;}

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private
	VMUTABLE vFontID _family;	// family - vfSerif, etc.
	VMUTABLE vFontID _style; 	// style - nomal, italic
	VMUTABLE vFontID _weight;	// weight - normal, light, bold
	VMUTABLE int _pointSize;		// point size of font
	VMUTABLE int _underlined;	// if underlined or not
	VMUTABLE int _angle;		// angle to print at

	VMUTABLE XFontStruct* _XFont;		// windows handle to font

      };

#endif
