//===============================================================
// vIcon - V icon - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vapp.h>
#include <v/vicon.h>
#include <v/vctlclrs.h>
#include <v/vcolor.h>
#define _XDisplay (theApp->display())

  GC vIcon::_GC = 0;
  GC vIcon::_insGC = 0;

//====================>>> vIcon::vIcon <<<==========================
  vIcon::vIcon(unsigned char* ic, int h, int w, int d, IconType it)
  {
     icon = ic; height = h; width = w; depth = d;
     _pm = 0; _inspm = 0;
     iType = it;
  }

//====================>>> vIcon::~vIcon <<<==========================
  vIcon::~vIcon()
  {
    if (_pm != 0)
      {
	XFreePixmap(theApp->display(),_pm);
	_pm = 0;
      }
    if (_inspm != 0)
      {
	XFreePixmap(theApp->display(),_inspm);
	_inspm = 0;
      }
  }

//====================>>> vIcon::GetXPM <<<==========================
  Pixmap vIcon::GetXPM(int cmd, unsigned long pbg) VCONST
  {
    unsigned int r, g, b;

    if (_GC == 0)			// No GC yet
      {
	XGCValues values;		// to setup values

	values.line_style = LineSolid;	// only solid lines for now
	values.line_width = 1;		// width 1
	values.fill_style = FillSolid;
	values.function = GXcopy;
	values.foreground = theApp->Xfg();
	values.background = theApp->Xbg();

	// Make a shared GC for all pixmaps

	_GC = XtAllocateGC(theApp->vHandle(), 0,
	    GCForeground | GCBackground | GCFunction | GCLineStyle | 
	      GCLineWidth | GCFillStyle ,
	    &values,
	    GCForeground | GCBackground | GCFunction | GCLineStyle | // changable
	      GCLineWidth | GCFillStyle | GCFont,
	    0);
      }

    if (_pm != 0)			// Pixmap already mapped?
	return _pm;

    if (depth == 1)		// monochrome bitmap
      {
	unsigned long fg, bg;
	if (theApp->Xdepth() <= 1)	// I don't know why , but
	  {				// but monochrome works differently...
	    bg = (unsigned long)  XBlackPixel(theApp->display(),0);
	    fg = (unsigned long) XWhitePixel(theApp->display(),0);
	  }
	else
	  {
	    fg = (unsigned long)  XBlackPixel(theApp->display(),0);
#ifdef Athena
	    if (cmd == 1)
		bg = (unsigned long) _vDialogBG;
	    else if (cmd == 2)
		bg = (unsigned long) _vControlFace;
	    else
#endif
	    if (cmd == 3)		// supplied fg, bg
	      {
		 bg = pbg;
	      }
	    else
		bg = (unsigned long) theApp->Xbg();
	  } 
	_pm = XCreatePixmapFromBitmapData(
	    _XDisplay,  // our display
	    DefaultRootWindow(_XDisplay),	// a Drawable
	    (char*)icon,               // the bitmap data
	    width,          		// width and height
	    height,
	    fg, bg,
	    DefaultDepth(_XDisplay,DefaultScreen(_XDisplay)));
      }
    else if (depth <= 8)			// color icon
      {
	Pixel mappedColors[256];		// for mapped colors
	unsigned char *from = icon;	// to retrieve data

	unsigned int numColors = (unsigned int) *from++ + 1; // how many colors to map

	unsigned long bg;
	if (cmd == 3)		// supplied fg, bg
	  {
	    bg = pbg; // use supplied color 
	  }
	else
	    bg = theApp->Xbg(); // default

	for (unsigned int ix = 0 ; ix < numColors ; ++ix) // map the colors
	  {
	    r = ((unsigned int)*from++);
	    g = ((unsigned int)*from++);
	    b = ((unsigned int)*from++);

	    // Now, map the RGB to a Pixmap index

            mappedColors[ix] = vXAllocColor(r,g,b);
	  }

	// Colors mapped, from now points to beginning of data
	// First, create a pixmap to draw into

	_pm = XCreatePixmap(
	    _XDisplay,  // our display
	    DefaultRootWindow(_XDisplay),	// a Drawable
	    width,          		// width and height
	    height,
	    theApp->Xdepth());			// same depth as display

	// Now draw into the pixmap
	XSetBackground(_XDisplay, _GC, bg); // the basics
	XSetFillStyle(_XDisplay,_GC,FillSolid);
	int prevPix = -1;			// no previous color
	unsigned int transPix = *(from + width*(height-1));
	for (int row = 0 ; row < height ; ++row)
	  {
	    for (int col = 0 ; col < width ; ++col)
	      {
		unsigned int pix = (unsigned int)*from++; // get pixel
		if (pix != prevPix)
		  {
		    if (pix == transPix)
			XSetForeground(_XDisplay, _GC, bg);
		    else
			XSetForeground(_XDisplay, _GC, mappedColors[pix]);
		  }
		prevPix = pix;			// remember last color
		XDrawPoint(_XDisplay, _pm, _GC, col, row);
	      }
	  }
      }
    else if (depth == 24)
      {
	unsigned char *from = icon;	// to retrieve data
	Pixel Pix;

	// First, create a pixmap to draw into

	_pm = XCreatePixmap(
	    _XDisplay,  // our display
	    DefaultRootWindow(_XDisplay),	// a Drawable
	    width,          		// width and height
	    height,
	    theApp->Xdepth());			// same depth as display

	// Now draw into the pixmap
	XSetBackground(_XDisplay, _GC, theApp->Xbg()); // the basics
	XSetFillStyle(_XDisplay,_GC,FillSolid);
	Pixel PrevPix = (Pixel) -1;		// no previous Pixel
	for (int row = 0 ; row < height ; ++row)
	  {
	    for (int col = 0 ; col < width ; ++col)
	      {
		r = ((unsigned int)*from++);
		g = ((unsigned int)*from++);
		b = ((unsigned int)*from++);

		// Now, map the RGB to a Pixmap index

		Pix = vXAllocColor(r,g,b);
		if (Pix != PrevPix)
		  {
		    XSetForeground(_XDisplay, _GC, Pix);
		  }
		PrevPix = Pix;			// remember last color
		XDrawPoint(_XDisplay, _pm, _GC, col, row);
	      }
	  }
      }
    return _pm;
  }

