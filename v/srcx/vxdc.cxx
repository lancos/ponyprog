//===============================================================
// vXDC - a basic canvas for drawing
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <math.h>

#include <v/vxdc.h>
#include <v/vmemdc.h>

#include <v/vapp.h>		// need access to the app
#include <v/vicon.h>		// for icons

//-----------------------------------------------------------------------
// Define some X bitmaps for patterned fill
//
#define ldiag_width 16
#define ldiag_height 16
static char ldiag_bits[] = {
   0x01, 0x01, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20,
   0x40, 0x40, 0x80, 0x80, 0x01, 0x01, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08,
   0x10, 0x10, 0x20, 0x20, 0x40, 0x40, 0x80, 0x80};
   static Pixmap ldiagPM = 0;

#define cdiag_width 16
#define cdiag_height 16
static char cdiag_bits[] = {
   0x81, 0x81, 0x42, 0x42, 0x24, 0x24, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24,
   0x42, 0x42, 0x81, 0x81, 0x81, 0x81, 0x42, 0x42, 0x24, 0x24, 0x18, 0x18,
   0x18, 0x18, 0x24, 0x24, 0x42, 0x42, 0x81, 0x81};
   static Pixmap cdiagPM = 0;

#define cross_width 15
#define cross_height 15
static char cross_bits[] = {
   0x84, 0x10, 0x84, 0x10, 0xff, 0x7f, 0x84, 0x10, 0x84, 0x10, 0x84, 0x10,
   0x84, 0x10, 0xff, 0x7f, 0x84, 0x10, 0x84, 0x10, 0x84, 0x10, 0x84, 0x10,
   0xff, 0x7f, 0x84, 0x10, 0x84, 0x10};
   static Pixmap crossPM = 0;

#define rdiag_width 16
#define rdiag_height 16
  static char rdiag_bits[] = {
   0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04,
   0x02, 0x02, 0x01, 0x01, 0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10,
   0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x01, 0x01};
   static Pixmap rdiagPM = 0;

#define horiz_width 15
#define horiz_height 15
static char horiz_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xff, 0x7f, 0x00, 0x00, 0x00, 0x00};
   static Pixmap horizPM = 0;

#define verti_width 15
#define verti_height 15
static char verti_bits[] = {
   0x84, 0x10, 0x84, 0x10, 0x84, 0x10, 0x84, 0x10, 0x84, 0x10, 0x84, 0x10,
   0x84, 0x10, 0x84, 0x10, 0x84, 0x10, 0x84, 0x10, 0x84, 0x10, 0x84, 0x10,
   0x84, 0x10, 0x84, 0x10, 0x84, 0x10};
   static Pixmap vertiPM = 0;

  vFont vXDC::_GCFont;		// the font actually in the GC
  vPen vXDC::_GCPen;		// the pen in use
  vBrush vXDC::_GCBrush;	// the brush in use

//================>>> vXDC::vXDC <<<========================
  vXDC::vXDC()
  {

    SysDebug(Constructor,"vXDC::vXDC() constructor\n")
  }

//================>>> vXDC::~vXDC <<<========================
  vXDC::~vXDC()
  {
    SysDebug(Destructor,"vXDC::~vXDC() destructor\n")
  }

//=====================>>> vXDC::GetDrawable <<<======================
  Drawable vXDC::GetDrawable()
  {
    return 0;
  }

//=====================>>> vXDC::Clear <<<==========================
  void vXDC::Clear(void)
  {
    ClearRect(0, 0, _physWidth, _physHeight);
    return;
  }

//==================>>> vXDC::ClearRect <<<==========================
  void vXDC::ClearRect(int x, int y, int width, int height)
  {
    // Clear a rectangluar area starting at left,top of width and height

    if (height == 0 || width == 0)	// ignore 0 h/w
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int h = Scale(height);
    int w = Scale(width);

    if (w < 0)		// convert to something we like
      {
	w = -w;		// make a positive width
	xx = xx - w;	// translate x
      }
    if (h < 0)
      {
	h = -h;		// positive h
	yy = yy - h;	// translate x
      }

    Drawable dw = GetDrawable();
    if (dw != 0)
	XClearArea(_XDisplay, dw, xx, yy, w, h, 0);
  }

