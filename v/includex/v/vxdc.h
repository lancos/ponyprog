//===============================================================
// vcpdc.h: CanvasPaneDC class .h file - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VXDC_H
#define VXDC_H

#include <v/vdc.h>

extern "C"
{
#include <X11/Xlib.h>
}

    class vWindow;		// we are part of a window
    class vMemoryDC;

    class vXDC : public vDC
      {

      public:		//---------------------------------------- public

	vXDC();

 	virtual ~vXDC();

	// Drawing

	virtual void Clear(void);
	virtual void ClearRect(int left, int top, int width, int height);
	virtual void CopyFromMemoryDC(vMemoryDC* vmemdc, int destX, int destY,  
	  int srcX = 0, int srcY = 0, int srcW = 0, int srcH = 0); // V:1.13
#ifdef DRAWARC
	virtual void DrawArc(int xx1, int yy1, int xx2, int yy2, int xxc, int yyc);
#endif
  	virtual void DrawAttrText(int x, int y, VCONST char* text, const ChrAttr attr);
  	virtual void DrawText(int x, int y, VCONST char* text);
	virtual void DrawEllipse(int x, int y, int width, int height);
	virtual void DrawIcon(int x, int y, VCONST vIcon& icon);
  	virtual void DrawLine(int x, int y, int xend , int yend);
  	virtual void DrawLines(vLine* lineList, int count);
	virtual void DrawLines(vPoint* points, int n);
	virtual void DrawColorPoints(int x, int y, int nPoints, vColor* pointList);
	virtual void DrawPoint(int x, int y);
	virtual void DrawPoints(vPoint* pointList, int count);
	virtual void DrawPolygon(int n, vPoint points[], int fillStyle);
	virtual void DrawRectangle(int x, int y, int width, int height);
	virtual void DrawRectangles(vRect* rectList, int count);
	virtual void DrawRoundedRectangle(int x, int y,
		int width, int height, int radius);
  	virtual void DrawRubberLine(int x, int y, int xend, int yend);
	virtual void DrawRubberEllipse(int x, int y, int width, int height);
	virtual void DrawRubberPoint(int x, int y);
	virtual void DrawRubberRectangle(int x, int y, int width, int height);

	virtual void SetFont(VCONST vFont& vf);	// Set Text font to vf

	// Appearance

	virtual void SetBackground(VCONST vColor& color);

	virtual void SetPen(VCONST vPen& pen);

	virtual void SetBrush(VCONST vBrush& brush);

	virtual int TextHeight(int& asc, int& des) VCONST;
	virtual int TextWidth(VCONST char* str) VCONST;

      protected:	//--------------------------------------- protected

	virtual Drawable GetDrawable();

	unsigned long _canvasFG;	// the foreground color
	unsigned long _canvasBG;	// the background color

	Display* _XDisplay;

	void SetPenStyle(GC gc);
  	void SetGCtoPen();
  	void SetGCtoBrush();

	static vFont _GCFont;		// the font actually in the GC
	static vPen _GCPen;		// the pen in use
	static vBrush _GCBrush;		// the brush in use

	// Default GC for the widget
	GC _GC;				// will be filled in
	GC _XorGC;			// will be filled in

      private:		//--------------------------------------- private

      };
#endif
