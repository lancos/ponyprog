//=================================================================
//
// vtexted.cpp - the V text editor class
//
// last edit date: 31 Mar 2000 12:16:55
//
// Copyright (C) 1995-2000  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//
// This code is based on Bruce Wampler's editor, See. I think a
// bit of history about See will explain some things about the
// reasons some of the code structure and variable names are
// what they are. See end end of this file for the See history.
//
//===============================================================

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <v/vwindow.h>
#include <v/vkeys.h>

#include <v/vtexted.h>
#include <v/vfinddlg.h>
#include <v/vrepldlg.h>
#include <v/vmodald.h>
#include <v/vdialog.h>
//@@@
#include <v/vutil.h>

/// Working notes:
//      allow line list to grow dynamically by adding lines and
//      copying old list to new one, then deleting old one.

    const long lineListSize = 5000;     // allocation increment for lines

    // one static find pattern
    globalState vTextEditor::gState = {
	0,	// findAtBeginning
	0,	// fixed_scroll
	1,	// delOnInsert
	8,	// tabspc
	0,	// wraplm
	2	// braceIndent
      };
    int vTextEditor::findCaseSensitive = 0;
    int vTextEditor::findDown = 1;
    int vTextEditor::findWrap = 0;
    int vTextEditor::replaceConfirm = 0;
    int vTextEditor::replaceAll = 0;
    char vTextEditor::theFindPat[MAX_LINE + 2] = "";
    char vTextEditor::theReplPat[MAX_LINE + 2] = "";

// =========================>>> vTextEditor::vTextEditor <<<================
  vTextEditor::vTextEditor(VCONST vCmdWindow* parent)
  {

    _parent = const_cast<vCmdWindow*>(parent);		// remember who my parent was

    // Set the command interp
    _teCInt = new vTextEdCmdInterp(this, (vCmdWindow*)parent);

    state.readOnly =
    use_wild = 0;		// use wild cards in match

    oldcol = 1;

    state.ins_mode = 1;		// insert (vs. overtype)

    b_lastln =
    leftmg = xoutcm =
    tvdlin = state.echof = lastLine = 1;

    linptr = 0;

    mouseMoveRow = mouseMoveCol =
    mouseCol = mouseRow = -1;   // no mouse stuff yet
    scroll_lin = -1;
    setCursor = 0;

    for (int ix = 0 ; ix < 26 ; ++ix)
	noteloc[ix] = 0;

    _lines = 0;			// make these null for smarter destructor

    _highlighter = ChHighlight;

    // create space, initialize buffers

    undoBuff = new char[MAX_UNDO+2];
    undoBuff[0] = 0;

    initBuff();			// initialize buffer routines
    resetBuff();

    myConfirm = new vTEConfirm(_parent,"Replace?");
  }

//=========================>>> vTextEditor::~vTextEditor <<<================
  vTextEditor::~vTextEditor()
  {
    if (_teCInt)
	delete _teCInt;		// kill our command interp

    delete [] undoBuff;		// zap the undo buffer

    if (_lines)
      {
	for (long ix = 0 ; ix < _nextLine ; ++ix)	// delete contents
	    delete [] _lines[ix];
	delete [] _lines;	// delete if created
      }
  }

//=========================>>> vTextEditor::ChangeCmdInterp <<<================
  void vTextEditor::ChangeCmdInterp(vTextEdCmdInterp* newCI)
  {
    if (_teCInt)
	delete _teCInt;		// delete current command interp
    if (newCI == 0)		// use default CI
	_teCInt = new vTextEdCmdInterp(this, (vCmdWindow*)_parent);
    else
	_teCInt = newCI;
  }

// #########################################################################
//
// buffer methods
//
// These are designed to be easily overriden to allow more complex
// buffer management -- it should be possible to easily design a
// package that allows unlimited file size, for example.
//
// #########################################################################

//===========================>>> vTextEditor::initBuff <<<==================
  void vTextEditor::initBuff(void)
  {
    // initialize buffer routines - handled as a method so
    // can override

    _lines  = new char*[lineListSize];
    _maxLines = lineListSize - 1;	// safety factor

    _lines[0] = new char[2]; strcpy(_lines[0],"\n");
    _lines[1] = new char[2]; strcpy(_lines[1],"\n");
    for (long lx = 2 ; lx < lineListSize ; ++lx)
	_lines[lx] = 0;
    _nextLine = 2;
    lastLine = lastLineBF();
  }

// =======================>>> vTextEditor::resetBuff <<<====================
  void vTextEditor::resetBuff()		// open the buffer
  {

    // values used by buffer management

    if (_lines)
      {
	delete [] _lines[0];
	delete [] _lines[1];
	for (long ix = 2 ; ix < _nextLine ; ++ix)	// delete contents
	  {
	    if (_lines[ix])
		delete [] _lines[ix];
	  }
	_lines[0] = new char[2]; strcpy(_lines[0],"\n");
	_lines[1] = new char[2]; strcpy(_lines[1],"\n");
      }
    else
      {
	_lines  = new char*[lineListSize];
	_maxLines = lineListSize - 1;   // safety factor

	_lines[0] = new char[2]; strcpy(_lines[0],"\n");
	_lines[1] = new char[2]; strcpy(_lines[1],"\n");
      }

    _curLineBF =
    _nextLine = 1;

    wasColCmd = 0;

    // values used by editor code

    mark.beg_lin = mark.end_lin =	/* manually clear mark range */
    mark.beg_col = mark.end_col = 0;    /* DO NOT CALL ClearMarkRange() */
    mark.beg_chr = mark.end_chr = 0;

    curlin = lastLine = 1;

    curchr = _lines[1];

    last_col_out =
    state.counter =
    state.changes = 0;

    dsplin = ddline = 1;
    state.ins_mode = 1;		// insert (vs. overtype)
  }

// =========================>>> vTextEditor::addLine <<<====================
  int vTextEditor::addLine(char* line)
  {
    // This method adds the line to the end of the working buffer.
    // Note: line numbers start at 1!

    if (!line)
	return 0;

    if (_nextLine >= _maxLines)		// check for max lines
      if (!reallocLines())
	return 0;

    BUFFPTR bp = appendToBuff(line);

    if (!bp)
	return 0;

    if (curchr == _lines[_nextLine])
      {
	// When the very first line is added, curchr must be updated to
	// point to the new line instead of to the initial "\n"!
	curchr = bp;
      }

    _lines[_nextLine++] = bp;   // point line array to beginning of line
    lastLine = lastLineBF();	// update

    return 1;
  }

// ======================>>> vTextEditor::appendToBuff <<<==================
  BUFFPTR vTextEditor::appendToBuff(char* line)
  {
    // add text to the buffer -- allocate the spaced right here!

    BUFFPTR buff;
    BUFFPTR bp;
    int len = 0;
    char* cp;

    for (cp = line ; *cp && *cp != '\n' && *cp != '\r' ; ++cp) // break on eos or eol
      {
	++len;
      }

    buff = (BUFFPTR) new char[len+2];   // the space for the line

    if (!buff)				// did we get the space?
	return 0;

    for (bp = buff, cp = line ; *cp && *cp != '\n' && *cp != '\r' ; ) // break on eos or eol
      {
	*bp++ = *cp++;
      }
     *bp++ = '\n';
     *bp++ = 0;

    return buff;
  }

// =========================>>> vTextEditor::autoIndent <<<====================
  int vTextEditor::autoIndent()
  {
    if (!state.autoIndent)
	return 1;
    if (curlin < 1 || curlin >= _nextLine)
      {
	return 1;
      }

    BUFFPTR inCharP = GLine(curlin - 1);
    for (char chr = GCh(inCharP) ; chr == ' ' || chr == '\t' ;
               chr = GCh(++inCharP))
	charInsert((int)chr);
       
    return 1;			// assume successful
  }

// =========================>>> vTextEditor::insertLine <<<====================
  int vTextEditor::insertLine(char* line, long before, bool doUndo)
  {
    // This method adds the line before line before.
    // line numbers start at 1

    if (!line)
	return 0;

    if (doUndo)
	undoOff("Undo");

    if (_nextLine >= _maxLines)		// check for max lines
      if (!reallocLines())
	  return 0;

    BUFFPTR bp = appendToBuff(line);	// allocate space for line

    if (!bp)
	return 0;

    if (curchr == _lines[_nextLine])
      {
	// When the very first line is added, curchr must be updated to
	// point to the new line instead of to the initial "\n"!
	curchr = bp;
      }

    for (long to = _nextLine++ ; to > before ; --to)
      {
	_lines[to] = _lines[to-1];
      }

    _lines[before] = bp;   // point line array to beginning of line
    lastLine = lastLineBF();
    adjustLines(before,1);

    return 1;
  }

// =====================>>> vTextEditor::getFirstLine <<<===================
  int vTextEditor::getFirstLine(char* line, int maxChars)
  {
    // return the first line in the buffer

    return getLine(line, maxChars,1);
  }

// =========================>>> vTextEditor::getNextLine <<<================
  int vTextEditor::getNextLine(char* line, int maxChars)
  {
    // return the next line in the text buffer, not including end of line
    //    returns the number of characters, -1 if at end of file

    return getLine(line, maxChars, _curLineBF +1);
  }

// =========================>>> vTextEditor::getLine <<<===================
  int vTextEditor::getLine(char* line, int maxChars, long lineNum)
  {
    // return the next line in the text buffer, not including end of line
    //    returns the number of characters, -1 if at end of file

    if (lineNum >= _nextLine || lineNum < 1)		// at end!
	return -1;

    _curLineBF = lineNum;		// Set _curlineBF
    int len = 0;

    for (char* ix = _lines[_curLineBF] ; *ix != 0 && *ix != '\n' && len < maxChars ; )
      {
	line[len++] = *ix++;		// copy & track length
      }
    line[len] = 0;			// terminate
    return len;
  }

// ===================>>> vTextEditor::displayBuff <<<======================
  void vTextEditor::displayBuff(long lineNum, int paintIt)
  {
    // All lines added, so now display the buffer


    if (lineNum >= _nextLine)
	lineNum = _nextLine - 1;
    if (lineNum < 1)
	lineNum = 1;

    curlin = lineNum;   // Internal stuff begins with line 1
    curchr = GLine(curlin);     /* first char of buffer */
    lastLine = lastLineBF();
    int oldef = state.echof;
    state.echof = paintIt;
    ShowVScroll(1);
    Verify();

    state.echof = oldef;
  }

// =========================>>> vTextEditor::GLine <<<======================
  BUFFPTR vTextEditor::GLine(long lineNum)
  {
    if (lineNum < 1 || lineNum >= _nextLine)
      {
	return _lines[1];	// something safe
      }

    return _lines[lineNum];
  }

// =====================>>> vTextEditor::deleteCharBF <<<===================
  BUFFPTR vTextEditor::deleteCharBF(BUFFPTR charNum, long lineNum)
  {
    // First, shift everything up to next end of line left
    // This routine must not assume that lines are stored contiguously


    BUFFPTR ix;
    BUFFPTR newCurChar = charNum;

    char combine[MAX_LINE*2+2];

    char chrDeleted = GCh(charNum);

    if (lastLineBF() < 1)
	return 0;

    if (IsEndLine(chrDeleted))		// combine two lines?
      {
	if (lineNum < (_nextLine - 1))  // There IS a next line
	  {
	    BUFFPTR bp;
	    int to;
	    int offset;
	    int len1 = 0;
	    int len2 = 0;

	    // combine this line with the next line
	    to = 0;
	    for (bp = _lines[lineNum] ; *bp && !IsEndLine(*bp) ; )
	      {
		++len1;
		combine[to++] = *bp++;
	      }

	    offset = to;		// track where we were

	    for (bp = _lines[lineNum+1] ; *bp && !IsEndLine(*bp) ; )
	      {
		++len2;
		combine[to++] = *bp++;
	      }

	    if (len1 + len2 >= MAX_LINE)
		return 0;		// fail if too long now

	    *bp = 0;			// kill the old eol

	    combine[to] = 0;

	    // create buffer for it
	    bp = appendToBuff(combine);
	    if (!bp)
		return 0;

	    newCurChar = bp + offset;   // this is new cur char

	    // free the old space
	    delete [] _lines[lineNum];
	    delete [] _lines[lineNum+1];
	    _lines[lineNum] = bp;

	    // shift up the rest of the lines
	    for (long lp = lineNum + 1 ; lp < _nextLine ; ++lp)
		_lines[lp] = _lines[lp + 1];
	    --_nextLine;
	    lastLine = lastLineBF();
	  }
	adjustLines(lineNum,-1);	// one line deleted
      }
    else
      {
	newCurChar = charNum;
	for (ix = charNum + 1 ; *ix && !IsEndLine(*ix) ; ++ix)
	    *(ix-1) = *ix;
	*(ix-1) = *ix;
	*ix = 0;
      }
    return newCurChar;
  }

// ===================>>> vTextEditor::deleteLinesBF <<<====================
  long vTextEditor::deleteLinesBF(long start, long end)
  {
    // delete the lines in the specified range,
    // return new last line of file

    long to, from;

    if (lastLineBF() < 1)
	return 0;

    for (to = start ; to <= end ; ++to)
	delete[] _lines[to];

    for (to = start, from = end+1 ; from < _nextLine ; ++to, ++from)
      {
	_lines[to] = _lines[from];  // shift
      }
    _lines[from] = 0;
    _nextLine -= (end - start + 1);
    adjustLines(start, -(end - start + 1));
    lastLine = lastLineBF();

    if (_nextLine <= 1)
      {
	_lines[1] = new char[2]; strcpy(_lines[1], "\n");
	return 1;
      }

    return _nextLine - 1;
  }

// =====================>>> vTextEditor::insertCharBF <<<===================
  int vTextEditor::insertCharBF(int chr, BUFFPTR& curchar, long& curline)
  {
    // insert a char into the buffer at curline, curchar.
    // return new curchar, curline

    char ln1[MAX_LINE+2];
    char ln2[MAX_LINE+2];
    char* newLine;
    BUFFPTR origLine = _lines[curline];  // original full line
    int offSet;
    int ix;
    BUFFPTR fp;


    if (chr == 0)			// don't allow this case
	return 0;

    offSet = 0;
    for (BUFFPTR bp = origLine ; bp != curchar ; ++bp, ++offSet)
	;

    if (chr != '\n')
      {
	for (ix = 0, fp = origLine ; *fp != 0 && ix < MAX_LINE ; )
	  {
	    if (ix == offSet)
		ln1[ix++] = chr;		// add the new char
	    ln1[ix++] = *fp++;			// copy original line
	  }
	if (ix == offSet)
	    ln1[ix++] = chr;			// add the new char
	ln1[ix] = 0;
	newLine = appendToBuff(ln1);
	if (!newLine)				// see if got the space
	    return 0;
	delete [] _lines[curline];		// free the old space
	_lines[curline] = newLine;		// point line to new space
	curchar = newLine + offSet + 1;		// update curchar
      }
    else					// adding newline
      {
	// we are inserting a newline!
	for (ix = 0, fp = origLine ; *fp != 0 && ix < MAX_LINE ; )
	  {
	    if (ix == offSet)
		ln1[ix++] = chr;		// add the new char
	    ln1[ix++] = *fp++;			// copy original line
	  }
	if (ix == offSet)
	    ln1[ix++] = chr;			// add the new char
	ln1[ix] = 0;

	// ln1 now has two lines in it, split em up.

	char* cp;
	for (cp = ln1 ; *cp != '\n' ; ++cp)     // find first part
	    ;
	*cp++ = 0;

	char *tp;
	for (tp = ln2 ; *cp ; )			// copy second part
	    *tp++ = *cp++;
	*tp = 0;

	newLine = appendToBuff(ln1);
	if (!newLine)				// see if got the space
	    return 0;
	delete [] _lines[curline];		// free the old space
	_lines[curline] = newLine;		// point line to new space

	if (_nextLine >= _maxLines)		// check for max lines
	    if (!reallocLines())		// add some more lines
		return 0;

	    long lx;
	    for (lx = _nextLine ; lx > curline ; --lx)
		_lines[lx] = _lines[lx - 1];

	    newLine = appendToBuff(ln2);
	    if (!newLine)			// see if got the space
		return 0;

	    ++curline; ++_nextLine;
	    lastLine = lastLineBF();
	    adjustLines(curline,1);		// one more line

	    _lines[curline] = newLine;		// point line to new space

	    curchar = _lines[curline];		// point to line beginning
	  }
	return 1;
      }

// ==========================>>> vTextEditor::lineLenBF <<<=================
  int vTextEditor::lineLenBF(long lineNum)
  {
    if (lineNum < 1 || lineNum >= (_nextLine - 1))
	return 0;

    int len = 0;
    for (BUFFPTR ix = _lines[lineNum] ; *ix != '\n' && *ix != 0 ; ++ix)
	++len;
    return len;
  }

// ===================>>> vTextEditor::adjustLines <<<======================
  void vTextEditor::adjustLines(long line_1, int by)
  {
    /* adjust starting line number int note locaetions */

    int ix;

    for (ix = 0 ; ix < 26 ; ++ix)		/* adjust notes */
      {
	if (noteloc[ix] > line_1)		/* page needs fixing */
	    noteloc[ix] += (long) by;
      }
  }

// #########################################################################
//
// Command Interface methods
//
// #########################################################################

