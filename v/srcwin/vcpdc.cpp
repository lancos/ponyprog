//===============================================================
// vCanvasPaneDC - a basic canvas for drawing
//
// Copyright (C) 1995,1996, 1997, 1998   Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================


#include <v/vwin32.h>		// for Win 32 stuff

#include <math.h>

#include <v/vcpdc.h>

#include <v/vapp.h>		// need access to the app
#include <v/vcanvas.h>		// our own canvas widget

//-----------------------------------------------------------------------

//================>>> vCanvasPaneDC::vCanvasPaneDC <<<========================
  vCanvasPaneDC::vCanvasPaneDC(vCanvasPane* parentPane) : vWinDC()
  {
    SysDebug(Constructor,"vCanvasPaneDC::vCanvasPaneDC() constructor\n")

    _parentPane = parentPane;

    _drawWidget = _parentPane->DrawingWindow();	// drawing to this widget

  }

//================>>> vCanvasPaneDC::~vCanvasPaneDC <<<========================
  vCanvasPaneDC::~vCanvasPaneDC()
  {

    SysDebug(Destructor,"vCanvasPaneDC::~vCanvasPaneDC() destructor\n")

  }

//=====================>>> vCanvasPaneDC::Clear <<<==========================
  void vCanvasPaneDC::Clear(void)
  {
    GetHDC();				// Get a DC to use
    ::SetBkColor(_hdc, _canvasBG);

    HBRUSH brush = ::CreateSolidBrush(_canvasBG);
    RECT rect;

    ::GetClientRect(_drawWidget, &rect);

    ::FillRect(_hdc, &rect, brush);

    ::DeleteObject(brush);		// need to free the resource!

    ReleaseHDC();			// Release the DC
  }

//==================>>> vCanvasPaneDC::ClearRect <<<==========================
  void vCanvasPaneDC::ClearRect(int left, int top, int width, int height)
  {
    // Clear a rectangluar area starting at left,top of width and height

    if (height == 0 || width == 0)
	return;

    GetHDC();				// Get a DC to use
    ::SetBkColor(_hdc, _canvasBG);

    HBRUSH brush = ::CreateSolidBrush(_canvasBG);
    RECT rect;

    rect.top = top; rect.left = left;
    rect.bottom = top+height; rect.right = left + width;

    ::FillRect(_hdc, &rect, brush);
    ::DeleteObject(brush);		// need to free the resource!

    ReleaseHDC();			// Release the DC
  }

//================>>> vCanvasPaneDC::SetBackground <<<==========================
  void vCanvasPaneDC::SetBackground(VCONST vColor& color)
  {

   _canvasBG = color.pixel();		// retrieve X pixel value

    GetHDC();
    ::SetBkColor(_hdc, _canvasBG);
    ReleaseHDC();
    Clear();
    _parentPane->Redraw(0,0,0,0);

  }

//======================>>> vCanvasPaneDC::SetFont <<<===========================
  void vCanvasPaneDC::SetFont(VCONST vFont& vf)
  {
    // Change the font associated with this window.

    _font = vf;

    // First, make sure the font is loaded

    _parentPane->FontChanged(_font); // We have changed the font,
					// so we may need to do some things
  }

