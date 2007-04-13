//===============================================================
// vWinDC - Windows Base DC
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================


#include <v/vwin32.h>		// for Win 32 stuff

#include <math.h>

#include <v/vwindc.h>
#include <v/vmemdc.h>

#include <v/vapp.h>		// need access to the app
#include <v/vcanvas.h>		// our own canvas widget
#include <v/vicon.h>

//-----------------------------------------------------------------------

//================>>> vWinDC::vWinDC <<<========================
  vWinDC::vWinDC()
  {
    SysDebug(Constructor,"vWinDC::vWinDC() constructor\n")

    _font = theApp->GetDefaultFont();	// Use the default fixed font by default!

    _hdc = 0;  _hdcPaint = 0;
    _hpen = (HPEN) ::GetStockObject(BLACK_PEN);
    _hbrush = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
    _canvasBG = RGB(255,255,255);
    _isPrinterDC = 0;			// Assume not printer
    _isWin32s = 0;
#ifndef VWIN16
    OSVERSIONINFO vi;
    vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (GetVersionEx(&vi))
      {
        _isWin32s = vi.dwPlatformId == VER_PLATFORM_WIN32s;
      }
#endif
  }

//================>>> vWinDC::~vWinDC <<<========================
  vWinDC::~vWinDC()
  {

    SysDebug(Destructor,"vWinDC::~vWinDC() destructor\n")

  }

//=====================>>> vXDC::CopyFromMemoryDC <<<==========================
  void vWinDC::CopyFromMemoryDC(vMemoryDC* vmemdc, int destX, int destY,
	int srcX, int srcY, int srcW, int srcH) // V:1.13
  {
    // Copy myself to another DC
    int ch = vmemdc->_physHeight;
    int cw = vmemdc->_physWidth;
    int cx = 0;
    int cy = 0;

    if (srcX > 0)               // Fixed: 10/11/96
	cx = srcX;
    if (srcY > 0)
	cy = srcY;
    if (srcW > 0)
	cw = srcW;
    if (srcH > 0)
	ch = srcH;

    if ((cw + cx) > vmemdc->_physWidth && _physWidth > 0)       // only copy what will fi
	cw = vmemdc->_physWidth - cx;
    if ((ch + cy) > vmemdc->_physHeight && _physHeight > 0)
	ch = vmemdc->_physHeight - cy;

    if ((cw + destX) > _physWidth && _physWidth > 0)
        cw = _physWidth - destX;
    if ((ch + destY) > _physHeight && _physHeight > 0)
        ch = _physHeight - destY;

    GetHDC();			// Get my hdc
    vmemdc->GetHDC();		// the source hdc
    HDC memDC = vmemdc->_hdc;


    ::BitBlt(_hdc, destX, destY, cw, ch, memDC, cx, cy, SRCCOPY);

    vmemdc->ReleaseHDC();
    ReleaseHDC();
  }

#ifdef DRAWARC
//====================>>> vWinDC::DrawArc <<<==========================
  void vWinDC::DrawArc(int xx1, int yy1, int xx2, int yy2, 
    int xxc, int yyc)
  {
    double dx = xx1 - xxc;
    double dy = yy1 - yyc;
    double radius = sqrt(dx * dx + dy * dy);
    int r = (int) radius;

const double PIx2 = 6.283185;

    double radius1, radius2;

    if (xx1 == xx2 && yy1 == yy2)
      {
	radius1 = 0.0;
	radius2 = 360.0;
      }
    else if (radius == 0.0)
	radius1 = radius2 = 0.0;
    else
      {
	if (xx1 - xxc == 0)
	  {
	    if (yy1 - yyc < 0)
		radius1 = 90.0;
	    else
		radius1 = -90.0;
	  }
	else
	    radius1 = -atan2 ((double) (yy1 - yyc), (double) (xx1 - xxc)) * 360.0 / PIx2;

	if (xx2 - xxc == 0)
	  {
	    if (yy2 - yyc < 0)
		radius2 = 90.0;
	    else
		radius2 = -90.0;
	  }
	else
	    radius2 = -atan2 ((double) (yy2 - yyc), (double) (xx2 - xxc)) * 360.0 / PIx2;
      }
    radius1 *= 64.0;
    radius2 *= 64.0;
    int alpha1 = (int) radius1;
    int alpha2 = (int) (radius2 - radius1);
    while (alpha2 <= 0)
	alpha2 += 360 * 64;

    while (alpha2 > 360 * 64)
	alpha2 -= 360 * 64;

    if (_brush.brushStyle != vTransparent)
       {
	  XFillArc(_XDisplay, _XWindow, _GC, 
	    xxc - r, yyc - r, 2 * r, 2 * r, alpha1, alpha2);
       }

    if (_pen.penStyle != vTransparent)
      {
	XDrawArc(_XDisplay, _XWindow, _GC,
	    xxc - r, yyc - r, 2 * r, 2 * r, alpha1, alpha2);
      }
  }
