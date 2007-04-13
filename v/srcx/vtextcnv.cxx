//===============================================================
// vtextcnv.cxx - TextCanvasPane, terminal like Text Drawing Canvas
//
// Copyright (C) 1995,1996,1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vtextcnv.h>

//================>>> vTextCanvasPane::vTextCanvasPane <<<====================
  vTextCanvasPane::vTextCanvasPane() : vCanvasPane(P_TextCanvas)
  {

    SysDebug(Constructor,"vTextCanvasPane::vTextCanvasPane() constructor\n")

    screen = new dChar[MAX_ROWS*MAX_COLS];

    rowsAllocated = MAX_ROWS;
    colsAllocated = MAX_COLS;

    for (int row = 0 ; row < MAX_ROWS ; ++row)
	for (int col = 0 ; col < MAX_COLS ; ++col)
	  {
	    ScrChar(row,col) = 0 ; ScrAttr(row,col) = 0;
	  }

    cHeight = 0; cWidth = 0;
    curRow = 0; curCol = 0;

    cursorRow = -1; cursorCol = -1;	// no cursor yet
    cursorChr = 0; cursorAttr = ChNormal;

    maxRows = -1; maxCols = -1;		// We don't know this yet

    _cursorSet = 0;
  }

//================>>> vTextCanvasPane::vTextCanvasPane <<<====================
  vTextCanvasPane::vTextCanvasPane(const vTextCanvasPane& t)
  {
    vSysError("vTextCanvasPane - V semantics do not support copy constructors!");
  }

//================>>> vTextCanvasPane::vTextCanvasPane <<<====================
  vTextCanvasPane::~vTextCanvasPane()
  {

    SysDebug(Destructor,"vTextCanvasPane::~vTextCanvasPane() desstructor\n")

    delete [] screen;
  }

//======================>>> vTextCanvasPane::Clear <<<======================
  void vTextCanvasPane::Clear(void)
  {
    // Clear screen - including contents

    SysDebug(Text,"vTextCanvasPane::Clear\n")

    for (int row = 0 ; row < rowsAllocated ; ++row)
	for (int col = 0 ; col < colsAllocated ; ++col)
	  {
	    ScrChar(row,col) = 0 ; ScrAttr(row,col) = ChNormal;
	  }

    vCanvasPane::Clear();

    cursorChr = 0; cursorAttr = ChNormal;
    cursorRow = cursorCol = -1;		// The cursor is gone

    GotoRC(0,0);			// Go to Row & Column
  }

//=================>>> vTextCanvasPane::ClearRow <<<======================
  void vTextCanvasPane::ClearRow(const int r, const int c)
  {
    // Clear screen row - including the contents

    SysDebug2(Text,"vTextCanvasPane::ClearRow(r:%d, c: %d)\n",r,c)


    //1.18 GotoRC(r,c);			// update cursor for sure

    // Clearing implies that the previous part of the row should
    // have valid characters, so change them to blanks if now 0
    for (int cc = 0 ; cc < c ; ++cc)
      {
	if (ScrChar(r,cc) == 0)
	  {
	    ScrChar(r,cc) = ' '; ScrAttr(r,cc) = ChNormal;
	  }
      }

    GotoRC(r,c);			// update cursor for sure (1.18)

    // Now clear the rest of the row
    for (int col = c ; col < maxCols ; ++col)
      {
	ScrChar(r,col) = 0 ; ScrAttr(r,col) = ChNormal;
      }

    cursorChr = 0; cursorAttr = ChNormal; 	// cursor is now "0"
						// instead of whatever it was
    ClearRowDisplay(r,c);

    GotoRC(r,c);
  }

//=============>>> vTextCanvasPane::ClearRowDisplay <<<======================
  void vTextCanvasPane::ClearRowDisplay(const int r, const int c)
  {
    // Clear display row 

    int x, y;

    x = ColToX(c);
    y = RowToY(r);

    ClearRect(x, y - cAscent, _width - x + 1, cDescent+cAscent);
  }

//=================>>> vTextCanvasPane::ClearToEnd <<<======================
  void vTextCanvasPane::ClearToEnd(const int r, const int c)
  {
    // Clear from r,c to end of screen
    
    SysDebug2(Text,"vTextCanvasPane::ClearToEnd(r:%d, c: %d)\n",r,c)
    ClearRow(r,c);
    for (int row = r+1 ; row < maxRows ; ++row)
	ClearRow(row,0);

    GotoRC(r,c);
  }