// ===================>>> vTextEditor::EditCommand <<<======================
  int vTextEditor::EditCommand(int id, long val)
  {
    // returns 1 if command succeeded, 0 if failed
    // or -1 if not processed

    int retval = 1;		// assume OK
    switch (id)
      {
	case edCut:		// ^X : Cut
	  {
	    EditCut();
	    break;
	  }

	case edCopy:		// ^C : Copy
	  {
	    EditCopy();
	    break;
	  }

	case edPaste:		// ^V: Paste
	  {
	    EditPaste();
	    break;
	  }

	case edBalMatch:		// find matching paren (up to val lines away)
	    retval = BalMatch(val);
	    break;

	case edFind:		// find
	  {
	    vFindDialog fdlg((vBaseWindow*)_parent);
	    if (!fdlg.AskFindPat(theFindPat, MAX_LINE,
		findCaseSensitive, findDown, findWrap))
		return 0;
	    retval = Find(theFindPat,
	    findCaseSensitive,findDown,findWrap);
	    break;
	  }

	case edFindNext:	// find again
	  {
	    if (*theFindPat)	// something to find
		retval = FindNext(findCaseSensitive,findDown,findWrap);
	    else
		retval = 0;
	    break;
	  }

	case edReplace:		// Replace
	  {
	    vReplaceDialog rdlg((vBaseWindow*)_parent);

	    if (!rdlg.AskReplacePat(theFindPat, MAX_LINE,
		theReplPat, MAX_LINE,
		findCaseSensitive, findDown, findWrap,
		replaceConfirm, replaceAll))
		return 0;
	    retval = Replace(theFindPat, theReplPat,
		findCaseSensitive, findDown, findWrap,
		replaceConfirm, replaceAll);
	    break;
	  }

	case edBufferBottom:	// move to bottom of file (no val)
	    bufferBottom();
	    break;

	case edCenterScreen:	// center cursor in screen
	    CenterScreen();
	    break;

	case edCharDelete:	// delete val chars
	    retval = charDelete(val);
	    break;

	case edCharFoldCase:    // swap case of val letters
	    retval = charFoldCase(val);
	    break;

	case edCharInsert:      // insert char val
	    retval = charInsert(val);
	    break;

	case edCharRight:       // move val chars right
	    retval = charRight(val);
	    break;

	case edFill:
	    retval = lineFill(val);
	    break;

	case edHelp:		// Show help screen
	    _teCInt->CmdInterpHelp();
	    break;

	case edIndent:		// AutoIndent if on
	    retval = autoIndent();
	    break;

	case edLineBeginning:   // move to line beginning (no val)
	    lineBeginning();
	    break;

	case edLineDown:        // move down val lines in column
	    retval = lineDown(val);
	    break;

	case edLineDownBeg:     // move down val lines
	    retval = lineDownBeg(val);
	    break;

	case edLineDelete:      // delete val lines
	    lineDelete(val);
	    break;

	case edLineDeleteFront: // delete to beginning of line (no val)
	    retval = lineDeleteFront();
	    break;

	case edLineDeleteToEnd: // delete to end of line (no val)
	    retval = lineDeleteToEnd();
	    break;

	case edLineEnd:         // move to end of line (no val)
	    lineEnd();
	    break;

	case edLineGoto:        // move cursor to line val
	    retval = lineGoto(val);
	    break;

	case edLineOpen:        // open val new blank lines
	    retval = lineOpen(val);
	    break;

	case edFormatC:		// format C code lines
	    retval = formatC(val);
	    break;

	case edNoteLocation:	// note which line we are on
	    if (val >= 'a' && val <= 'z' )
		val = val-'a';
	    if (val < 0 || val > 25)	// 0 - 25 valid
		val = 0;
	    noteloc[val] = GetCurLine();
	    break;

	case edNoteGoto:	// goto noted locateion
	    if (val >= 'a' && val <= 'z' )
		val = val-'a';
	    if (val < 0 || val > 25)	// 0 - 25 valid
		val = 0;
	    retval = lineGoto(noteloc[val]);
	    break;

	case edUndo:		// undo the last edit
	    retval = undoEdit();
	    break;

	case edWordRight:       // move cursor val words right
	    wordRight(val);
	    break;

	case edScrollDown:      // scroll val lines without changing cursor
	    scrollDown(val);
	    break;

	case edVerify:          // repaint screen
	    Verify();
	    break;

	default:                        // editor can't handle
	    retval = -1;
	    break;
      }
    return retval;
  }

// ======================>>> vTextEditor::EditKeyIn <<<=====================
  int vTextEditor::EditKeyIn(vKey key, unsigned int shift)
  {
     // process keystrokes -- return 1 if processed, 0 if
     // command fails, and -1 if not recognized

    return _teCInt->ProcessKey(key, shift);

  }

// ===================>>> vTextEditor::defaultKeyIn <<<=====================
  int vTextEditor::defaultKeyIn(vKey key, unsigned int shift)
  {
    // this is a virtual function so that it would be possible
    // to override this method to implement a modal editor like
    // vi or see.
    // Returns -1 if not processed, 1 if successful, 0 if
    // command didn't succeed.

    int chr = (int)key;

    if ((shift & VKM_Alt || shift & VKM_Ctrl)
	&& (chr < ' ' || chr > 0x7E))
	return -1;

    if (chr < ' ' && chr != '\r' && chr != '\t' && chr != '\n')
	return -1;

    if (chr > 0xFE)     // ignore function and alt keys
	return -1;

    int retval = charInsert(key);
    if (retval && (key == '\n' || key == '\r'))
	retval = autoIndent();
    return retval;
  }

// =======================>>> vTextEditor::Find <<<=========================
  int vTextEditor::Find(char* pat, int caseSensitive, int Down, int Wrap)
  {
    // Copy a new pattern to the pattern buffer

    char *fp = pat;
    int ix;
    for (ix = 0 ; *fp && ix < MAX_LINE ; )
	theFindPat[ix++] = *fp++;
    theFindPat[ix] = 0;

    return FindNext(caseSensitive, Down, Wrap); // now use findNext
  }

// =====================>>> vTextEditor::FindNext <<<=======================
  int vTextEditor::FindNext(int caseSensitive, int Down, int Wrap)
  {
    // find a pattern = return 1 if found, 0 if not

    BUFFPTR findBP;
    char* foundAt;
    int ix;
    int lineOffset = 0;

    char workPat[MAX_LINE + 2]; // working pattern
    char workLine[MAX_LINE + 2];	// working line

    long findLine = curlin;
    long origLine = curlin;

    if (lastLineBF() < 1)
	return 0;

    int patLen = strlen(theFindPat);

    for (ix = 0 ; theFindPat[ix] != 0 ; ++ix)
      {
	if (caseSensitive)
	    workPat[ix] = theFindPat[ix];
	else
	    workPat[ix] = tolower(theFindPat[ix]);
      }
    workPat[ix] = 0;

    if (Down)
      {
	if (gState.findAtBeginning)
	  {
	    charRight(1,0);
	  }
	findBP = curchr;
	for ( ; ; )
	  {
	    lineOffset = findBP - GLine(findLine);  // if not at beginning
	    for (ix = 0 ; ix < MAX_LINE && GCh(findBP) != 0 ; ++findBP, ++ix)
	      {
		if (caseSensitive)
		    workLine[ix] = GCh(findBP);
		else
		    workLine[ix] = tolower(GCh(findBP));
	      }
	    workLine[ix] = 0;

	    foundAt = strstr(workLine,workPat);		// see if in there

	    if (foundAt != 0)			// We found it!
	      {
		undoOff("Undo");
		ClearMarkRange();		/* no range now */
		curlin = findLine;
		int offset = foundAt - &workLine[0];

		curchr = GLine(curlin) + offset + lineOffset;

		// set up mark range
		mark.beg_lin = mark.end_lin = curlin;
		mark.beg_col = col_pos(curchr,0);
		mark.end_col = col_pos(curchr+patLen,0);
		mark.beg_chr = curchr;
		mark.end_chr = curchr + patLen;

		if (!gState.findAtBeginning)
		    curchr += patLen;		// leave at end

		// need intelligent screen updating
		// This puts the pattern in the middle if we've
		// moved over a screen
		if ((curlin - origLine) > GetRows())
		    CenterScreen();
		else
		    update(curlin - origLine);
		return 1;
	      }
	    if (findLine >= lastLine)
	      {
		StatusMessage(" Pattern Not Found");
		if (gState.findAtBeginning)
		  {
		    charRight(-1,0);
		  }
		return 0;
	      }
	    else
	      {
		findLine++;
		findBP = GLine(findLine);
	      }

	  }
	if (gState.findAtBeginning)
	  {
	    charRight(-1,0);
	  }
	StatusMessage(" Pattern Not Found");
	return 0;
      }
    else	// find up
      {
	if (!gState.findAtBeginning)
	  {
	    charRight(-1,0);
	  }
	findBP = GLine(findLine);
    	BUFFPTR lim = curchr;	// for first time
	for ( ; ; )
	  {
	    for (ix = 0 ; ix < MAX_LINE && GCh(findBP) != 0 ;
		  ++findBP, ++ix)
	      {
	 	if (lim != 0 && findBP >= lim)
		    break;		// first line only
		if (caseSensitive)
		    workLine[ix] = GCh(findBP);
		else
		    workLine[ix] = tolower(GCh(findBP));
	      }
	    lim = 0;			// done with first line
	    workLine[ix] = 0;

	    foundAt = strstr(workLine,workPat);	// search from end


	    if (foundAt != 0)		// We found it!
	      {
		// We found the first instance, now look for the last
		char *cp = foundAt;
		char *lastPat = 0;
		char *nextPat = 0;
		for (cp++ ; (nextPat = strstr(cp,workPat)) != 0 ; ++cp)
		  {
		    lastPat = nextPat;		// remember last one
		  }
		if (lastPat != 0)		// there was a later pat
		  {
		    foundAt = &workLine[strlen(workLine)-strlen(lastPat)];
		  }

		undoOff("Undo");
		ClearMarkRange();		/* no range now */
		curlin = findLine;
		int offset = foundAt - &workLine[0];

		curchr = GLine(curlin) + offset;

		// set up mark range
		mark.beg_lin = mark.end_lin = curlin;
		mark.beg_col = col_pos(curchr,0);
		mark.end_col = col_pos(curchr+patLen,0);
		mark.beg_chr = curchr;
		mark.end_chr = curchr + patLen;

		if (!gState.findAtBeginning)
		    curchr += patLen;		// leave at end

		// need intelligent screen updating
		// This puts the pattern in the middle if we've
		// moved over a screen
		if ((curlin - origLine) > GetRows())
		    CenterScreen();
		else
		    update(curlin - origLine);
		return 1;
	      }
	    if (findLine <= 0)
	      {
		if (!gState.findAtBeginning)
		  {
		    charRight(1,0);
		  }
		StatusMessage(" Pattern Not Found");
		return 0;
	      }
	    else
	      {
		findLine--;
		findBP = GLine(findLine);
	      }

	  }
	if (!gState.findAtBeginning)
	  {
	    charRight(1,0);
	  }
	StatusMessage(" Pattern Not Found");
	return 0;
      }
  }

// =====================>>> vTextEditor::Replace <<<====================
  int vTextEditor::Replace(const char* pat, const char* repl,
	int caseSensitive, int Down, int Wrap, int Confirm, int All)
  {
    const char* fp = pat;
    const char* rp = repl;
    int ix;
    for (ix = 0 ;  fp && *fp && ix < MAX_LINE ; )
	theFindPat[ix++] = *fp++;
    theFindPat[ix] = 0;
    for (ix = 0 ;  rp && *rp && ix < MAX_LINE ; )
	theReplPat[ix++] = *rp++;
    theReplPat[ix] = 0;

    if (state.echof && All && !Confirm)		// echo off if all
	state.echof = 0;

    while (FindNext(caseSensitive, Down, Wrap)) // now use findNext
      {
	ReplaceConfirm doit = YesRepl;		// assume replace once
	if (Confirm)
	  {
	    doit = (ReplaceConfirm) myConfirm->ConfirmRep();
	  }

	if (doit == YesRepl || doit == YesReplNext || doit == RepAll)
	  {
	    checkIfScrolled();
	    (void) SaveKillLine(1,true); 	// save current line
	    RemoveMarkRange(false);
	    ClearMarkRange();			/* no range now */
	    for (const char* cp = repl ; *cp ; ++cp)
	      {
		charInsert((int)*cp,false);
	      }
	    undoOn("Undo Replace");
	    // just overrite the undo stuff caused by RemoveMarkRange
	    // undo: dodoReplace(findpattern,repl);
	  }

	if (doit == RepAll)		// change to replace all mode
	  {
	    Confirm = 0; All = 1;
	    if (state.echof)
		state.echof = 0;
	  }

	if (doit == QuitR)
	  {
	    if (All && !Confirm)
	      {
		state.echof = 1;
		Verify();
	      }
	    return 1;
	  }

	if (doit == YesReplNext || doit == NoReplNext || All)
	    continue;
	else
	  {
	    if (All && !Confirm)
	      {
		state.echof = 1;
		Verify();
	      }
	    return 1;
	  }
      }

    if (All && !Confirm)
      {
	state.echof = 1;
	Verify();
      }
    return 0;
  }

// =======================>>> vTextEditor::HPage <<<========================
  void vTextEditor::HPage(int shown, int top)
  {
    vTextCanvasPane::HPage(shown, top);
  }

// =======================>>> vTextEditor::VPage <<<========================
  void vTextEditor::VPage(int shown, int top)
  {

    long target;
    if (lastLineBF() < 1 || lastLine <= GetRows())
      {
	setScrollBar();
	return;
      }
    if ((top+shown) >= 100)
	target = lastLine;
    else if (top < 1)
	target = 1;
    else
	target = ((long)top * lastLine) / 100;

    if (target < 1)
	target = 1;
    if (target > lastLine)
	target = lastLine;
    if (scroll_lin < 0)
	scrollDown(target - curlin);
    else
	scrollDown(target-scroll_lin);
  }

// =====================>>> vTextEditor::HScroll <<<========================
  void vTextEditor::HScroll(int step)
  {
    vTextCanvasPane::HScroll(step);
  }

// =====================>>> vTextEditor::VScroll <<<========================
  void vTextEditor::VScroll(int step)
  {
    if (step < 0)
	scrollDown(-1);
    else
	scrollDown(1);
  }

// =====================>>> vTextEditor::FontChanged <<<====================
  void vTextEditor::FontChanged(VCONST vFont& newFont)
  {
    vTextCanvasPane::FontChanged(newFont);
  }

// ====================>>> vTextEditor::TextMouseDown <<<===================
  void vTextEditor::TextMouseDown(int row, int col, int button)
  {

     if (button != 1)			// ignore right button moves
      {
	vTextCanvasPane::TextMouseDown(row, col, button);
	return;
      }

    mouseMoveRow = mouseRow = row + 1;  // convert to 1,1 corner
    mouseMoveCol = mouseCol = col + 1;
    MouseMoveTo(mouseRow,mouseCol);     // doing the move at mouse down
					// makes selections easier
    setScrollBar();
    ClearMarkRange();			// forces deselect
  }

// ======================>>> vTextEditor::TextMouseUp <<<===================
  void vTextEditor::TextMouseUp(int r, int c, int button)
  {
    if (button != 1)			// ignore right button moves
      {
	vTextCanvasPane::TextMouseUp(r, c, button);
	return;
      }
  }

// ======================>>> vTextEditor::CenterScreen <<<=================
  void vTextEditor::CenterScreen()
  {
    // Some tricky stuff to put the cursor in the middle.
    int old_ddline = ddline;
    if (lastLine > GetRows())
	ddline = GetRows() / 2;
    else
	ddline = lastLine / 2;
    if (ddline < 1)
	ddline = 1;
    newscr();
    tvxy(findX(),tvy);          /* reset cursor to current position */
    ddline = old_ddline;

    setScrollBar();		// MUST do this!
    ChangeLoc(curlin,col_pos(curchr,0));
  }

// ======================>>> vTextEditor::MouseMoveTo <<<=================
  void vTextEditor::MouseMoveTo(int row, int col)
  {

    // used by MouseUp and MouseMove to update cursor location
    if (scroll_lin < 0)		// regular move
      {
	int cnt = row - tvdlin;
	if (cnt != 0)                       // difference
	    lineDownBeg((long)cnt,0);               // changing
	else
	    lineBeginning();                // same row
	tvhdln();
      }
    else                    // switching position to scroll window
      {
	curlin = maxl((long)1,minl(scroll_lin + row - 1,lastLine));
	curchr = GLine(curlin);
	scroll_lin = -1;
	long old_ddline = ddline;   // trick - terrible awful coding, but...
	ddline = row;
	newscr();
	int xf = findX();
	tvxy(xf,tvy);       /* reset cursor to current position */
	ddline = old_ddline;
	tvhdln();
      }

    b_lastln = curlin;

    while (!IsEndLine(GCh(curchr)) && col > findX() )
      {
	++curchr;
      }
    tvhdln();

    ChangeLoc(curlin,col_pos(curchr,0));
    _teCInt->MouseJump(curlin, col_pos(curchr,0));	// notify cmdinterp
  }

// ======================>>> vTextEditor::TextMouseMove <<<=================
  void vTextEditor::TextMouseMove(int r, int c, int button)
  {

    if (button != 1)		// ignore right button moves
      {
	vTextCanvasPane::TextMouseMove(r, c, button);
      }
    else                // highlighting text
      {
	if (mouseRow <= 0 || mouseCol <= 0)
	    return;

	if (curlin < 1 || GetCols() < 1 || lastLineBF() < 1)
	    return;

	int row = r + 1;                // convert to 1,1 corner
	int col = c + 1;

	if (mouseMoveRow != row)        // have moved a row down or up
	  {
	    if (row > mouseMoveRow)     // moved down
	      {
		int cnt = row - mouseMoveRow;
		AddToRangeRow(cnt);   // add rows
		mouseMoveRow = row;
		// and update to columns
		AddToRangeCol(col);
		mouseMoveCol = col;
	      }
	    else                        // moving mouse up rows
	      {
		int cnt = row - mouseMoveRow;
		AddToRangeRow(cnt);   // add rows
		mouseMoveRow = row;
		// and update to columns
		AddToRangeCol(col);
		mouseMoveCol = col;
	      }

	  }
	else if (mouseMoveCol != col)   // mouse to right of start
	  {
	    AddToRangeCol(col);
	    mouseMoveCol = col;
	  }
      }
  }