#endif

//====================>>> vWinDC::DrawEllipse <<<==========================
  void vWinDC::DrawEllipse(int x, int y, int width, int height)
  {
    if (height == 0 || width == 0)
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int x2 = Scale(x+_tx+width);
    int y2 = Scale(y+_ty+height);

    GetHDC();				// Get a DC to use
    _hpen = _pen.GetHPEN();		// Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);    	// Select it
    _hbrush = _brush.GetHBRUSH();
    HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(_hdc, _hbrush));

    (void)::Ellipse(_hdc, xx, yy, x2, y2);

    ::SelectObject(_hdc, oldpen);	// original
    ::SelectObject(_hdc, oldbrush);	// something safe

    ReleaseHDC();			// Release the DC
  }

//====================>>> vWinDC::DrawIcon <<<==========================
  void vWinDC::DrawIcon(int x, int y, VCONST vIcon& icon)
  {

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    // Now, draw the icon
    GetHDC();			// get our hdc
    ::SetBkColor(_hdc, _canvasBG);

    HBRUSH hbrush, oldbrush;
    HPEN oldpen;

    HDC hdcMem = ::CreateCompatibleDC(_hdc);

    if (_isPrinterDC)
      {
	hbrush = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	oldpen = (HPEN)::SelectObject(_hdc, ::GetStockObject(BLACK_PEN));
      }
    else
	hbrush = ::CreateSolidBrush(_pen.GetColor().pixel()); // use pen color!

    oldbrush = static_cast<HBRUSH>(::SelectObject(_hdc, hbrush));

    HBITMAP hbm = icon.GetIconHBM(_hdc);	// get the bitmap
    HBITMAP oldbm = static_cast<HBITMAP>(::SelectObject(hdcMem,hbm));

    // Now BitBlt the icon BMP into the drawing canvas, leaving
    // background untouched, and the pen color to draw 1 bits
    if (icon.depth == 1)
      {
	::BitBlt(_hdc, xx, yy, icon.width, icon.height,
	  hdcMem, 0, 0, 0xE20746L);
      }
    else
      {
	if (icon.iType == Transparent)
	  {
	    // Prepare a memory DC with the mask
	    HDC hdcMemMask = ::CreateCompatibleDC( _hdc );
	    HBITMAP hmsk = icon.GetIconMask( _hdc ); // get icon mask HBITMAP
	    HBITMAP oldBM = static_cast<HBITMAP>(::SelectObject( hdcMemMask, hmsk ));

	    // Prepare DC for scratch pad
	    HDC hdcScratch = ::CreateCompatibleDC( _hdc );
	    HBITMAP hscr = ::CreateCompatibleBitmap( _hdc, icon.width, icon.height );
	    HBITMAP scrBM = static_cast<HBITMAP>(::SelectObject( hdcScratch, hscr ));

	    // Copy inverted mask to the scratch dc
	    ::BitBlt( hdcScratch, 0, 0, icon.width, icon.height,
	    hdcMemMask, 0, 0, NOTSRCCOPY);

	    // Leave the background untouched where the mask is black
	    // Erase the background where the mask is white
	    ::BitBlt( _hdc, xx, yy, icon.width, icon.height,
	    hdcScratch, 0, 0, SRCAND);

	    // Paint the icon on the scratch DC only where the mask is white
	    // Erase scratch DC where the mask is black
	    ::BitBlt( hdcScratch, 0, 0, icon.width, icon.height,
	    hdcMem, 0, 0, SRCERASE);

	    // Merge the background DC with the scratch DC (or)
	    ::BitBlt( _hdc, xx, yy, icon.width, icon.height,
	    	hdcScratch, 0, 0, SRCPAINT);

	    // Delete the scratch DC and the bitmap
	    ::SelectObject( hdcScratch, scrBM );
	    ::DeleteObject( hscr );
	    ::DeleteDC( hdcScratch );

	    // Delete the mask DC
	    ::SelectObject( hdcMemMask, oldBM );
	    ::DeleteDC( hdcMemMask );
	  }
	else
	    ::BitBlt(_hdc, xx, yy, icon.width, icon.height,
		hdcMem, 0, 0, SRCCOPY);
      }
    ::DeleteDC(hdcMem);
    if (!_isPrinterDC)
	::DeleteObject(hbrush);	// free brush
    else
        ::SelectObject(_hdc, oldpen);	// something safe
    ::SelectObject(_hdc, oldbrush);	// something safe

    ::SelectObject(hdcMem,oldbm);

    ReleaseHDC();			// Release the DC


  }