//=====================>>> vXDC::CopyFromMemoryDC <<<==========================
  void vXDC::CopyFromMemoryDC(vMemoryDC* vmemdc, int destX, int destY,
	int srcX, int srcY, int srcW, int srcH)
  {
    // Copy myself to another DC
    int ch = vmemdc->_physHeight;
    int cw = vmemdc->_physWidth;
    int cx = 0;
    int cy = 0;

    if (srcX > 0)		// Fixed: 10/11/96
	cx = srcX;
    if (srcY > 0)
	cy = srcY;
    if (srcW > 0)
	cw = srcW;
    if (srcH > 0)
	ch = srcH;

    if ((cw + cx) > vmemdc->_physWidth && _physWidth > 0)       // only copy what will fi
	cw = _physWidth - cx;
    if ((ch + cy) > vmemdc->_physHeight && _physHeight > 0)
	ch = _physHeight - cy; 

    if ((cw + destX) > _physWidth && _physWidth > 0)
        cw = _physWidth - destX;
    if ((ch + destY) > _physHeight && _physHeight > 0)
        ch = _physHeight - destY;

    Drawable dw = GetDrawable();
    Drawable mdw = vmemdc->GetDrawable();

    if (dw != 0 && mdw != 0)
	XCopyArea(_XDisplay, 
	    mdw,		 	// Source drawable
	    dw,				// Destination drawable
	    _GC,			// Use Destination GC
	    cx,cy,			// draw from 0,0 in source
	    cw, ch,			// using this width and height
	    destX, destY);		// to here in destination
	
  }

#ifdef DRAWARC
//====================>>> vXDC::DrawArc <<<==========================
  void vXDC::DrawArc(int xx1, int yy1, int xx2, int yy2, 
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
	 SetGCtoBrush();
	 if (GetDrawable() != 0)
	     XFillArc(_XDisplay, GetDrawable(), _GC, 
	        xxc - r, yyc - r, 2 * r, 2 * r, alpha1, alpha2);
       }

    if (_pen.penStyle != vTransparent)
      {
	SetGCtoPen();
	if (GetDrawable()!=0)
	    XDrawArc(_XDisplay, GetDrawable(), _GC,
	        xxc - r, yyc - r, 2 * r, 2 * r, alpha1, alpha2);
      }
  }
#endif

//====================>>> vXDC::DrawEllipse <<<==========================
  void vXDC::DrawEllipse(int x, int y, int width, int height)
  {
    static const int angle = (360 * 64);
    
    if (height == 0 || width == 0)	// ignore 0 h/w
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int w = Scale(width);
    int h = Scale(height);

    if (w < 0)		// convert to something we like
      {
	w = -w;		// make a positive width
	xx = xx - w;	// translate x
      }
    if (h < 0)
      {
	h = -h;		// positive h
	yy = yy - h;	// translate x
      }

    if (_brush.brushStyle != vTransparent)
      {
	SetGCtoBrush();
	XFillArc(_XDisplay, GetDrawable(), _GC, xx, yy, w, h, 0, angle);
      }

    if (_pen.penStyle != vTransparent)
      {
	SetGCtoPen();
	XDrawArc(_XDisplay, GetDrawable(), _GC, xx, yy, w, h, 0, angle);
      }
  }

//====================>>> vXDC::DrawIcon <<<==========================
  void vXDC::DrawIcon(int x, int y, VCONST vIcon& icon)
  {

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    Pixmap iconPM = icon.GetXPM(0);		// may already be mapped

    if (iconPM == 0)
      {
	DrawRectangle(x,y,icon.width,icon.height);
      }
    else if (icon.depth == 1)		// monochrome bitmap
      {
	XCopyPlane (_XDisplay, iconPM, GetDrawable(), _GC,
            0, 0, icon.width, icon.height, xx, yy, 1);
      }
    else
      {
	XCopyArea(_XDisplay, iconPM, GetDrawable(), _GC,
            0, 0, icon.width, icon.height, xx, yy);
      }
  }

//====================>>> vXDC::DrawLine <<<==========================
  void vXDC::DrawLine(int x, int y, int xend, int yend)
  {
    // Draw a line from x,y to xend,yend

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int xe = Scale(xend+_tx);
    int ye = Scale(yend+_ty);

    SetGCtoPen();

#ifdef NO_NEGATIVE_LINES	// Some versions of X die on this
    if (xx < 0) xx = 0;
    if (yy < 0) yy = 0;
    if (xe < 0) xe = 0;
    if (ye < 0) ye = 0;		// X hates negative coords
#endif

    XDrawLine(_XDisplay, GetDrawable(), _GC, xx, yy, xe, ye);
  }

