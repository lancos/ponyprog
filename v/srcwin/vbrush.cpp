//===============================================================
// vbrush.cxx: vBrush class for drawing - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>           // for Win 32 stuff
#include <v/vapp.h>
#include <v/vbrush.h>

//=======================>>> vBrush::vBrush <<<===========================
  vBrush::vBrush(unsigned int r, unsigned int g, unsigned int b,
              int style, int fillMode)
  {
      brushColor._r = (unsigned char) r;
      brushColor._g = (unsigned char) g;
      brushColor._b = (unsigned char) b;
      brushColor._pixel = RGB(r,g,b);
      brushStyle = style;
      brushFillMode = fillMode;
      _created = 0; _hbrush = 0;
  }

//=======================>>> vBrush::~vBrush <<<===========================
  vBrush::~vBrush()
  {
    if (_created)
      ::DeleteObject(_hbrush);
  }

//=====================>>> vBrush::vBrush <<<===========================
  vBrush::vBrush(const vBrush& p)
  {
    // Copy constructor - needs to delete object if already
    // created, and copy the values as needed
    if (_created)
        ::DeleteObject(_hbrush);
    _created = 0;
    _hbrush = 0;
    brushColor = p.brushColor;
    brushFillMode = p.brushFillMode;
    brushStyle = p.brushStyle;
  }

//=====================>>> vBrush::operator= <<<===========================
  vBrush& vBrush::operator =(const vBrush& p)
  {
    if (this == &p)     // assigning to self
      {
        return *this;
      }

    // now just like a copy constructor

    if (_created)
        ::DeleteObject(_hbrush);
    _created = 0;
    _hbrush = 0;
    brushColor = p.brushColor;
    brushFillMode = p.brushFillMode;
    brushStyle = p.brushStyle;

    return *this;               // allow r to l multiple assignment
  }

//=====================>>> vBrush::SetStyle <<<===========================
  void vBrush::SetStyle(int style)
  {
    if (style == brushStyle)
        return;                 // no change
    brushStyle = style;
    if (_created)
      {
        ::DeleteObject(_hbrush);
        _hbrush = 0; _created = 0;
      }
  }

//=====================>>> vBrush::SetFillMode <<<===========================
  void vBrush::SetFillMode(int fm)
  {
    if (fm == brushFillMode)
        return;                 // no change
    brushFillMode = fm;
    if (_created)
      {
        ::DeleteObject(_hbrush);
        _hbrush = 0; _created = 0;
      }
  }

//=====================>>> vBrush::SetColor <<<===========================
  void vBrush::SetColor(vColor c)
  {
    if (c == brushColor)
        return;                 // no change
    brushColor = c;
    if (_created)
      {
        ::DeleteObject(_hbrush);
        _hbrush = 0; _created = 0;
      }
  }

//=====================>>> vBrush::SetColor <<<===========================
  void vBrush::SetColor(unsigned int r, unsigned int g, unsigned int b)
  {
    if (brushColor.r() == r && brushColor.g() == g && brushColor.b() == b)
        return;                 // no change

    brushColor.Set(r,g,b);
    if (_created)
      {
        ::DeleteObject(_hbrush);
        _hbrush = 0; _created = 0;
      }
  }

//=====================>>> vBrush::GetHBRUSH <<<===========================
  HBRUSH vBrush::GetHBRUSH() VCONST
  {
    if (! _created)
      {
        switch (brushStyle)
          {
            case vSolid:
                _hbrush = ::CreateSolidBrush(brushColor._pixel);
                break;

            case vTransparent:
              {
                LOGBRUSH lb;
                lb.lbStyle = BS_NULL;
                lb.lbColor = 0;
                lb.lbHatch = 0;
                _hbrush = ::CreateBrushIndirect(&lb);
                break;
              }
            case vHorizontalHatch:
                _hbrush = ::CreateHatchBrush(HS_HORIZONTAL,brushColor._pixel);
                break;

            case vVerticleHatch:
                _hbrush = ::CreateHatchBrush(HS_VERTICAL,brushColor._pixel);
                break;

            case vLeftDiagonalHatch:
                _hbrush = ::CreateHatchBrush(HS_FDIAGONAL,brushColor._pixel);
                break;

            case vRightDiagonalHatch:
                _hbrush = ::CreateHatchBrush(HS_BDIAGONAL,brushColor._pixel);
                break;

            case vCrossHatch:
                _hbrush = ::CreateHatchBrush(HS_CROSS,brushColor._pixel);
                break;

            case vDiagonalCrossHatch:
                _hbrush = ::CreateHatchBrush(HS_DIAGCROSS,brushColor._pixel);
                break;

            default:
                _hbrush = ::CreateSolidBrush(brushColor._pixel);
                break;


          }

         _created = 1;
      }
      return _hbrush;                   // return a created brush handle
  }