//====================>>> vWinDC::DrawLine <<<==========================
  void vWinDC::DrawLine(int x, int y, int xend, int yend)
  {
    // Draw a line from x,y to xend,yend

    // This method has the typical sequence of code for drawing. All other
    // drawing methods are modeled using this approach

    // First, do the required scaling

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int xe = Scale(xend+_tx);
    int ye = Scale(yend+_ty);

    // Windows needs a DC to draw into. For efficiency, the DC we use
    // can be created in one of two ways. While the user program is
    // drawing stuff interactively (usually in response to mouse input),
    // the DC is created and released each time the drawing method is
    // used. For Redraw, the canvas redraw event will issue a BeginPaint
    // and EndPaint, which will bracket all drawing, and creates and
    // releases a DC (_hdcPaint). So, the GetHDC will either use the
    // existing _hdcPaint, or create a new one as needed. ReleaseHDC
    // then either releases the DC, or does nothing, and lets EndPaint
    // do the job.

    GetHDC();				// Get a DC to use
    _hpen = _pen.GetHPEN();		// Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);    	// Select it

    // Shapes will do a similar sequence for brushes as well

    // Now the code to actually do the drawing

    ::MoveTo(_hdc,xx, yy);
    ::LineTo(_hdc, xe, ye);

    // We need to set the pen (and brush for shapes) to a system
    // defined object so it is safe to fiddle with our pen and brush
    // during BeginPaint / EndPaint blocks. Changing pens and brushes
    // causes them to be destroyed and recreated, and you can't do
    // that if they are selected into the DC, which could happen
    // inside the BeginPaint / EndPaint sequence.

    ::SelectObject(_hdc, oldpen);	// something safe

    ReleaseHDC();			// Release the DC
  }

//====================>>> vWinDC::DrawLines <<<==========================
  void vWinDC::DrawLines(vLine* lineList, int count)
  {

    if (count < 1 || lineList == 0)
	return;

    GetHDC();				// Get a DC to use
    _hpen = _pen.GetHPEN();		// Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);    	// Select it

    vLine* lp = lineList ;
    for (int num = 0 ; num < count ; ++lp, ++num)
      {

	int xx = Scale(lp->x+_tx);
	int yy = Scale(lp->y+_ty);
	int xe = Scale(lp->xend+_tx);
	int ye = Scale(lp->yend+_ty);

	::MoveTo(_hdc,xx, yy);
	::LineTo(_hdc, xe, ye);
      }

    ::SelectObject(_hdc, oldpen);	// something safe

    ReleaseHDC();			// Release the DC
  }