//====================>>> vXDC::DrawLines <<<==========================
  void vXDC::DrawLines(vLine* lineList, int count)
  {
    // Draw a line from x,y to xend,yend

    if (count < 1 || lineList == 0)
	return;

    SetGCtoPen();

    vLine* lp = lineList ;
    for (int num = 0 ; num < count ; ++lp, ++num)
      {
	int xx = Scale(lp->x+_tx);
	int yy = Scale(lp->y+_ty);
	int xe = Scale(lp->xend+_tx);
	int ye = Scale(lp->yend+_ty);


#ifdef NO_NEGATIVE_LINES	// Some versions of X die on this
	if (xx < 0) xx = 0;
        if (yy < 0) yy = 0;
	if (xe < 0) xe = 0;
	if (ye < 0) ye = 0;		// X hates negative coords
#endif

        XDrawLine(_XDisplay, GetDrawable(), _GC, xx, yy, xe, ye);
      }
  }

//==================>>> vXDC::DrawLines <<<======================
  void vXDC::DrawLines(vPoint* points, int n)
  {
    // Draw a poly line contecting each point


    if (n < 2 || points == 0)
	return;

    SetGCtoPen();

    XPoint* xpoints;

    if (_hasScale || _tx != 0 || _ty != 0) // If we have to scale, then we need to copy
      {
	xpoints = new XPoint[n];	// allocate space for copy
	for (int i = 0; i < n; i++)
	  {
	    xpoints[i].x = ((points[i].x+_tx) * _Mult) / _Div;	// scale
	    xpoints[i].y = ((points[i].y+_ty) * _Mult) / _Div;
	  }
      }
    else
      {
	xpoints = (XPoint*)points;	// just use original values
      }

    XDrawLines (_XDisplay, GetDrawable(), _GC, xpoints, n, 0);

    if (_hasScale)
	delete[]xpoints;
  }

//==================>>> vXDC::DrawColorPoints <<<======================
  void vXDC::DrawColorPoints(int x, int y, int nPoints, vColor* pointList)
  {
    // Draw a bunch of color points. 

    if (nPoints < 1 || pointList == 0)
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    if (yy < 0)
	return;				// row not displayed

    Pixel curpix = pointList[0].pixel();	// First pixel
    Pixel prevpix = curpix;

    XSetBackground(_XDisplay, _GC, _canvasBG);
    XSetForeground(_XDisplay, _GC, curpix);

    if (xx < 0)				// need to check every time
      {
	for (int ix = 0 ; ix < nPoints ; ++ix)
	  {
	    if (xx+ix < 0)
		continue;			// this one not displayed
	    curpix = pointList[ix].pixel();
	    if (curpix != prevpix)		// don't reset color if same
	      {
		XSetForeground(_XDisplay, _GC, curpix);
		prevpix = curpix;
	      }
	    XDrawPoint(_XDisplay, GetDrawable(), _GC, xx+ix, yy);
	  }
      }
    else				// don't need xx check
      {
	for (int ix = 0 ; ix < nPoints ; ++ix)
	  {
	    curpix = pointList[ix].pixel();
	    if (curpix != prevpix)		// don't reset color if same
	      {
		XSetForeground(_XDisplay, _GC, curpix);
		prevpix = curpix;
	      }
	    XDrawPoint(_XDisplay, GetDrawable(), _GC, xx+ix, yy);
	  }
      }
  }

//==================>>> vXDC::DrawPoint <<<======================
  void vXDC::DrawPoint(int x, int y)
  {

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    if (xx < 0 || yy < 0) return;	// not displayable

    SetGCtoPen();

    XDrawPoint(_XDisplay, GetDrawable(), _GC, xx, yy);
  }

//==================>>> vXDC::DrawPoints <<<======================
  void vXDC::DrawPoints(vPoint* pointList, int count)
  {

    if (count < 1 || pointList == 0)
	return;
    SetGCtoPen();

    vPoint* pl = pointList ;
    for (int num = 0 ; num < count ; ++pl, ++num)
      {
	int xx = Scale(pl->x+_tx);
	int yy = Scale(pl->y+_ty);

	if (xx < 0 || yy < 0) 
	    continue;	// not displayable

	XDrawPoint(_XDisplay, GetDrawable(), _GC, xx, yy);
      }
  }

