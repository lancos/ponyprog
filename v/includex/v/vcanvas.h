//===============================================================
// vcanvas.h Canvas C++ class .h file - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VCANVAS_H
#define VCANVAS_H

#include <v/v_defs.h>		// Basic defs
#include <v/vpane.h>		// we are derived from vPane
#include <v/vcpdc.h>		// we also derive from vDrawingCanvasDC
#include <v/vfont.h>		// for font stuff

extern "C"
{
#include <X11/Xlib.h>
#include <X11/cursorfont.h>

#ifndef Athena
    void CinputCB(Widget w, XtPointer client_date, XtPointer call_data);
    void CexposeCB(Widget w, XtPointer client_date, XtPointer call_data);
    void CresizeCB(Widget w, XtPointer client_date, XtPointer call_data);
#else	// ## Athena
    void CchangeCB(Widget w, XtPointer client_data, XtPointer call_data);

    void CExposeEV(Widget w, XtPointer client_data, XEvent* event, char *x);

    void CMouseDownEV(Widget w, XtPointer client_data, XEvent* event, char *x);
    void CMouseUpEV(Widget w, XtPointer client_data, XEvent* event, char *x);
    void CMouseMoveEV(Widget w, XtPointer client_data, XEvent* event, char *x);
#endif

    // Common to both Motif and Athena
    void CHJumpProcCB(Widget w, XtPointer This, XtPointer pc_ptr);
    void CHScrollProcCB(Widget w, XtPointer This, XtPointer position);
    void CVJumpProcCB(Widget w, XtPointer This, XtPointer pc_ptr);
    void CVScrollProcCB(Widget w, XtPointer This, XtPointer position);
    void CEnterEV(Widget w, XtPointer client_data, XEvent* event, char *x);
    void CLeaveEV(Widget w, XtPointer client_data, XEvent* event, char *x);

}

    typedef int VCursor;		// For cursors

// Define Cursor stuff

