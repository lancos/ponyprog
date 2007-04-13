//===============================================================
// vpen.cxx: vColor class for drawing - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vapp.h>
#include <v/vpen.h>

//=======================>>> vPen::vPen <<<===========================
  vPen::vPen(unsigned int r, unsigned int g, unsigned int b,
	      int width, int style)
  {
      penColor._r = r, penColor._g = g; penColor._b = b;
      penColor._pixel = RGB(r,g,b);
      penWidth = width; penStyle = style; _created = 0; _hpen = 0;
  }

//=======================>>> vPen::~vPen <<<===========================
  vPen::~vPen()
  {
    ReleaseHpen();
  }

//=====================>>> vPen::vPen <<<===========================
  vPen::vPen(const vPen& p)
  {
    _created = 0;
    _hpen = 0;
    penColor = p.penColor;
    penWidth = p.penWidth;
    penStyle = p.penStyle;
  }

//=====================>>> vPen::operator= <<<===========================
  vPen & vPen::operator =(const vPen& p)
  {
    if (this == &p)     // assigning to self
      {
	return *this;
      }

    ReleaseHpen();
    // now just like a copy constructor
    penColor = p.penColor;
    penWidth = p.penWidth;
    penStyle = p.penStyle;

    return *this;               // allow r to l multiple assignment
  }

//=====================>>> vPen::SetStyle <<<===========================
  void vPen::SetStyle(int style)
  {
    if (style == penStyle)
	return;			// no change
    penStyle = style;
    ReleaseHpen();
  }

//=====================>>> vPen::SetWidth <<<===========================
  void vPen::SetWidth(int width)
  {
    if (width == penWidth)
	return;			// no change
    penWidth = width;
    ReleaseHpen();
  }


//=====================>>> vPen::SetColor <<<===========================
  void vPen::SetColor(const vColor& c)
  {
    if (c == penColor)
	return;			// no change
    penColor = c;
    ReleaseHpen();
  }

//=====================>>> vPen::SetColor <<<===========================
  void vPen::SetColor(unsigned int r, unsigned int g, unsigned int b)
  {
    if (penColor.r() == r && penColor.g() == g && penColor.b() == b)
	return;			// no change

    penColor.Set(r,g,b);
    ReleaseHpen();
  }


//=====================>>> vPen::GetHPEN <<<===========================
  HPEN vPen::GetHPEN() VCONST
  {
    if (!_created)
      {
	int ps;
	int pw = penWidth;

	switch (penStyle)
	  {
	    case vSolid:
		ps = PS_SOLID;
		break;

	    case vTransparent:
		ps = PS_NULL;
		break;

	    case vDash:
		ps = PS_DASH;
		pw = 1;		// $#$% Windows only allows width of 1
		break;

	    case vDot:
		ps = PS_DOT;
		pw = 1;
		break;

	    case vDashDot:
		ps = PS_DASHDOT;
		pw = 1;
		break;

	    default:
		ps = PS_SOLID;
		break;
	  }

	 _hpen = ::CreatePen(ps, pw, penColor._pixel);
	 _created = 1;
      }
      return _hpen;			// return a created pen handle
  }
