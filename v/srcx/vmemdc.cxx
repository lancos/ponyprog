//===============================================================
// vMemoryDC - a memory drawing canvas
//
// Copyright (C) 1995,1998 Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vmemdc.h>
#include <v/vapp.h>

//================>>> vMemoryDC::vMemoryDC <<<========================
  vMemoryDC::vMemoryDC(int width, int height)
  {
    XColor fg;
    XColor bg;

    _physHeight = height;		// V 1.15a
    _physWidth = width;

    SysDebug(Constructor,"vMemoryDC::vMemoryDC() constructor\n")

    _GC = _XorGC = 0;

    _GCFont =
    _font = theApp->GetDefaultFont();	// get the default font

    _XDisplay = theApp->display();

    if (_physWidth == 0)
	_physWidth = theApp->DefaultWidth();
    if (_physHeight == 0)
	_physHeight = theApp->DefaultHeight();
    
    // Create the Pixmap to draw into

    _pixmap = XCreatePixmap(_XDisplay, theApp->xWindow(),
	_physWidth, _physHeight, theApp->Xdepth());

    if (_pixmap == 0)
	return;

    _GC = makeGC();		// make a normal GC to use
    _XorGC = makeXorGC();	// make a GC to use

    _pen.SetPenToPixel(_canvasFG);
    _brush.SetBrushToPixel(_canvasFG);

    SetPen(_pen);
    SetBrush(_brush);

  }

//================>>> vMemoryDC::~vMemoryDC <<<========================
  vMemoryDC::~vMemoryDC()
  {
    SysDebug(Destructor,"vMemoryDC::~vMemoryDC() destructor\n")

    if (_XorGC)					// Free the GCs
	XFreeGC(_XDisplay, _XorGC);
    if (_GC)
	XFreeGC(_XDisplay, _GC);
    if (_pixmap != 0)
	XFreePixmap(_XDisplay, _pixmap);	// free the pixmap

  }

//=====================>>> vMemoryDC::GetDrawable <<<======================
  Drawable vMemoryDC::GetDrawable() VCONST
  {
    return (Drawable) _pixmap;
  }

//=====================>>> vMemoryDC::Clear <<<==========================
  void vMemoryDC::Clear(void)
  {
    // Clear by setting to background color
    XSetForeground(_XDisplay, _GC, _canvasBG);
    XFillRectangle(_XDisplay, _pixmap, _GC, 0,0, _physWidth, _physHeight);
  }

//=====================>>> vMemoryDC::makeGC <<<=========================
  GC vMemoryDC::makeGC()
  {
    // This one gets a default GC to used for the base canvas

    GC gc;		// local copy
    XGCValues values;	// to setup values
    Pixel fg, bg;

    int n = 0;

    // set colors according to type of canvas

    _canvasFG = theApp->Xfg();
    _canvasBG = theApp->Xbg();


    values.line_style = LineSolid;	// only solid lines for now
    values.line_width = 1;		// width 1
    values.fill_style = FillSolid;
    values.function = GXcopy;
    values.foreground = _canvasFG;
    values.background = _canvasBG;

    // This not a shared GC for memory canvases.

    gc = XCreateGC(_XDisplay, GetDrawable(),
	GCForeground | GCBackground | GCFunction | GCLineStyle | 
	  GCLineWidth | GCFillStyle ,
	&values);

    // set the font

    // Be sure the font is loaded
    _font.LoadFont();

    XSetFont(theApp->display(), gc, _font.GetXFont()->fid);

    return gc;
  }

//=====================>>> vMemoryDC::makeXorGC <<<==========================
  GC vMemoryDC::makeXorGC()
  {
    // This one gets a default GC to used for the base canvas

    GC gc;		// local copy
    XGCValues values;	// to setup values

    int n = 0;

    // set colors according to type of canvas

    values.line_width = 1;		// width 1
    values.function = GXxor;

    if (_canvasFG == 0)
	values.foreground = ~0;		// force black
    else
	values.foreground = _canvasFG;

    // This not a shared GC for memory canvases.

    gc = XCreateGC(_XDisplay, GetDrawable(),
	GCForeground | GCFunction | GCLineWidth,
	&values);

    return gc;
  }

//================>>> vMemoryDC::SetBackground <<<==========================
  void vMemoryDC::SetBackground(VCONST vColor& color)
  {
    vXDC::SetBackground(color);
  }