//====================>>> vWinDC::DrawLines <<<==========================
  void vWinDC::DrawLines(vPoint* points, int n)
  {
    // draw a complete polyline

    if ( n < 2 || points == 0 )
        return;

    POINT* xpoints;

    if (_hasScale || _tx != 0 || _ty != 0)      // If we have to scale, then we need to copy
      {
        xpoints = new POINT[n]; // allocate space for copy
        for (int i = 0; i < n; i++)
          {
            xpoints[i].x = ((points[i].x+_tx) * _Mult) / _Div;  // scale
            xpoints[i].y = ((points[i].y+_ty) * _Mult) / _Div;
          }
      }
    else
#ifdef _WIN32
      {
        xpoints = new POINT[n]; // allocate space for copy
        for (int i = 0; i < n; i++)
          {
            xpoints[i].x = points[i].x;
            xpoints[i].y = points[i].y;
          }
      }
#else
      {
        xpoints = (POINT*)points;       // just use original values
      }
#endif

    GetHDC();                           // Get a DC to use
    _hpen = _pen.GetHPEN();             // Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);          // Select it
    HBRUSH oldbrush = _hbrush = _brush.GetHBRUSH();
    ::SelectObject(_hdc, _hbrush);

    (void)::Polyline(_hdc, xpoints, n);

    ::SelectObject(_hdc, oldpen);       // something safe
    ::SelectObject(_hdc, oldbrush);     // something safe

    ReleaseHDC();                       // Release the DC

#ifndef _WIN32
    if (_hasScale)
#endif
        delete[]xpoints;
}

//==================>>> vWinDC::DrawColorPoints <<<======================
  void vWinDC::DrawColorPoints(int x, int y, int nPoints, vColor* pointList)
  {
    // Draw a bunch of color points. 

    if (nPoints < 1 || pointList == 0)
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    if (yy < 0)
	return;				// row not displayed

    GetHDC();				// Get a DC to use

    COLORREF curpix = pointList[0].pixel();
    COLORREF prevpix = curpix;
    COLORREF usepix = curpix;		// rep used by windows

    if (xx < 0)				// need to check every time
      {
	for (int ix = 0 ; ix < nPoints ; ++ix)
	  {
	    if (xx+ix < 0)
		continue;			// this one not displayed
	    curpix = pointList[ix].pixel();
	    if (curpix != prevpix)		// don't reset color if same
	      {
		usepix = prevpix = curpix;
	      }
	    usepix = ::SetPixel(_hdc,xx+ix,yy,usepix);
	  }
      }
    else				// don't need xx check
      {
	for (int ix = 0 ; ix < nPoints ; ++ix)
	  {
	    curpix = pointList[ix].pixel();
	    if (curpix != prevpix)		// don't reset color if same
	      {
		usepix = prevpix = curpix;
	      }
	    usepix = ::SetPixel(_hdc,xx+ix,yy,usepix);
	  }
      }

    ReleaseHDC();			// Release the DC
  }

//==================>>> vWinDC::DrawPoint <<<======================
  void vWinDC::DrawPoint(int x, int y)
  {

    // First, do the required scaling

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    GetHDC();				// Get a DC to use
    _hpen = _pen.GetHPEN();		// Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);    	// Select it

       // Now the code to actually do the drawing

    ::MoveTo(_hdc,xx, yy);
    ::LineTo(_hdc, xx+1, yy+1);

    ::SelectObject(_hdc, oldpen);	// something safe

    ReleaseHDC();			// Release the DC

  }

//==================>>> vWinDC::DrawPoints <<<======================
  void vWinDC::DrawPoints(vPoint* pointList, int count)
  {

    if (count < 1 || pointList == 0)
	return;

    // First, do the required scaling

    GetHDC();				// Get a DC to use
    _hpen = _pen.GetHPEN();		// Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);    	// Select it

       // Now the code to actually do the drawing

    vPoint* pl = pointList ;
    for (int num = 0 ; num < count ; ++pl, ++num)
      {
	int xx = Scale(pl->x+_tx);
	int yy = Scale(pl->y+_ty);

	::MoveTo(_hdc,xx, yy);
	::LineTo(_hdc, xx+1, yy+1);
      }

    ::SelectObject(_hdc, oldpen);	// something safe

    ReleaseHDC();			// Release the DC

  }