//==================>>> vXDC::DrawPolygon <<<======================
  void vXDC::DrawPolygon(int n, vPoint points[], int fillStyle)
  {
    // draw a complete polygon (starting point specified twice!)


    XPoint* xpoints;

    if (_hasScale || _tx != 0 || _ty != 0) // If we have to scale, then we need to copy
      {
	xpoints = new XPoint[n];	// allocate space for copy
	for (int i = 0; i < n; i++)
	  {
	    xpoints[i].x = ((points[i].x+_tx) * _Mult) / _Div;	// scale
	    xpoints[i].y = ((points[i].y+_ty) * _Mult) / _Div;
	  }
      }
    else
      {
	xpoints = (XPoint*)points;	// just use original values
      }


    if (_brush.brushStyle != vTransparent)
      {
	SetGCtoBrush();
	XSetFillRule (_XDisplay, _GC,	// how to fill the polygon
	    (fillStyle == vAlternate) ? EvenOddRule : WindingRule);
						// xpoints1
	XFillPolygon (_XDisplay, GetDrawable(), _GC, xpoints, n-1, Complex, 0);
	XSetFillRule (_XDisplay, _GC, EvenOddRule);	// default mode
      }

    if (_pen.penStyle != vTransparent)
      {
	SetGCtoPen();
					// xpoints2
	XDrawLines (_XDisplay, GetDrawable(), _GC, xpoints, n, 0);
      }

    if (_hasScale)
	delete[]xpoints;
}

//==================>>> vXDC::DrawRectangle <<<======================
  void vXDC::DrawRectangle(int x, int y, int width, int height)
  {

    if (height == 0 || width == 0)	// ignore 0 h/w
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int w = Scale(width);
    int h = Scale(height);

    if (w < 0)		// convert to something we like
      {
	w = -w;		// make a positive width
	xx = xx - w;	// translate x
      }
    if (h < 0)
      {
	h = -h;		// positive h
	yy = yy - h;	// translate x
      }

    if (_brush.brushStyle != vTransparent)
      {
	SetGCtoBrush();
	XFillRectangle(_XDisplay, GetDrawable(), _GC, xx, yy, w, h);
      }

    if (_pen.penStyle != vTransparent)
      {
	SetGCtoPen();
	XDrawRectangle(_XDisplay, GetDrawable(), _GC, xx, yy, w, h);
      }
  }

//==================>>> vXDC::DrawRectangles <<<======================
  void vXDC::DrawRectangles(vRect* rectList, int count)
  {

    if (count < 1 || rectList == 0)
	return;

    if (_brush.brushStyle != vTransparent)
	SetGCtoBrush();

    if (_pen.penStyle != vTransparent)
	SetGCtoPen();

    vRect* rp = rectList ;
    for (int num = 0 ; num < count ; ++rp, ++num)
      {
	int xx = Scale(rp->x + _tx);
	int yy = Scale(rp->y + _ty);
	int w = Scale(rp->w);
	int h = Scale(rp->h);

	if (h == 0 || w == 0)	// ignore 0 h/w
	    continue;

	if (w < 0)		// convert to something we like
	  {
	    w = -w;		// make a positive width
	    xx = xx - w;	// translate x
	  }
	if (h < 0)
	  {
	    h = -h;		// positive h
	    yy = yy - h;	// translate x
	  }

	if (_brush.brushStyle != vTransparent)
	  {
	    XFillRectangle(_XDisplay, GetDrawable(), _GC, xx, yy, w, h);
	  }

	if (_pen.penStyle != vTransparent)
	  {
	    XDrawRectangle(_XDisplay, GetDrawable(), _GC, xx, yy, w, h);
	  }
      }
  }

