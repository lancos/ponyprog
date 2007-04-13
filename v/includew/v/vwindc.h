//===============================================================
// vwindc.h: vWinDC class .h file - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VWINDC_H
#define VWINDC_H

#include <v/vdc.h>

    class V_EXPORT vIcon;
    class V_EXPORT vColor;

    class V_EXPORT vWinDC : public vDC
      {

      public:		//---------------------------------------- public

	vWinDC();

	virtual ~vWinDC();

	// Drawing

	virtual void Clear(void){}
	virtual void ClearRect(int left, int top, int width, int height){}
        virtual void CopyFromMemoryDC(vMemoryDC* vmemdc, int destX, int destY,
	    int srcX = 0, int srcY = 0, int srcW = 0, int srcH = 0); // V:1.13
#ifdef DRAWARC
	virtual void DrawArc(int xx1, int yy1, int xx2, int yy2, int xxc, int yyc);
#endif
  	virtual void DrawAttrText(int x, int y, VCONST char* text, 
		const ChrAttr attr);
	virtual void DrawText(int x, int y, VCONST char* text);
	virtual void DrawEllipse(int x, int y, int width, int height);
	virtual void DrawIcon(int x, int y, VCONST vIcon& icon);
	virtual void DrawLine(int x, int y, int xend , int yend);
	virtual void DrawColorPoints(int x, int y, int nPoints, vColor* pointList);
	virtual void DrawLines(vLine* lineList, int count);
	virtual void DrawLines(vPoint* points, int n);
	virtual void DrawPoints (vPoint* pointList, int count);
        virtual void DrawRectangles(vRect* rectList,int count);

	virtual void DrawPoint(int x, int y);
	virtual void DrawPolygon(int n, vPoint points[], int fillStyle);
	virtual void DrawRectangle(int x, int y, int width, int height);
	virtual void DrawRoundedRectangle(int x, int y,
		int width, int height, int radius);
	virtual void DrawRubberLine(int x, int y, int xend, int yend);
	virtual void DrawRubberEllipse(int x, int y, int width, int height);
	virtual void DrawRubberPoint(int x, int y);
	virtual void DrawRubberRectangle(int x, int y, int width, int height);

	virtual void SetFont(VCONST vFont& vf){}
	// Appearance

	virtual void SetBackground(VCONST vColor& color){}

	virtual void SetPen(VCONST vPen& pen);

	virtual void SetBrush(VCONST vBrush& brush);

	virtual int TextHeight(int& asc, int& des) VCONST;
	virtual int TextWidth(VCONST char* str) VCONST;

	virtual int BeginPrinting(){return 0;}
	virtual void EndPrinting(){}
	virtual void BeginPage(){}
	virtual void EndPage(){}
	virtual void SetPrinter(vPrinter& printer){}


      protected:	//--------------------------------------- protected

	unsigned long _canvasFG;	// the foreground color
	unsigned long _canvasBG;	// the background color

        int _isPrinterDC;

	// These methods are to be used only internally and are assumed to
	// be used correctly - there is no error checking. The idea is to have
	// the canvas use BeginPaint/EndPaint for Redraw, and the drawing
	// rotuines to use GetHDC and ReleaseHDC.

	virtual void GetHDC() VCONST {}
	virtual void BeginPaint(){}
	virtual void EndPaint(){}
	virtual void ReleaseHDC() VCONST {}

        virtual int Scale(int x) { return (_hasScale) ? ((x*_Mult)/_Div) : x; }

	VMUTABLE HDC _hdc;			// HDC to USE for draws
        HDC _hdcPaint;				// HDC set by begin paint
	PAINTSTRUCT _ps;			// paint struct used by begin paint

	HPEN _hpen;         			// Handle for pen
	HBRUSH _hbrush;                         // Handle for brush

      private:		//--------------------------------------- private
        int _isWin32s;


      };
#endif
