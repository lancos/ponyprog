//===============================================================
// vnulldc.h: a NULL DC class .h file - All platforms
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VNULLDC_H
#define VNULLDC_H

#include <v/vdc.h>

class V_EXPORT vNullDC : public vDC
{
public:		//---------------------------------------- public
	vNullDC(void) {}
	virtual ~vNullDC() {}

	// Drawing
	virtual void Clear(void) {}
	virtual void ClearRect(int left, int top, int width, int height) {}
  	virtual void DrawAttrText(int x, int y, VCONST char* text, VCONST ChrAttr attr) {}
  	virtual void DrawText(int x, int y, VCONST char* text) {}
	virtual void DrawEllipse(int x, int y, int width, int height) {}
	virtual void DrawIcon(int x, int y, VCONST vIcon& icon) {}
  	virtual void DrawLine(int x, int y, int xend , int yend) {}
	virtual void DrawPoint (int x, int y) {}
	virtual void DrawPolygon (int n, vPoint points[],
									  int fillStyle = vAlternate) {}
	virtual void DrawRoundedRectangle(int x, int y,
												 int width, int height, int radius = 10) {}
	virtual void DrawRectangle(int x, int y, int width, int height) {}
	virtual void DrawLines(vLine* lineList){};
	virtual void DrawPoints (vPoint* pointList){};
	virtual void DrawRectangles(vRect* rectList){};

	// Misc

	virtual int GetPhysHeight() VCONST { return 0; }
	virtual int GetPhysWidth() VCONST { return 0; }

	virtual void SetBackground(VCONST vColor& color) {}

	virtual void SetScale(int mult, int div) {}
	virtual void GetScale(int& m, int& d) VCONST { m = 1; d = 1; }

	void SetTranslate(int x, int y) { }
	void SetTransX(int x) {}
	void SetTransY(int y) {}
	void GetTranslate(int& x, int& y) VCONST {x = 0; y = 0;}
	int GetTransX() VCONST { return 0; }
	int GetTransY() VCONST { return 0; }

 	virtual void SetFont(VCONST vFont& vf) {}
	virtual vFont GetFont() VCONST { return _vf; }

	virtual void SetPen(VCONST vPen& /*pen*/) {}
	virtual vPen GetPen() VCONST { return _vp; }

	virtual void SetBrush(VCONST vBrush& brush) {};
	virtual vBrush GetBrush() VCONST { return _vb; }

	virtual int TextHeight(int& asc, int& des) VCONST {asc = des = 0; return 0;}
	virtual int TextWidth(VCONST char* str) VCONST {return 0;}

private:
	vFont _vf;
	vBrush _vb;
	vPen _vp;
};
#endif
