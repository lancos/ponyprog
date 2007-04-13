//===============================================================
// vpen.h: pen class header for drawing - X11R5
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

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    class vPen
      {
      public:		//---------------------------------------- public
	vPen(unsigned int r = 0, unsigned int g = 0, unsigned int b = 0, 
	      int width = 1, int style = vSolid)
	  {
	    penColor._r = r, penColor._g = g; penColor._b = b;
	    penWidth = width; penStyle = style;
	  }

	// Default shallow copy constructor and assignment will be OK

	int operator ==(const vPen& p2) VCONST
	  { return (penColor == p2.penColor && penWidth == p2.penWidth &&
		penStyle == p2.penStyle); }

	int operator !=(const vPen& p2) VCONST
	  { return (penColor != p2.penColor || penWidth != p2.penWidth ||
		penStyle != p2.penStyle); }

	void SetStyle(int style) { penStyle = style;}
	int GetStyle(void) VCONST { return penStyle; }
	void SetWidth(int width) { penWidth = width;}
	int GetWidth() VCONST  { return penWidth; }

	void SetColor(VCONST vColor& c) {if (c != penColor) penColor = c; }
        void SetColor(unsigned int r, unsigned int g, unsigned int b)
	  {  if (penColor.r() != r || penColor.g() != g || penColor.b() != b)
		penColor.Set(r,g,b);
	  }
        vColor GetColor() VCONST { return penColor; }

	~vPen() {}
	
	void SetPenToPixel(Pixel px) { penColor._pixel = px; penColor._mapped = 1;}

	vColor penColor;
	int penWidth;
	int penStyle;

      protected:	//--------------------------------------- protected
      private:		//--------------------------------------- private
      };

#endif
