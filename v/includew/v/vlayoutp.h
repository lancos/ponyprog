//===============================================================
// vlayoutp.h - Layout Pane C++ class .h file - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VLAYOUTP_H
#define VLAYOUTP_H

#include <v/v_defs.h>		// Basic defs
#include <v/vpane.h>		// we are derived from vPane

    class vWindow;		// we are part of a window

    class vLayoutPane : public vPane
      {
        friend vWindow;
      public:		//---------------------------------------- public

	vLayoutPane(PaneType pt = P_Canvas);

 	virtual ~vLayoutPane();

	virtual void ShowPane(int OnOrOff);

        void SetWidthHeight(int width, int height);


	// Events
	virtual void MouseDown(int x, int y, int button) {} // Mouse Down at x,y
	virtual void MouseUp(int x, int y, int button) {}	  // Mouse Up at x,y
	virtual void MouseMove(int x, int y, int button) {} // Mouse Moved to x,y
	virtual void MouseMotion(int x, int y) {} 	// No Button Mouse Move

	virtual void EnterFocus(void);
	virtual void LeaveFocus(void);

	// Expose/redraw event
	virtual void Redraw(int x, int y, int width , int height);
	virtual void Resize(int newW, int newH);  // Resize event

	// Information
	virtual int GetWidth() { return _width; }   // Get width of canvas
	virtual int GetHeight() { return _height; } // Get Height of canvas

	// Appearance

	// Platform dependent


      protected:	//--------------------------------------- protected

	virtual void EnterEV(void);
        virtual void LeaveEV(void);

	virtual void initialize(vWindow* pWindow, HWND pWidget);

	int _height;	// height of draw window
	int _width;	// width of draw window

        HWND _drawWindow;		// window for drawing

        int _hasFocus;			// for focus workaround

      private:		//--------------------------------------- private

	// Widgets for the canvas


	static VCursor _currentCursor;	// for cursor

	void HScrollEV(int code, int pos, HWND control) {}
	void VScrollEV(int code, int pos, HWND control) {}

	void ResizeEV(int w, int h);
	void ExposeEV(void);
      };
#endif
