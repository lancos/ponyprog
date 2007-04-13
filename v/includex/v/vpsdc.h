//===============================================================
// vpsdc.h: PostScriptDC class .h file - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VPSDC_H
#define VPSDC_H

#include <v/vdc.h>
#include <v/vprinter.h>
#include <fstream.h>

    class vPostScriptDC : public vDC
      {

	friend class PSDCPrinterSetup;

      public:		//---------------------------------------- public

	vPostScriptDC(const int scalePrinting = 0);

 	virtual ~vPostScriptDC();

	// Drawing

	virtual void Clear(void);
	virtual void ClearRect(int left, int top, int width, int height);
#ifdef DRAWARC
	virtual void DrawArc(int xx1, int yy1, int xx2, int yy2, int xxc, int yyc);
#endif
  	virtual void DrawAttrText(int x, int y, VCONST char* text, const ChrAttr attr);
  	virtual void DrawColorPoints(int x, int y, int nPoints, vColor* pointList);
  	virtual void DrawText(int x, int y, VCONST char* text);
	virtual void DrawEllipse(int x, int y, int width, int height);
	virtual void DrawIcon(int x, int y, vIcon& icon);
  	virtual void DrawLine(int x, int y, int xend , int yend);
  	virtual void DrawLines(vLine* lineList, int count);
	virtual void DrawLines(vPoint* points, int n);
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


	// Appearance

	virtual void SetBackground(vColor& color);
	virtual void SetBrush(vBrush& brush);
	virtual void SetFont(vFont& vf);
	virtual void SetPen(vPen& pen);
	virtual void SetPrinter(vPrinter& printer);

	virtual int TextHeight(int& asc, int& des);
	virtual int TextWidth(VCONST char* str);

	virtual int BeginPrinting(char* filename = 0, int ask = 1, int isColor = 0);
	virtual void EndPrinting();
	virtual void BeginPage();
	virtual void EndPage();

      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private

        void SetPSColor(vColor& bc);

	int _pages;
	int _fontIndex;
	ofstream *pstream;	// PostScript output stream

	vColor _curColor;
	vBrush _curBrush;
	vPen _curPen;
	vPrinter _printer;	// the printer

	Pixmap iconPM;

  	void SetGCtoPen();
  	void SetGCtoBrush();
      };
#endif