//================>>> vXDC::DrawRoundedRectangle <<<===================
  void vXDC::DrawRoundedRectangle(int x, int y,
	int width, int height, int radius)
  {
    if (height == 0 || width == 0)	// ignore 0 h/w
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int w = Scale(width);
    int h = Scale(height);
    int r;

    if (w < 0)		// convert to something we like
      {
	w = -w;		// make a positive width
	xx = xx - w;	// translate x
      }
    if (h < 0)
      {
	h = -h;		// positive h
	yy = yy - h;	// translate x
      }

    if (radius < 0)
      {
	// Negative radius means divide average of hight and width
	// by this
	r = ((w+h)/(-2 * radius));
      }
    else
	r = Scale(radius);

    int cw = r + r;

    if (_brush.brushStyle != vTransparent)
      {
	SetGCtoBrush();
	// Draw three rectangles: the vertical, and then the sides

	XFillRectangle(_XDisplay, GetDrawable(), _GC, // full height in middle
		xx+r, yy, w-cw, h);
	XFillRectangle(_XDisplay, GetDrawable(), _GC, // left vertical box
		xx, yy+r, r, h-cw);
	XFillRectangle(_XDisplay, GetDrawable(), _GC, // right vertical box
		xx+w-r, yy+r, r, h-cw);

	XFillArc(_XDisplay, GetDrawable(), _GC, // upper left
		xx,yy,cw,cw, 90*64, 90*64);
	XFillArc(_XDisplay, GetDrawable(), _GC, // upper right
		xx+w-cw, yy, cw, cw, 0, 90*64);
	XFillArc(_XDisplay, GetDrawable(), _GC, // bottom left
		xx, yy+h-cw, cw, cw, 180 * 64, 90 * 64);
	XFillArc(_XDisplay, GetDrawable(), _GC, // bottom right
		xx+w-cw, yy+h-cw,cw,cw, 270 * 64, 90 * 64);
      }

    if (_pen.penStyle != vTransparent)
      {
	SetGCtoPen();
	XDrawLine(_XDisplay, GetDrawable(), _GC, xx+r, yy, xx+w-r, yy);
	XDrawLine(_XDisplay, GetDrawable(), _GC, xx+r, yy+h, xx+w-r, yy+h);
	XDrawLine(_XDisplay, GetDrawable(), _GC, xx, yy+r, xx, yy+h-r);
	XDrawLine(_XDisplay, GetDrawable(), _GC, xx+w, yy+r, xx+w, yy+h-r);
	XDrawArc(_XDisplay, GetDrawable(), _GC, // upper left
		xx,yy,cw,cw, 90*64, 90*64);
	XDrawArc(_XDisplay, GetDrawable(), _GC, // upper right
		xx+w-cw, yy, cw, cw, 0, 90*64);
	XDrawArc(_XDisplay, GetDrawable(), _GC, // bottom left
		xx, yy+h-cw, cw, cw, 180 * 64, 90 * 64);
	XDrawArc(_XDisplay, GetDrawable(), _GC, // bottom right
		xx+w-cw, yy+h-cw,cw,cw, 270 * 64, 90 * 64);
      }
  }

//==================>>> vXDC::DrawRubberEllipse <<<===================
  void vXDC::DrawRubberEllipse(int x, int y, int width, int height)
  {
    if (height == 0 || width == 0)	// ignore 0 h/w
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int w = Scale(width);
    int h = Scale(height);

    if (w < 0)		// convert to something we like
      {
	w = -w;		// make a positive width
	xx = xx - w;	// translate x
      }
    if (h < 0)
      {
	h = -h;		// positive h
	yy = yy - h;	// translate x
      }

    static const int angle = (360 * 64);

    SetPenStyle(_XorGC);
    XDrawArc(_XDisplay, GetDrawable(), _XorGC, xx, yy, w, h, 0, angle);
  }

//==================>>> vXDC::DrawRubberLine <<<======================
  void vXDC::DrawRubberLine(int x, int y, int xend, int yend)
  {
    // Draw a rubber-band line from x,y to xend,yend. Redrawing
    // over the same with will erase it.

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int xe = Scale(xend+_tx);
    int ye = Scale(yend+_ty);
    if (xx < 0) xx = 0;
    if (yy < 0) yy = 0;
    if (xe < 0) xe = 0;
    if (ye < 0) ye = 0;		// X hates negative coords

    SetPenStyle(_XorGC);
    XDrawLine(_XDisplay, GetDrawable(), _XorGC, xx, yy, xe, ye);
  }

//==================>>> vXDC::DrawRubberPoint <<<======================
  void vXDC::DrawRubberPoint(int x, int y)
  {
    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    if (xx < 0) xx = 0;
    if (yy < 0) yy = 0;

    SetPenStyle(_XorGC);
    XDrawPoint(_XDisplay, GetDrawable(), _GC, xx, yy);
  }

//==================>>> vXDC::DrawRubberRectangle <<<==================
  void vXDC::DrawRubberRectangle(int x, int y, int width, int height)
  {
    if (height == 0 || width == 0)	// ignore 0 h/w
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int w = Scale(width);
    int h = Scale(height);

    if (w < 0)		// convert to something we like
      {
	w = -w;		// make a positive width
	xx = xx - w;	// translate x
      }
    if (h < 0)
      {
	h = -h;		// positive h
	yy = yy - h;	// translate x
      }

    SetPenStyle(_XorGC);
    XDrawRectangle(_XDisplay, GetDrawable(), _XorGC, xx, yy, w, h);
  }

