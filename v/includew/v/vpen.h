//===============================================================
// vpen.h: pen class header for drawing - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VPEN_H
#define VPEN_H

#include <v/v_defs.h>		// Basic defs
#include <v/vcolor.h>

// Use Windows for line styles, map to something reasonable for X

// Windows:
//enum PenStyle { Solid = PS_SOLID, Dash = PS_DASH, Dot = PS_DOT,
//	DashDot = PS_DASHDOT, Transparent = PS_NULL };

    class V_EXPORT vPen
      {
      public:		//---------------------------------------- public

	vPen(unsigned int r = 0, unsigned int g = 0, unsigned int b = 0,
	      int width = 1, int style = vSolid);
	vPen(const vPen &pen);	// copy constructor

	~vPen();

	vPen& operator =(const vPen& s);  // vPen = vPen
	int operator ==(const vPen& p2) VCONST
	  { return (penColor == p2.penColor && penWidth == p2.penWidth &&
		penStyle == p2.penStyle); }

	int operator !=(const vPen& p2) VCONST
	  { return (penColor != p2.penColor || penWidth != p2.penWidth ||
		penStyle != p2.penStyle); }

	void SetStyle(int style);
	int GetStyle() VCONST { return penStyle; }
	void SetWidth(int width);
	int GetWidth() VCONST { return penWidth; }
	void SetColor(const vColor& c);
	void SetColor(unsigned int r, unsigned int g, unsigned int b);
        vColor GetColor() VCONST { return penColor; }

	HPEN GetHPEN() VCONST ;

      protected:	//--------------------------------------- protected
      private:		//--------------------------------------- private

	void ReleaseHpen()
	 { if (_created) {_created = 0; ::DeleteObject(_hpen); _hpen = 0;}}

        vColor penColor;
	int penWidth;
	int penStyle;

	VMUTABLE HPEN _hpen;
	VMUTABLE int _created;
      };

#endif