//==================>>> vWinDC::DrawPolygon <<<======================
  void vWinDC::DrawPolygon(int n, vPoint points[], int fillStyle)
  {
    // draw a complete polygon (starting point specified twice!)


    POINT* xpoints;

    if (_hasScale || _tx != 0 || _ty != 0)  	// If we have to scale, then we need to copy
      {
	xpoints = new POINT[n];	// allocate space for copy
	for (int i = 0; i < n; i++)
	  {
	    xpoints[i].x = ((points[i].x+_tx) * _Mult) / _Div;	// scale
	    xpoints[i].y = ((points[i].y+_ty) * _Mult) / _Div;
	  }
      }
    else
#ifndef VWIN16
      {
	xpoints = new POINT[n];	// allocate space for copy
	for (int i = 0; i < n; i++)
	  {
	    xpoints[i].x = points[i].x;
	    xpoints[i].y = points[i].y;
	  }
      }
#else
      {
	xpoints = (POINT*)points;	// just use original values
      }
#endif

    GetHDC();				// Get a DC to use
    _hpen = _pen.GetHPEN();		// Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);    	// Select it
    HBRUSH oldbrush = _hbrush = _brush.GetHBRUSH();
    ::SelectObject(_hdc, _hbrush);

    int prev = ::SetPolyFillMode(_hdc, (fillStyle == vAlternate) ? ALTERNATE : WINDING);

    (void)::Polygon(_hdc, xpoints, n - 1);

    ::SetPolyFillMode(_hdc,prev);

    ::SelectObject(_hdc, oldpen);	// something safe
    ::SelectObject(_hdc, oldbrush);	// something safe

    ReleaseHDC();			// Release the DC

#ifdef VWIN16
    if (_hasScale)
#endif
	delete[]xpoints;
}

//==================>>> vWinDC::DrawRectangle <<<======================
  void vWinDC::DrawRectangle(int x, int y, int width, int height)
  {
    if (height == 0 || width == 0)
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int x2 = Scale(x+width+1+_tx);
    int y2 = Scale(y+height+1+_ty);

    GetHDC();				// Get a DC to use
    if (height == 1 && width == 1)
      {
	COLORREF curpix = _brush.GetColor().pixel();
	::SetPixel(_hdc,xx,yy,curpix);
	ReleaseHDC();
	return;
      }
    _hpen = _pen.GetHPEN();		// Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);    	// Select it
    _hbrush = _brush.GetHBRUSH();
    HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(_hdc, _hbrush));

    (void)::Rectangle(_hdc, xx, yy, x2, y2);

    ::SelectObject(_hdc, oldpen);	// something safe
    ::SelectObject(_hdc, oldbrush);	// something safe

    ReleaseHDC();			// Release the DC

  }

//==================>>> vWinDC::DrawRectangles <<<======================
  void vWinDC::DrawRectangles(vRect* rectList, int count)
  {
    if (count < 1 || !rectList)
	return;

    GetHDC();				// Get a DC to use

    _hpen = _pen.GetHPEN();		// Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);    	// Select it
    _hbrush = _brush.GetHBRUSH();
    HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(_hdc, _hbrush));

    vRect* rp = rectList ;
    for (int num = 0 ; num < count ; ++rp, ++num)
      {
	int xx = Scale(rp->x+_tx);
	int yy = Scale(rp->y+_ty);
	int x2 = Scale(rp->x+rp->w+1+_tx);
	int y2 = Scale(rp->y+rp->h+1+_ty);

	if (rp->h == 0 && rp->w == 0)
	    continue;

	if (rp->h == 1 && rp->w == 1)
	  {
	    COLORREF curpix = _brush.GetColor().pixel();
	    ::SetPixel(_hdc,xx,yy,curpix);
	  }

	else
	    (void)::Rectangle(_hdc, xx, yy, x2, y2);
      }

    ::SelectObject(_hdc, oldpen);	// something safe
    ::SelectObject(_hdc, oldbrush);	// something safe

    ReleaseHDC();			// Release the DC

  }

