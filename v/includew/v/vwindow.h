//===============================================================
// vWindow.h - vWin class definitions - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VWINDOW_H
#define VWINDOW_H

#include <v/v_defs.h>
#include <v/vbasewin.h>		// we are derived from vBaseWindow
#include <v/vapp.h>		// we are a friend of the app class
#include <v/vcanvas.h>		// need for SetWinCursor

//#include <v/vmenu.h>	// so we can get to GetMenuThis

    // We will allow a Button Bar and a Status Bar in Windows

    class V_EXPORT vPane;

    typedef struct PaneList
      {
	vPane* pane;		// pointer to the pane object
	PaneList *nextPL;	// next pane
      } PaneList;

    class V_EXPORT vWindow : public vBaseWindow
      {

	friend class V_EXPORT vPane;
        friend class V_EXPORT vMenuPane;
	friend class V_EXPORT vCanvasPane;

      public:		//---------------------------------------- public
	vWindow(VCONST char *name = "+", int width = 0, int height = 0,
	  WindowType wintype = WINDOW);	// Constructor

        vWindow(const vWindow&);		// Copy Constructor

	virtual ~vWindow();		// Destructor

	// Methods to Override

	virtual void KeyIn(vKey keysym, unsigned int shift);  // KeyIn method
	virtual void MenuCommand(ItemVal id);
	virtual void WindowCommand(ItemVal id, ItemVal retval, CmdType ctype);
        virtual void WorkSlice(void) {};	// Work - default: no op

	// Utility Methods

	virtual void AddPane(vPane* add_pane);	// add a pane to the window
	int CreatedOK() { return _createdOK;}
	virtual int GetValue(ItemVal id) VCONST;
	virtual int GetTextIn(ItemVal id, char* val, int maxlen) VCONST;
  	virtual void RaiseWindow(void);	// raise this window
	virtual void SetValue(ItemVal id, int val, ItemSetType setType);
	virtual void SetString(ItemVal id, VCONST char* str);
	virtual void SetValueAll(ItemVal id, int val, ItemSetType setType);
	virtual void SetStringAll(ItemVal id, VCONST char* str);
	virtual void SetTitle(VCONST char* title);
	void ShowPane(vPane* wpane, int OnOrOff) VCONST;
	virtual void UpdateView(vWindow* sender, int hint, void* pHint);


	virtual void GetPosition(int& left, int& top, int& right,
		int& bottom) VCONST;
	virtual void SetPosition(int left, int top);

	// Other Methods

	virtual void CloseWin(void);	// callback for close button

		// Internal Methods
	long MdiWindowProc(HWND hWnd, UINT message, 
		WPARAM wParam, LPARAM lParam);

	int getWinHeight() VCONST { return _WinHeight; }
	int getWinWidth() VCONST { return _WinWidth; }
	void setWinHeight(const int h) { _WinHeight = h; }
	void setWinWidth(const int w) { _WinWidth = w; }

      protected:	//--------------------------------------- protected

	int _WinHeight;		// height of draw window
	int _WinWidth;		// width of draw window

      private:		//--------------------------------------- private

	virtual void initialize(void);
	void SetWinCursor(VCursor vcursor);

	PaneList* _paneList;		// panes for this window
        vPane* _menuPane;		// our menu pane 

        vCanvasPane* _canvasPane;	// our one canvas pane

	vWindow* _NextWin;		// link to next window

	void KeyInEV(unsigned int keycode, int map, int extended);
	int _shiftKeyDown;
	int _ctrlKeyDown;

        int _createdOK;
	
	HMENU _menuBar;
	int _WindowMenuIndex;		// number of the Windows menu
	int _justActivated;		// for mouse control
	int _mouseDown;			// to track mouse up/down
        HCURSOR _cursor;		// mouse cursor

	static int _numWindows;	// how many total windows are up

	static vWindow* _WinList;	// all windows
	static HWND _curWin;
	static HWND _prevWin;


      };
#endif