//=====================>>> vXDC::DrawAttrText <<<==========================
  void vXDC::DrawAttrText(int x, int y, VCONST char* text, const ChrAttr attr)
  {
    // Draw text with attributes at given x, y.

    // The fonts may not be equal because we are using a shared GC,
    // and another window might change the font in the GC.  We
    // need to check the current font and change to our font if
    // it is not the same

    static int mapColor[] =
      {
	vC_Black, vC_Blue, vC_Green, vC_Cyan,
        vC_Red, vC_Magenta, vC_Yellow, vC_White,
        vC_DarkGray, vC_DimBlue, vC_DimGreen, vC_DimCyan,
	vC_DimRed, vC_DimMagenta, vC_DimYellow, vC_MedGray
      };
    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    if (_GCFont.GetXFont() != _font.GetXFont())
      {
	// Change the font in the server
	XSetFont( theApp->display(), _GC, 
	    _font.GetXFont()->fid);
	_GCFont = _font;	// update current font
      }

    Pixel attrColor = _pen.penColor.pixel();	// Default
    if (attr & 0xF0 && theApp->Xdepth() > 1)	// a color attribute
	attrColor = vStdColors[ mapColor[((attr & 0xF0) >> 4)] ].pixel();

    if (attr & ChReverse || attr & ChHighlight)
      {
	XSetForeground(_XDisplay, _GC, _canvasBG);
	XSetBackground(_XDisplay, _GC, attrColor);
      }
    else if (attr & ChBlackOnBG)
      {
	XSetForeground(_XDisplay,_GC,
		vStdColors[ mapColor[vC_Black] ].pixel());
	XSetBackground(_XDisplay, _GC,
		vStdColors[ mapColor[((attr & 0xF0) >> 4)] ].pixel());
      }
    else if (attr & ChWhiteOnBG)
      {
	XSetForeground(_XDisplay,_GC,
		vStdColors[ mapColor[vC_Black] ].pixel());
	XSetBackground(_XDisplay, _GC,
		vStdColors[ mapColor[((attr & 0xF0) >> 4)] ].pixel());
      } 
    else
      {
	XSetForeground(_XDisplay, _GC, attrColor);
	XSetBackground(_XDisplay, _GC, _canvasBG);
      }

    int len = strlen(text);
    if (_font.GetAngle() == 90 || _font.GetAngle() == 180)
      {
	int asc, des;
	int height = _font.XFontH(asc, des);
	int ny;
	if (_font.GetAngle() == 90)
	    ny = yy - (height-des)*(len-1);
	else
	    ny = yy;
	for (int i = 0 ; i<len ; i++, y += (height-des))
	  {
	    if (_pen.penStyle != vTransparent)
		XDrawImageString(_XDisplay, GetDrawable(),
                             _GC, xx, y, &text[i], 1);
	    else
		XDrawString(_XDisplay, GetDrawable(),
                        _GC, xx, y, &text[i], 1);
	  }

	  // X can't do underlined, so we have to do it ourselves
	  if (_font.GetUnderlined())          // untested
	    {
	      int width = XTextWidth(_font.GetXFont(),text,len);
	      XDrawLine(_XDisplay, GetDrawable(), _GC, xx+1, yy, xx+1, yy+width);
	    }      
      }
    else
      {
	if (_font.GetAngle() == 270)
	  {
	    xx = xx - XTextWidth(_font.GetXFont(),text,len);
	  }
	if (_pen.penStyle != vTransparent)
	    XDrawImageString(_XDisplay, GetDrawable(),
	    _GC, xx, yy, text, len);
	else
	    XDrawString(_XDisplay, GetDrawable(),
	    _GC, xx, yy, text, len);

	// X can't do underlined, so we have to do it ourselves
	if (_font.GetUnderlined())
	  {
	    int width = XTextWidth(_font.GetXFont(),text,len);
	    XDrawLine(_XDisplay, GetDrawable(), _GC, xx, yy+1, xx+width, yy+1);
	  }
      }

    if (attr)		// & ChReverse)
      {
	XSetForeground(_XDisplay, _GC, _pen.penColor.pixel());
	XSetBackground(_XDisplay, _GC, _canvasBG);
      }
  }