// =========================>>> vTextEditor::setSelecton <<<=================
  void vTextEditor::setSelection(int howmuch)
  {
    // set the selection based on current cursor location
    // howmuch: 0: one char
    //		1: one word
    //		2: C symbol (uses .,->)
    //		3: whole line

    BUFFPTR origBP;
    BUFFPTR endchr, begchr;
    BUFFPTR begLine = GLine(curlin);

    origBP = curchr;		// remember where we are now

    ClearMarkRange();

    // first, move left until previous char is not a-z, A-Z, 0-9 or _)

    if (howmuch == 1)
      {
	for (begchr = curchr ; begchr != begLine ; --begchr )
	  {
	    if (!IsWordChar(GCh(begchr-1)))  // break if not word char
		break;
	  }
	// now, find the end of the word
	for (endchr = curchr ; !IsEndLine(GCh(endchr)) ; ++endchr)
	  {
	    if (!IsWordChar(GCh(endchr)))
		break;
	  }
      }
    else if (howmuch == 2)		// C identifier
      {
	char p;
	// first, go left to simple beginning of symbol
	for (begchr = curchr ; begchr != begLine ; --begchr )
	  {
	    p = GCh(begchr-1);
	    if (!IsWordChar(p))
		break;
	  }
	// now, find the end of the word - including operators and parens
	int plev = 0;
	for (endchr = curchr ; !IsEndLine(GCh(endchr)) ; ++endchr)
	  {
	    p = GCh(endchr);
	    if (plev > 0)
	      {
		if (p == ')' || p == ']')
		    --plev;
		else if (p == '(' || p == '[')
		    ++plev;
		if (plev == 0)		// done when back to 0
		  {
		    ++endchr;
		    break;
		  }
	      }
	    else if (!IsWordChar(p) && p != '-' && p != '>' && p != '.')
	      { 
		if (p == '(' || p == '[')
		   ++plev;
		else
		    break;
	      }
	  }
      }
    else if (howmuch == 3)		// whole line
      {
	for (begchr = curchr ; begchr != begLine ; --begchr )
	  {
	  }
	for (endchr = curchr ; !IsEndLine(GCh(endchr)) ; ++endchr)
	  {
	  }
      }

    mark.end_lin = mark.beg_lin = curlin;

    mark.beg_chr = begchr;
    mark.beg_col = col_pos(begchr,0);

    mark.end_chr = endchr;
    mark.end_col = col_pos(endchr,0);
    curchr = origBP;
    tvxy(1,tvy);                // Finally, fix up the highlight
    type_lines(curlin, 1);      // by repainting current line
    tvhdln();                   // and rehome cursor
    ((vWindow *)_parent)->SetValue(M_Cut, 1, Sensitive);
    ((vWindow *)_parent)->SetValue(M_Copy, 1, Sensitive);

  }

// =========================>>> vTextEditor::addToRange <<<=================
  void vTextEditor::addToRange(long rows, long cols)
  {
    // add number of rows and columns to range
    // assumes done intelligently....

    int oldef = state.echof;
    // first, do rows

    if (curlin + rows > lastLine)    /* past end? 1.24b */
	return;

    if (rows != 0)
      {
	// the way AddToRangeRow works assumes the caller will
	// then call AddToRangeCol as well.

	if (rows < -1)	// screen update is broken for multiple rows up
	    state.echof = 0;	// so kludge up this workaround
	AddToRangeRow(rows);
	AddToRangeCol(col_pos(curchr,0));
      }
    if (cols != 0)
      {
	if (cols > 0)		// moving right
	  {
	    if (cols > 999)	// special case for Shift-End
	      {
		if (rows == 0)	// just Shift-End
		  {
		    while ( !IsEndLine(GCh(curchr)) )
			AddToRangeCol(col_pos(curchr,0)+1);
		  }
		else
		    AddToRangeCol(1000);  // Really from Ctrl-Shift-End
	      }
	    else		// Simple Shift-Right
	      {
		if (IsEndLine(GCh(curchr))) // If at end of line, special
		  {
		    if (curlin >= lastLine)    /* past end? 1.24b */
			return;
		    AddToRangeRow(1);	// Down one row
		    AddToRangeCol(0);	// but fix up column
		  }
		else
		    AddToRangeCol(col_pos(curchr,0)+1); // finally, and easy one
	      }
	  }
	else
	  {
	    if (cols < -999)	// for Shift-Home
	      {
		if (rows == 0)	// just Shift-Home
		  {
		    while (curchr != GLine(curlin))
			AddToRangeCol(col_pos(curchr,0)-1);
		  }
		else		// Really Ctrl-Shift-Home
		    AddToRangeCol(0);

	      }
	    else		// Shift-Left
	      {
		if (curchr == GLine(curlin))	// at line beginning?
		  {
		    AddToRangeRow(-1);		// up
		    AddToRangeCol(1000);	// fix column by going to end
		  }
		else
		    AddToRangeCol(col_pos(curchr,0)-1); // just Shift-Left
	      }
	  }
      }
    if (rows < -1)	// screen update is broken for multiple rows up
      {
	state.echof = oldef;
	Verify();		// so just do an extra verify
      }
  }

// =========================>>> vTextEditor::AddToRangeCol <<<=================
  void vTextEditor::AddToRangeCol(int col)
  {

    if (col < 1)
	col = 1;

    if (col_pos(curchr,0) < col)	// add to right
      {
	if (mark.beg_lin == 0)          /* no lines yet */
	  {
	    mark.end_lin = mark.beg_lin = curlin;
	    mark.end_chr = mark.beg_chr = curchr;
	    mark.end_col = mark.beg_col = col_pos(curchr,0);
	    while (!IsEndLine(GCh(curchr))
		&& col_pos(curchr,0) < col)  // find new col
	        charRight(1,0);
	    mark.end_chr = curchr;
	    mark.end_col = col_pos(curchr,0);
	  }
	else if (mark.end_lin < curlin)
	  {
	    mark.end_lin = curlin;
	    mark.end_chr = curchr;
	    mark.end_col = col_pos(curchr,0);
	  }
	else if (mark.beg_lin == curlin && mark.end_lin == curlin
	    && col_pos(curchr,0) > mark.end_col)
	  {
	    mark.beg_col = mark.end_col;
	    mark.beg_chr = mark.end_chr;
	    mark.end_chr = curchr;
	    mark.end_col = col_pos(curchr,0);
	  }
	else if (mark.beg_lin == curlin &&  // some of this line marked
	    ((mark.end_lin == curlin && col_pos(curchr,0) < mark.end_col)
                 || mark.end_lin != mark.beg_lin))
          {
	    while (!IsEndLine(GCh(curchr))
		&& col_pos(curchr) < col)    // find new col
	    charRight(1,0);
	    mark.beg_col = col_pos(curchr,0);
	    mark.beg_chr = curchr;
	  }
	else                            // normal add to marked
	  {
	    while (!IsEndLine(GCh(curchr))
		&& col_pos(curchr) < col)    // find new col
                charRight(1,0);

	    mark.end_lin = curlin;
	    mark.end_chr = curchr;
	    mark.end_col = col_pos(curchr,0);
	  }
      }
    else                                // add to left
      {
	if (mark.beg_lin == 0)          // no lines yet, so note it
	  {
	    if (curchr == GLine(curlin))
		return;                 // No op at line beginning
	    mark.end_lin = mark.beg_lin = curlin;
	    mark.end_chr = mark.beg_chr = curchr;
	    mark.end_col = mark.beg_col = col_pos(curchr,0);
	    while (col_pos(curchr) > col)       // find new col
		charRight(-1,0);
	    mark.beg_chr = curchr;
	    mark.beg_col = col_pos(curchr,0);

	  }
	else if (mark.end_lin < curlin)
	  {
	    mark.end_lin = curlin;
	    mark.end_chr = curchr;
	    mark.end_col = col_pos(curchr,0);
	  }
	else if (mark.end_lin == curlin &&  // some of this line marked
	    ((mark.beg_lin == curlin && col_pos(curchr,0) > mark.beg_col)
		|| mark.beg_lin != curlin))
	  {
	    // handles two cases: highlight on current line only,
	    // or highlight starts on some line before
	    while (col_pos(curchr) > col)       // find new col
		charRight(-1,0);
	    mark.end_col = col_pos(curchr,0);
	    mark.end_chr = curchr;
	  }
	else if (mark.end_lin == curlin  // some of this line marked
                 && col_pos(curchr,0) > mark.beg_col)
          {
	    while (col_pos(curchr) > col)       // find new col
		charRight(-1,0);
	    mark.end_col = col_pos(curchr,0);
	    mark.end_chr = curchr;
	  }
	else                            // normal add to marked
	  {
	    while (col_pos(curchr) > col)       // find new col
		charRight(-1,0);

	    mark.beg_lin = curlin;
	    mark.beg_chr = curchr;
	    mark.beg_col = col_pos(curchr,0);
	  }
      }

    tvxy(1,tvy);                // Finally, fix up the highlight
    type_lines(curlin, 1);      // by repainting current line
    tvhdln();                   // and rehome cursor
    ((vWindow *)_parent)->SetValue(M_Cut, 1, Sensitive);
    ((vWindow *)_parent)->SetValue(M_Copy, 1, Sensitive);
  }

// =========================>>> vTextEditor::AddToRangeRow <<<=================
  void vTextEditor::AddToRangeRow(long cnt)
  {
    /* add to mark range varaiable, update current line, then go down */

    if (cnt == 0)
      {
	ClearMarkRange();               /* no range now */
	return;
      }

    if (cnt > 0)
      {
	if (curlin + cnt > lastLine)    /* past end? */
	    cnt = lastLine - curlin + 1;
	if (mark.beg_lin == 0)  /* no lines yet */
	  {
	    mark.beg_lin = curlin;
	    mark.beg_chr = curchr;
	    mark.beg_col = col_pos(curchr,0);
	  }
	if (mark.end_lin == 0)
	  {
	    mark.end_lin = curlin + cnt - 1;
	  }
	else if (mark.beg_lin == curlin   // some of this line marked
	    && mark.end_lin != mark.beg_lin)
	  {
	    if (mark.end_lin == curlin + cnt
	      && col_pos(curchr,0) > mark.end_col)
	      {
		mark.beg_lin = curlin + cnt;
		mark.beg_col = mark.end_col;
		mark.beg_chr = mark.end_chr;
	      }
	    else
	      {
		mark.beg_col = col_pos(curchr,0);
		mark.beg_chr = GLine(curlin+cnt);       // Something legal
		mark.beg_lin = curlin + cnt;
	      }
	  }
	else
	  {
	    mark.end_lin += cnt;
	    mark.end_col = 1000;            /* BIG */
	    mark.end_chr = 0;               /* end of line */
	  }

	/* paint the lines to highlight */
	if (tvdlin+cnt-1 <= GetRows())
	  {
	    tvxy(1,tvy);        /* fix it up */
	    type_lines(curlin,(int)cnt);
	  }

	if (curlin == lastLine && cnt > 0)      /* down from last line? */
	  {
	    return;
	  }
	lineDown(cnt,0);

      }
    else if (cnt < 0)                   // up lines
      {
        if (curlin + cnt < 1)           // past start?
            cnt = -curlin;
        if (mark.end_lin == curlin && mark.beg_lin == curlin
	    && mark.end_lin != 0)
	  {
	    mark.end_col = mark.beg_col;
	    mark.end_chr = mark.beg_chr;
	    lineDown(cnt,0);
	    mark.beg_lin = curlin;
	    mark.beg_col = col_pos(curchr,0);
	    mark.beg_chr = curchr;
	  }
	else
	  {
	    if (mark.end_lin == 0)              //no lines yet
	      {
		mark.beg_lin = mark.end_lin = curlin;
		mark.end_chr = curchr;
		mark.end_col = col_pos(curchr,0);
		mark.beg_col = 1;
		mark.beg_chr = GLine(curlin);
	      }

	    if (mark.end_lin == curlin   // some of this line marked
		&& mark.end_lin != mark.beg_lin)
	      {
		if (mark.beg_lin == curlin + cnt
		    && col_pos(curchr,0) < mark.beg_col)
		  {
		    mark.end_lin = curlin + cnt;
		    mark.end_col = mark.beg_col;
		    mark.end_chr = mark.beg_chr;
		    lineDown(cnt,0);
		    mark.beg_col = col_pos(curchr,0);
		    mark.beg_chr = curchr;
		  }
		else
		  {
		    lineDown(cnt,0);
		    mark.end_col = col_pos(curchr,0);
		    mark.end_chr = GLine(curlin+cnt);   // Something legal
		    mark.end_lin = curlin;
		  }
	      }
	    else
	      {
		// now move up specified number of lines, no clrRange
		lineDown(cnt, 0);

		mark.beg_lin = curlin;
		mark.beg_chr = curchr;
		mark.beg_col = col_pos(curchr,0);
	      }
	  }

	/* paint the lines to highlight */
	tvxy(1,tvy);        /* fix it up */
	type_lines(curlin,(int)-cnt+1);
	tvhdln();
      }
    ((vWindow *)_parent)->SetValue(M_Cut, 1, Sensitive);
    ((vWindow *)_parent)->SetValue(M_Copy, 1, Sensitive);
  }


// =========================>>> vTextEditor::Redraw <<<=====================
  void vTextEditor::Redraw(int x, int y, int w, int h)
  {
    vTextCanvasPane::Redraw(x,y,w,h);
  }

// ======================>>> vTextEditor::ResizeText <<<====================
  void vTextEditor::ResizeText(const int rows, const int cols)
  {

    // now, if the curlin is on the new scrolled screen, we need to
    // repaint it so the cursor and highlight area show

    Verify();

    if (ddline >= GetRows())
      {
	HideTextCursor();
	setCursor = 1;
      }
    else
	setCursor = 0;

    setScrollBar();
  }

// #########################################################################
//
// screen update stuff - maps old see/tv to vTextCanvasPane methods
//
// #########################################################################

// =========================>>> vTextEditor::tvxy <<<=======================
  void vTextEditor::tvxy(int ix, int iy)
  {
    // tvxy - position cursor at position x,y 
    //          x=1 is left most column
    //          y=1 is top most line

    tvx = ix;
    tvy = iy;
    if (!state.echof)
	return;
    GotoRC(iy-1,ix-1);  // Convert to 0,0 based coords
  }

// =========================>>> vTextEditor::tvplin <<<=====================
  void vTextEditor::tvplin(long lineNum, BUFFPTR chrptr, int whole_line,
	int hibeg, int hiend)
  { /* tvplin - put line beginning at chrptr
	will only type what will fit on screen (using xout) */

    char tmp;
    int linlen, origx, need_hi_end;

    int hiStart = -1;
    int hiLast = -1;

    BUFFPTR i;

    char linout[MAX_LINE+1];

    need_hi_end = 0;

    last_col_out = linptr = 0;
    origx = xoutcm;		/* save x so can get true linelen */
    for (i =  chrptr; !IsEndLine(GCh(i)) && xoutcm < MAX_LINE; ++i)
      {
	/* xoutcm has current column.  If a hilite goes here,
	 *add to the linout array as 0x01 or 0x02.
	 */
	if (xoutcm == hibeg)
	  {
	    hiStart = linptr;           // where highlight starts
	    need_hi_end = 1;            /* will need a follow */
	  }
	if (xoutcm == hiend)
	  {
	    hiLast = linptr;            // where highlight ends
	    need_hi_end = 0;            /* don't need a follow now */
	  }
	if (GCh(i) < ' ' && GCh(i) >= 0)        /* control character? */
	  {
	    if (GCh(i) == '\t')
	      {
		if (gState.tabspc > 0)
		  {
		    do
		      {
			linout[linptr++] = ' '; /* replace with blanks */
			++xoutcm;
		      }
		    while ( ((xoutcm-1) % gState.tabspc) != 0);
		  }
		else
		  {
		    linout[linptr++] = '^';
		    linout[linptr++] = 'I';
		    xoutcm += 2;
		    linout[linptr++] = '*';
		    ++xoutcm;
		  }
		continue;
	      }
	    else                /*  other control character */
	      {
		linout[linptr++] = '^';
		++xoutcm;
		if (xoutcm == GetCols() && !(IsEndLine(GCh(i))) )
		    continue;
		tmp = GCh(i) + '@';
		linout[linptr++] = tmp;

	      }
	  } /*# end if control character */
	else
	  {
	    linout[linptr++] = GCh(i);
	  }
	++xoutcm;
      }

    if (need_hi_end)
      {
	hiLast = linptr;
	need_hi_end = 0;        /* don't need a follow now */
      }

    linout[linptr] = 0;		// terminate the line

    int lineStart =  0;

    // This whole line stuff is left over. I don't know exactly what it
    // did, but it breaks repainting when the margin is shifted, so just
    // ignore the whole_line value...  BEW: 10/1/98

//@    if (whole_line)		/* write whole line */
      {
	last_col_out = linlen = mint(GetCols(),linptr-leftmg+1);
	lineStart = leftmg - 1;
      }

    if (linlen > 0)
	paintLine(linout, lineStart, hiStart, hiLast, lineNum);

  }

// ======================>>> vTextEditor::paintLine <<<=====================
  void vTextEditor::paintLine(char* linout, int lineStart,
        int hiStart, int hiLast, long lineNum)
  {
    // paint a line. This can be overridden (e.g., for syntax highlighting).
    // linout: the line to output with tabs converted to spaces, etc.
    // lineStart: where to begin printing the line (for hoiz. scrolling)
    // hiStart, hiLast: reverse text attribute
    // lineNum: the real line number in the buffer this is. This is unused
    //          normally, but can be used for syntax highlighting to get
    //          surrounding context.

    int linlen = strlen(linout);
    if (linlen > 0)             // only draw if there!
      {
	char tmpChr;                // to hold char

	if (hiStart < lineStart && hiLast < lineStart)  // no highlight
	  {
	    DrawText(&linout[lineStart]);       // simple case
	  }
	else                    // has highlighting
	  {
	    if (hiStart > lineStart)    // highlight from beginning
	      {
		tmpChr = linout[hiStart];       // remember char
		linout[hiStart] = 0;
		DrawText(&linout[lineStart]);  // normal part
		linout[hiStart] = tmpChr;       // replace
	      }

	    tmpChr = linout[hiLast];    // remember char
	    linout[hiLast] = 0;         // make end of string
	    DrawAttrText(&linout[hiStart],getHighlight()); // highlight part
	    linout[hiLast] = tmpChr;    // replace

	    if (linlen > hiLast)        // rest of line
		DrawText(&linout[hiLast]);
	  }
      }
  }