//=================>>> vTextCanvasPane::DrawTextCursor <<<=================
  void vTextCanvasPane::DrawTextCursor(int r, int c)
  {
    // Draw a cursor need the lower level code here because drawchar
    // calls drawcursor

    ChrAttr attr;
    char copy[2];

    copy[1] = 0;			// terminante

    if (cursorRow >= 0)			// We have a cursor, so unreverse
      {
	// Fix screen buffer contents
	ScrChar(cursorRow,cursorCol) = cursorChr ;
	ScrAttr(cursorRow,cursorCol) = cursorAttr;
	copy[0] = cursorChr;		// need to copy
	if (cursorChr == 0)
	    copy[0] = ' ';
	vCanvasPane::DrawAttrText(ColToX(cursorCol), RowToY(cursorRow),
	  copy, cursorAttr);
      }

    // OK, now draw a cursor at new location

    cursorRow = r;		// update to new location
    cursorCol = c;
    if (cursorRow < 0)
	return;			// This is how to turn the cursor off

    // remember what character was there before
    if ((cursorChr = ScrChar(cursorRow,cursorCol)) == 0)
	ScrChar(cursorRow,cursorCol) = ' ';

    copy[0] = ScrChar(cursorRow,cursorCol);	// need to copy for redraw

    cursorAttr = ScrAttr(cursorRow,cursorCol);	// remember old attribute

   
    // Toggle the reverse attribute to get a cursor

    attr = (cursorAttr & ChReverse) ? (cursorAttr & ~ChReverse)  // rev off
				    : (cursorAttr | ChReverse);  // rev on

    ScrAttr(cursorRow,cursorCol) = attr;
    
    vCanvasPane::DrawAttrText(ColToX(cursorCol), RowToY(cursorRow),
	copy, attr);
  }

//======================>>> vTextCanvasPane::DrawChar <<<======================
  void vTextCanvasPane::DrawChar(const char chr, const ChrAttr attr)
  {
     // Overwrite a char at current cursor, incr cursor
    char copy[2];	// for a copy

    copy[0] = chr; copy[1] = 0;

    SysDebug1(Text,"vTextCanvasPane::DrawChar(chr:%c)\n",chr)

    ScrChar(curRow,curCol) = chr ; ScrAttr(curRow,curCol) = attr;

    // for now, assume just reverse or normal on WHOLE row

    if (attr != ChNormal)		// normal or with attributes?
	vCanvasPane::DrawAttrText(ColToX(curCol), RowToY(curRow), copy, attr);
    else
	vCanvasPane::DrawText(ColToX(curCol), RowToY(curRow), copy);

    if (cursorRow == curRow && cursorCol == curCol)
	cursorRow = cursorCol = -1;		// we just fixed the cursor

    if (++curCol >= maxCols)
	curCol = maxCols - 1;

    cursorRow = -1;		// we just overwrote the cursor @@@
    GotoRC(curRow, curCol);
  }  

//======================>>> vTextCanvasPane::DrawText <<<======================
  void vTextCanvasPane::DrawText(const char* text)
  {
    DoDrawText(text, ChNormal);	// Draw normal text
  }

//======================>>> vTextCanvasPane::DrawAttrText <<<======================
  void vTextCanvasPane::DrawAttrText(const char* text, const ChrAttr attr)
  {
    DoDrawText(text, attr);	// Draw reverse text
  }