//=====================>>> vXDC::DrawText <<<==========================
  void vXDC::DrawText(int x, int y, VCONST char* text)
  {
    // simple draw text at given x, y

    // The fonts may not be equal because we are using a shared GC,
    // and another window might change the font in the GC.  We
    // need to check the current font and change to our font if
    // it is not the same

    if (!text || !*text)
	return;
    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    if (_GCFont.GetXFont() != _font.GetXFont())
      {
	// Change the font in the server
	XSetFont( theApp->display(), _GC, 
	    _font.GetXFont()->fid);
	_GCFont = _font;	// update current font
      }

    XSetForeground(_XDisplay, _GC, _pen.penColor.pixel());
    XSetBackground(_XDisplay, _GC, _canvasBG);

    int len = strlen(text);
    if (_font.GetAngle() == 90 || _font.GetAngle() == 180)
      {
	int asc, des;
	int height = _font.XFontH(asc, des);
	int ny;
	if (_font.GetAngle() == 90)
	    ny = yy - (height-des)*(len-1);
	else
	    ny = yy;
	for (int i = 0 ; i<len ; i++, ny += (height-des))
	  {
	    if (_pen.penStyle != vTransparent)
		XDrawImageString(_XDisplay, GetDrawable(),
                             _GC, xx, ny, &text[i], 1);
	    else
		XDrawString(_XDisplay, GetDrawable(),
                        _GC, xx, ny, &text[i], 1);
	  }

	  // X can't do underlined, so we have to do it ourselves
	  if (_font.GetUnderlined())          // untested
	    {
	      int width = XTextWidth(_font.GetXFont(),text,len);
	      XDrawLine(_XDisplay, GetDrawable(), _GC, xx+1, yy, xx+1, yy+width);
	    }      
      }
    else
      {
	if (_font.GetAngle() == 270)
	  {
	    xx = xx - XTextWidth(_font.GetXFont(),text,len);
	  }

	if (_pen.penStyle != vTransparent)
	    XDrawImageString(_XDisplay, GetDrawable(),
	      _GC, xx, yy, text, len);
	else
	    XDrawString(_XDisplay, GetDrawable(),
    	      _GC, xx, yy, text, len);

        // X can't do underlined, so we have to do it ourselves
	if (_font.GetUnderlined())
	  {
	    int width = XTextWidth(_font.GetXFont(),text,len);
	    XDrawLine(_XDisplay, GetDrawable(), _GC, xx, yy+1, xx+width, yy+1);
	  }
      }

  }

//================>>> vXDC::SetBackground <<<==========================
  void vXDC::SetBackground(VCONST vColor& color)
  {
    if (!color.IsMapped())	// Map the color?
	color.MapColor();
    
    _canvasBG = color.pixel();		// retrieve X pixel value
  }

//=====================>>> vXDC::SetBrush <<<============================
 void vXDC::SetBrush(VCONST vBrush& brush)
  {
    _brush = brush;
    if (!_brush.brushColor.IsMapped())	// Map the color?
	_brush.brushColor.MapColor();
  }

//=====================>>> vXDC::SetGCtoBrush <<<========================
  void vXDC::SetGCtoBrush()
  {

    XSetForeground(_XDisplay, _GC, _brush.brushColor.pixel());

    if (_GCBrush == _brush)
	return;

    Pixmap my_stipple = 0;

    switch (_brush.brushStyle)
      {
	case vSolid:
	    XSetFillStyle(_XDisplay,_GC,FillSolid);
	    break;

	case vTransparent:
	    XSetFillStyle(_XDisplay,_GC,FillSolid);
	    break;

	case vHorizontalHatch:
	  {
	    if (!horizPM)
		horizPM = XCreateBitmapFromData (_XDisplay,
			      RootWindow (_XDisplay, DefaultScreen (_XDisplay)),
				     horiz_bits, horiz_width, horiz_height);
	    XSetFillStyle(_XDisplay,_GC,FillStippled);	// we will set pixmaps
	    XSetStipple (_XDisplay, _GC, horizPM);
	    break;
	  }

	case vVerticleHatch:
	  {
	    if (!vertiPM)
		vertiPM = XCreateBitmapFromData (_XDisplay,
			      RootWindow (_XDisplay, DefaultScreen (_XDisplay)),
				     verti_bits, verti_width, verti_height);
	    XSetFillStyle(_XDisplay,_GC,FillStippled);	// we will set pixmaps
	    XSetStipple (_XDisplay, _GC, vertiPM);
	    break;
	  }

	case vLeftDiagonalHatch:
	  {
	    if (!ldiagPM)
		ldiagPM = XCreateBitmapFromData (_XDisplay,
			      RootWindow (_XDisplay, DefaultScreen (_XDisplay)),
				     ldiag_bits, ldiag_width, ldiag_height);
	    XSetFillStyle(_XDisplay,_GC,FillStippled);	// we will set pixmaps
	    XSetStipple (_XDisplay, _GC, ldiagPM);
	    break;
	  }

	case vRightDiagonalHatch:
	  {
	    if (!rdiagPM)
		rdiagPM = XCreateBitmapFromData (_XDisplay,
			      RootWindow (_XDisplay, DefaultScreen (_XDisplay)),
				     rdiag_bits, rdiag_width, rdiag_height);
	    XSetFillStyle(_XDisplay,_GC,FillStippled);	// we will set pixmaps
	    XSetStipple (_XDisplay, _GC, rdiagPM);
	    break;
	  }

	case vCrossHatch:
	  {
	    if (!crossPM)
		crossPM = XCreateBitmapFromData (_XDisplay,
			      RootWindow (_XDisplay, DefaultScreen (_XDisplay)),
				     cross_bits, cross_width, cross_height);
	    XSetFillStyle(_XDisplay,_GC,FillStippled);	// we will set pixmaps
	    XSetStipple (_XDisplay, _GC, crossPM);
	    break;
	  }

	case vDiagonalCrossHatch:
	  {
	    if (!cdiagPM)
		cdiagPM = XCreateBitmapFromData (_XDisplay,
			      RootWindow (_XDisplay, DefaultScreen (_XDisplay)),
				     cdiag_bits, cdiag_width, cdiag_height);
	    XSetFillStyle(_XDisplay,_GC,FillStippled);	// we will set pixmaps
	    XSetStipple (_XDisplay, _GC, cdiagPM);
	    break;
	  }

	default:
	    XSetFillStyle(_XDisplay,_GC,FillSolid);
	    break;
      }

    _GCBrush = _brush;

  }