//====================>>> vIcon::GetInsensXPM <<<==========================
  Pixmap vIcon::GetInsensXPM(int cmd, unsigned long pbg) VCONST
  {
    unsigned int r, g, b;


    if (_insGC == 0)			// No GC yet
      {
	XGCValues values;		// to setup values

	values.line_style = LineSolid;	// only solid lines for now
	values.line_width = 1;		// width 1
	values.fill_style = FillSolid;
	values.function = GXcopy;
	values.foreground = theApp->Xfg();
	values.background = theApp->Xbg();

	// Make a shared GC for all pixmaps

	_insGC = XtAllocateGC(theApp->vHandle(), 0,
	    GCForeground | GCBackground | GCFunction | GCLineStyle | 
	      GCLineWidth | GCFillStyle ,
	    &values,
	    GCForeground | GCBackground | GCFunction | GCLineStyle | // changable
	      GCLineWidth | GCFillStyle | GCFont ,
	    0);

      }

    if (_inspm != 0)			// Pixmap already mapped?
	return _inspm;

    if (depth == 1)		// monochrome bitmap
      {
	unsigned long fg, bg;
	if (theApp->Xdepth() <= 1)	// I don't know why , but
	  {				// but monochrome works differently...
	    bg = (unsigned long)  XBlackPixel(theApp->display(),0);
	    fg = (unsigned long) XWhitePixel(theApp->display(),0);
	  }
	else
	  {
	    fg = (unsigned long) _vDarkControlShadow;	// this is insensitve
#ifdef Athena
	    if (cmd == 1)
		bg = (unsigned long) _vDialogBG;
	    else if (cmd == 2)
		bg = (unsigned long) _vControlFace;
	    else
#endif
	    if (cmd == 3)		// supplied fg, bg
	      {
		 bg = pbg;
	      }
	    else
		bg = (unsigned long) theApp->Xbg();
	  } 
	_inspm = XCreatePixmapFromBitmapData(
	    _XDisplay,  // our display
	    DefaultRootWindow(_XDisplay),	// a Drawable
	    (char*)icon,               // the bitmap data
	    width,          		// width and height
	    height,
	    fg, bg,
	    DefaultDepth(_XDisplay,DefaultScreen(_XDisplay)));
      }
    else if (depth <= 8)			// color icon
      {
	Pixel mappedColors[256];		// for mapped colors
	unsigned char *from = icon;	// to retrieve data

	unsigned int numColors = (unsigned int) *from++ + 1; // how many colors to map

	unsigned long bg;
	if (cmd == 3)		// supplied fg, bg
	  {
	    bg = pbg; // use supplied color 
	  }
	else
	    bg = theApp->Xbg(); // default

	for (unsigned int ix = 0 ; ix < numColors ; ++ix) // map the colors
	  {
	    r = ((unsigned int)*from++);
	    g = ((unsigned int)*from++);
	    b = ((unsigned int)*from++);

	    // Now, map the RGB to a Pixmap index

            mappedColors[ix] = vXAllocColor(r,g,b);
	  }

	// Colors mapped, from now points to beginning of data
	// First, create a pixmap to draw into

	_inspm = XCreatePixmap(
	    _XDisplay,  // our display
	    DefaultRootWindow(_XDisplay),	// a Drawable
	    width,          		// width and height
	    height,
	    theApp->Xdepth());			// same depth as display

	// Now draw into the pixmap
	XSetBackground(_XDisplay, _insGC, theApp->Xbg()); // the basics
	XSetFillStyle(_XDisplay,_insGC,FillStippled);
	int prevPix = -1;			// no previous color
	unsigned int transPix = *(from + width*(height-1));
	for (int row = 0 ; row < height ; ++row)
	  {
	    for (int col = 0 ; col < width ; ++col)
	      {
		unsigned int pix = (unsigned int)*from++; // get pixel
		if (pix != prevPix)
		  {
		    if (pix == transPix)
			XSetForeground(_XDisplay, _insGC, bg);
		    else
			XSetForeground(_XDisplay, _insGC, mappedColors[pix]);
		  }
		prevPix = pix;			// remember last color
		XDrawPoint(_XDisplay, _inspm, _insGC, col, row);
	      }
	  }

	// now stipple
	XSetForeground(_XDisplay, _insGC, (unsigned long) _vControlFace);
	for (int row = 0 ; row < height ; row++)
	  {
	    int start = 0;
	    if ((row/2)*2 == row)
		start = 1;
	    for (int col = start ; col < width ; col+=2)
	      {
		XDrawPoint(_XDisplay, _inspm, _insGC, col++, row);
	      }
	  }
      }
    else if (depth == 24)
      {
	unsigned char *from = icon;	// to retrieve data
	Pixel Pix;

	// First, create a pixmap to draw into

	_inspm = XCreatePixmap(
	    _XDisplay,  // our display
	    DefaultRootWindow(_XDisplay),	// a Drawable
	    width,          		// width and height
	    height,
	    theApp->Xdepth());			// same depth as display

	// Now draw into the pixmap
	XSetBackground(_XDisplay, _insGC, theApp->Xbg()); // the basics
	XSetFillStyle(_XDisplay,_insGC,FillStippled);
	Pixel PrevPix = (Pixel) -1;		// no previous Pixel
	for (int row = 0 ; row < height ; ++row)
	  {
	    for (int col = 0 ; col < width ; ++col)
	      {
		r = ((unsigned int)*from++);
		g = ((unsigned int)*from++);
		b = ((unsigned int)*from++);

		// Now, map the RGB to a Pixmap index

		Pix = vXAllocColor(r,g,b);
		if (Pix != PrevPix)
		  {
		    XSetForeground(_XDisplay, _insGC, Pix);
		  }
		PrevPix = Pix;			// remember last color
		XDrawPoint(_XDisplay, _inspm, _insGC, col, row);
	      }
	  }
	// now stipple
	XSetForeground(_XDisplay, _insGC, (unsigned long) _vControlFace);
	for (int row = 0 ; row < height ; row++)
	  {
	    int start = 0;
	    if ((row/2)*2 == row)
		start = 1;
	    for (int col = start ; col < width ; col+=2)
	      {
		XDrawPoint(_XDisplay, _inspm, _insGC, col++, row);
	      }
	  }
      }
    return _inspm;
  }