//================>>> vWinDC::DrawRoundedRectangle <<<===================
  void vWinDC::DrawRoundedRectangle(int x, int y,
	int width, int height, int radius)
  {
    if (height == 0 || width == 0)
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int x2 = Scale(x+width+_tx);
    int y2 = Scale(y+height+_ty);
    int r;

    if (radius < 0)
      {
	// Negative radius means divide average of hight and width
	// by this
	r = (Scale(width+height)/(-2 * radius));
      }
    else
	r = Scale(radius);


    GetHDC();				// Get a DC to use
    _hpen = _pen.GetHPEN();		// Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);    	// Select it
    _hbrush = _brush.GetHBRUSH();
    HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(_hdc, _hbrush));

    (void)::RoundRect(_hdc, xx, yy, x2, y2, r, r);

    ::SelectObject(_hdc, oldpen);	// something safe
    ::SelectObject(_hdc, oldbrush); // something safe

    ReleaseHDC();			// Release the DC
  }

//==================>>> vWinDC::DrawRubberEllipse <<<===================
  void vWinDC::DrawRubberEllipse(int x, int y, int width, int height)
  {
    if (height == 0 || width == 0)
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int x2 = Scale(x+width+_tx);
    int y2 = Scale(y+height+_ty);
    vPen xorPen = _pen;			// start with current pen

    xorPen.SetColor(255,255,255);	// need it to be white

    GetHDC();				// Get a DC to use
    _hpen = xorPen.GetHPEN();		// Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);    	// Select it
    HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(_hdc, ::GetStockObject(NULL_BRUSH)));
    ::SetROP2(_hdc, R2_XORPEN);		// draw in XOR
    (void)::Ellipse(_hdc, xx, yy, x2, y2);
    ::SetROP2(_hdc, R2_COPYPEN);	// regular pen
    ::SelectObject(_hdc, oldpen);	// something safe
    ::SelectObject(_hdc, oldbrush);	// something safe

    ReleaseHDC();			// Release the DC
 }

//==================>>> vWinDC::DrawRubberLine <<<======================
  void vWinDC::DrawRubberLine(int x, int y, int xend, int yend)
  {
    // Draw a rubber-band line from x,y to xend,yend. Redrawing
    // over the same with will erase it.

     int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int xe = Scale(xend+_tx);
    int ye = Scale(yend+_ty);

    vPen xorPen = _pen;			// start with current pen

    xorPen.SetColor(255,255,255);	// need it to be white

    GetHDC();				// Get a DC to use
    _hpen = xorPen.GetHPEN();		// Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);   	// Select
    ::SetROP2(_hdc, R2_XORPEN);		// draw in XOR
    ::MoveTo(_hdc,xx, yy);
    ::LineTo(_hdc, xe, ye);
    ::SetROP2(_hdc, R2_COPYPEN);	// regular pen
    ::SelectObject(_hdc, oldpen);	// something safe
    ReleaseHDC();
  }

//==================>>> vWinDC::DrawRubberPoint <<<======================
  void vWinDC::DrawRubberPoint(int x, int y)
  {
    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    vPen xorPen = _pen;			// start with current pen

    xorPen.SetColor(255,255,255);	// need it to be white

    GetHDC();				// Get a DC to use
    _hpen = xorPen.GetHPEN();		// Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);   	// Select
    ::SetROP2(_hdc, R2_XORPEN);		// draw in XOR
    ::MoveTo(_hdc,xx, yy);
    ::LineTo(_hdc, xx+1, yy+1);
    ::SetROP2(_hdc, R2_COPYPEN);	// regular pen
    ::SelectObject(_hdc, oldpen);	// something safe
    ReleaseHDC();
 }

//==================>>> vWinDC::DrawRubberRectangle <<<==================
  void vWinDC::DrawRubberRectangle(int x, int y, int width, int height)
  {
    if (height == 0 || width == 0)
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int x2 = Scale(x+width+_tx);
    int y2 = Scale(y+height+_ty);
    vPen xorPen = _pen;			// start with current pen

    xorPen.SetColor(255,255,255);	// need it to be white

    GetHDC();				// Get a DC to use
    _hpen = xorPen.GetHPEN();		// Get the pen
    HPEN oldpen = (HPEN)::SelectObject(_hdc, _hpen);    	// Select it
    HBRUSH oldbrush = static_cast<HBRUSH>(::SelectObject(_hdc, ::GetStockObject(NULL_BRUSH)));
    ::SetROP2(_hdc, R2_XORPEN);		// draw in XOR
    (void)::Rectangle(_hdc, xx, yy, x2, y2);
    ::SetROP2(_hdc, R2_COPYPEN);	// regular pen
    ::SelectObject(_hdc, oldpen);	// something safe
    ::SelectObject(_hdc, oldbrush);	// something safe

    ReleaseHDC();			// Release the DC
  }

