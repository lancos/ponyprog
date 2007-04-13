//===============================================================
// vMemoryDC - a memory drawing canvas - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>
#include <v/vmemdc.h>
#include <v/vapp.h>

//================>>> vMemoryDC::vMemoryDC <<<========================
  vMemoryDC::vMemoryDC(int width, int height)
  {

    // Mem canvases are implemented with bitmaps. Most of the
    // windows books I've seen don't seem to explain bitmaps
    // very well. The idea is this. You create a bitmap (_memBM) that
    // is compatible with some DC - like the app's frame DC for our
    // purposes. This gives the bitmap a nice color map. Then, the
    // tricky part. To use a bitmap, you have to select it into
    // a DC. So, you create a new DC (_hdc) , also compatible with
    // the app frame DC. Then you select the bitmap into the new
    // DC. Now you can draw into the new DC. When done drawing,
    // you can release/delete the memory DC until you draw again.
    // When you are ALL done, finally free the bitmap. Now isn't
    // that all kind of a wierd way of doing things?

    _physHeight = height;		// V 1.15a
    _physWidth = width;

    SysDebug(Constructor,"vMemoryDC::vMemoryDC() constructor\n")

    if (_physWidth == 0)
	_physWidth = theApp->DefaultWidth();
    if (_physHeight == 0)
	_physHeight = theApp->DefaultHeight();

    _memBM = 0;		// Not created yet
    _hdc = 0;
    _oldBM = 0;
  }

//================>>> vMemoryDC::~vMemoryDC <<<========================
  vMemoryDC::~vMemoryDC()
  {
    SysDebug(Destructor,"vMemoryDC::~vMemoryDC() destructor\n")

    if (!_memBM)
	return;

    ::DeleteObject(_memBM);
    _memBM = 0;

  }

//====================>>> vMemoryDC::GetHDC <<<========================
  void vMemoryDC::GetHDC() VCONST
  {
    // Create the DC to draw into

    HDC appDC = ::GetDC(theApp->winHwnd());	// DC of the app

    if (_memBM == 0)	// Need to create a bitmap
      {
	// Now create a bitmap
	_memBM = ::CreateCompatibleBitmap(appDC,_physWidth,_physHeight);
      }

    if (_memBM == 0)
      {
	::ReleaseDC(theApp->winHwnd(),appDC);
	return;
      }

    // Now, create an HDC for the bitmap, compatible with the app DC

    _hdc = ::CreateCompatibleDC(appDC);
    ::ReleaseDC(theApp->winHwnd(),appDC);	// Done with app's dc

    _oldBM = static_cast<HBITMAP>(::SelectObject(_hdc,_memBM)); // Select into _hdc

    // We are now ready to use _hdc!
  }

//================>>> vMemoryDC::ReleaseHDC <<<========================
  void vMemoryDC::ReleaseHDC() VCONST
  {
    if (_hdc == 0)
	return;
    ::SelectObject(_hdc,_oldBM);	// reselect old bm
    ::DeleteDC(_hdc);			// free the DC
  }

//=====================>>> vMemoryDC::Clear <<<==========================
  void vMemoryDC::Clear(void)
  {
    ClearRect(0,0,_physWidth, _physHeight);
  }

//==================>>> vMemoryDC::ClearRect <<<==========================
  void vMemoryDC::ClearRect(int left, int top, int width, int height)
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

//================>>> vMemoryDC::SetBackground <<<==========================
  void vMemoryDC::SetBackground(VCONST vColor& color)
  {

   _canvasBG = color.pixel();		// retrieve X pixel value

    GetHDC();
    ::SetBkColor(_hdc, _canvasBG);
    ReleaseHDC();
  }

//======================>>> vMemoryDC::SetFont <<<===========================
  void vMemoryDC::SetFont(VCONST vFont& vf)
  {
    // Change the font associated with this window.

    _font = vf;
  }