//=====================>>> vXDC::SetPenStyle <<<========================
  void vXDC::SetPenStyle(GC gc)
  {
    static char dot[] = {2, 5};
    static char dash[] = {4, 4};
    static char dashdot[] = {6, 6, 2, 6};

    int ps;
    switch (_pen.penStyle)		// find a suitable X line style
      {
	case vSolid:
	case vTransparent:
	    ps = LineSolid;
	    break;

	case vDash:
	    ps = LineOnOffDash;
	    XSetDashes(_XDisplay, gc, 0, dash, 2);
	    break;

	case vDot:
	    ps = LineOnOffDash;
	    XSetDashes(_XDisplay, gc, 0, dot, 2);
	    break;

	case vDashDot:
	    ps = LineOnOffDash;
	    XSetDashes(_XDisplay, gc, 0, dashdot, 4);
	    break;

	default:
	    ps = LineSolid;
	    break;
      }

    XSetLineAttributes(_XDisplay, gc, _pen.penWidth, ps, CapButt, JoinMiter);
  }

//=====================>>> vXDC::SetGCtoPen <<<========================
  void vXDC::SetGCtoPen()
  {

    XSetForeground(_XDisplay, _GC, _pen.penColor.pixel());
    XSetBackground(_XDisplay, _GC, _canvasBG);

    if (_GCPen == _pen)
	return;

    SetPenStyle(_GC);

    XSetFillStyle(_XDisplay,_GC,FillSolid);

    _GCPen = _pen;

  }

//=====================>>> vXDC::SetPen <<<============================
 void vXDC::SetPen(VCONST vPen& pen)
  {
    _pen = pen;
    if (!_pen.penColor.IsMapped())	// Map the color?
	_pen.penColor.MapColor();
  }

//======================>>> vXDC::SetFont <<<===========================
  void vXDC::SetFont(VCONST vFont& vf)
  {
    // Change the font associated with this window.

    _font = vf;

    // First, make sure the font is loaded

    _GCFont = _font;		// we have changed the font in
					// the shared GC
    _font.LoadFont();

    XSetFont( theApp->display(), _GC, 
	_font.GetXFont()->fid);
  }

//====================>>> vXDC::TextHeight <<<=============================
  int vXDC::TextHeight(int& asc, int& des) VCONST
  {
    // Return total height of this font. V will use total height, which
    // is most often made up of ascent + descent.  This is too much
    // detail for the kind of apps V will support.

    return _font.XFontH(asc, des);
  }

//========================>>> vXDC::TextWidth <<<==========================
  int vXDC::TextWidth(VCONST char* str) VCONST
  {
    if (_font.GetAngle() != 0)
      {
	int asc, des;
	int height = _font.XFontH(asc, des);
	if (_font.GetAngle() == 90 || _font.GetAngle() == 180)
	    return (height-des)*(strlen(str));
      }

    return _font.XTextW(str);
  }
