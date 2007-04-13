//===============================================================
// vbrush.h: brush class header for filling - Windows
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

// Use Windows for brush styles, map to something reasonable for X

    class V_EXPORT vBrush
      {
      public:		//---------------------------------------- public

	vBrush(unsigned int r = 0, unsigned int g = 0, unsigned int b = 0,
	      int style = vSolid, int fillMode = vAlternate);

	vBrush(const vBrush& p);

	vBrush& operator =(const vBrush& p);

	int operator ==(const vBrush& b2) VCONST
	  { return (brushColor == b2.brushColor && brushFillMode == b2.brushFillMode &&
		brushStyle == b2.brushStyle); }

	int operator !=(const vBrush& b2) VCONST
	  { return (brushColor != b2.brushColor || brushFillMode != b2.brushFillMode ||
		brushStyle != b2.brushStyle); }

	~vBrush();

	void SetStyle(int style);
	int GetStyle() VCONST { return brushStyle; }

	void SetColor(vColor c);
	void SetColor(unsigned int r, unsigned int g, unsigned int b);
	vColor GetColor() VCONST { return brushColor; }

	void SetFillMode(int fillMode);
	int GetFillMode() VCONST { return brushFillMode; }

	HBRUSH GetHBRUSH() VCONST;

      protected:	//--------------------------------------- protected
      private:		//--------------------------------------- private
	int brushFillMode;
	int brushStyle;
	VMUTABLE HBRUSH _hbrush;
	VMUTABLE int _created;
        vColor brushColor;
      };

#endif