// ====================>>> vTextEditor::highLightCurLine <<<======================
    void vTextEditor::highLightCurLine()
    {
      // Highlight current line using reverse instead of highlight
      ChrAttr old = getHighlight();
      setHighlight(ChReverse);
      type_lines(curlin, 1, 1);		// by repainting current line
      tvhdln();				// and rehome cursor
      setHighlight(old);
    }

// ====================>>> vTextEditor::type_lines <<<======================
  void vTextEditor::type_lines(long ibeg, int icnt, int wholeLine)
  { /* type_lines - type icnt lines starting at lines[ibeg]
		no cr/lf on the last line */

    long i, lim;
    int hibeg, hiend;
    BUFFPTR start;

    if (!state.echof)
	return;
    if (ibeg < 1 || ibeg > lastLine)
	return;
    xoutcm = tvx;
    lim = ibeg+icnt-1;

    for (i = ibeg ; i <= lim && i <= lastLine ; ++i)
      {
	/* simple check for whole line highlighting for now */
	if (wholeLine)
	  {
	    hibeg = 1;
	    hiend = 1000;
	  }
	else if (i == mark.beg_lin)
	  {
	    hibeg = mark.beg_col;
	    if (i == mark.end_lin)
		hiend = mark.end_col;
	    else
		hiend = 1000;
	  }
	else if (i >= mark.beg_lin && i <= mark.end_lin)
	  {
	    hibeg = 1;
	    if (i == mark.end_lin)
		hiend = mark.end_col;
	    else
		hiend = 1000;
	  }
	else
	  {
	    hibeg = hiend = -1;
	  }

	start = GLine(i);
	tvplin(i,start,1,hibeg, hiend); /* type out a wole line */
	xoutcm = 1;
	if (last_col_out < GetCols())
	    tvelin();   /* erase rest of line */
	if ( i != lim )
	    tvxy(1,++tvy);
      }
  }

// =========================>>> vTextEditor::Verify <<<=====================
  void vTextEditor::Verify(void)
  { // Verify - rewrite the screen or type current line with cursor

    int xf, old_ddline;

    if (!state.echof)
	return;

    if (lastLineBF() < 1)
      {
	tvclr();
	return;
      }
    old_ddline = ddline;
    ddline = dsplin;
    newscr();
    xf = findX();
    tvxy(xf,tvy);       /* reset cursor to current position */
    ddline = old_ddline;
    ChangeLoc(curlin,col_pos(curchr,0));
    setScrollBar();
  }

// =====================>>> vTextEditor::tvbotb <<<=========================
  void vTextEditor::tvbotb(int n)
  {  // tvbotb - make n blank lines at the bottom of the screen
 
    if (!state.echof)
	return;
    if (n >= GetRows())
      {
	tvclr();
      }
    else 
      {
	tvxy(1,GetRows());      /* go to real last line */

	ScrollText(n);
	int j = GetRows()-n+1;  /* home to virtual last line */
	tvxy(1,j);      /* position at first new blank line */
      }
  }
 
// ==========================>>> vTextEditor::tvclr  <<<==========================
  void vTextEditor::tvclr(void)
  {  // tvclr - clear the entire screen and home

    if (!state.echof)
	return;
    Clear();
    tvxy(1,1);
    tvx = tvy = 1;
  }
 
// ========================>>> vTextEditor::tvelin <<<======================
  void vTextEditor::tvelin(void)
  {  // tvelin - erase the rest of the current line 
 
    if (!state.echof)
	return;
    int r,c;
    GetRC(r,c);
    ClearRow(r, c);
  }

// ========================>>> vTextEditor::tvescr <<<======================
  void vTextEditor::tvescr(void)
  {  // tvelin - erase from cursor to end of screen

    if (!state.echof)
	return;
    ClearToEnd(tvy-1, tvx - 1);
  }

// ==========================>>> vTextEditor::tvtopb <<<====================
  void vTextEditor::tvtopb(int n)
  {  // tvtopb - create n blank lines at the top of the screen
 
    if (!state.echof)
	return;

    tvxy(1,1);          /* home first */
    if ( n >= GetRows())
      {
	tvescr();       /* simply erase the screen */
      }
    else
      {
	ScrollText(-n);
      }
    tvxy(1,1);          /* home first */
  }
 
// =======================>>> vTextEditor::bufferBottom <<<=================
  void vTextEditor::bufferBottom(void)
  { // bufferBottom - move cursor to bottom of current buffer

    if (lastLineBF() < 1)
	return;

    ClearMarkRange();		/* no range now */
    wasColCmd = 0;

    curlin = lastLine;		/* the last real line of text */
    curchr = GLine(curlin);     /* the first char of that line */
    lineEnd();			/* goto end of the line */
    newscr();			// update the screen
  }

// #########################################################################
//
// char methods
//
// #########################################################################

// =======================>>> vTextEditor::charDelete <<<===================
  int vTextEditor::charDelete(long cnt, bool doUndo, bool removeRange)
  {  // charDelete - delete next cnt characters

    static char chdel;
    int abscnt,newx;
    int i;

    if (state.readOnly || lastLineBF() < 1)
	return 0;

    checkIfScrolled();
    if (removeRange && RemoveMarkRange(doUndo))      /* there was a range to kill */
	return 1;
    ClearMarkRange();		/* no range now */
    wasColCmd = 0;

    abscnt = (cnt > 0) ? cnt : (-cnt);  /* remember absolute value of cnt */

    state.changes += abscnt;	/* track changes */

    if (cnt < 0)
	charRight(cnt,0);

    if (doUndo)
      {
	long cntCurlin = curlin;	// copy of current line
	BUFFPTR cntCurchr = curchr;	// copy of current char

	// find out how many lines are involved, then save that many...

  	bool partialLine = false;
	bool lastDeleted = false;	// end of line in last line deleted?
	int cntLines = 1;
	for (i = 0 ; i < abscnt ; ++i)
	  {
	    if (IsEndLine(GCh(cntCurchr))) /* at end of line */
	      {
		if (cntCurlin >= lastLine)
		  {
		    break;              /* don't go beyond end! */
		  }
		lastDeleted = true;	// have reached end of line
		++cntLines;		// how many lines we've seen
		++cntCurlin;
		cntCurchr = GLine(cntCurlin);
	      }
	    else
	      {
		lastDeleted = false;	// haven't deleted whole line
		++cntCurchr;
	      }
	  }

	// We need to handle partial lines on undo. If we delete just
	// part of the first line, the first line will be combined
	// with the next line. Same is true if we have a partial last
	// line - it will be combined with the line above it. In
	// either case, we need to delete the line that remains at
	// curlin after the delete before restoring the unkill buffer.

	partialLine = curchr != GLine(curlin);	// partial first line

	if (curchr == GLine(curlin) && lastDeleted && cntLines == 2)
	    partialLine = true;	// exactly one line (but count is 2!)

	if (!lastDeleted)	// partial last line
	    partialLine = true;

	if (!SaveKillLine(cntLines, partialLine))
	  {
		//@@ abort if can't save all. no-op for now.
	  }
      }

    for (i = 0 ; i < abscnt ; ++i)
      {
	BUFFPTR newCurChr;

	chdel = GCh(curchr); /* remember the char we are deleting */

	newCurChr = deleteCharBF(curchr,curlin);
	if (newCurChr == 0)		// deleteCharBF might fail!
	  {
	    if (cnt < 0)
		charRight(-cnt,0);	// go back
	    return 0;
	  }
	else
	    curchr = newCurChr;

	if (!IsEndLine(chdel))
	  {
	    if (i + 1 == abscnt || IsEndLine(GCh(curchr)))
	      {
		if (abscnt > 1)
		    Verify();		// need to verify for big deletes
		else
		    retypeCurlin(1);	// just retype current line
	      }
	  }
	else
	  {
	    lastLine = lastLineBF();
	    if (tvdlin < dsplin)        // not at end
	      {
		tvescr();               /* erase rest of screen */
		tvxy(1,tvy);            /* fix it up */
		type_lines(curlin,mint((int)(GetRows() - tvdlin+1),
		(int)(lastLine - curlin))  );
		newx = findX();         /* where cursor will go */
		tvxy(newx,tvy);         /* reposition cursor */
	      }
	    else                        /* at end of buffer */
		Verify(); 
	  }
      }

    ChangeLoc(curlin,col_pos(curchr,0));
    return 1;
  }

//=====================>>> vTextEditor::charFoldCase <<<====================
  int vTextEditor::charFoldCase(long cnt)
  {
        /* fold from upper to lower or lower to upper case if letter */
	int ni;
	char fchr;

	if (state.readOnly)
	    return 0;

	ClearMarkRange();   /* no range now */
	wasColCmd = 0;

	for (ni = 0 ; ni < cnt ; ++ni)
	  {
	    fchr = GCh(curchr);     /* get current character */
	    if (fchr >= 'a' && fchr <= 'z')
		fchr = cupper(fchr);
	    else if (fchr >= 'A' && fchr <= 'Z')
		fchr = clower(fchr);
	    if (IsEndLine(fchr))
		charRight((long)1,1);
	    else
	      {
		if (!charDelete((long) 1, false))	// delete cur char
		    return 0;
		if (!charInsert((int)fchr,false))         /* and put back */
		    return 0;
	      }
	  }
	return 1;
      }

// ====================>>> vTextEditor::charInsert <<<======================
  int vTextEditor::charInsert(int ival, bool doUndo)
  {
    int xf;
    int chr;

    if (state.readOnly)
	return 0;
    checkIfScrolled();

    if (doUndo)
      {
	// for now, we don't undo inserts
	undoOff("Undo");
      }

    if (gState.delOnInsert)
	RemoveMarkRange(doUndo);		/* no range now */

    wasColCmd = 0;

    chr = ival;					/* use the passed value */

    ++state.changes;				/* count changes */

    if (lastLineBF() == 0)			// First actual insert!
      {
	char ln1[4];
	resetBuff();
	if (chr == '\n')        // this effecitvely is two lines!
	  {
	    ln1[0] = 0;
	    addLine(ln1);
	    addLine(ln1);
	    tvdlin = 2;
	    displayBuff(1);             // get a proper display
	  }
	else
	  {
	    ln1[0] = chr;
	    ln1[1] = 0;
	    addLine(ln1);
	    if (!_lines[1])
		return 0;
	    curlin = 1;
	    curchr = _lines[1] + 1;
	    tvdlin = 1;
	    displayBuff(1);             // get a proper display
	    lineEnd();          // move to end of the line
	  }
	setScrollBar();
	return 1;
      }

    if (!state.ins_mode)                        // overtype mode?
      {
	if (!charDelete(1,false))             // delete nextchar
	    return 0;
      }

    if (chr == '\r')
	chr = '\n';                     // this is our convention

    if (chr == 0)
	chr = ' ';

    if (!insertCharBF(chr, curchr, curlin))
	return 0;
    lastLine = lastLineBF();            // update this one

    if (chr != '\n')
      {
	retypeCurlin(0);                        // just retype whole line
      }
    else
      {
	long dummy1; int dummy2;
	FindDispLine(dummy1,dummy2);
	if (tvdlin < dsplin)    // not at end
	  {
	    tvescr();               /* erase rest of screen */
	    tvxy(1,tvy);    /* fix it up */
	    type_lines(curlin,mint((int)(GetRows() - tvdlin + 1),
	    (int)(lastLine - curlin))  );
	  }
	else                        /* at end of buffer */
	    Verify();
	setScrollBar();
	tvhdln();   /* home to display line */
	xf = findX();
	tvxy(xf,tvy);       /* reset cursor to current position */
      }

    ChangeLoc(curlin,col_pos(curchr,0));

    return 1;
  }

// ====================>>> vTextEditor::retypeCurlin  <<<======================
  void vTextEditor::retypeCurlin(int eraseLine)
  {
    if (!state.echof)
	return;
    if (eraseLine)
	tvelin();
    tvxy(1,tvy);
    tvtyln(curlin,GLine(curlin), curchr == GLine(curlin));
    tvhdln();                   // home to display line
    int xf = findX();
    tvxy(xf,tvy);               // reset cursor to current position
  }

// ====================>>> vTextEditor::charRight  <<<======================
  int vTextEditor::charRight(long cnt, int clear_mark)
  {  // charRight: move cursor right cnt characters
     // newlines count as one character

    long change;
    int i, rv;

    if (lastLineBF() < 1)
	return 0;
    if (cnt == 0)
       return 1;

    checkIfScrolled();
    if (clear_mark)
	ClearMarkRange();               /* no range now */

    wasColCmd = 0;

    rv = 1;			/* assume success */
    change = 0;			/* no change yet */
    if (cnt > 0)		/* moving right */
      {
	for (i = 1 ; i <= cnt ; ++i)
	  {
	    if (IsEndLine(GCh(curchr))) /* at end of line */
	      {
		if (curlin >= lastLine)
		  {
		    rv = 0;
		    break;              /* don't go beyond end! */
		  }
		++curlin;
		++change;               /* we've gone down one line */
		curchr = GLine(curlin);
	      }
	    else
		++curchr;
	  }
      }
    else if (cnt < 0)           /* moving left */
      {
	cnt = (-cnt);           /* fix this */
	for (i = 1 ; i <= cnt ; ++i)
	  {
	    if (curchr == GLine(curlin)) /* at front */
	      {
		if (curlin > 1) /* can only go up on >= line 2 */
		  {
		    --curlin;
		    --change;
		    for (curchr = GLine(curlin) ;
			!IsEndLine(GCh(curchr)) ;
		    ++curchr)
		    ;       /* bump curchr to end of the line */
		  }
		else
		  {
		    rv = 0;
		    break;              /* don't go beyond front! */
		  }
	      }
	    else
		--curchr;
	  }
      }
    if (change != 0)            /* don't do unnecessary change */
	update(change);
    if (clear_mark)
	ChangeLoc(curlin,col_pos(curchr,0));
    tvhdln();
    return rv;
  }

// #########################################################################
//
// line methods
//
// #########################################################################

// ===================>>> vTextEditor::lineBeginning <<<====================
  void vTextEditor::lineBeginning()
  {  /* lineBeginning - move cursor to beginning of current line */

    int xf;

    if (lastLineBF() < 1)
	return;

    ClearMarkRange();           /* no range now */
    wasColCmd = 0;

    curchr = GLine(curlin);     /* point to current character */
    xf = findX();       /* this line needed to make the next */
    /* call eval order independent, if you wondered */
    tvxy(xf,tvy);       /* and move cursor */
    ChangeLoc(curlin,col_pos(curchr,0));
  }

// =====================>>> vTextEditor::lineDelete <<<=====================
  void vTextEditor::lineDelete(long cnt, bool doUndo)
  { // lineDelete - delete cnt lines

    int ityp;

    long line_1, last_line;     /* range of lines to kill */
    long istrt;

    if (state.readOnly || lastLineBF() < 1)
	return;
    ClearMarkRange();           /* no range now */
    wasColCmd = 0;

    if (cnt == 0)
	return;

    if (cnt < 0)                /* negative kill */
      {
	cnt = minl(-cnt,curlin-1);      /* all upwards? */
	lineDownBeg(-cnt,0);            /* go up that far */
      }

    if (doUndo)
      {
	if (!SaveKillLine(cnt))
	  {
	    //@@ abort if can't save all. no-op for now.
	  }
      }

    if (cnt != 0)
      {
	range(cnt,&line_1,&last_line);  /* calculate the line numbers to kill */

	curlin = line_1;                /* remember new current line */


	++state.changes;                        /* count changes */
	lastLine = deleteLinesBF(line_1,last_line);     /* delete the lines from the buffer(s) */

	if (lastLine < curlin)
	    curlin = lastLine;          /* don't go past end */

	curchr = GLine(curlin); /* remember new current character */

	if (cnt >= 0 && curlin+(GetRows() - tvdlin) <= lastLine &&
          tvdlin < GetRows())   /* killing down */
          {
	    tvxy(1,tvy);        /* get to start of line */
	    ityp = (int) minl((long)(GetRows()-tvdlin+1),lastLine - curlin + 1);
	    tvescr();   /* erase the screen */
	    istrt = curlin;
	    type_lines(istrt, ityp);
	    tvhdln();   /* home to display line */
	  }
	else 
	    Verify();                           /* kill up, retype screen */
      }
    ChangeLoc(curlin,col_pos(curchr,0));
  }

// =====================>>> vTextEditor::lineDeleteFront  <<<===============
  int vTextEditor::lineDeleteFront(bool doUndo)
  { // lineDeleteFront - delete from cursor to beginning of line

    int chrs;

    if (state.readOnly || lastLineBF() < 1)
	return 0;
    ClearMarkRange();   /* no range now */
    wasColCmd = 0;

    chrs = curchr - GLine(curlin);      /* how much to delete */
    if (chrs > 0)
	return charDelete((long)(-chrs),doUndo);	// won't cause a combine, so don't worry
    return 1;
  }

// =====================>>> vTextEditor::lineDeleteToEnd  <<<===============
  int vTextEditor::lineDeleteToEnd(bool doUndo)
  { // lineDeleteToEnd:
    //       kill the rest of the line, not including cursor and endLine

    int chrs;
    BUFFPTR i;

    if (state.readOnly || lastLineBF() < 1)
	return 0;
    ClearMarkRange();		// no range now
    wasColCmd = 0;

    chrs = 0;
    for (i = curchr; !IsEndLine(GCh(i)) ; ++i)
	++chrs;			// count how much to delete
    if (chrs > 0)
        return charDelete((long) chrs, doUndo);	// won't cause combine
    return 1;
  }