//======================>>> vTextCanvasPane::DoDrawText <<<======================
  void vTextCanvasPane::DoDrawText(const char* text, const ChrAttr attr)
  {
    // Draw scrolling  text starting at current r,c

    if (!text || !*text)
	return;

    int startC = curCol;
    int needsFinalDraw = 0;


    for (int cc = 0 ; cc < curCol ; ++cc)  // change 0's to blanks if in middle of row
      {
	if (ScrChar(curRow,cc) == 0)
	  {
	    ScrChar(curRow,cc) = ' '; ScrAttr(curRow,cc) = ChNormal;
	  }
      }

    cursorRow = cursorCol = -1;		// we just fixed the cursor

    for (const char* cp = text ; *cp != 0 ; ++cp)
      {
	if (*cp == '\n')		// new lines take special treatment
	  {
	    // Terminate current row with 0
	    ScrChar(curRow,curCol) = ' ' ; ScrAttr(curRow,curCol) = attr;

	    // And draw row on the screen!
	    DrawTextScr(curRow,startC,ScrAddr(curRow,startC));
	    startC = curCol = 0;	// reset column to 0
	    ++curRow;			// move to the next row
	    needsFinalDraw = 0;		// Don't need final draw
	    // Next row is empty
	    ScrChar(curRow,curCol) = 0 ; ScrAttr(curRow,curCol) = ChNormal;
	    if (curRow >= maxRows)	// see if off bottom
	      {
		ScrollText(1);		// scroll up 1 row
	      }
	  }
	else if (curCol < maxCols)	// overwrite current line
	  {
	    ScrChar(curRow,curCol) = *cp ; ScrAttr(curRow,curCol) = attr;
	    ++curCol;
	    needsFinalDraw = 1;		// Need final draw
	  }
      }


    if (needsFinalDraw)
	DrawTextScr(curRow,startC,ScrAddr(curRow,startC));

    GotoRC(curRow,curCol);
  }

//======================>>> vTextCanvasPane::DrawAttrsText <<<======================
  void vTextCanvasPane::DrawAttrsText(const char* text, const ChrAttr* attr)
  {
    // Draw scrolling  text starting at current r,c
    // this was simply copied from DrawAttrText but uses array for attr (1.23)

    if (!text || !*text)
	return;

    int startC = curCol;
    int needsFinalDraw = 0;


    for (int cc = 0 ; cc < curCol ; ++cc)  // change 0's to blanks if in middle of row
      {
	if (ScrChar(curRow,cc) == 0)
	  {
	    ScrChar(curRow,cc) = ' '; ScrAttr(curRow,cc) = ChNormal;
	  }
      }

   
    cursorRow = cursorCol = -1;		// we just fixed the cursor

    int ix;
    for ( ix = 0; text[ix] != 0 ; ++ix)
      {
	if (text[ix] == '\n')		// new lines take special treatment
	  {
	    // Terminate current row with 0
	    ScrChar(curRow,curCol) = ' ' ; ScrAttr(curRow,curCol) = attr[ix];

	    // And draw row on the screen!
	    DrawTextScr(curRow,startC,ScrAddr(curRow,startC));
	    startC = curCol = 0;	// reset column to 0
	    ++curRow;			// move to the next row
	    needsFinalDraw = 0;		// Don't need final draw
	    // Next row is empty
	    ScrChar(curRow,curCol) = 0 ; ScrAttr(curRow,curCol) = ChNormal;
	    if (curRow >= maxRows)	// see if off bottom
	      {
		ScrollText(1);		// scroll up 1 row
	      }
	  }
	else if (curCol < maxCols)	// overwrite current line
	  {
	    ScrChar(curRow,curCol) = text[ix] ; ScrAttr(curRow,curCol) = attr[ix];
	    ++curCol;
	    needsFinalDraw = 1;		// Don't need final draw
	  }
      }

    if (needsFinalDraw)
	DrawTextScr(curRow,startC,ScrAddr(curRow,startC));

    GotoRC(curRow,curCol);
  }

//======================>>> vTextCanvasPane::DrawTextScr <<<======================
  void vTextCanvasPane::DrawTextScr(int r, int c, VCONST dChar* txt)
  {
    char copy[MAX_COLS];	// for a copy

    char* to = copy;

    ChrAttr curAttr = txt->attr;  // initial attribute

    int curColl = c;

    int startColl = c;

    for (dChar* dcp = txt ; dcp->chr != 0 ; ++dcp)
      {
	if (dcp->attr != curAttr)	// changing attr - need to show what
	  {				// we've built so far
	    *to = 0;			// terminate
	    if (curAttr != ChNormal)	// normal or with attributes?
	      {
		vCanvasPane::DrawAttrText(ColToX(startColl), RowToY(r), 
			copy, curAttr);
	      }
	    else
	      {
		vCanvasPane::DrawText(ColToX(startColl), RowToY(r), copy);
	      }
	    // Now resetup for next portion of the row
	    to = copy;			// restart buffer
	    curAttr = dcp->attr;	// new attribute
	    startColl = curColl;		// and new start column
	    // fall through for rest of the row
	  }
	*to++ = dcp->chr;
	curColl++;
      }
    *to = 0;

    // write rest of the row

    if (copy[0] != 0)			// don't bother with empty row
      {
	if (curAttr != ChNormal)	// normal or with attributes?
	  {
	    vCanvasPane::DrawAttrText(ColToX(startColl), RowToY(r),
		copy, curAttr);
	  }
	else
	  {
	    vCanvasPane::DrawText(ColToX(startColl), RowToY(r), copy);
	  }
      }
  }

