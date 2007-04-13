//===============================================================
// vtexted.h - vTextEditor class definitions - based on vTextCanvas
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VTEXTED_H
#define VTEXTED_H

#include <v/vtextcnv.h>

// for X version
#ifndef V_EXPORT
#define V_EXPORT
#endif

    class V_EXPORT vCmdWindow;
    class V_EXPORT vTextEdCmdInterp;

    // These values may be used a the id paramater of the
    // EditCommand method, which will then carry out the
    // specfied command. The val parameter is used to
    // pass a count when necessary, and can usually be
    // a positive or negative value. Thus edCharRight with
    // a positive val moves right; with a negative val, left.
    enum                        // editor commands
      {
        edBalMatch = 10000,     // find matching paren (if > 1, up to val lines away)
        edBufferBottom,         // move to bottom of file (no val)
	edCenterScreen,		// center cursor in display window
        edCharDelete,           // delete +/- val chars
        edCharFoldCase,         // swap case of +/- val letters
        edCharInsert,           // insert char val
        edCharRight,            // move +/- val chars right
        edCut,                  // Cut
        edCopy,                 // Copy
        edPaste,                // Paste
        edFill,                 // fill current line
        edFind,                 // invoke TextEd's find dialog (no val)
        edFindNext,             // find next occurrence of prev (no val)
	edHelp,			// show help screen
	edIndent,		// autoindent if on
        edLineBeginning,        // move to line beginning (no val)
        edLineDown,             // move down +/- val lines in column
        edLineDownBeg,          // move down +/- val lines
        edLineDelete,           // delete +/- val lines
        edLineDeleteFront,      // delete to beginning of line (no val)
        edLineDeleteToEnd,      // delete to end of line (no val)
        edLineEnd,              // move to end of line (no val)
        edLineGoto,             // move cursor to line val
        edLineOpen,             // open val new blank lines
	edFormatC,		// format a C Line
        edNoteLocation,         // note which line we are on
        edNoteGoto,             // goto noted locateion
        edScrollDown,           // scroll val lines without changing cursor
	edReplace,		// Find/Replace
	edUndo,			// UnDo the last edit
	edUnDoMove,		// UnDo the last move
        edVerify,               // force repaint of screen (no val)
        edWordRight,            // move cursor +/- val words right
        edNoOp
      };

    enum FileType
      {
	Text,
	CPP,
	HTML,
	Java,
	TeX,
	gccError,
	bccError,
	javaError,
	Perl,
	Fortran,
	Lisp,
	XML,
	Python,
	Tcl,
	Makefile,
	Shellscript
      };

    enum ReplaceConfirm
      {
	YesRepl = 800, 		// Replace this instance
	YesReplNext,		// Replace, go to next
	NoRepl, 		// Don't replace
	NoReplNext,		// Don't replace, goto next
	QuitR,			// Quit replacing
	RepAll			// Quit confirming, replace all
      };

    const int MAX_LINE = 500;   // Max line length we will handle
    const int MAX_UNDO = 4094;

    typedef char* BUFFPTR;

    typedef struct MARK_RANGE
      {
        long beg_lin;           /* first line of marked range */
        long end_lin;           /* last line of marked range */
        int beg_col;            /* col where first line begins */
        int end_col;            /* col where last line ends */
        BUFFPTR beg_chr;                /* first chr */
        BUFFPTR end_chr;
      } MARK_RANGE;

    typedef struct edState
      {
        long changes;           // count of changes

        FileType ftype;         // what kind of file type
        int
	    autoIndent,		// autoindent flag
            counter,            // counter for + insert
            echof,              // whether or not to echo action
            ins_mode,           // true if insert mode
	    delOnInsert,	// true if delete selection on insert
            readOnly;           // true if read only
      } edState;

    typedef struct globalState
      {
        int
            findAtBeginning,    // leave find at beginning of pattern
            fixed_scroll,       // flag if using fixed scroll
	    delOnInsert,	// true if delete selection on insert
            tabspc,             // tab spacing
            wraplm,             // right limit
	    braceIndent;	// indenting for {}'s
      } globalState;