// ========================>>> vTextEditor::lineDown <<<====================
  int vTextEditor::lineDown(long cnt, int clrRange)
  { /* lineDown - move down in column */

    int curcol,l,oldef,needns,ic,ix,lim,rv;

    if (lastLineBF() < 1)
        return 0;
    if (clrRange)
	ClearMarkRange();           /* no range now */

    oldef = state.echof;
    needns = 0;
    if (leftmg > 1)		/* handle right virtual screen different */
      {
	needns = 1;
      }

    if (wasColCmd)      // going up/down columnwise
        curcol = oldcol;
    else
	oldcol = curcol = col_pos(curchr,1);    /* calculate the current column */

    rv = lineDownBeg(cnt,0,clrRange);    /* go down given lines, update screen */

    state.echof = 0;

    if (curlin >= 1 && curlin <= lastLine && curcol > 1)        /* not at ends? */
      {
	l = lineLenBF(curlin);
	lim = mint(curcol-1,l);
	for (ix = 0, ic = col_pos(curchr,1) ; ix < lim && ic < curcol ;
	    ++ix, ic = col_pos(curchr,1))
	  ;
	  charRight((long)ix,clrRange);
      }

    state.echof = oldef;
    if (needns)                 /* needed new screen */
      {
	Verify();
      }
    else
	tvhdln();
    ChangeLoc(curlin,col_pos(curchr,0));
    wasColCmd = 1;
    return rv;
  }

// ====================>>> vTextEditor::lineDownBeg <<<=====================
  int vTextEditor::lineDownBeg(long cnt, int notify, int clrRange)
  { /* lineDownBeg - move dot down cnt lines */

    long oldlin,change;

    if (lastLineBF() < 1)
	return 0;
    if (clrRange)
	ClearMarkRange();           /* no range now */
    wasColCmd = 0;

    if (curlin == lastLine && cnt > 0)  /* down from last line? */
      {
	lineEnd();
	return 0;               /* make loops fail */
      }
    oldlin = curlin;            /* remember where we started from */
    if (curlin + cnt < 1)
	curlin = 1;
    else if (curlin + cnt > lastLine)
        curlin = lastLine;
    else
        curlin = curlin + cnt;
    curchr = GLine(curlin);     /* point to the current character */
    change = curlin-oldlin;     /* calculate how many lines changed */
    update(change);             /* update the screen */
    if (notify)
        ChangeLoc(curlin,col_pos(curchr,0));
    return change != 0;
  }

// =====================>>> vTextEditor::lineEnd <<<========================
  void vTextEditor::lineEnd(int clrRange)
  { // lineEnd - move cursor to end of the line

    int knt;
    BUFFPTR i;

    if (lastLineBF() < 1)
        return;
    if (clrRange)
        ClearMarkRange();           /* no range now */
    wasColCmd = 0;

    knt = 0;
    for (i = curchr ; !IsEndLine(GCh(i)) ; ++i) /* find end of line */
        ++knt;
    charRight((long)knt,clrRange);             /* move to end of line */
    ChangeLoc(curlin,col_pos(curchr,0));
  }

// ======================>>> vTextEditor::lineOpen <<<======================
  int vTextEditor::lineOpen(long cnt)
  {  // lineOpen - open a new line

    int i;
    long pcnt;


    if (state.readOnly || lastLineBF() < 1)
        return 0;

    undoOff("Undo");
    pcnt = cnt >= 0 ? cnt : (-cnt);     /* only allow positive opens */

    for (i=1; i<=pcnt; ++i)
      {
        if (!charInsert('\n'))          /* insert right number of newlines */
            return 0;
      }

    lineDownBeg(-pcnt,0);               /* and goto beginning of the opened line */

    lineEnd();
    return 1;
  }

// ======================>>> vTextEditor::lineGoto <<<======================
  int vTextEditor::lineGoto(long cnt)
  { // lineGoto: move cursor to line cnt

    if (cnt < 0 || lastLineBF() < 1)
        return 0;

    undoOff("Undo");

    ClearMarkRange();           /* no range now */
    wasColCmd = 0;

    curlin = maxl(minl(cnt,lastLine),(long) 1); /* move to lines */
    curchr = GLine(curlin);     /* point to the current character */
    Verify();
    return 1;
  }

// =====================>>> vTextEditor::lineAutoFill <<<===================
  void vTextEditor::lineAutoFill(void)
  {
    BUFFPTR bx, startx;
    int lines, old_ef, old_y, ityp, go_right, cp, chr;
    long start_line, istrt;

    if (state.readOnly || lastLineBF() < 1)
        return;

    if (gState.wraplm < 1)
        return;                 /* no auto-wrap going on */

    /*  1. Check if current line needs to be wrapped */

    startx = GLine(curlin);     /* first char of line */
    bx = lastBX(curlin);        /* index of last char of line */
    if (startx == bx)
        return;                 /* blank line, can't need tidy */
    if (col_pos(bx,0) <= gState.wraplm) /* is last char beyond limit? */
        return;                 /* line doesn't need tidy */

    /*  2. If it does, figure out where the cursor is. */

    go_right = curchr - startx; /* where CURRENT char is */
    start_line = curlin;
    old_y = tvdlin;             /* original line */

    /*  3. Echo off. */

    old_ef = state.echof;
    state.echof = 0;

    /*  4. Tidy down to PP or blank line or line beginning with white */

    for (lines = 1 ; lines < 50 && curlin <= lastLine ;
         ++lines )
      {
        lineFill((long) 1);     /* fix current line */
        chr = GCh(curchr);
        if (chr == '.' || chr == ' ' || chr == '\t' || IsEndLine(chr))
            break;
      }

    /*  5. Restore echo */

    state.echof = old_ef;

    /*  6. Return to current line, making sure it stays where it was
           on the screen (or shift by one if it gets moved),
           and repaint the rest of the screen.
    */

    curlin = start_line;
    curchr = GLine(curlin);
    tvdlin = old_y;

    if (state.echof)
      {
        tvxy(1,old_y);
        istrt = curlin;
        ityp =(int) minl((long)(GetRows()-tvdlin+1),
                         lastLine - curlin + 1);
        type_lines(istrt, ityp);
      }

    tvhdln();   /* home to display line */

    for ( ; ; )
      {
        bx = lastBX(curlin);    /* see if still on this line */
        cp = col_pos(bx,0);
        if (go_right <= cp)     /* on this line */
          {
            curchr = GLine(curlin) + go_right;
            break;
          }
        go_right -= cp; /* next line */
        if (!lineDownBeg((long) 1,0))
            break;
      }
    tvhdln();   /* home to display line */
    ChangeLoc(curlin,col_pos(curchr,0));
  }

// =====================>>> vTextEditor::lineFill <<<=======================
  int vTextEditor::lineFill(long count)
  {  // lineFill - fill lines to current margin

    char curline[20];           /* first part of current line */

    int oldef, i ;
    BUFFPTR linbeg, old_cc;
    int retval;

    if (state.readOnly || lastLineBF() < 1 || gState.wraplm < 1)
        return 0;                       /* no auto-wrap going on */

    undoOff("Undo");

    ClearMarkRange();           /* no range now */
    wasColCmd = 0;

    retval = 1;
    oldef = state.echof;
    if (count > 1)
        state.echof = 0;
    if (gState.wraplm <= 1 || curlin > lastLine)
        goto l900;              /* work only if wrap limit turned on */

    for (i = 1 ; i <= count ; ++i)
      {
        lineBeginning();                /* start at beginning of line */
        if (curlin >= lastLine)
            goto l900;

        // don't fill leading space, cr, period,  tab, 
	// latex keyword, or some HTML

        getCurLineFront(curline,curlin);     /* current line */
        if (!(*curline) || isSpecial(curline))
          {
	    lineDownBeg((long) 1,0);
            continue;           /* skip special lines */
          }

        while (curlin < lastLine)
          {
            if (IsEndLine(GCh(curchr)))
              {
                if (tvx+leftmg-1 < gState.wraplm)        /* combine lines! */
                  {
                    /* pt to first char of next line */
                    linbeg = GLine(curlin+1);
		    getCurLineFront(curline,curlin+1);	// get the front
		    if (IsEndLine(GCh(linbeg)) || isSpecial(curline))
                      {
                        lineDownBeg((long) 1);
                        break;  /* no more combining */
                      }
                    if (! Fill1(1,32))
                        goto l990;
                    /* fall thru to test for wrap */
                  }
                else
                  {
                    lineDownBeg((long) 1);      /* no more combining on line */
                    break;
                  }
              }

            old_cc = curchr;
            wordRight((long) 1);
            if (tvx+leftmg-1 > gState.wraplm)
              {
                if (tvx+leftmg-2 == gState.wraplm && IsEndLine(GCh(curchr)) )
                  {
                    lineDownBeg((long) 1);
                    break;
                  }
                else if ((tvx+leftmg-2 == gState.wraplm ||
                    tvx+leftmg-3 == gState.wraplm) &&
                    (GCh(curchr-1) == ' ' || GCh(curchr-1) == '\t'))
                  {
                    if (!Fill1(-1,'\n'))
                        goto l990;
                  }
                else if (GCh(old_cc-1) == ' ' ||
                    GCh(old_cc-1) == '\t')
                  {
                    curchr = old_cc;
                    if (!Fill1(-1,'\n'))
                        goto l990;
                  }
                else if (GCh(old_cc-2) == ' ' ||
                    GCh(old_cc-2) == '\t')
                  {
                    curchr = old_cc-1;
                    if (!Fill1(-1,'\n'))
                        goto l990;
                  }
                else if (IsEndLine(GCh(curchr)) )
                  {
                    lineDownBeg((long) 1);
                    break;
                  }
                else if (GCh(curchr-1) == ' ' ||
                    GCh(curchr-1) == '\t')
                  {
                    if (!Fill1(-1,'\n'))
                        goto l990;
                  }
                else
                  {
                    for (wordRight((long) -1) ;  /* go back a word, then break */
                        (GCh(curchr-1) != ' ' &&  GCh(curchr-1) != '\t'
                        && !IsEndLine(GCh(curchr-1)) ) ;
                        wordRight((long) -1))
                      {
                        /* this line can't be filled - stop now */
                        if (curchr == GLine(curlin))
                            goto l990;
                      }
                    charInsert('\n',false);
                  }
                break;
              }

          } /* end of for (;;) */

      } /*# end of the for (count) */
l900:
    state.echof = oldef;
    if (oldef && count > 1)
        Verify();

    ChangeLoc(curlin,col_pos(curchr,0));
    return retval;

l990:                           /* failure return */
    retval = 0;
    goto l900;
  }

//===========================>>> vTextEditor::isSpecial <<<=====================
  bool vTextEditor::isSpecial(char* curline)
  {

    static char* special[] =           /* special keywords */
      {                         /* that shouldn't cause a wrap */
	"<a ",
	"<applet",
	"<area",
	"<base",
	"<block",
	"<body",
	"<br",
	"<center",
	"<cite",
	"<code",
	"<col",
	"<dd",
	"<dir",
	"<div",
	"<dl",
	"<dt",
	"<form",
	"<frame",
	"<h1",
	"<h2",
	"<h3",
	"<h4",
	"<h5",
	"<h6",
	"<head",
	"<hr",
	"<html",
	"<iframe",
	"<img",
	"<input",
	"<li",
	"<map",
	"<menu",
	"<meta",
	"<no",
	"<obj",
	"<ol",
	"<opt",
	"<p>",
	"<p ",
	"<pre",
	"<samp",
	"<table",
	"<tbody",
	"<td",
	"<th",
	"<title",
	"<tr",
	"<ul",
        ""
      };

    if (*curline == ' ' || *curline == '\t' || *curline == '.')
	return true;
    if (*curline == '<')	// html?

    for (int iy = 0 ; *special[iy] ; ++iy)      /* search keyword list */
      {
	if (strstr(curline,special[iy]) == curline)
	    return true;
      }
    return false;
  }

//===========================>>> vTextEditor::formatC <<<=====================
 int vTextEditor::formatC(long count)
  {
    /* format C code according to Bruce's style */

    int oldef, i, ij, ik, spaces, tabs, prev_key;
    long prev_lnum;

#define MAX_PREV 100
    char prev_line[MAX_PREV];
    char prev_prev_line[MAX_PREV];

    oldef = state.echof;
    if ( curlin >= _nextLine-1)	/* can't do last line */
	return 0;

    if (count > 1)
	state.echof = 0;

    for (i = 1 ; i <= count ; ++i, lineDownBeg((long)1))
      {
	if ((prev_lnum = get_prev(prev_line,curlin)) <= 0)	/* get the previous line */
	    continue;			/* handle 1st line of file! */

	(void) get_prev(prev_prev_line,prev_lnum);	/* and the line before it */

	if (*prev_line != ' ' && *prev_line != '\t')	/* funny line */
	  {
	    lineDownBeg((long)1);
	    break;			/* give up */
	  }

	/* count previous tabs/spaces */
	tabs = spaces = 0;
	for (ij = 0 ; ij < 38 ; ++ij)
	  {
	    if (prev_line[ij] == '\t')
	      {
		spaces = 0;		/* skip embedded spaces */
		++tabs;
	      }
	    else if (prev_line[ij] == ' ')
		++spaces;
	    else
		break;			/* break on first non tab non blank */
	  }
	if (tabs == 0 && spaces <= 2)
	  {
	    lineDownBeg((long) 1);
	    break;			/* give up on function heads */
	  }

	/* now process current line */
	lineBeginning();		/* start at beginning of line */

	if (curlin >= _nextLine-1)
	    break;		/* done */

	/* don't fix blank lines */

	if (GCh(curchr) != ' ' && GCh(curchr) != '\t' )
	  {
	    lineDownBeg((long) 1);
	    break;		/* break on line I don't know about */
	  }

	while (GCh(curchr) == ' ' || GCh(curchr) == '\t')
	    charDelete((long)1,false);		/* delete them */

	if (GCh(curchr) == '\n')
	    continue;		/* skip blank lines */

	/* determine spacing of current line based on prev line */

	prev_key = 0;
	if (is_key_word(&prev_line[ij],1))	/* a kew word! */
	  {
	    prev_key = 1;			/* last line was key */
	    if (GCh(curchr) == '{')
	      {
		spaces += gState.braceIndent;
	      }
	    else
		spaces += 4;
	  }
	else if (prev_line[ij] == '{')
	  {
	    spaces += (4-gState.braceIndent);
	  }
	else if (prev_line[ij] == '}')
	  {
	    // handle } else { K&R
	    if (prev_line[ij+1] && has_key(&prev_line[ij+2],"else"))
		spaces += 4;
	    else
		spaces -= gState.braceIndent;
	  }

	if (has_key((char*)curchr, "else"))
	  {
	    if (prev_line[ij] != '}')
		spaces -= 4;
	  }
	else if (has_key((char *)curchr, "case") ||
	  has_key((char*)curchr, "default:"))
	  {
	    if (prev_line[ij] != '{')
		spaces -= 4;
	  }
	else if (!prev_key && prev_line[ij] != '{' && 
	  prev_line[ij] != '}' && is_key_word(prev_prev_line,0))
	  {
	    // could handle K&R here...
	    // check for '{' in prev_prev_line
	    // this is fairly simple minded - but then the whole
	    // beautifier is. It will fail if there is a {
            // in a string in the control statement.
	    for (int ib = 0 ; prev_prev_line[ib] ; ++ib)
	      {
		char curChr = prev_prev_line[ib];	// for convenience
		if (curChr == '/' &&
		    (prev_prev_line[ib+1] == '/' || prev_prev_line[ib+1] == '*'))
		    break;
		if (curChr == '{' && prev_prev_line[ib+1] != '\'')
		  {
		    spaces += 4;	// will undo on break;
		    break;
		  }
	      }

	    spaces -= 4;
	  }

	/* don't else next one, works with last else if */
	if (GCh(curchr) == '}')	/* } */
	    spaces -= (4-gState.braceIndent);

	/* fix tab/space counts */
	if (spaces < 0)
	  {
	    if (--tabs < 0)
		tabs = 0;
	    spaces = 8 + spaces;
	  }
	while (spaces >= 8)
	  {
	    ++tabs;
	    spaces -= 8;
	  }

	for (ik = 0 ; ik < tabs ; ++ik)
	    charInsert('\t',false);
	for (ik = 0 ; ik < spaces ; ++ik)
	    charInsert(' ',false);

      }	/* end of main for loop */

    state.echof = oldef;
    if (oldef && count > 1)
	Verify();

    return 1;

  }

// =============================>>> vTextEditor::has_key <<<=======================
  int vTextEditor::has_key(char *buff_ptr, char *key)
  {
    while (*key)
      {
	if (*buff_ptr++ != *key++)
	    return 0;
      }
    return 1;	/* buff_ptr matches key up to key's EOS */

  }

// ==========================>>> vTextEditor::is_key_word <<<====================== 
  int vTextEditor::is_key_word(char *bf, int case_def)
  {
    //char *strstr();

    while (*bf == ' ' || *bf == '\t')	/* skip leading white */
	++bf;
    if (strstr(bf,"if(") == bf)
	return 1;
    if (strstr(bf,"if (") == bf)
	return 1;
    if (strstr(bf,"for(") == bf)
	return 1;
    if (strstr(bf,"for (") == bf)
	return 1;
    if (strstr(bf,"while(") == bf)
	return 1;
    if (strstr(bf,"while (") == bf)
	return 1;
    if (strstr(bf,"do") == bf)
	return 1;
    if (strstr(bf,"catch(") == bf)
	return 1;
    if (strstr(bf,"catch (") == bf)
	return 1;
    if (strstr(bf,"else") == bf)
	return 1;
    if (strstr(bf,"try") == bf)
	return 1;
    if (strstr(bf,"typedef ") == bf)
	return 1;
    if (strstr(bf,"struct ") == bf)
	return 1;
    if (strstr(bf,"switch(") == bf)
	return 1;
    if (strstr(bf,"switch (") == bf)
	return 1;
    if (case_def && strstr(bf,"case ") == bf)
	return 1;
    if (case_def && strstr(bf,"default:") == bf)
	return 1;
    return 0;
  }