//======================>>> vTextCanvasPane::FontChanged <<<======================
  void vTextCanvasPane::FontChanged(VCONST vFont& newFont)
  {
    // The font has changed, so we must recalculate everything

    Clear();
    ResizeText();
  }

//======================>>> vTextCanvasPane::GotoRC <<<======================
  void vTextCanvasPane::GotoRC(const int r, const int c)
  {

    // Move cursor to row, col
    // First, a sanity check
    
    int row = (r < 0) ? 0 : r;
    int col = (c < 0) ? 0 : c;

    if (row >= maxRows)
	row = maxRows - 1;
    if (col >= maxCols)
	col = maxCols - 1;

    curRow = row;
    curCol = col;

    // have to be sure column is blank filled if moved to new column

    for (int cc = 0 ; cc < col ; ++cc)
      {
	if (ScrChar(row,cc) == 0)
	  {
	    ScrChar(row,cc) = ' '; ScrAttr(row,cc) = ChNormal;
	  }
      }

    DrawTextCursor(curRow, curCol);		// draw the cursor to here
  }

//======================>>> vTextCanvasPane::MouseDown <<<======================
  void vTextCanvasPane::MouseDown(int x, int y, int button)
  {
    // Map xy to text positions, call alternate mouse down

    TextMouseDown(YToRow(y), XToCol(x), button);
  }

//========================>>> vTextCanvasPane::MouseUp <<<======================
  void vTextCanvasPane::MouseUp(int x, int y, int button)
  {
    // Map xy to text positions, call alternate mouse up
    TextMouseUp(YToRow(y), XToCol(x), button);
  }

//======================>>> vTextCanvasPane::MouseMove <<<======================
  void vTextCanvasPane::MouseMove(int x, int y, int button)
  {
    // Map xy to text positions, call alternate mouse Move
    TextMouseMove(YToRow(y), XToCol(x), button);
  }

//=========================>>> vTextCanvasPane::Redraw <<<======================
  void vTextCanvasPane::Redraw(int x, int y, int width, int height)
  {
    if (maxRows < 0)		// This is the FIRST redraw
      {
	ResizeText();		// resize the text!
	return;			// don't redraw twice!
      }

#ifdef vDEBUG		// Don't have a SysDebug4 macro, so do it by hand
    if (DebugState.WindowEvents && DebugState.System)
	fprintf(stderr,"vTextCanvasPane::Redraw(x=%d, y=%d, w=%d, h=%d)\n",x,y,width,height);
#endif

    if (!_cursorSet)
      {
	SetCursor(VC_IBar);		// Change here
	_cursorSet = 1;
      }

    // OK, do a REALLY stupid redraw

    for (int r = 0 ; r < maxRows ; ++r)
	DrawTextScr(r,0,ScrAddr(r,0));

  }

//===================>>> vTextCanvasPane::SetTextRowsCols <<<======================
  void vTextCanvasPane::SetTextRowsCols(int rows, int cols)
  {
    int newheight = (rows * cHeight) + (2*MARGIN) + cHeight;;
    int newwidth = (cols * cWidth) + (2*MARGIN) + cWidth;;
    SetWidthHeight(newwidth, newheight);
    ResizeText();
  }

//===================>>> vTextCanvasPane::ResizeText <<<======================
  void vTextCanvasPane::ResizeText(const int rows, const int cols)
  {
    // This one is overridable so user can pick up new size

    SysDebug2(WindowEvents,"vTextCanvasPane::ResizeText(%d,%d)\n",rows,cols)
  }

