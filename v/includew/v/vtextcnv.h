//===============================================================
// vtextcnv.h - vTextCanvasPane class - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VTEXTCNV_H
#define VTEXTCNV_H

const int MAX_ROWS = 100;
const int MAX_COLS = 400;


const int MARGIN = 2;


#define ScrChar(r,c) ((screen+(r*colsAllocated)+c)->chr)
#define ScrAttr(r,c) ((screen+(r*colsAllocated)+c)->attr)
#define ScrAddr(r,c) (screen+(r*colsAllocated)+c)

#include <v/vcanvas.h>

    class V_EXPORT vTextCanvasPane : public vCanvasPane
      {
	friend class V_EXPORT vWindow;
      public:		//---------------------------------------- public
	vTextCanvasPane();
	vTextCanvasPane(const vTextCanvasPane& t);

	virtual ~vTextCanvasPane();

	// Window management/drawing

	virtual void Clear(void);		// Clear text canvas window

	virtual void ClearRow(const int r, const int c);   // Clear row of text
	virtual void ClearToEnd(const int r, const int c); // Clr to end of screen

	int GetCols() VCONST { return maxCols; }
	int GetRows() VCONST { return maxRows; }

	void GetRC(int& r, int& c) VCONST { r = curRow; c = curCol; }
	void GotoRC(const int r, const int c);

	virtual void DrawAttrText(const char* text, const ChrAttr attr);
	virtual void DrawAttrsText(const char* text, const ChrAttr* attr);

	virtual void DrawChar(const char chr, const ChrAttr attr);

	void DrawText(const char* text);

        virtual void DrawAttrText(int x, int y, char* text, const ChrAttr attr)
	  { vCanvasPane::DrawAttrText(x, y, text, attr); }

  	virtual void DrawText(int x, int y, char* text)
	  { vCanvasPane::DrawText(x, y, text); }

	void HideTextCursor(void) {DrawTextCursor(-1,-1);}
	void ShowTextCursor(void) {DrawTextCursor(curRow,curCol);}
        void SetTextRowsCols(int rows, int cols);

	// Scrolling
	void ScrollText(const int lineCount);

	// Events
	virtual void FontChanged(VCONST vFont& newfont);

	virtual void ResizeText(const int rows, const int cols);
	virtual void Redraw(int x, int y, int width , int height);
	virtual void Resize(int w, int h);		// Resize event

	virtual void TextMouseDown(int row, int col, int button)
	  {
	    SysDebug3(MouseEvents,"TextMouseDown(r:%d,c:%d,btn:%d)\n",row,col,button);
	  };
	virtual void TextMouseUp(int row, int col, int button)
	  {
	    SysDebug3(MouseEvents,"TextMouseUp(r:%d,c:%d,btn:%d)\n",row,col,button);
	  };
	virtual void TextMouseMove(int row, int col, int button)
	  {
	    SysDebug3(MouseEvents,"TextMouseMove(r:%d,c:%d,btn:%d)\n",row,col,button);
	  };
      	virtual void initialize(vWindow* pWindow, HWND pWidget);

	int ColToX(int c)		// convert col to X coord
	  {
	    return (c*cWidth) + textMargin;
	  }
	int RowToY(int r)		// convert row to Y coord
	  {
	    return (r*cHeight) +cHeight + textMargin;
	  }

	int getMargin() { return textMargin;}
	void setMargin(int marg) { textMargin = marg;}

      protected:	//--------------------------------------- protected


        virtual void EnterEV(void);
        virtual void LeaveEV(void);

        int cHeight;			// height of a character
	int cAscent;			// Ascent of a character
	int cDescent;			// Descent of a character
	int cWidth;			// width of a chacter
	int maxRows;			// # rows that can be displayed
	int maxCols;			// # columns that can be displayed

	int curRow;			// Current row
	int curCol;			// Current col

	int caretOn;

	int rowsAllocated;		// how many rows and columns
	int colsAllocated;		// actually allocated to

	dChar* screen;		// array of pointers to dChar rows

	// make these protected now

	virtual void MouseDown(int x, int y, int button);
	virtual void MouseUp(int x, int y, int button);
	virtual void MouseMove(int x, int y, int button);

	virtual void ResizeText(void);

      private:		//--------------------------------------- private

	int textMargin;			// left margin for text
	int _cursorSet;

	void ClearRowDisplay(const int r, const int c); // Clear screen only

  	void DoDrawText(const char* text, const ChrAttr attr);

	void DrawTextScr(int r, int c, dChar* txt);

	void DrawTextCursor(int r, int c);

	int YToRow(int y);
	int XToCol(int x);
      };
#endif
