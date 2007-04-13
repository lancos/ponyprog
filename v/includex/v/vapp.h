//================================================================
// vapp.h - the vapp base object - X11R5 version
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

extern "C"		// wrap in "C"
{
#include <X11/Core.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
}


    extern int AppMain(int, char**);

    class vWindow;

    typedef struct WindList
      {
	vWindow* window;
	vAppWinInfo* info;
	WindList* nextWinList;
      } WindList;

#ifndef Athena
    typedef struct TaskList
      {
	vWindow* window;
	ItemVal cmd;
	TaskList* nextTask;
      } TaskList;
#endif

    class _appWorkTimer : public vTimer
      {
      public:		//---------------------------------------- public
	_appWorkTimer() {}
	~_appWorkTimer() {}
	virtual void TimerTick();
      private:		//--------------------------------------- private
      };

    class vApp : public vBaseItem
      {
	// give friend access to some special things
	friend class vWindow; // Provide access for vWindow class
	friend int main(int, char**);	// give main access
	friend class _appWorkTimer;

      public:		//---------------------------------------- public

	vApp(VCONST char* appName, int simSDI = 0, int fh = 0, int fw = 0);	// constructor
	virtual ~vApp()	// destructor
	  {
	    SysDebug(Destructor,"vApp::~vApp destructor\n")
	    if (theClipboard != 0)
		delete [] theClipboard;		// free last chunk
	  }

	// Methods to override

	virtual void AppCommand(vWindow* win, ItemVal id, ItemVal retval, CmdType ctype);
	virtual void CloseLastCmdWindow(vWindow* win, int exitcode);
	virtual int CloseAppWin(vWindow* win);
	virtual int CloseHelpWin(vWindow* win);
	int IsHelpWin(vWindow *Win);
	virtual void Exit(void);
	virtual void KeyIn(vWindow* win, vKey key, unsigned int shift);
	virtual vWindow* NewAppWin(vWindow* win, VCONST char* name, int w, int h, 
		vAppWinInfo* winInfo = 0);
	virtual  vWindow* NewHelpWin(vWindow* win, VCONST char* name, int w, int h);
	virtual void DropFile(const char* fn) {}


        int ClipboardSetText(VCONST char* text, int DoXOwn = 1) VCONST;
        char* ClipboardGetText() VCONST;
        int ClipboardCheckText() VCONST;
        void ClipboardClear() VCONST;

	// Tasking
	void CheckEvents();

	int EnableWorkSlice(long slice);
	virtual void WorkSlice() {}	// No op by default

	// Utlity methods

	void showAppWin(bool show) {}	// No op in X
	void GetVVersion(int& major, int& minor)
	   { major = V_VersMajor; minor = V_VersMinor; }
	int DefaultHeight() { return _DefaultHeight; }
	int DefaultWidth() {return _DefaultWidth; }
	int ScreenHeight() { return _DisplayHeight; }
	int ScreenWidth() {return _DisplayWidth; }
	int IsRunning() {return _running;}  // see if app is running
	vFont GetDefaultFont();
	void SendWindowCommandAll(ItemVal id, int val, CmdType ctype);
	void UpdateAllViews(vWindow* sender = NULL, int hint = 0, 
		void* pHint = NULL);
	void SetValueAll(ItemVal id, int val, ItemSetType setType) VCONST;
	void SetAppTitle(VCONST char* title);
	void SetStringAll(ItemVal id, VCONST char* str);
	int ShowList();
	vAppWinInfo *getAppWinInfo(vWindow* Win);

	// Required internally
	// These need to be global, sorry...

#ifndef Athena
	Boolean MotifIdleWork(XtPointer client_data);
	int MotifSetTask(vWindow* vw, ItemVal id);
#endif
	int XCheckSelection();
	void XWaitUntilMapped(Widget w);

	Atom vx_wm_protocols;		// Stuff for X protocols
	Atom vx_wm_delete_window;

	vMenuPane* GetMenuThis(Widget w);	// needed for submenus

	int simSDI() { return _simSDI;}
	// X stuff
	Display* display()	// To get the X display
	  { return _display; }
	Window xWindow();

	XtAppContext appContext()	// To get the context
	  { return _appContext; }

	int Xdepth() {return _depth;}
	Pixel Xfg() {return _foregroundPix;}
	Pixel Xbg() {return _backgroundPix;}
	Colormap Xcolormap() {return _colormap;}

	XFontStruct* _XDefaultFont;	// set by default or -fn switch
	void doXEventLoop();

      protected:	//--------------------------------------- protected

	WindList* _WindowList;		// List of "top level" windows

	int _running;			// if we are running

	void registerWindow(vWindow* Win, vAppWinInfo* awinfo);
	void unregisterWindow(vWindow* Win);

	// The following is called from main.
	void initialize(int& argc, char** argv);

      private:		//--------------------------------------- private

	int _frameWidth, _frameHeight;
	int _DefaultHeight;
	int _DefaultWidth;
	int _simSDI;		// not used in X, but in windows

        XtWorkProcId work_id;

#ifndef Athena
	TaskList* _taskList;
#endif
	_appWorkTimer* _workTimer;	// timer for work slice

        void DispatchWork(void);

	// X stuff
	Display* _display;		// Needed by X sometimes
	Window _xwindow;		// Window of root window
	XtAppContext _appContext;	// Main app context
	Colormap _colormap;
	Pixel _foregroundPix;		// foreground pixel value
	Pixel _backgroundPix;   	// background pixel value
	int _depth;			// depth
	int _DisplayHeight;		// height of display
	int _DisplayWidth;		// width of display

	static const int ClipAllocUnit = 1024;
	VMUTABLE int ClipMax;
	VMUTABLE char* theClipboard;


      };

    extern vApp *theApp;		// Pointer to single global instance
#endif