//===================>>> vTextCanvasPane::ResizeText <<<======================
  void vTextCanvasPane::ResizeText(void)
  {
    // determine how many rows and columns we have

    cHeight = TextHeight(cAscent, cDescent);
    cWidth  = TextWidth("c");	// Use "c" as average char

    maxRows = (_height-(2*MARGIN)) / cHeight;		// how many rows
    if (maxRows > rowsAllocated)
	maxRows = rowsAllocated;

    maxCols = (_width-(2*MARGIN)) / cWidth;

    if (maxCols >= (colsAllocated - 1))
	maxCols = colsAllocated - 2;		// leave room for 0 terminator

    if (curRow > maxRows)			// sanity check
	curRow = 0;
    if (curCol > maxCols)
	curCol = 0;

    Clear();

    ResizeText(maxRows,maxCols);

    Redraw(0,0,_width,_height);			// force a redraw
  }

//======================>>> vTextCanvasPane::Resize <<<======================
  void vTextCanvasPane::Resize(int w, int h)
  {
    vCanvasPane::Resize(w,h);
    ResizeText();
  }

//======================>>> vTextCanvasPane::ScrollText <<<======================
  void vTextCanvasPane::ScrollText(const int lineCount)
  {
    // scroll text on screen up or down by lineCount

    int count = (lineCount < 0) ? -lineCount : lineCount;    // abs val

    if (count >= maxRows)	// see if doing the whole screen
      {
	Clear();
	if (lineCount < 0)
	    GotoRC(maxRows-1,0);
      }

    // May need to fix cursor...

    if (cursorRow >= 0 && cursorCol >= 0)
      {
	if (cursorChr != 0)	// V 1.15a
	  {
	    ScrChar(cursorRow,cursorCol) = cursorChr ;
	    ScrAttr(cursorRow,cursorCol) = cursorAttr;
	  }
	else
	  {
	    ScrChar(cursorRow,cursorCol) = ' ' ;
	    ScrAttr(cursorRow,cursorCol) = ChNormal;
	  }
      }

    if (lineCount > 0)		// Scroll up -- blank lines at bottom
      {
	int r,c, to;
	// Do the scroll: move all the data up count rows
	for (r = count ; r < maxRows ; ++r)
	  {
	    to = r - count;	// ScrChar is a macro, so need this
	    // copy one row up
	    for (c = 0 ; c < maxCols ; ++c)
	      {
		ScrChar(to,c) = ScrChar(r,c);
		ScrAttr(to,c) = ScrAttr(r,c);
	      }
	  }

	curRow = maxRows - count;	// set to bottom used row of screen
	curCol = 0;

	// Now, draw the screen with new rows
	for (r = 0 ; r < curRow ; ++r)
	  {
	    ClearRowDisplay(r,0);
	    DrawTextScr(r,0,ScrAddr(r,0));
	  }
	// And clear the rest of the screen
	for (r = curRow ; r < maxRows ; ++r)
	  {
	    ClearRow(r,0);
	  }
      }
    else			// scroll down
      {
	int r,c, to;
	int lastRow = maxRows -1;	// the last row on the screen
	// Do the scroll: move all the data down count rows
	for (r = lastRow - count ; r >= 0 ; --r)
	  {
	    to = r + count;	// ScrChar is a macro, so need this
	    // copy one row down
	    for (c = 0 ; c < maxCols ; ++c)
	      {
		ScrChar(to,c) = ScrChar(r,c);
		ScrAttr(to,c) = ScrAttr(r,c);
	      }
	  }

	curRow = count;	// set to topmost used row of screen
	curCol = 0;

	// Now, draw the screen with new rows
	for (r = curRow ; r < maxRows ; ++r)
	  {
	    ClearRowDisplay(r,0);
	    DrawTextScr(r,0,ScrAddr(r,0));
	  }
	// And clear the top part of the screen
	for (r = 0 ; r < curRow ; ++r)
	  {
	    ClearRow(r,0);
	  }
      }
  }

//======================>>> vTextCanvasPane::YToRow <<<======================
  int vTextCanvasPane::YToRow(int y)
  {
    if (cHeight == 0)
	cHeight = 1;
    int row = (y - MARGIN) / cHeight;


    if (row >= maxRows)
	return maxRows - 1;		// adjust for margins
    else if (row < 0)
	return 0;
    else
	return row;
  }

//======================>>> vTextCanvasPane::XToCol <<<======================
  int vTextCanvasPane::XToCol(int x)
  {
    if (cWidth == 0)
	cWidth = 1;
    int col = (x - MARGIN) / cWidth;

    if (col >= maxCols)
	return maxCols - 1;		// adjust for margins
    else if (col < 0)
	return 0;
    else
	return col;
  }