#include <v/vmodald.h>

    class V_EXPORT vTEConfirm : protected vModalDialog
      {
	public:
	    vTEConfirm(VCONST vCmdWindow *bw, VCONST char* title = "Replace?"):
		vModalDialog((vBaseWindow*)bw,title) {added = 0;}
	    ~vTEConfirm() {}
	    int ConfirmRep();

	protected:
	    
	    static DialogCmd ConfirmCmds[];
	    
	private:
	    virtual void DialogCommand(ItemVal id, ItemVal val, 
		CmdType ctype);
	    int added;
      };

    class V_EXPORT vTextEditor : public vTextCanvasPane          // The main way to deal with a file
      {
  
        public:           //---------------------------------------- public
            vTextEditor(VCONST vCmdWindow* parent);
            ~vTextEditor();     // Destructor

            virtual void resetBuff();                           // open the buffer
            virtual int addLine(char* line);                    // add a line to end of buffer
            virtual int getFirstLine(char* line, int maxChars); // first line in buffer
            virtual int getNextLine(char* line, int maxChars);  // next line in buffer, -1 = END
            virtual int getLine(char* line, int maxChars, long lineNum); // retrieve given line
            virtual void displayBuff(long lineNum = 1, int paintIt = 1);
                      // finished with buffer
            virtual int insertLine(char* line, long before, bool doUndo = true);
            virtual void adjustLines(long line_1, int by);

            // Editor command interpreter

            virtual int EditCommand(int id, long val);
            virtual int EditKeyIn(vKey key, unsigned int shift);
            void ChangeCmdInterp(vTextEdCmdInterp* newCI);
            vTextEdCmdInterp* GetCmdInterp() VCONST { return _teCInt; }

            // State Notification

            virtual void ChangeLoc(long line, int col) {}  // just state change
            virtual void ChangeInsMode(int IsInsMode, char* msg = 0) {}
            virtual void StatusMessage(char *msg) {}
            virtual void ErrorMsg(char *str) {};
            globalState GetGlobalState() VCONST { return gState; }
            void SetGlobalState(globalState setGState) { gState = setGState; }
            edState GetEdState() VCONST { return state; }
            void SetEdState(edState setState) { state = setState; }
            void SetRdOnly(int ro) {state.readOnly = ro;}
            void SetInsMode(int ins) {state.ins_mode = ins;}
            void SetEchoF(int e) {state.echof = e;}
	    void SetAutoIndent(int ai) {state.autoIndent = ai;}
            FileType GetFileType() VCONST { return state.ftype; }
            void SetFileType(FileType ft) {state.ftype = ft; }
            long JLine() VCONST {return b_lastln; }
            void SetJLine(long j) {b_lastln = j;}
            void SetFindPat(char* newpat)
               {if (strlen(newpat) < MAX_LINE) strcpy(theFindPat,newpat);  }
            char* GetFindPat() VCONST { return theFindPat; }

            long GetLines() VCONST { return lastLine; }
            long GetCurLine() VCONST { return curlin; }
            int getColPos() {return col_pos(curchr);}
            int OldLen() VCONST { return oldlen; }
            void SetOldLen(int val) {oldlen = val; }
            int Changed() VCONST { return state.changes > 0; }
            void IncChanges() { ++state.changes; };
            void Verify(void);                  // repaint screen

    // *******  Methods used by command interpreters ************

            // Character oriented methods

            int charDelete(long cnt, bool doUndo = true,
		bool removeRange = true); // delete next cnt chars
            int charFoldCase(long cnt); // swap case of letter
            int charInsert(int ival, bool doUndo = true); // forced insert
            int charRight(long cnt, int clear_mark = 1); // move char right

            // line oriented methods

            void lineAutoFill(void);            // automatic filling
            void lineBeginning();               // move to line beginning
            int lineDown(long cnt, int clrRange = 1);// move down cnt lines
            int lineDownBeg(long cnt, int notify = 1,
                            int clrRange = 1); // move down cnt lines
            void lineDelete(long cnt, bool doUndo = true); // delete cnt lines
            int lineDeleteFront(bool doUndo = true);	// delete to beginning of line
            int lineDeleteToEnd(bool doUndo = true);	// delete to end of line
            void lineEnd(int clrRange = 1);     // move to end of line
            int lineFill(long count);           // fill count lines
            int lineGoto(long cnt);             // move cursor to line cnt
            int lineOpen(long cnt);             // open new blank line

	    int formatC(long count);

	    // misc 
	    int autoIndent();
            int BalMatch(long val);             // find matching paren
	    void CenterScreen();            
            int wordRight(long cnt);
            void scrollDown(long delta);
	    int getSelection(char* sel, int max);
	    void setSelection(int howmuch);
            int EditCut();
            int EditCopy();
            int EditPaste();
            int Find(char* pat, int caseSensitive = 0, int Down = 1, int Wrap = 0);
            int FindNext(int caseSensitive = 0, int Down = 1, int Wrap = 0);
            int Replace(const char* pat, const char* repl, int caseSensitive = 0,
		int Down = 1, int Wrap = 0, int Confirm = 0, int replaceAll = 0);
            int undoEdit(void);
            void bufferBottom(void);            // move to bottom of file
            void newscr(void);

            void ClearMarkRange(void);
            int RemoveMarkRange(bool doUndo = true);

	    int insertToRange(char* atFront, char* atEnd);
            void addToRange(long rows, long cols); // add to mark range

	    void highLightCurLine();

            virtual int defaultKeyIn(vKey key, unsigned int shift);

            virtual void TextMouseDown(int row, int col, int button);
            virtual void TextMouseUp(int row, int col, int button);
            virtual void TextMouseMove(int row, int col, int button);

            int mint(int v1, int v2);
            int maxt(int v1, int v2);
            long minl(long v1,long v2);
            long maxl(long v1, long v2);
            int clower(int ch);
            int cupper(int ch);

        protected:        //--------------------------------------- protected

            virtual void initBuff(); // create buffers
            virtual BUFFPTR GLine(long lineNum);
            virtual int GCh(BUFFPTR charNum) VCONST { return *(charNum); }
            virtual BUFFPTR deleteCharBF(BUFFPTR charNum, long lineNum);
            virtual long deleteLinesBF(long start, long end);
            virtual int insertCharBF(int chr, BUFFPTR& curchr, long& curlin);
            virtual int lineLenBF(long lineNum);
            virtual long lastLineBF() VCONST { return _nextLine - 1; } // last line in buff

            // Scrolling

            virtual void HPage(int, int);
            virtual void VPage(int, int);

            virtual void HScroll(int);
            virtual void VScroll(int);

            // Events

            virtual void FontChanged(VCONST vFont& newFont);
            virtual void ResizeText(const int rows, const int cols);
            virtual void Redraw(int x, int y, int width , int height);

	    // utilities for formatC
	    int has_key(char *buff_ptr, char *key);
	    int is_key_word(char *bf, int case_def);
	    long get_prev(char *prev_buff, long start);

            // misc methods

            int SelectionLen();                 // get len of marked range
            void AddToRangeRow(long cnt); // add to mark range
            void AddToRangeCol(int col); // add to mark range
            int CopySelection(char* buff, int max, int unmark);

	    void setHighlight(ChrAttr hl) {_highlighter = hl;}
	    ChrAttr getHighlight() {return _highlighter;}

            virtual void paintLine(char* linout, int lineStart, 
                int hiStart, int hiLast, long lineNum);
           
            vCmdWindow* _parent;

            edState state;                      // state stuff for outside
	    static globalState gState;
            static int findCaseSensitive;
            static char theFindPat[MAX_LINE + 2]; // one static find pattern
            static char theReplPat[MAX_LINE + 2]; // one static repl pattern
	    static int findDown;
	    static int findWrap;
    	    static int replaceConfirm;
    	    static int replaceAll;

        private:          //--------------------------------------- private

            int IsEndLine(int chr) VCONST { return (chr == '\n' || chr == '\r'); }
            void tvhdln(void);
            void setScrollBar();
            void checkIfScrolled();
            void FindDispLine(long& ibeg, int& cnt);
            int CombineLines(void);
            int findX(void);
            int line_can_fit(long l);
            int Fill1(int dir, int val);
            void range(long cnt,long *lbeg, long *lend);
            void updateScroll(long change);
            void updateNoScroll(long change);
            void getCurLineFront(char* buff, long start);
	    bool isSpecial(char* curline);
            BUFFPTR lastBX(long line);
            void newScrollScreen(long ibeg);
            bool SaveKillLine(long cnt, bool partialLine = false);
            void tvtyln(long lineNum,BUFFPTR chrptr, int whole_line);
            int IsWordChar(int chr);
            void update(long change);
            void MouseMoveTo(int row, int col);
            int reallocLines();
            void retypeCurlin(int eraseLine=0);

	    void undoOn(const char* msg);
	    void undoOff(const char* msg);

            // Screen manip

            void tvxy(int ix, int iy);
            void tvplin(long lineNum, BUFFPTR chrptr, int whole_line, int hibeg, int hiend);
            void type_lines(long ibeg, int icnt, int wholeLine = 0);
            void tvbotb(int n);
            void tvclr(void);
            void tvelin(void);
            void tvescr(void);
            void tvtopb(int n);

            // Utils
            int col_pos(BUFFPTR chr_pos, int do_shift = 0 );

            BUFFPTR appendToBuff(char* line);

            // Operating state - type of scrolling, etc.

            // variables -------------------------------------------
            int
                ddline,         // default display line
                dsplin,         // the default display line
                last_col_out,   // last column output
                leftmg,         // left margin of display
                linptr,         // used to build output lines
                mouseCol,       // where mouse is (on down)
                mouseRow,
                mouseMoveCol,
                mouseMoveRow,
                oldcol,         // beginning column for dwncol
                oldlen,         // length for '=' command
		setCursor,	// if need to reset cursor after resize
                tvdlin,         // the "active" display line
                tvx,            // current x cursor position
                tvy,            // current y cursor position
                use_wild,       // use wild cards in match
                wasColCmd,      // if last command was a column
                xoutcm;         // used for 240 col virtual screen

            long
                b_lastln,       // last line for JUMP
                curlin,         // pointer to current line
                noteloc[26],    // note locations 0-25 (or a-z)
                lastLine,       // last line in file
                scroll_lin;     // for scrolling commands

            BUFFPTR
                curchr,         // pointer to current character
                scroll_chr;     // for scroll commands

            MARK_RANGE mark;    // marked portion of this

            BUFFPTR* _lines;

            long _nextLine;     // index into line array
            long _curLineBF;    // current line for getXLine
            long _maxLines;     // size of line array

	    ChrAttr _highlighter;

	    // UnDo stuff
            char* undoBuff;	// one line of unkill
	    int undoCurcol;	// column of current character
	    long undoCurlin;
	    bool undoPartialLine;  // if a partial line saved
	    bool undoKillLast;


	    // objects used by editor

            vTextEdCmdInterp* _teCInt;
	    vTEConfirm* myConfirm;

      };

     class V_EXPORT vTextEdCmdInterp     // easy class to override
      {

        public:
          vTextEdCmdInterp(VCONST vTextEditor* textEd, VCONST vCmdWindow* cw);
          virtual ~vTextEdCmdInterp();


          virtual int ProcessKey(vKey key, unsigned int shift);
          virtual vTextEditor* te() VCONST { return _textEd; }
          virtual vCmdWindow* cmdWin() VCONST { return _myCmdWindow; }
          virtual void InitCmdInterp();
          virtual void CmdInterpHelp();
	  virtual void MouseJump(int row, int col) { /* no-op by default */ }

	protected:

	    long cmdCount;	// how many times to repeat command

	    int
	    	countWait,	// if building a count
		metaChar,	// the meta char for commands
		metaWait;	// if waiting on a meta command

        private:
	    vTextEditor* _textEd;
	    vCmdWindow* _myCmdWindow;
      };

#endif
/* ************************************************************************ */