//=====================>>> vWinDC::DrawAttrText <<<==========================
  void vWinDC::DrawAttrText(int x, int y, VCONST char* text, const ChrAttr attr)
  {
    // Draw text with attributes at given x, y.

    static int mapColor[] =
      {
	vC_Black, vC_Blue, vC_Green, vC_Cyan,
	vC_Red, vC_Magenta, vC_Yellow, vC_White,
        vC_DarkGray, vC_DimBlue, vC_DimGreen, vC_DimCyan,
	vC_DimRed, vC_DimMagenta, vC_DimYellow, vC_MedGray
      };

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    COLORREF oldbk = 0;
    COLORREF oldtc;

    GetHDC();				// Get a DC to use

    _font.LoadFont(_hdc,_isPrinterDC > 0);		// need to setup font!

    HFONT oldfont = (HFONT)::SelectObject(_hdc, _font.GetHFONT());	// select it in

    if (attr & ChHighlight)
      {
	oldbk = ::SetBkColor(_hdc, ::GetSysColor(COLOR_HIGHLIGHT));
	oldtc = ::SetTextColor(_hdc, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
	if (attr & 0xF0)		// A color overide
	  {
	    ::SetTextColor(_hdc,vStdColors[ mapColor[((attr & 0xF0) >> 4)] ].pixel());
	  }
      }
    else if (attr & ChReverse)
      {
        COLORREF crf = _canvasBG;
	oldtc = ::SetTextColor(_hdc, crf);
	if (_font.GetFontColor() != 0)
          {
            crf = _font.GetFontColor();
	    oldbk = ::SetBkColor(_hdc, crf);
          }
	else
          {
            crf = _pen.GetColor().pixel();
	    oldbk = ::SetBkColor(_hdc, crf);
          }
	if (attr & 0xF0)		// A color overide
	  {
	    ::SetTextColor(_hdc,vStdColors[ mapColor[((attr & 0xF0) >> 4)] ].pixel());
	  }
      }
    else if (attr & ChBlackOnBG)
      {
	oldtc = ::SetTextColor(_hdc,vStdColors[ mapColor[vC_Black] ].pixel());
	oldbk = ::SetBkColor(_hdc,vStdColors[ mapColor[((attr & 0xF0) >> 4)] ].pixel());
      }
    else if (attr & ChWhiteOnBG)
      {
	oldtc = ::SetTextColor(_hdc,vStdColors[ mapColor[vC_White] ].pixel());
	oldbk = ::SetBkColor(_hdc,vStdColors[ mapColor[((attr & 0xF0) >> 4)] ].pixel());
      }
    else
      {
	oldbk = ::SetBkColor(_hdc, _canvasBG);
	if (_font.GetFontColor() != 0)
	    oldtc = ::SetTextColor(_hdc, _font.GetFontColor());
	else
	    oldtc = ::SetTextColor(_hdc, _pen.GetColor().pixel());

	if (attr & 0xF0)		// A color overide
	  {
	    ::SetTextColor(_hdc,vStdColors[ mapColor[((attr & 0xF0) >> 4)] ].pixel());
	  }
      }


    int oldMode = 0;

    if (_pen.GetStyle() == vTransparent)
      {
	oldMode = ::GetBkMode(_hdc);		// current background mode
	::SetBkMode(_hdc,TRANSPARENT);
      }

    // Now the code to actually do the drawing
    if (_isPrinterDC > 0)
	::TextOut(_hdc, xx, yy-(_font.GetPointSize()), text, strlen(text));
    else
      {
        TEXTMETRIC tm;
        int fh = 8;
	if (::GetTextMetrics(_hdc, &tm))
            fh = tm.tmAscent + tm.tmDescent;
 	::TextOut(_hdc, xx, yy-fh, text, strlen(text));
      }

    if (oldMode != 0)
	::SetBkMode(_hdc,oldMode);		// Restore mode

    ::SelectObject(_hdc, oldfont);

    if (oldbk != 0)
      {
	::SetBkColor(_hdc, oldbk);
      }
    ::SetTextColor(_hdc,oldtc);

    ReleaseHDC();			// Release the DC

  }

//=====================>>> vWinDC::DrawText <<<==========================
  void vWinDC::DrawText(int x, int y, VCONST char* text)
  {
    // simple draw text at given x, y

    // First, do the required scaling

    if (!text || !*text)
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    COLORREF oldtc;

    GetHDC();				// Get a DC to use
    ::SetBkColor(_hdc, _canvasBG);

    _font.LoadFont(_hdc,_isPrinterDC > 0);		// need to setup font!
    HFONT curfont = _font.GetHFONT();
    HFONT oldfont = (HFONT)::SelectObject(_hdc, curfont);	// select it in

    if (_font.GetFontColor() != 0)
	oldtc = ::SetTextColor(_hdc, _font.GetFontColor());
    else
	oldtc = ::SetTextColor(_hdc, _pen.GetColor().pixel());

    // Now the code to actually do the drawing

    int oldMode = 0;

    if (_pen.GetStyle() == vTransparent)
      {
	oldMode = ::GetBkMode(_hdc);		// current background mode
	::SetBkMode(_hdc,TRANSPARENT);
      }

    if (_isPrinterDC > 0)
	::TextOut(_hdc, xx, yy-(_font.GetPointSize()), text, strlen(text));
    else
      {
	TEXTMETRIC tm;
        int fh = 8;
	if (::GetTextMetrics(_hdc, &tm))
            fh = tm.tmAscent + tm.tmDescent;
 	::TextOut(_hdc, xx, yy-fh, text, strlen(text));
      }

    if (oldMode != 0)
	::SetBkMode(_hdc,oldMode);		// Restore mode

    ::SetTextColor(_hdc,oldtc);

    ::SelectObject(_hdc, oldfont);

    ReleaseHDC();			// Release the DC

  }

//=====================>>> vWinDC::SetBrush <<<============================
 void vWinDC::SetBrush(VCONST vBrush& brush)
  {
    _brush = brush;
  }

//=====================>>> vWinDC::SetPen <<<============================
 void vWinDC::SetPen(VCONST vPen& pen)
  {
    _pen = pen;
  }

//====================>>> vWinDC::TextHeight <<<=============================
  int vWinDC::TextHeight(int& asc, int& des) VCONST
  {
    // Return total height of this font. V will use total height, which
    // is most often made up of ascent + descent.  This is too much
    // detail for the kind of apps V will support.

    GetHDC();				// Get a DC to use
    _font.LoadFont(_hdc,_isPrinterDC > 0);		// need to setup font!
    HFONT oldfont = (HFONT)::SelectObject(_hdc, _font.GetHFONT());	// select it in

    TEXTMETRIC tm;
    int a = 5, d = 3;			// something reasonable?

    if (::GetTextMetrics(_hdc, &tm))
      {
	a = tm.tmAscent; d = tm.tmDescent;
      }

    asc = a ; des = d;

    ::SelectObject(_hdc, oldfont);	// restore

    ReleaseHDC();			// Release the DC


    return a+d;
  }

//========================>>> vvWinDC::TextWidth <<<==========================
  int vWinDC::TextWidth(VCONST char* str) VCONST
  {
    int len = strlen(str);	// strlen

    GetHDC();				// Get a DC to use
    _font.LoadFont(_hdc,_isPrinterDC > 0);		// need to setup font!
    HFONT oldfont = (HFONT)::SelectObject(_hdc, _font.GetHFONT());	// select it in

#ifndef VWIN16
    SIZE size;
    if (!_isWin32s)
        ::GetTextExtentPoint32(_hdc, str, len,&size);
    else
        ::GetTextExtentPoint(_hdc, str, len,&size);
    ::SelectObject(_hdc,oldfont);
    ReleaseHDC();			// Release the DC
    return size.cx;
#else
    DWORD extent = ::GetTextExtent(_hdc, str, len);
    ReleaseHDC();			// Release the DC
    // call x to get the length
    return LOWORD(extent);
#endif
  }
