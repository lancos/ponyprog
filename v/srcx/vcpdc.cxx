//===============================================================
// vCanvasPaneDC - a basic canvas for drawing
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vcpdc.h>
#include <v/vapp.h>
#include <v/vcanvas.h>

//================>>> vCanvasPaneDC::vCanvasPaneDC <<<========================
  vCanvasPaneDC::vCanvasPaneDC(vCanvasPane* parentPane)
  {

    SysDebug(Constructor,"vCanvasPaneDC::vCanvasPaneDC() constructor\n")

    _parentPane = parentPane;

    _drawWidget = _parentPane->DrawingWindow();	// drawing to this widget

    _GCFont =
    _font = theApp->GetDefaultFont();	// get the default font

    _GC = makeGC(_drawWidget);		// make a normal GC to use
    _XorGC = makeXorGC(_drawWidget);	// make a GC to use

    _pen.SetPenToPixel(_canvasFG);
    _brush.SetBrushToPixel(_canvasFG);

    SetPen(_pen);
    SetBrush(_brush);

    _XDisplay = theApp->display();
    _physWidth = theApp->ScreenWidth();
    _physHeight = theApp->ScreenHeight();
  }

//================>>> vCanvasPaneDC::~vCanvasPaneDC <<<========================
  vCanvasPaneDC::~vCanvasPaneDC()
  {
    XtReleaseGC(_drawWidget,_GC);	// Release the GCs we allocated
    XtReleaseGC(_drawWidget,_XorGC);

    SysDebug(Destructor,"vCanvasPaneDC::~vCanvasPaneDC() destructor\n")
  }

//=====================>>> vCanvasPaneDC::GetDrawable <<<======================
  Drawable vCanvasPaneDC::GetDrawable()
  {
    return _parentPane->GetXDrawable();
  }

//=====================>>> vCanvasPaneDC::Clear <<<==========================
  void vCanvasPaneDC::Clear(void)
  {
    Drawable dw = GetDrawable();
    if (dw != 0)
	XClearArea(_XDisplay, dw, 0, 0, 
    	  _parentPane->GetWidth(), _parentPane->GetHeight(), 0);
  }

//=====================>>> vCanvasPaneDC::makeGC <<<=========================
  GC vCanvasPaneDC::makeGC(Widget w)
  {
    // This one gets a default GC to used for the base canvas

    GC gc;		// local copy
    XGCValues values;	// to setup values

    // set colors according to type of canvas

    _canvasFG = theApp->Xfg();
    _canvasBG = theApp->Xbg();

    XtVaSetValues(w,			// reset fg/bg
	XtNforeground, _canvasFG,
	XtNbackground, _canvasBG,
	NULL);

    values.line_style = LineSolid;	// only solid lines for now
    values.line_width = 1;		// width 1
    values.fill_style = FillSolid;
    values.function = GXcopy;
    values.foreground = _canvasFG;
    values.background = _canvasBG;

    // This is a SHARED GC, so anything that changes must be
    // reset on a per window basis before drawing anything.

    gc = XtAllocateGC(w, 0,
	GCForeground | GCBackground | GCFunction | GCLineStyle | 
	  GCLineWidth | GCFillStyle ,
	&values,
	GCForeground | GCBackground | GCFunction | GCLineStyle | // changable
	  GCLineWidth | GCFillStyle | GCFont,
	0);

    // set the font

    // Be sure the font is loaded
    _font.LoadFont();

    XSetFont(theApp->display(), gc, _font.GetXFont()->fid);

    return gc;
  }

//=====================>>> vCanvasPaneDC::makeXorGC <<<==========================
  GC vCanvasPaneDC::makeXorGC(Widget w)
  {
    // This one gets a default GC to used for the base canvas

    GC gc;		// local copy
    XGCValues values;	// to setup values

    // set colors according to type of canvas

    values.line_width = 1;		// width 1
    values.function = GXxor;

    if (_canvasFG == 0)
	values.foreground = ~0;		// force black
    else
	values.foreground = _canvasFG;

    gc = XtAllocateGC(w, 0,
	GCForeground | GCFunction | GCLineWidth,
	&values,
	GCLineWidth,			// Width changes
	0);

    return gc;
  }

//================>>> vCanvasPaneDC::SetBackground <<<==========================
  void vCanvasPaneDC::SetBackground(VCONST vColor& color)
  {
    vXDC::SetBackground(color);

    XtVaSetValues(_drawWidget,			// reset fg/bg
	XtNbackground, _canvasBG,
	NULL);
  }

//======================>>> vCanvasPaneDC::SetFont <<<===========================
  void vCanvasPaneDC::SetFont(VCONST vFont& vf)
  {
    // Change the font associated with this window.

    vXDC::SetFont(vf);

    _parentPane->FontChanged(_font);		// We have changed the font,
					// so we may need to do some things
  }
