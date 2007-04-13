//===============================================================
// vbrush.h: brush class header for filling - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VBRUSH_H
#define VBRUSH_H

#include <v/v_defs.h>		// Basic defs
#include <v/vcolor.h>

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

// Use Windows for brush styles, map to something reasonable for X

    class vBrush
      {
      public:		//---------------------------------------- public
	vBrush(unsigned int r = 0, unsigned int g = 0, unsigned int b = 0, 
	      int style = vSolid, int fillMode = vAlternate)
	  {
	    brushColor._r = r, brushColor._g = g; brushColor._b = b;
	    brushFillMode = fillMode; brushStyle = style;
	  }

	//vBrush(const vBrush& p); Default OK

	int operator ==(const vBrush& b2) VCONST
	  { return (brushColor == b2.brushColor && brushFillMode == b2.brushFillMode &&
		brushStyle == b2.brushStyle); }

	int operator !=(const vBrush& b2) VCONST
	  { return (brushColor != b2.brushColor || brushFillMode != b2.brushFillMode ||
		brushStyle != b2.brushStyle); }

	~vBrush() {}
	
	void SetStyle(int style) { brushStyle = style; }
        int GetStyle() VCONST { return brushStyle; }

	void SetColor(vColor c) { if (c != brushColor) brushColor = c; }
        void SetColor(unsigned int r, unsigned int g, unsigned int b)
	  {
	    if (brushColor._r != r || brushColor._g != g || brushColor._b != b)
		brushColor.Set(r,g,b);
	  }
        vColor GetColor() { return brushColor; }

	void SetFillMode(int fillMode)
	  { if (fillMode != brushFillMode) brushFillMode = fillMode; }
        int GetFillMode() { return brushFillMode; }

	void SetBrushToPixel(Pixel px) { brushColor._pixel = px; brushColor._mapped = 1;}

	vColor brushColor;
	int brushFillMode;
	int brushStyle;

      protected:	//--------------------------------------- protected
      private:		//--------------------------------------- private
      };
#endif