//=============================>>> vTextEditor::get_prev <<<======================
  long vTextEditor::get_prev(char *prev_buff, long start)
  {	// Get previous non-blank line in text buffer (BEW: 10/1/98)
    long pline;
    int ix, blank_line;
    BUFFPTR bi;

    *prev_buff = 0;			/* empty line for sure */
    pline = start;
GP_TOP:
    --pline;				/* previous line */

    if (pline <= 1)
	return 0;			/* there is no previous line */

    bi = GLine(pline);		/* first char of buffer */

    blank_line = 1;
    for (ix = 0 ; ix < MAX_PREV-2 ; ++ix)
      {
	if (GCh(bi+ix) == '\n')
	    break;
	prev_buff[ix] = GCh(bi+ix);	/* copy the char */
	if (prev_buff[ix] != ' ' || prev_buff[ix] != '\t')
	   blank_line = 0;		/* not a blank line */
      }
    prev_buff[ix] = 0;			/* terminate the string */
    if (blank_line || prev_buff[0] == '#')
	goto GP_TOP;			/* find previous non-blank line */

    return pline;
  }


// ########################################################################
//
//  Misc. methods
//
// ########################################################################

// ======================>>> vTextEditor::getSelection <<<=================
  int vTextEditor::getSelection(char* sel, int max)
  {
    // Fetch the highlighted text

    long sl = SelectionLen();           // length of selection
    if (sl <= 0 || sl >= max-1)
        return 0;

    CopySelection(sel, max, 0);        // copy the selection
    return 1;
  }

// ======================>>> vTextEditor::EditCut <<<=================
  int vTextEditor::EditCut()
  {
    // Cut text to clipboard

    long sl = SelectionLen();           // length of selection
    if (sl < 0)
        return 0;
    char* cut = new char[sl+1];         // space for a copy

    CopySelection(cut, sl+1, 0);        // copy the selection
    theApp->ClipboardSetText(cut);      // set the clipboard

    delete [] cut;                      // free the space

    RemoveMarkRange();                  // finally, cut it
    return 1;
  }

// ======================>>> vTextEditor::EditCopy <<<=================
  int vTextEditor::EditCopy()
  {
    // Cut text to clipboard

    long sl = SelectionLen();           // length of selection
    if (sl < 0)
        return 0;
    char* cut = new char[sl+1];         // space for a copy

    CopySelection(cut, sl+1, 0);        // copy the selection
    theApp->ClipboardSetText(cut);      // set the clipboard

    delete [] cut;                      // free the space

    return 1;
  }

// ======================>>> vTextEditor::EditPaste <<<=================
  int vTextEditor::EditPaste()
  {
    char* paste = theApp->ClipboardGetText();   // get the text
    if (!paste || !*paste)
        return 0;
    long pasteLen = strlen(paste);
    long orig_line = curlin;

    int old_echo = state.echof;

    if (pasteLen > 80)
        state.echof = 0;

    while (*paste)              // dumb paste - char at a time insert
      {
        if (*paste != '\r')     // ignore \r on both X and Windows
          {
            if (*paste == '\n')
                charInsert((int)'\r',false);  // this works for both X and Windows
            else
                charInsert((int)*paste,false);
          }
        ++paste;
      }
    if (state.echof == 0)
      {
        state.echof = old_echo;
  //      Verify();
      }

    if (curlin != orig_line)        /* refresh screen if moved */
        Verify();
    else
        tvhdln();                   /* just fix the cursor */
    return 1;
  }

// ======================>>> vTextEditor::SelectionLen <<<=================
  int vTextEditor::SelectionLen()
  {
    long lines;
    int len;
    MARK_RANGE r_mark;

    if (mark.beg_lin != 0)              // we have some text to count
      {
        r_mark = mark;                  // use a copy

        // count the range

        lines = r_mark.end_lin - r_mark.beg_lin + 1;

        if (lines == 1)                   /* all on current line */
          {
            if (r_mark.end_chr) /* find type range */
              {
                len = r_mark.end_chr - r_mark.beg_chr;
              }
            else
              {
                for (len = 0 ; GCh(r_mark.beg_chr + len) != 0 ; ++len)
                    ;
              }
            return len;
          }
        else                    // more than one line
          {
            // Count 1st line
            for (len = 0 ; GCh(r_mark.beg_chr + len) != 0 ; ++len)
               ;
            // Count middle lines
            for (long mid = r_mark.beg_lin + 1 ;
                        mid < r_mark.end_lin ; ++mid)
              {
                int linlen;
                for (linlen = 0 ; GCh(GLine(mid) + linlen ) != 0 ;
                      ++linlen)
                  ;                     // count len of this line
                len += linlen;          // add to total
              }
            // Count last line - count up to r_mark.end_chr, or
            // to the end of the line (some places we call end_chr
            // a big number that really isn't the end of the line
            int lastlen;
            for (lastlen = 0 ;
                 GLine(r_mark.end_lin) + lastlen < r_mark.end_chr
                 && GCh(GLine(r_mark.end_lin) + lastlen) != 0 ;
                 ++lastlen)
                ;
            len += lastlen;
            return len;
          }
      }
    return 0;
  }

// ======================>>> vTextEditor::CopySelection <<<=================
  int vTextEditor::CopySelection(char* buff, int max, int unmark)
  {
    long cnt;
    int ix;
    char *to;
    MARK_RANGE r_mark;

    to = buff;

    *to = 0;				// always a valid string, even NULL
    if (mark.beg_lin != 0)              /* we have some text to kill */
      {
        if (max < SelectionLen() + 1)   // make sure fits
            return 0;

        r_mark = mark;                  /* make copy to avoid recursion */

        /* now safe to kill off range */
        if (unmark)
          {
            mark.beg_lin = mark.end_lin =
            mark.beg_col = mark.end_col = 0;
            mark.beg_chr = mark.end_chr = 0;
            theApp->SetValueAll(M_Cut, 0, Sensitive);   // zap menu
            theApp->SetValueAll(M_Copy, 0, Sensitive);
          }


        /* clean up screen */
        if (curlin != r_mark.beg_lin ||
            r_mark.end_lin - r_mark.beg_lin > 0)
          {
            Verify();
          }
        else
          {
            tvxy(1,tvdlin);     /* fix it up */
            type_lines(curlin, 1);
            tvhdln();
          }

        /* copy the range */

        cnt = r_mark.end_lin - r_mark.beg_lin + 1;

        if (cnt == 1)                   /* all on current line */
          {
            if (r_mark.end_chr) /* find type range */
              {
                cnt = r_mark.end_chr - r_mark.beg_chr;
              }
            else
              {
                for (cnt = 0 ; GCh(r_mark.beg_chr + cnt) != 0 && cnt < (max - 1) ;
                        ++cnt)
                    ;
              }
            for (ix = 0 ; ix < cnt && ix < (max - 1) ; ++ix)  /* copy directly */
              {
                *(to+ix) = GCh(r_mark.beg_chr+ix);
                if (*(to+ix) == 0)
                    break;
              }
            *(to+ix) = 0;
          }
        else                    /* more than one line */
          {
            // Copy 1st line
            for (long len1 = 0 ; GCh(r_mark.beg_chr + len1) != 0 ; ++len1)
               *to++ = GCh(r_mark.beg_chr + len1);
            // Copy middle lines
            for (long mid = r_mark.beg_lin + 1 ;
                        mid < r_mark.end_lin ; ++mid)
              {
                for (int linlen = 0 ; GCh(GLine(mid) + linlen) != 0 ;
                      ++linlen)
                  *to++ = GCh(GLine(mid) + linlen);
              }
            // Count last line - count up to r_mark.end_chr, or
            // to the end of the line (some places we call end_chr
            // a big number that really isn't the end of the line
            for (int lastlen = 0 ;
                 GLine(r_mark.end_lin) + lastlen < r_mark.end_chr
                 && GCh(GLine(r_mark.end_lin) + lastlen) != 0 ;
                 ++lastlen)
                *to++ = GCh(GLine(r_mark.end_lin) + lastlen);
            *to++ = 0;
          }
        return 1;
      }
    return 0;
  }

// ======================>>> vTextEditor::ClearMarkRange <<<================
  void vTextEditor::ClearMarkRange(void)
  {

    checkIfScrolled();                  // can check for scroll here

    /* clear the range variables */
    if (mark.beg_lin != 0)
      {
        mark.beg_lin = mark.end_lin =
        mark.beg_col = mark.end_col = 0;
        mark.beg_chr = mark.end_chr = 0;
        Verify();
      }
    else                        /* just be sure we stay in phase */
      {
        mark.beg_lin = mark.end_lin =
        mark.beg_col = mark.end_col = 0;
        mark.beg_chr = mark.end_chr = 0;
      }
    theApp->SetValueAll(M_Cut, 0, Sensitive);
    theApp->SetValueAll(M_Copy, 0, Sensitive);
  }

// =====================>>> vTextEditor::insertToRange <<<================
  int vTextEditor::insertToRange(char* atFront, char* atEnd)
  {
    // This will insert the text supplied around the marked
    // range, clearing the range, and leaving the cursor
    // at the end of the new text
    return 1;
  }

// =====================>>> vTextEditor::RemoveMarkRange <<<================
  int vTextEditor::RemoveMarkRange(bool doUndo)
  {
     /* delete the text in the marked range
        return
            0 : no text deleted
            1 : text deleted
    */

    static int inHere = 0;                      // don't let it be recursive
    int retval = 0;

    long cnt, orig_line;
    MARK_RANGE r_mark;
    BUFFPTR orig_chr;

    if (inHere)
        return 0;
    if (mark.beg_lin == 0)              /* we have some text to kill */
	return 0;

    inHere = 1;

    r_mark = mark;                  /* make copy to avoid recursion */

    /* now safe to kill off range */
    mark.beg_lin = mark.end_lin =
    mark.beg_col = mark.end_col = 0;
    mark.beg_chr = mark.end_chr = 0;

    /* kill off range */
    orig_line = curlin;
    orig_chr = curchr;
    curlin = r_mark.beg_lin;
    curchr = r_mark.beg_chr;
    cnt = r_mark.end_lin - r_mark.beg_lin + 1;


    if (!state.readOnly && cnt == 1)                        /* all on current line */
      {
	if (r_mark.end_chr)         /* find type range */
	  {
	    long dcnt = r_mark.end_chr - r_mark.beg_chr;
	    charDelete(dcnt,doUndo);
	  }
	else                        /* ^M type range */
	  {
	    if (curchr == GLine(curlin))
	      {
		lineDelete(cnt,doUndo);		/* kill whole current line */
	      }
	    else
	      {
		lineDeleteToEnd(doUndo);	// past line beg, just kill rest
	      }
	  }
	retval = 1;
	if (curlin != orig_line)        /* refresh screen if moved */
	    Verify();
	else
	    tvhdln();                   /* just fix the cursor */

      }
    else if (!state.readOnly)	// more than one line
      {
	int oldef = state.echof;
	state.echof = 0;            // turn off echo

	if (doUndo)
	  {
	    bool partialLine;
	    partialLine = curchr != GLine(curlin);	// partial first line
	    if (!IsEndLine(GCh(r_mark.end_chr)))
		partialLine = true;
	    if (!SaveKillLine(cnt,partialLine))
	      {
	      }
	  }

	lineDeleteToEnd(false);	// past line beg, just kill rest
	lineDownBeg((long)1,0);     /* and the end of line */
	--cnt;                      /* one less */
	lineDelete(cnt - 1, false);	// kill off all but last line
	if (!r_mark.end_chr)        /* kill whole last line */
	  {
	    lineDelete((long)1,false);
	  }
	else
	  {
	    curchr = r_mark.end_chr;        /* get last char */
	    if (IsEndLine(GCh(curchr)))
		lineDelete((long)1,false);
	    else
		lineDeleteFront(false);	// kill last line
	  }
	charDelete((long)-1,false);	// and the lead cr
	retval = 1;
	state.echof = oldef;
        if (doUndo)
            undoOn("Undo Delete");
	Verify();
      }

    ChangeLoc(curlin,col_pos(curchr,0));

    inHere = 0;
    return retval;
  }

// ===========================>>> BalMatch <<<==============================
  int vTextEditor::BalMatch(long val)
  {
    /* Find balance of )]} or [{( */

    int orig_line, dir, nest, old_ef, echo_off;
    long limit, ix;
    BUFFPTR orig_chr;
    char start_c, match;

    if (lastLineBF() < 1)
        return 0;
    ClearMarkRange();           /* no range now */
    wasColCmd = 0;

    old_ef = state.echof;
    echo_off = 0;               /* haven't turned off echo */

    limit = ((long) val) * 4000L;       /* limit for search */

    orig_chr = curchr;
    orig_line = curlin;
    start_c = GCh(curchr);              /* original char */
    switch (start_c)
      {
        case '(':
            match = ')';
            dir = 1;
            break;

        case '{':
            match = '}';
            dir = 1;
            break;

        case '[':
            match = ']';
            dir = 1;
            break;

        case ')':
            match = '(';
            dir = -1;
            break;

        case '}':
            match = '{';
            dir = -1;
            break;

        case ']':
            match = '[';
            dir = -1;
            break;

        default:
	    state.echof = old_ef;
            return 0;           /* no op if not a paren thing */

      }

    for (ix = 1, nest = 0 ; ix < limit ; ++ix)
      {
        charRight((long)dir,1);         /* go right */
        if (!echo_off && (curlin != orig_line))
          {
            state.echof = 0;
            echo_off = 1;       /* disable echoing */
            StatusMessage("Scanning for matching paren");
          }
        if (GCh(curchr) == start_c)
            ++nest;             /* bump nest */
        else if (GCh(curchr) == match)
          {
            if (nest)
                --nest;                 /* undo nest */
            else
              {
		state.echof = old_ef;
		if (state.echof)	/* update if it was on */
		    Verify();
                return 1;		/* found the matching thing */
              }
          }
      }

    /* fall through ==> died */
    curchr = orig_chr;
    curlin = orig_line;
    state.echof = old_ef;
    newscr();
    ChangeLoc(curlin,col_pos(curchr,0));
    return 0;
  }

// #########################################################################
//
// Private helper methods
//
// #########################################################################

//========================>>> vTextEditor::reallocLines <<<======================
  int vTextEditor::reallocLines()
  {
    BUFFPTR* oldLines;

    oldLines = _lines;                  // remember old lines
    _lines  = new char*[_maxLines + lineListSize];
    if (!_lines)
      {
        _lines = oldLines;              // failed to get more lines
        return 0;
      }

    long lx;
    for (lx = 0 ; lx < _maxLines ; ++lx)  // copy old lines
        _lines[lx] = oldLines[lx];

    _maxLines = _maxLines + lineListSize - 1;       // safety factor

    for (; lx < _maxLines ; ++lx)  // null new lines
        _lines[lx] = 0;

    delete [] oldLines;                 // free the old lines
    return 1;
  }

//========================>>> vTextEditor::col_pos <<<======================
  int vTextEditor::col_pos(BUFFPTR chr_pos, int do_shift)
  {  /* col_pos - find the x position of the character chr_pos on current line
                handles spacing for tabs, control characters etc */

    BUFFPTR i;
    int pos;

    pos = 1;
    for (i = GLine(curlin) + 1 ; i <= chr_pos ; ++i)
      {
        if (GCh(i-1) < ' ' && GCh(i-1) > 0)     /* cur pos depends on last chr */
          {
            if (GCh(i-1) == '\t' && gState.tabspc > 0)   /* handle tabs */
              {
                for (++pos ; ((pos-1) % gState.tabspc) != 0; ++pos)
                    ;
              }
            else                /* control characters (echoed as ^X) */
                pos += 2;       /* 2 spaces for other control character */
          }
        else                    /* normal character */
            ++pos;
      }

    while (do_shift)
      {
        if (pos < leftmg)       /* won't fit on screen */
          {
            leftmg -= 16;       /* shift left */
          }
        else if (pos >= GetCols() + leftmg)
          {
            leftmg += 16;
          }
        else
            break;
      }

    if (do_shift)
        return pos - leftmg + 1;
    else
        return pos;
  }

// ===========================>>> vTextEditor::mint <<<=====================
  int vTextEditor::mint(int v1, int v2)
  {
    return (v1 > v2 ? v2 : v1);
  }

// ============================>>> vTextEditor::maxt <<<====================
  int vTextEditor::maxt(int v1, int v2)
  {
    return (v1 > v2 ? v1 : v2);
  }

// ===========================>>> vTextEditor::minl <<<=====================
  long vTextEditor::minl(long v1,long v2)
  {
    return (v1 > v2 ? v2 : v1);
  }

// ===========================>>> vTextEditor::maxl <<<=====================
  long vTextEditor::maxl(long v1, long v2)
  {
    return (v1 > v2 ? v1 : v2);
  }

// ===========================>>> vTextEditor::clower <<<===================
  int vTextEditor::clower(int ch)
  {
    return ((ch >='A' && ch<='Z') ? ch + ' ' : ch);
  }

// ==========================>>> vTextEditor::cupper  <<<===================
  int vTextEditor::cupper(int ch)
  {
    return ((ch >= 'a' && ch <= 'z') ? ch - ' ' : ch);
  }

// ===================>>> vTextEditor::line_can_fit <<<=====================
  int vTextEditor::line_can_fit(long l)
  {
    /* if line can't fit onto screen width, we need to update deleted
        characters a bit differently.
    */
    BUFFPTR to;
    int len;

    for (to = GLine(l) + 1, len = 0 ; !IsEndLine(GCh(to)) ; ++to)
      {
        if (GCh(to) < ' ')
            return 0;                   /* ctrl chars mess it up, so false */
        ++len;
      }
    return (len < GetCols());
  }

// =======================>>> vTextEditor::FindDispLine <<<=================
  void vTextEditor::FindDispLine(long& ibeg, int& cnt)
  {  /* FindDispLine - find the display line
        known: current line, calculate where it would go on the screen */

    if (curlin <= dsplin)
      {                                 /* it is in first part of the display */
        ibeg = 1;
        cnt = (int) minl((long)GetRows(),(long)(lastLine));
        tvdlin = (int)curlin;           /* update the display line */
      }
    else if (lastLine-curlin < GetRows()-dsplin)
      {  /* at bottom of display */
        ibeg = maxl((long)1,(long)(lastLine - GetRows() + 1));
        cnt = mint(GetRows(),(int) lastLine);
        tvdlin = minl(curlin,(long)(GetRows()-(lastLine-curlin+1)+1));
      }
    else                        /* normal case: in middle */
      {
        ibeg = maxl((long) 1,(long)(curlin-dsplin+1));
        cnt = minl((long)GetRows(),(long)(lastLine-(ibeg)+1));
        tvdlin = dsplin;
      }
 }

