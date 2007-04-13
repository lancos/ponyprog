//===============================================================
// vapp.h - the vapp base object - Windows version
//
// Copyright (C) 1995-1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VAPP_H
#define VAPP_H

#include <v/vbaseitm.h>	// our parent class
#include <v/vfont.h>	// our parent class
#include <v/vawinfo.h>
#include <v/vmenu.h>
#include <v/vtimer.h>

#define MaxAppFonts FontIdsUsed

// DLL definitions - beginning with V 1.24 and mingw 2.95.2-1
// These definitions contributed by Franco Bez <franco.bez@gmx.de>
#if defined(USE_V_DLL) && !defined(BUILD_V_DLL)
#if defined (__MINGW32__) || defined(__CYGWIN32__)
    // This forces the GNU Linker to include vstartup.o in the executable
    // Only needed here for DLL use, it must be in one of the Applications 
    // object files for the static library -> see vApp.cpp
    // bug in mingw32 and cygwin loader
    static int (*gnuw32dummy)(HINSTANCE,HINSTANCE,LPSTR,int)=WinMain;
#endif
#endif

    class V_EXPORT vWindow;
    class V_EXPORT vCmdWindow;
    class V_EXPORT vCommandPane;

    typedef struct WindList
      {
	vWindow* window;
	vAppWinInfo* info;
	WindList* nextWinList;
      } WindList;

    typedef struct CmdPaneList
      {
	vCommandPane* commandPane;
	CmdPaneList* nextCPList;
      } CmdPaneList;

    long CALLBACK PMdiFrameProc(HWND, UINT, UINT, LPARAM);
    long CALLBACK PMdiWindowProc(HWND, UINT, UINT, LPARAM);

    class V_EXPORT _appWorkTimer : public vTimer
      {
      public:		//---------------------------------------- public
	_appWorkTimer() {}
	~_appWorkTimer() {}
	virtual void TimerTick();
      private:		//--------------------------------------- private
      };

    class V_EXPORT vApp : public vBaseItem
      {
	// give friend access to some special things
	friend class V_EXPORT vWindow; // Provide access for vWindow class
        friend class V_EXPORT vCmdWindow; // Provide access for vWindow class
	friend class V_EXPORT vMenuPane; // Menu Pane needs access
	friend class V_EXPORT vCommandPane;
	friend V_EXPORT int CMain(HANDLE hInstance, HANDLE hPrevInstance,
		      LPSTR lpszCmdLine, int nCmdShow);	// give main access
	friend long WINAPI V_EXPORT PMdiFrameProc(HWND, UINT, UINT, LPARAM);
	friend long WINAPI V_EXPORT PMdiWindowProc(HWND, UINT, UINT, LPARAM);
	friend class _appWorkTimer;
	

      public:		//---------------------------------------- public

	vApp(VCONST char* appName, int simSDI = 0, int frameHeight = 0, int frameWidth = 0);	// constructor
	virtual ~vApp();	// destructor
	  
	// Methods to override

	virtual void AppCommand(vWindow* win, ItemVal id, ItemVal retval,
				CmdType ctype);
	virtual void CloseLastCmdWindow(vWindow* win, int exitcode);
	virtual int CloseAppWin(vWindow* win);
	virtual int CloseHelpWin(vWindow* win);
		int IsHelpWin(vWindow *Win);
	virtual void Exit(void);
	virtual void KeyIn(vWindow* win, vKey key, unsigned int shift);
	virtual vWindow* NewAppWin(vWindow* win, VCONST char* name, 
		int w = 0, int h = 0, vAppWinInfo* winInfo = 0);
	virtual  vWindow* NewHelpWin(vWindow* win, VCONST char* name, 
		int w, int h);

	virtual void DropFile(const char *fn);

	// Utlity methods

	void showAppWin(bool show);

	vFont GetDefaultFont();

	void GetVVersion(int& major, int& minor)
           { major = V_VersMajor; minor = V_VersMinor;}
	int DefaultHeight() { return _DefaultHeight; }
	int DefaultWidth() {return _DefaultWidth; }
	int IsRunning() {return _running;}  // see if app is running
        void SendWindowCommandAll(ItemVal id, int val, CmdType ctype);
	void UpdateAllViews(vWindow* sender = NULL, int hint = 0, 
		void* pHint = NULL);
	void SetValueAll(ItemVal id, int val, ItemSetType setType);
        void SetAppTitle(VCONST char* title);
	void SetStringAll(ItemVal id, VCONST char* str);
	int ShowList();
	vAppWinInfo *getAppWinInfo(vWindow* Win);
	void AppExit(int exitVal);
	int InExit() { return _inExit; }


        int ClipboardSetText(VCONST char* text) VCONST;
        char* ClipboardGetText() VCONST;
        int ClipboardCheckText() VCONST;
        void ClipboardClear() VCONST;

	// Tasking
	void CheckEvents();

	int EnableWorkSlice(long slice);
	virtual void WorkSlice() {}	// No op by default

	// Windows stuff

	HWND winHwnd() VCONST { return _Frame; }
	HWND winClientHwnd() VCONST { return _Client; }
        int simSDI() VCONST {return _simSDI;}

	HMENU AppFrameMenu() {return hFrameMenu;}

	HPEN _WhitePen;
	HPEN _GrayPen;		// for drawing stuff
	HPEN _LightPen;
        HPEN _ShadowPen;
	HBRUSH _BarBrush;


      protected:	//--------------------------------------- protected

	WindList* _WindowList;		// List of "top level" windows
	CmdPaneList* _CmdPaneList;	// List of command panes


	int _running;			// if we are running

	// The following two are called from main.
	void initialize(int& argc, char** argv,
		HANDLE hInstance, HANDLE hPrevInstance, int nCmdShow);
	int doEventLoop();

      private:		//--------------------------------------- private

	int _DefaultHeight;
	int _DefaultWidth;
        int _frameWidth, _frameHeight;
	int _inExit;		  	// If in exit procudure
	int _simSDI;			// if Simulate SDI interface

	WindList* _DeleteList;		// List of windows to delete

        VMUTABLE char* _clipText;

	_appWorkTimer* _workTimer;	// timer for work slice

	void addToDeleteList(vWindow *Win);
	void registerWindow(vWindow* Win, vAppWinInfo* awinfo);
	void unregisterWindow(vWindow* Win);
	void DispatchWork(void);

	void registerCmdPane(vCommandPane* cmdPane);
	void unregisterCmdPane(vCommandPane* cmdPane);
	void selectCmdPanes(vWindow* parent);

	// Windows stuff

        long MdiFrameProc(HWND hwnd, UINT message,
				UINT wParam, LPARAM lParam);
	void OnDnD(HWND hWnd, HDROP wParam);
	int OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct);
	void OnClose(HWND hwnd);
	int OnQueryEndSession(HWND hwnd);
	void OnDestroy(HWND hwnd);
	void OnSysCommand(HWND hwnd, UINT cmd, int x, int y);
	void OnSize(HWND hwnd, UINT state, int cx, int cy);
	void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	LRESULT Frame_DefProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnResizeMDIClient(HWND hwnd);

	HMENU hFrameMenu, hFrameSubMenu;	// Default menu
	HWND  _Frame, _Client ;		// Need these for MDI frame
	HANDLE hAccel;
	vWindow* _curThis;	// Needed to fake things out at the start

        HICON _appicon, _winicon; 
      };
// theApp is accessed by a function call.
//
    V_EXPORT DWORD vGetApp();
#define theApp ((vApp*)vGetApp())

//    extern vApp *theApp;		// Pointer to single global instance
#endif