const VCursor VC_None = 0;		// No/Default cursor
const VCursor VC_Arrow = 1;		// XC_top_left_arrow
const VCursor VC_CenterArrow = 2;	// XC_center_ptr
const VCursor VC_CrossHair = 3;		// XC_crosshair
const VCursor VC_EWArrows = 4;		// XC_sb_h_double_arrow
const VCursor VC_Hand = 5;		// XC_hand2
const VCursor VC_IBar = 6;		// XC_xterm
const VCursor VC_Icon = 7;		// XC_icon
const VCursor VC_NSArrows = 8;		// XC_sb_v_double_arrow
const VCursor VC_Pencil = 9;		// XC_Pencil
const VCursor VC_Question = 10;		// XC_question_arrow
const VCursor VC_Sizer = 11;		// XC_sizing
const VCursor VC_Wait = 12;		// XC_watch
const VCursor VC_X = 13;		// XC_X_cursor
const VCursor VC_LAST = 13;
const int maxCursors = 13;

    class vWindow;		// we are part of a window
    class vMemoryDC;

    class vCanvasPane : public vPane
      {
#ifndef Athena

	friend void CinputCB(Widget w, XtPointer client_date, XtPointer call_data);
	friend void CexposeCB(Widget w, XtPointer client_date, XtPointer call_data);
	friend void CresizeCB(Widget w, XtPointer client_date, XtPointer call_data);

#else		// Athena
	friend void CchangeCB(Widget w, XtPointer client_data, XtPointer call_data);
	friend void CExposeEV(Widget w, XtPointer client_data, XEvent* event, char *x);

	friend void CMouseDownEV(Widget w, XtPointer client_data, XEvent* event, char *x);
	friend void CMouseUpEV(Widget w, XtPointer client_data, XEvent* event, char *x);
	friend void CMouseMoveEV(Widget w, XtPointer client_data, XEvent* event, char *x);
#endif
	// Common to both Motif and Athena

	friend void CHJumpProcCB(Widget w, XtPointer This, XtPointer pc_ptr);
	friend void CHScrollProcCB(Widget w, XtPointer This, XtPointer position);
	friend void CVJumpProcCB(Widget w, XtPointer This, XtPointer pc_ptr);
	friend void CVScrollProcCB(Widget w, XtPointer This, XtPointer position);

	friend void CEnterEV(Widget w, XtPointer client_data, XEvent* event, char *x);
	friend void CLeaveEV(Widget w, XtPointer client_data, XEvent* event, char *x);


      public:		//---------------------------------------- public

	vCanvasPane(PaneType pt = P_Canvas);

 	virtual ~vCanvasPane();

	virtual void ShowPane(int OnOrOff) VCONST;

	void SetWidthHeight(int width, int height);
	// Cursor
	void SetCursor(VCursor id);
        VCursor GetCursor() { return _currentCursor; }

	void UnSetCursor(void);
	void CompressRedraw(int val) { _compressRedraw = val; }

	// Scrolling
	virtual void HPage(int Shown, int Top);	// Horiz page event
	virtual void VPage(int Shown, int Top);	// Vert page event
	virtual void HScroll(int step);		// Horiz scroll event
	virtual void VScroll(int step);		// Vert scroll event
	virtual void SetHScroll(int Shown, int Top);  // Set Horiz Scroll bar
	virtual void SetVScroll(int Shown, int Top);  // Set Vert Scroll bar
	virtual int GetVScroll(int& Shown, int& Top) VCONST;
	virtual int GetHScroll(int& Shown, int& Top) VCONST;
	virtual void ShowHScroll(int OnOff);
	virtual void ShowVScroll(int OnOff);

	// Change messages
	virtual void FontChanged(VCONST vFont& vf);

	// Events
	virtual void MouseDown(int x, int y, int button);	// Mouse Down at x,y
	virtual void MouseUp(int x, int y, int button);		// Mouse Up at x,y
	virtual void MouseMove(int x, int y, int button);	// Mouse Moved to x,y
	virtual void MouseMotion(int /* x */, int /* y */) {}	// no button: Mouse Moved to x,y

	virtual void EnterFocus();
	virtual void LeaveFocus();

	// Expose/redraw event
	virtual void Redraw(int x, int y, int width , int height);
	virtual void Resize(int newW, int newH);  // Resize event

	// Information
	virtual int GetWidth() VCONST { return _width; }   // Get width of canvas
	virtual int GetHeight() VCONST { return _height; } // Get Height of canvas

	// Drawing

	virtual void Clear(void)
	  { _cpDC->Clear(); }

	virtual void ClearRect(int left, int top, int width, int height)
	  { _cpDC->ClearRect(left, top, width, height); }

	virtual void CopyFromMemoryDC(vMemoryDC* vmemdc, int destX, int destY,  
	  int srcX = 0, int srcY = 0, int srcW = 0, int srcH = 0) // V:1.13
	  { _cpDC->CopyFromMemoryDC(vmemdc, destX, destY,srcX,srcY,srcW,srcH);}

#ifdef DRAWARC
	virtual void DrawArc(int xx1, int yy1, int xx2, int yy2, int xxc, int yyc)
	  { _cpDC->DrawArc(xx1, yy1, xx2, yy2, xxc, yyc); }
#endif

  	virtual void DrawAttrText(int x, int y, VCONST char* text, 
		const ChrAttr attr)
	  { _cpDC->DrawAttrText(x, y, text, attr); }

  	virtual void DrawText(int x, int y, VCONST char* text)
	  { _cpDC->DrawText(x, y, text); }

	virtual void DrawEllipse(int x, int y, int width, int height)
	  { _cpDC->DrawEllipse(x, y, width, height); }

	virtual void DrawIcon(int x, int y, vIcon& icon)
	  { _cpDC->DrawIcon(x,y,icon); }

  	virtual void DrawLine(int x, int y, int xend , int yend)
	  { _cpDC->DrawLine(x, y, xend , yend); }

  	virtual void DrawLines(vLine* lineList, int count)
	  { _cpDC->DrawLines(lineList, count); }

	virtual void DrawLines(vPoint* points, int n)
	  { _cpDC->DrawLines(points, n); }

	virtual void DrawColorPoints(int x, int y, int nPoints, vColor* pointList)
	  {_cpDC->DrawColorPoints(x,y,nPoints,pointList); }

	virtual void DrawPoint(int x, int y)
	  { _cpDC->DrawPoint(x, y); }

	virtual void DrawPoints(vPoint* pointList, int count)
	  { _cpDC->DrawPoints(pointList, count); }

	virtual void DrawPolygon(int n, vPoint points[], int fillStyle = vAlternate)
	  { _cpDC->DrawPolygon(n, points, fillStyle); }

	virtual void DrawRectangle(int x, int y, int width, int height)
	  { _cpDC->DrawRectangle(x, y, width, height); }

	virtual void DrawRectangles(vRect* rectList, int count)
	  { _cpDC->DrawRectangles(rectList, count); }

	virtual void DrawRoundedRectangle(int x, int y,
		int width, int height, int radius = 10)
	  { _cpDC->DrawRoundedRectangle(x, y, width, height, radius); }

  	virtual void DrawRubberLine(int x, int y, int xend, int yend)
	  { _cpDC->DrawRubberLine(x, y, xend, yend); }

	virtual void DrawRubberEllipse(int x, int y, int width, int height)
	  { _cpDC->DrawRubberEllipse(x, y, width, height); }

	virtual void DrawRubberRectangle(int x, int y, int width, int height)
	  { _cpDC->DrawRubberRectangle(x, y, width, height); }

	virtual vBrush GetBrush(void) VCONST
	  { return _cpDC->GetBrush(); }

	virtual void SetBrush(VCONST vBrush& brush)
	  { _cpDC->SetBrush(brush); }

	virtual vFont GetFont(void) VCONST
	  { return _cpDC->GetFont(); }

	virtual void SetFont(VCONST vFont& fid)
	  { _cpDC->SetFont(fid); }

	virtual int TextWidth(VCONST char* str) VCONST
	  { return _cpDC->TextWidth(str);}

	virtual int TextHeight(int& asc, int& des) VCONST
	  { return _cpDC->TextHeight(asc, des); } 

	// Appearance

	virtual void SetScale(int mult, int div)
	  { _cpDC->SetScale(mult,div); }

	virtual void GetScale(int& mult, int& div) VCONST
	  { _cpDC->GetScale(mult,div); }

	virtual void SetBackground(VCONST vColor color)
	  { _cpDC->SetBackground(color); }

	virtual void SetPen(VCONST vPen& pen)
	  { _cpDC->SetPen(pen); }

	virtual vPen GetPen() VCONST
	  { return _cpDC->GetPen(); }

	virtual void SetTranslate(int x, int y) { _cpDC->SetTranslate(x,y); }
	virtual void SetTransX(int x) {_cpDC->SetTransX(x);}
	virtual void SetTransY(int y) {_cpDC->SetTransY(y);}
	virtual void GetTranslate(int& x, int& y) VCONST {_cpDC->GetTranslate(x,y);}
	virtual int GetTransX() VCONST  { return _cpDC->GetTransX(); }
	virtual int GetTransY() VCONST { return _cpDC->GetTransY(); }

	virtual vDC* GetDC() { return _cpDC; }

	// Platform dependent

	Widget DrawingWindow() { return _drawWindow; }
	Drawable GetXDrawable() { return (_Drawable == 0) ? 
			(_Drawable = (Drawable)XtWindow(_drawWindow)) : _Drawable;}

      protected:	//--------------------------------------- protected

	virtual void CreateDC(void);

	vDC* _cpDC;		// pointer to our drawing canvas

	virtual void initialize(vWindow* pWindow, Widget pWidget,
		Widget drawCanvas = (Widget)0, Widget drawArea = (Widget)0);

	int _height;	// height of draw window
	int _width;	// width of draw window

	void CnvKeyInEv(unsigned int keycode, unsigned int state);

      private:		//--------------------------------------- private

	// Widgets for the canvas
	Widget _drawCanvas;		// whole canvas window
	Widget _drawHScroll;		// horiz scroll
	Widget _drawVScroll;		// vertical scroll
	Widget _drawWindow;		// window for drawing
	Drawable _Drawable;

	static VCursor _currentCursor;	// for cursor

	int _hasFocus;			// for focus workaround
	int _mouseDown;			// for tracking mouse

	int _HOn;		// on or off
	int _VOn;

	int _HScrlShown;
	int _HScrlTop;
	int _VScrlShown;
	int _VScrlTop;

	int _ResizeDone;
	int _compressRedraw;

	void ChangeCB(void);
	// Allow dual use for Athena and Motif
	void HJumpProcCB(float percent, int motifValue);
	void VJumpProcCB(float percent, int motifValue);
	void HScrollProcCB(int);
	void VScrollProcCB(int);

	void DrawHScroll(int Shown, int Top);
	void DrawVScroll(int Shown, int Top);

	void ExposeEV(int x, int y, int width, int height, int count);
	void EnterEV(void);
	void LeaveEV(void);
	void MouseUpEV(int x, int y, int button);
	void MouseDownEV(int x, int y, int button);
	void MouseMoveEV(int x, int y, int button);
      };
#endif