// ======================>>> vTextEditor::findX  <<<========================
  int vTextEditor::findX(void)
  {  /* findX - find the x position of the current character
                handles spacing for tabs, control characters etc */

    BUFFPTR i;
    int pos, need_newscr;

    if (curlin < 1 || GetCols() <= 1 || lastLineBF() < 1)
        return 1;
    need_newscr = 0;
    pos = 1;

    for (i = GLine(curlin)+1 ; i <= curchr ; ++i)
      {
        if (GCh(i-1) < ' ' && GCh(i-1) > 0)     /* cur pos depends on last chr */
          {
            if (GCh(i-1) == '\t' && gState.tabspc > 0)   /* handle tabs */
              {
                for (++pos ; ((pos-1) % gState.tabspc) != 0; ++pos)
                    ;
              }
            else                /* control characters (echoed as ^X) */
                pos += 2;       /* 2 spaces for other control character */
          }
        else                    /* normal character */
            ++pos;
      }

    for (;;)
      {
        if (pos < leftmg)       /* won't fit on screen */
          {
            leftmg -= 16;       /* shift left */
            need_newscr = 1;
            if (GetCols() <= 16)
            	break;
          }
        else if (pos >= GetCols()+leftmg)
          {
            leftmg += 16;
            need_newscr = 1;
            if (GetCols() <= 16)
            	break;
          }
        else
            break;
      }

    if (need_newscr)
      {
        Verify();
      }

    return (pos-leftmg+1);
  }

// ===================>>> vTextEditor::getCurLineFront <<<=======================
  void vTextEditor::getCurLineFront(char* buff, long start)
  {
    int ix;
    BUFFPTR bi;

    buff[0] = 0;                /* empty line for sure */

    if (lastLineBF() < 1)
        return;

    bi = GLine(start);          /* first char of buffer */

    for (ix = 0 ; ix < 18 ; ++ix)       /* 18 chars max */
      {
        if (IsEndLine(GCh(bi+ix)))
            break;
        buff[ix] = tolower(GCh(bi+ix));  /* copy the char */
      }
    buff[ix] = 0;                       /* terminate the string */
  }

// ====================>>> vTextEditor::lastBX <<<==========================
  BUFFPTR vTextEditor::lastBX(long line)
  {
    /* return the buff index of the last char of the line */
    BUFFPTR bx;

    for (bx = GLine(line) ; !IsEndLine(GCh(bx)) ; ++bx)
        ;                       /* find last char in line */
    return bx;
  }

// ====================>>> vTextEditor::Fill1  <<<==========================
  int vTextEditor::Fill1(int dir, int val)
  {  /* change character dir to val */

    int oldwrp;

    if (state.readOnly)
        return 0;
    oldwrp = gState.wraplm;
    gState.wraplm = 0;
    if (! charDelete((long)dir,false))
        goto l900;
    if (! charInsert(val,false))
        goto l900;
    gState.wraplm = oldwrp;
    return 1;
l900:
    gState.wraplm = oldwrp;
    return 0;
  }

// ======================>>> vTextEditor::newscr <<<========================
  void vTextEditor::newscr(void)
  { /* newscr - retype entire screen, updating cursor position if necessary */

    long ibeg;
    int cnt;

    if (!state.echof)
        return;

    if (curlin < 1)
      {
        tvclr();
        tvxy(1,1);
        return;
      }
    if (lastLine < GetRows())   /* two kinds of screen rewrite */
        tvclr();                        /* clear the screen and home */
    tvxy(1,1);

    dsplin = tvdlin = ddline;   /* home to middle */
    FindDispLine(ibeg, cnt);    /* calculate where it will go */
    type_lines(ibeg,cnt);       /* type it out */
    tvhdln();                   /* home to display line */
  }

// =====================>>> vTextEditor::range  <<<=========================
  void vTextEditor::range(long cnt, long *lbeg, long *lend)
  { /* determine a legal line number range given cnt */

    if (cnt <= 0)
      {
        if ((*lbeg = curlin + cnt) < 0)
            *lbeg = 1;
        *lend = curlin;
        if (cnt < 0)
           *lend = (*lend)-1;
      }
    else
      {
        *lbeg = curlin;
        if ((*lend = curlin+cnt-1) > lastLine)
            *lend = lastLine;
      }
 }

// ====================>>> vTextEditor::setScrollBar <<<====================
  void vTextEditor::setScrollBar()
  {
    long shown;
    long top;
    long last = lastLine;

    if (!state.echof)
        return;

    if (last < 1)
        last = 1;

    if (lastLine <= (long)GetRows())
        shown = 100L;
    else
        shown = ((long) GetRows() * 100L) / last;

    if (shown < 1)
        shown = 1;

    long cur = (scroll_lin > 0) ? scroll_lin : curlin;  // handle scrolling

    if (cur >= last)
        top = 100L;
    else if (cur == 1)
        top = 0;
    else
        top = (cur * 100L) / last;

    if (top < 0)
        top = 0;

    SetVScroll((int) shown, (int) top);
  }

// ====================>>> vTextEditor::checkIfScrolled <<<=================
  void vTextEditor::checkIfScrolled()
  {
    // If we are scrolled, we need to restore screen

    if (!state.echof)
        return;

    if (scroll_lin > 0 || setCursor) 
      {
	setCursor = 0;
        scroll_lin = -1;
        ShowTextCursor();
        Verify();
      }
  }

// ====================>>> vTextEditor::scrollDown <<<======================
  void vTextEditor::scrollDown(long delta)
  { /* scroll screen without moving cursor either 1 line or screenful*/

    long change;

    if (!state.echof)
        return;

    if (lastLineBF() < 1 || delta == 0)
      {
        setScrollBar();
        return;
      }

    if (!gState.fixed_scroll && scroll_lin < 0)  /* might be on same screen */
      {
        // this just adjusts the screen without really scrolling
        if (delta == 1 && dsplin > 1)           /* scroll down */
          {
            if ((curlin+GetRows()-tvdlin) >= lastLine)
                return;                         // no where to go
            tvbotb(1);          /* make room */
            tvxy(1,GetRows());  /* home to last line */
            dsplin = tvdlin = tvdlin - 1;       /* change line */
            type_lines((long)(curlin+GetRows()-tvdlin),1);      /* fix up screen */
            tvhdln();   /* home to display line */
            setScrollBar();
            return;
          }
        else if (delta == -1 && dsplin < GetRows())
          {
            if ((curlin-tvdlin) <= 0 )
                return;
            tvtopb(1);          /* make blank lines at top */
            dsplin = tvdlin = tvdlin + 1;       /* change line */
            type_lines((long)(curlin-tvdlin+1),1);      /* fill in */
            tvhdln();   /* home to display line */
            setScrollBar();
            return;
          }
      }

    if (scroll_lin < 0)         /* initial setup */
      {
        if (curlin <= dsplin)   /* on first screen */
          {
            scroll_lin = 1;             /* assume 1st line */
          }
        else if (lastLine - curlin < GetRows() - dsplin)
          {                             /* at bottom of display */
            scroll_lin = maxl((long)1,(long)(lastLine - GetRows() + 1));
          }
        else                    /* normal case: in middle */
          {
            scroll_lin = maxl((long)1,(long)(curlin-dsplin+1));
          }
      }

    if (delta < 0 && scroll_lin == 1)   /* at top already */
        return;

    change = delta;

    if (change < 0)                     /* have to scroll screen down */
      {
        if (change == -1)
          {
            if (scroll_lin <= 1)
                return;
            scroll_lin -= 1;
            tvtopb(1);          /* make blank lines at top */
            type_lines(scroll_lin,1);   /* fill in */
          }
        else                    /* a screenful or so */
          {
            scroll_lin = maxl((long)1,(long)(scroll_lin + delta));
            newScrollScreen(scroll_lin);
          }
      }
    else if (change > 0)                /* have to scroll screen up */
      {
        if ((scroll_lin+GetRows()) > lastLine)
                return;                         // no where to go
        if (change == 1)
          {
            scroll_lin += 1;
            tvbotb(1);          /* make blank lines at top */
            type_lines((long)(scroll_lin+GetRows()-1),1);       /* fill in */
          }
        else                    /* a screenful or so */
          {
            scroll_lin = minl(lastLine, (long)(scroll_lin + delta));
            newScrollScreen(scroll_lin);
          }
      }

    // now, if the curlin is on the new scrolled screen, we need to
    // repaint it so the cursor and highlight area show


    if (curlin >= scroll_lin  && curlin < (scroll_lin + GetRows()))
      {
	ShowTextCursor();
	int xf = findX();
	tvxy(xf, curlin - scroll_lin + 1);	// reset cursor to current position
        // scroll_lin = -1;  			// BEW: 3/31/99 removed reset to -1
      }
    else
        HideTextCursor();

    setScrollBar();

  }

// =================>>> vTextEditor::newScrollScreen <<<====================
  void vTextEditor::newScrollScreen(long ibeg)
  { /* newScrollScreen - retype entire screen,
        updating cursor position if necessary */

    if (!state.echof)
        return;

    tvclr();                    /* clear the screen and home */
    type_lines(ibeg,(int)minl(lastLine,(long)GetRows()));       /* type it out */
  }

// ===================>>> vTextEditor::undoOn <<<=====================
  void vTextEditor::undoOn(const char* msg)
  {
    if (!state.echof)
	return;
    ((vWindow *)_parent)->SetValue(M_UnDo, 1, Sensitive);
    if (msg && *msg)
	((vWindow *)_parent)->SetString(M_UnDo, (char*) msg);
  }

// ===================>>> vTextEditor::undoOff <<<=====================
  void vTextEditor::undoOff(const char* msg)
  {
    undoCurlin = -1;	// so can't try to undo...
    if (!state.echof)
	return;
    ((vWindow *)_parent)->SetValue(M_UnDo, 0, Sensitive);
    if (msg && *msg)
	((vWindow *)_parent)->SetString(M_UnDo, (char*) msg);
  }

// ===================>>> vTextEditor::SaveKillLine <<<=====================
  bool vTextEditor::SaveKillLine(long cnt, bool partialLine)
  {
    // SaveKillLine - save lines that will be killed
    //	killFirst and killLast are used with charDelete to note
    //    if we are killing part of a line that needs to be replaced
    //	  in case of an undo.

    char ln[MAX_LINE+2];
    // BUFFPTR from;

    undoOn("Undo Delete");

    // remember state
    undoCurcol = col_pos(curchr,1);
    undoCurlin = curlin;
    undoPartialLine = partialLine;

    // copy lines to buffer

    int to = 0;		// start at beginning of undo buffer

    for (long lx = 0 ; lx < cnt && to < MAX_UNDO ; ++lx)
      {
	int len = getLine(ln,MAX_LINE,curlin+lx);
	if (len < 0)	// out of lines?
	    break;
	for (int ix = 0 ; ix < len && to < MAX_UNDO ; ++ix)
	  {
	    undoBuff[to++] = ln[ix];	// copy to buffer
	  }
	undoBuff[to++] = '\n';		// add end of line
      }

    undoBuff[to] = 0;			// terminate the buffer

    if (to >= MAX_UNDO)			// oops!
      {
	// issue warning?
	return false;
      }
    return true;			// save succeeded
  }

// =======================>>> vTextEditor::undoEdit <<<=======================
  int vTextEditor::undoEdit(void)
  { /* unkill the single last line killed */

    char ln[MAX_LINE+2];	// temp line
    long lines = 0;

    if (undoCurlin < 0)
	return 0;
    long useCurlin = undoCurlin;	// keep local copy

    lineGoto(useCurlin);	// and go to beginning of undo stuff

    if (undoPartialLine)	// kills partial line before restore
	lineDelete(1,false);

    for (int from = 0 ; from < MAX_UNDO && undoBuff[from] != 0 ; ++from)
      { // scan whole buffer

	for (int to = 0 ; to < MAX_LINE && from < MAX_UNDO ; ++to)
	  { // copy line at a time
	     if (undoBuff[from] == '\n' || undoBuff[from] == 0)
	      { // \n marks end of each line
		ln[to] = 0;
		insertLine(ln, useCurlin + lines++, false);	// insert it
		break;
	      }
	    else
		ln[to] = undoBuff[from++];
	  }
      }

    lineGoto(useCurlin);	// and go to beginning of undo stuff
    //@@ goto curchr...
    long lim = lineLenBF(curlin);
    for (int ic = 0; ic < lim && col_pos(curchr,1) < undoCurcol ; ++ic)
	charRight(1, 0);

    if (state.readOnly)
        return 0;

    return 1;
  }

// =====================>>> vTextEditor::tvhdln <<<=========================
  void vTextEditor::tvhdln(void)
  { /* tvhdln - home to display line */

    int xf;

    if (curlin < 1)
        tvxy(1,1);
    else
      {
        if (mark.beg_lin > 0)
          {
            tvxy(1,tvdlin);
            type_lines(curlin, 1);
          }
        xf = findX();
        tvxy(xf,tvdlin);
      }
  }

// ========================>>> vTextEditor::tvtyln <<<======================
  void vTextEditor::tvtyln(long lineNum, BUFFPTR chrptr, int whole_line)
  { /* tvtyln - type a line on the screen without cr/lf */

    xoutcm = tvx;
    tvplin(lineNum,chrptr,whole_line,-1,-1);
  }

// =======================>>> vTextEditor::update <<<=======================
  void vTextEditor::update(long change)
  { /* update - update the screen when line position has changed
                will not be used if any alterations have been made */

    if (change == 0)
      {
        tvhdln();
        return;
      }
    setScrollBar();
    if (gState.fixed_scroll)
        updateScroll(change);   /* cursor stays on fixed line */
    else
        updateNoScroll(change); /* cursor roams around screen */
  }

// ===================>>> vTextEditor::updateScroll <<<=====================
  void vTextEditor::updateScroll(long change)
  { /* update - update the screen when line position has changed
                will not be used if any alterations have been made */

    long abschg;

    if (!state.echof)
        return;
    abschg =  change;

    if (change < 0)                     /* have to scroll screen down */
      {
        abschg = (-change);
        if (tvdlin-abschg < 1)
            Verify();
        else if (curlin < tvdlin)       /* won't fit exactly */
          {
            if (tvdlin >= dsplin && abschg != 1)
              {
                tvclr();                /* clear the screen */
                type_lines((long)1,GetRows());  /* type out a screen */
              }
            tvdlin = curlin;
          }
        else if (tvdlin - abschg >= dsplin)
            tvdlin -= abschg;
        else
          {
            if (tvdlin > dsplin)
              {                         /* moving up from below display line */
                abschg = dsplin-(tvdlin-abschg);
                tvdlin=dsplin;          /* update */
              }
            tvtopb((int)abschg);                /* make blank lines at top */
            type_lines((long)(curlin-tvdlin+1),(int)abschg);    /* fill in */
          }
      }
    else if (change > 0)                /* have to scroll screen up */
      {
        if (tvdlin+change > GetRows() && tvdlin < dsplin ||
                change >= GetRows())
            Verify();
        else if (tvdlin < dsplin || lastLine <= GetRows())
            if (tvdlin+change > dsplin && lastLine > GetRows())
                Verify();
            else
                tvdlin += change;
        else if (lastLine - curlin < GetRows() - tvdlin)        /* on bottom part */
          {
            if (tvdlin<=dsplin && abschg != 1)
              {
                tvclr();                /* rewrite whole screen */
                type_lines((long)(lastLine - GetRows() + 1), GetRows());
              }
            tvdlin = (int) minl((long)GetRows(), (long)(lastLine))
                         - (lastLine - curlin + 1) + 1;
          }
        else
          {
            tvbotb((int)abschg);                /* make room */
            tvxy(1,(int)(GetRows()-abschg+1));  /* home to right line */
            type_lines((long)(curlin+GetRows()-tvdlin-abschg+1),(int)abschg);  /* fix up screen */
            if (tvdlin < dsplin)
                tvdlin = dsplin;
          }
      }
    tvhdln();
  }

// ==================>>> vTextEditor::updateNoScroll <<<====================
  void vTextEditor::updateNoScroll(long change)
  { /* update - update the screen when line position has changed
                will not be used if any alterations have been made */

    long abschg;

    if (!state.echof)
        return;
    abschg =  change;

    if (change < 0)                     /* have to scroll screen down */
      {
        abschg = (-change);
        if (curlin + abschg < tvdlin && curlin < tvdlin) // won't fit exactly
          {
            dsplin = tvdlin = curlin;
          }
        else if (tvdlin - abschg >= 1)
          {
            dsplin = tvdlin -= (int)abschg;
          }
        else if (abschg == 1) /* simple case */  
          {
            tvtopb((int)abschg);                /* make blank lines at top */
            type_lines((long)(curlin-tvdlin+1),(int)abschg);    /* fill in */
            dsplin = tvdlin = 1;
          }
        else            /* scroll to above top line */
          {
            dsplin = tvdlin;
            Verify();
          }
      }
    else if (change > 0)                /* have to scroll screen up */
      {
        if (tvdlin + change <= GetRows())
          {
            dsplin = 
            tvdlin = tvdlin + change;
          }
        else if (change == 1)
          {
            tvbotb((int)abschg);                /* make room */
            tvxy(1,(int)(GetRows()-abschg+1));  /* home to right line */
            type_lines((long)(curlin+GetRows()-tvdlin-abschg+1),(int)abschg);
            /* fix up screen */
            dsplin = 
            tvdlin = GetRows();
          }
        else            /* scroll to above top line */
          {
            dsplin = tvdlin;
            Verify();
          }
      }
    tvhdln();
  }

// #########################################################################
//
// word methods
//
// #########################################################################

// =====================>>> vTextEditor::wordRight  <<<=====================
  int vTextEditor::wordRight(long cnt)
  {  /* wordRight - move cursor over words */

    long lim, words, incr, lenmov;
    int rv;

    if (lastLineBF() < 1)
        return 0;
    ClearMarkRange();           /* no range now */
    wasColCmd = 0;

    rv = 1;
    lenmov = 0;
    if (cnt < 0)
      {
        incr = (-1);            /* change */
        lim = (-cnt);
      }
    else if (cnt == 0)
      {
        incr = -1;
        lim = 0;
      }
    else
      {
        incr = 1;
        lim = cnt;
      }

    for (words = 1; words <= lim; ++words)
      {
        if ((IsEndLine(GCh(curchr)) && incr > 0) ||
            (curchr == GLine(curlin) && incr < 0) )
          {
            if (curlin + incr > lastLine || curlin + incr < 1)
              {
                rv = 0;
                break;          /* at a buffer limit, so quit */
              }
            lineDownBeg((long)incr,0);  /* move up or down */
            lenmov += incr;
            if (cnt<0)
                lineEnd();
            continue;           /* move to next word */
          }

    /* ok, first, skip over word characters */
        while (IsWordChar(GCh(curchr)))
          {
            if (curchr == GLine(curlin) && incr < 0)
                goto l100;
            else
              {
                curchr += incr;
                lenmov += incr;
              }
          }

    /* now skip over remaining non word chars */
        while (! IsWordChar(GCh(curchr)))
           {
            if ((IsEndLine(GCh(curchr)) && incr > 0) ||
                (curchr == GLine(curlin) && incr < 0) )
                break;
            else
              {
                curchr += incr;
                lenmov += incr;
              }
          }
l100: ;
      }

    if (incr < 0)               /* move cursor to beginning of word */
      {
        while (IsWordChar(GCh(curchr-1)))
          {
            curchr += incr;
            lenmov += incr;
          }
      }
    tvhdln();
    oldlen = lenmov;
    ChangeLoc(curlin,col_pos(curchr,0));
    return rv;
  }

// ====================>>> vTextEditor::IsWordChar <<<======================
  int vTextEditor::IsWordChar(int chr)
  { /* IsWordChar - determine if a character is a "word" type character */

    if ((chr>='a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z') ||
        (chr >= '0' && chr <= '9') || chr == '_' || chr == '\\')
        return 1;
    else
        return 0;
  }

// #########################################################################
//
// DEFAULT vTextEdCmdInterp
//
// #########################################################################

// ==================>>> vTextEdCmdInterp::vTextEdCmdInterp <<<=================
  vTextEdCmdInterp::vTextEdCmdInterp(VCONST vTextEditor* textEd,
		VCONST vCmdWindow* cw)
  {
      _textEd = const_cast<vTextEditor*>(textEd);
      _myCmdWindow = const_cast<vCmdWindow*>(cw);
      metaWait = 0;
      countWait = 0;		// not waiting on meta cmd
      metaChar = 'A'-'@';	// ^A is default meta char
      cmdCount = 1;
  }

// ==================>>> vTextEdCmdInterp::~vTextEdCmdInterp <<<================
  vTextEdCmdInterp::~vTextEdCmdInterp()
  {
  }

// =====================>>> vTextEdCmdInterp::InitCmdInterp <<<====================
  void vTextEdCmdInterp::InitCmdInterp()
  {
  }


// ----------------------------- Default CI --------------------------------

// =====================>>> vTextEdCmdInterp::CmdInterpHelp <<<====================
  void vTextEdCmdInterp::CmdInterpHelp()
  {
    static char* helplist [] =
      {
	"Standard V Generic Editor",
	"Key  --  Command Description",
	"Esc  --  Prefix to enter count n",
	"  ",
	"  ***  Movement Commands  ***",
	"nLeft  --  Move left [^L]",
	"nCtl-Left  --  Move left a word",
	"nUp  --  Move up [^U]",
	"n^AUp  --  Move up to beg of line",
	"nRight  --  Move right [^R]",
	"nCtl-Right  --  Move right a word",
	"nDown  --  Move down [^D]",
	"n^ADown  --  Move down to beg of line",
	"Home  --  Goto beg of line [^A,]",
	"Ctrl-Home  --  Goto beg of file",
	"End  --  Goto end of line [^A.]",
	"Ctrl-End  --  Goto end of file",
	"nPgUp  --  Move a screenful up",
	"nCtrl-PgUp  --  Scroll a screenful up",
	"nPgDn  --  Move a screenful down",
	"nCtrl-PgDn  --  Scroll a screenful down",
	"n^G  --  Goto line n",
	"n^A^N  --  Note (mark) location n (1-25)",
	"n^AN  --  Goto noted location n",
	"^AM  --  Center Cursor in screen",
	"^Av  --  Repaint screen",
	" ",
	"  ***  Searching commands ***",
	"^A[  --  Balance match",
	"^F  --  Find pattern",
	"Shift-^F  --  Find next",
	"^Af  --  Find/Replace",
	" ",
	"  ***  Insertion Commands ***",
	"n^AIns  --  Insert char with value of n",
	"Ins  --  Toggle insert/overtype",
	"n^AO  --  Open a new blank line",
	" ",
	"  ***  Kill/change commands ***",
	"nBkspace  --  Delete previous char",
	"^ABkspace  --  Delete to line begin [^A']",
	"nDel  --  Delete next char",
	"^ADel  --  Delete to line end [^A\"]",
	"nShft-Del  --  Delete line [^K]",
	"nShft-^C  --  Fold case",
	"^V  --  Paste from clipboard",
	"^C  --  Copy highlight to clipboard",
	"^X  --  Cut highlight to clipboard",
	"^Z  -- Undo",
	" ",
	"  ***  File Commands ***",
	"^N -- Open new file",
	"^O -- Open file",
	"^S -- Save file",
	
	 0
      };

    static CommandObject HelpD[] =
      {
        {C_Button,M_Cancel,0,"Close",NoList,CA_None,isSens,NoFrame,0,0},
	{C_Label,8000,0,"List of Editor commands",NoList,CA_None,isSens,NoFrame,M_Cancel,0},
	{C_List,8001,0,"Help",(void*)helplist,CA_Size,
		isSens,	NoFrame,0,M_Cancel,32},
	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
        
      };

    // I'd have liked a modeless dialog, but then I'd have to track
    // and delete the object. This way it gets dynamically created
    // each  time.


    ItemVal temp;

    vModalDialog Help(te()->GetPaneParent(),"Editor Help");
    Help.AddDialogCmds(HelpD);
 
    (void)Help.ShowModalDialog("Generic Editor Help",temp);

  }

// =====================>>> vTextEdCmdInterp::ProcessKey <<<====================
  int vTextEdCmdInterp::ProcessKey(vKey key, unsigned int shift)
  {
// Commands supported by default keyboard mapping:
//
//      Most commands can be preceeded by a count, which is entered
//      with Esc followed by a value followed by the command. Negative
//      counts can be used for most commands as well. An n in front
//      of a command indicates it can use a count.
//
//      Some commands have synonyms compatible with See and
//      touch typing shown in []'s (e.g., ^A, for Home).
//
//      Commands that change text interact with highlighted text
//      by deleting it, but without copying it to the clipboard.
//      ^X and ^C interact with the clipboard.
//
//

    if (vk_IsModifier(key))             // ignore modifiers
        return -1;

    int retval = 1;

    if (countWait != 0)
      {
        if (key == '-')
          {
            countWait = -1;
            return 1;
          }
        else if (key >= '0' && key <= '9')	// it is a count
          {
            cmdCount = (cmdCount * 10) + (key - '0');
            return 1;
          }
        else
          {
            cmdCount *= countWait;	// negative?
            countWait = 0;		// done building count
          }
      }

    if (metaWait)	// Meta command
      {
        metaWait = 0;
        switch (key)
          {
            case vk_KP_Insert:
            case vk_Insert:
              {
                retval = te()->charInsert(cmdCount);
                break;
              }

            case '\'':
            case vk_BackSpace:
              {
                retval = te()->lineDeleteFront();
                break;
              }

            case '"':
            case vk_Delete:
            case vk_KP_Delete:
              {
                retval = te()->lineDeleteToEnd();
                break;
              }

            case ',':                   // line beginning
              {
                te()->lineBeginning();
                break;
              }

            case '.':                   // line end
              {
                te()->lineEnd();
                break;
              }

            case '[':                   // ^A[ - bal match
              {
                retval = te()->BalMatch(cmdCount);
                break;
              }

	    case 'F':
	    case 'f':
	      {
		te()->EditCommand(edReplace,cmdCount);
		break;
	      }

	    case 'M':
	    case 'm':
	      {
		te()->CenterScreen();
		break;
	      }


	    case 'N'-'@':           // ^N: note location
	      {
		retval = te()->EditCommand(edNoteLocation, cmdCount);
		break;
	      }

	    case 'N':		// go to noted location
	    case 'n':
	      {
		retval = te()->EditCommand(edNoteGoto,cmdCount);
		break;
	      }
	
            case 'O':           // O: Open line
	      {
		retval = te()->lineOpen(cmdCount);
		break;
	      }

            case 'v':
            case 'V':
            case 'V'-'@':
              {
                te()->Verify();
                break;
              }

	    case 'U'-'@':
	    case vk_Up:
	    case vk_KP_Up:
	      {
		retval = te()->lineDownBeg(-cmdCount);
		break;
	      }

	    case 'D'-'@':
	    case vk_KP_Down:
	    case vk_Down:
	      {
		retval = te()->lineDownBeg(cmdCount);
		break;
	      }

            default:
              {
                retval = -1;
                break;
              }
          }
        cmdCount = 1;
        return retval;
      }
    else if (key == (vKey) metaChar)
      {
        metaWait = 1;
        return 1;
      }

    switch (key)
      {
        case 'C'-'@':           // ^C : Copy ; Shift-^C: fold case
          {
            if (shift & VKM_Shift)      // Shift-^F
              {
                retval = te()->charFoldCase(cmdCount);
              }
            else
                te()->EditCopy();
            break;
          }

        case 'F'-'@':           // ^F: find, Shift-^F: find next
          {
            if (shift & VKM_Shift)      // Shift-^F
              {
                retval = te()->EditCommand(edFindNext, cmdCount);
              }
            else
              {
                retval = te()->EditCommand(edFind, cmdCount);
              }
            break;
          }

        case 'G'-'@':           // ^G: Goto line
          {
            retval = te()->lineGoto(cmdCount);
            break;
          }

        case 'K'-'@':           // ^K: Kill line
          {
            te()->lineDelete(cmdCount);
            break;
          }


        case 'N'-'@':           // ^N - New
	  {
	    vWindow* wp = te()->GetPaneParent();
	    wp->WindowCommand(M_New,1,C_Button);
	    break;
	  }

        case 'O'-'@':           // ^O- Open
	  {
	    vWindow* wp = te()->GetPaneParent();
	    wp->WindowCommand(M_Open,1,C_Button);
	    break;
	  }

        case 'S'-'@':           // ^S - Save
	  {
	    vWindow* wp = te()->GetPaneParent();
	    wp->WindowCommand(M_Save,1,C_Button);
	    break;
	  }

        case 'V'-'@':           // ^V: Paste
          {
            te()->EditPaste();
            break;
          }

        case 'X'-'@':           // ^X : Cut
          {
            te()->EditCut();
            break;
          }

        case 'Z'-'@':           // ^Z : Undo
          {
            te()->undoEdit();
            break;
          }

        case 'L'-'@':
        case vk_Left:
        case vk_KP_Left:
          {
            if (shift & VKM_Ctrl || shift & VKM_Shift)
                retval = te()->wordRight(-cmdCount);
            else
                retval = te()->charRight(-cmdCount,1);
            break;
          }

        case 'U'-'@':
        case vk_Up:
        case vk_KP_Up:
          {
	    retval = te()->lineDown(-cmdCount);
            break;
          }

        case 'R'-'@':
        case vk_Right:
        case vk_KP_Right:
          {
            if (shift & VKM_Ctrl)
                retval = te()->wordRight(cmdCount);
            else
                retval = te()->charRight(cmdCount,1);
            break;
          }

        case 'D'-'@':
        case vk_KP_Down:
        case vk_Down:
          {
	    retval = te()->lineDown(cmdCount);
            break;
          }

        case vk_BackSpace:
          {
            retval = te()->charDelete(-cmdCount);
            break;
          }

        case vk_Tab:
          {
	    retval = te()->defaultKeyIn('\t',shift);
            break;
          }

        case vk_Linefeed:
          {
            break;
          }

        case vk_Return:
        case vk_KP_Enter:
          {
            retval = te()->defaultKeyIn('\n',shift);
            break;
          }

        case vk_Escape:
          {
            countWait = 1;
            cmdCount = 0;
            return 1;
          }

        case vk_Delete:
        case vk_KP_Delete:
          {
            if (shift & VKM_Shift || shift & VKM_Ctrl)
                te()->lineDelete(cmdCount);
            else
                retval = te()->charDelete(cmdCount);
            break;
          }

        case vk_Home:
        case vk_KP_Home:
          {
            if (shift & VKM_Shift || shift & VKM_Ctrl)
                retval = te()->lineGoto(1);
            else
                te()->lineBeginning();
            break;
          }

        case vk_Page_Up:
        case vk_KP_Page_Up:
          {
            if (shift & VKM_Ctrl)
                te()->scrollDown(-cmdCount * te()->GetRows());
            else
                retval = te()->lineDown((long)(-cmdCount * te()->GetRows()));
            break;
          }

        case vk_Page_Down:
        case vk_KP_Page_Down:
          {
            if (shift & VKM_Ctrl)
                te()->scrollDown(cmdCount * te()->GetRows());
            else
                retval = te()->lineDown(
		    (long) te()->minl((cmdCount * te()->GetRows()), 
		    (long)(te()->GetLines() - te()->GetCurLine() + 1)));
            break;
          }

        case vk_End:
        case vk_KP_End:
          {
            if (shift & VKM_Shift || shift & VKM_Ctrl)
                te()->bufferBottom();
            else
                te()->lineEnd();
            break;
          }

        case vk_KP_Insert:
        case vk_Insert:
          {
            te()->SetInsMode(!(te()->GetEdState().ins_mode));
            te()->ChangeInsMode(te()->GetEdState().ins_mode);
            break;
          }

        default:
          {
            retval = te()->defaultKeyIn(key,shift);
            break;
          }
      }
    cmdCount = 1;
    return retval;
  }

//***********************************************************************
    DialogCmd vTEConfirm::ConfirmCmds[] =
      {
	{C_Frame,30,0,"",NoList,CA_NoSpace|CA_NoBorder,isSens,NoFrame,0,0},
	{C_Label, 31, 0,"Replace?",NoList,CA_None,isSens,30, 0, 0},
	{C_Button, YesRepl, 0, "Yes", NoList, CA_DefaultButton, isSens, 30, 31, 0, 0, "Replace this instance"},
	{C_Button, YesReplNext, 0, "Yes/Next", NoList, CA_None, isSens, 30, YesRepl, 0, 0, "Replace this instance, go on to next"},
	{C_Button, NoRepl,0," No",NoList,CA_None,isSens,30,31,YesRepl,0,"Don't replace"},
	{C_Button, NoReplNext,0," No/Next",NoList,CA_None,isSens,30,NoRepl,YesRepl,0,"Don't replace, go on to next"},

	{C_Button, QuitR, 0,"Stop",NoList,CA_None,isSens,30,31,NoRepl,0,"Stop Replace"},
	{C_Button, RepAll,0,"Yes/All",NoList,CA_None,isSens,30,QuitR,NoRepl,0,"Replace All"},
	

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
    };

// =====================>>> vTEConfirm::Confirm <<<====================
  int vTEConfirm::ConfirmRep()
  {
    int id, val;
    if (!added)
      {
	AddDialogCmds(ConfirmCmds);
	added = 1;
      }
    id = ShowModalDialog("Replace?",val);
    return id;
  }

//====================>>> vTEConfirm::DialogCommand <<<=======================
  void vTEConfirm::DialogCommand(ItemVal id, ItemVal val, CmdType ctype)
  {
    vModalDialog::DialogCommand(id,val,ctype);
    CloseDialog();
  }


//==================================================================
// I wrote the original version of the See editor, called TVX, in
// 1980 in a language called Ratfor, a preprocessor that gave
// Fortran the syntax of C. At that time, much of the TVX command
// structure was based on a Tenex editor called TV, which in turn
// had a command syntax similar to an editor called TECO. The Ratfor
// version of TVX actually worked quite will on a TRS-80 with 48K
// (that's right, 48K) of RAM. I've been using some decendent of
// TVX ever since. My fingers know the commands too well to
// change much.
//
// TVX was character based, and ported to many platforms,
// including CP/M, RT-11, RSX-11, VMS, and of course Unix. Much
// of the organization of the edit updates are designed to work
// over serial lines on a text terminal. This was accomplished
// using either a custom terminal driver, or a package such as
// TERMCAP on Unix. In fact, one could use the full screen editor
// quite well over even 1200 Baud serial lines (visual editors
// such as Vi really failed at much less than 9600 Baud).
// 
// TVX was translated (semi-automatically) from Ratfor to C. Most
// recently (about 1995) I converted it to C++, and finally to this
// C++ class for inclusion in the VGUI package. Somewhere along
// the way, I changed the name to See, which stood for Simple
// Enough Editor.
//
// Certainly, if I were designing an editor from scratch in C++,
// it would be quite different in structure than this editor.
// However, I always found it easier to port rather than rewrite.
// Many of the specialized editor commands I developed for See
// are still found in this code. A good emulation of many editor
// command sets can be implemented with different command
// parsers.
// 
// What this goes to show is that old code never dies, it just
// mutates. This code originated almost 20 years ago now. Many of
// the 6 character variable names owe their origin to Fortran
// variable name restrictions. Much of the seemingly complicated
// screen update stuff owes its origin to the restrictions of
// working with different terminals over slow baud rates. As
// time goes by, even this code will probably mutate.
//==================================================================
