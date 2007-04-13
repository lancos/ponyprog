//===============================================================
// vcolor.cxx: vColor class for drawing - Windows
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
#include <v/vcolor.h>

    // Define 16 standard colors for use by everyone

    vColor vGlobalStdColors[16] =
      {
	vColor( 0, 0, 0),	// vC_Black
	vColor( 255, 0, 0),	// vC_Red
	vColor( 127, 0, 0),	// vC_DimRed
	vColor( 0, 255, 0),	// vC_Green
	vColor( 0, 127, 0),	// vC_DimGreen
	vColor( 0, 0, 255),	// vC_Blue
	vColor( 0, 0, 127),	// vC_DimBlue
	vColor( 255, 255, 0),	// vC_Yellow
	vColor( 127, 127, 0),	// vC_DimYellow
	vColor( 255, 0, 255),	// vC_Magenta
	vColor( 127, 0, 127),	// vC_DimMagenta
	vColor( 0, 255, 255),	// vC_Cyan
	vColor( 0, 127, 127),	// vC_DimCyan
	vColor( 63, 63, 63),	// vC_DarkGray
	vColor( 127, 127, 127),	// vC_MedGray
	vColor( 255, 255, 255)	// vC_White
      };

    char* vGlobalColorNames[16] =
	{
	"Black", "Red", "Dim Red", "Green", "Dim Green", "Blue",
	"Dim Blue", "Yellow", "Dim Yellow", "Magenta", "Dim Magenta",
	"Cyan", "Dim Cyan", "Dark Gray", "Med Gray", "White"
	};


// Following added in 1.21 for DLL
//=======================>>> vGetStdColors <<<===========================
vColor* vGetStdColors()
{
	return &vGlobalStdColors[0];
}

//=======================>>> vGetColorNames <<<===========================
char**  vGetColorNames()
{
	return &vGlobalColorNames[0];
}

//=======================>>> vColor::vColor <<<===========================
  vColor::vColor(unsigned int rd, unsigned int gr, unsigned int bl)
  {
    _r = (unsigned char) rd;
    _g = (unsigned char) gr;
    _b = (unsigned char) bl;
    _pixel = RGB(_r,_g,_b);
  }

//=======================>>> vColor::BitsOfColor <<<===========================
  int vColor::BitsOfColor() VCONST
  {

    HDC hdc = ::GetDC(theApp->winHwnd());
    int bits = ::GetDeviceCaps(hdc,BITSPIXEL);
    ::ReleaseDC(theApp->winHwnd(),hdc);
    return bits;
  }

//=======================>>> vColor::ResetColor <<<===========================
  void vColor::ResetColor(unsigned int rd, unsigned int gr, unsigned int bl)
  {
    _r = (unsigned char) rd;
    _g = (unsigned char) gr;
    _b = (unsigned char) bl;
    _pixel = RGB(_r,_g,_b);
  }

//=======================>>> vColor::ResetColor <<<===========================
  void vColor::ResetColor(VCONST vColor& c)
  {
    _r = c.r();
    _g = c.g();
    _b = c.b();
    _pixel = RGB(_r,_g,_b);
  }

//=======================>>> vColor::Set <<<===========================
  void vColor::Set(unsigned int rd, unsigned int gr, unsigned int bl)
  {
    _r = (unsigned char) rd;
    _g = (unsigned char) gr;
    _b = (unsigned char) bl;
    _pixel = RGB(_r,_g,_b);
  }

//=======================>>> vColor::Set <<<===========================
  void vColor::Set(VCONST vColor& c)
  {
    _r = c.r();
    _g = c.g();
    _b = c.b();
    _pixel = RGB(_r,_g,_b);
  }

//=======================>>> vColor::SetR <<<===========================
  void vColor::SetR(unsigned int rd)
  {
    _r = (unsigned char) rd;
    _pixel = RGB(_r,_g,_b);
  }

//=======================>>> vColor::SetG <<<===========================
  void vColor::SetG(unsigned int gr)
  {
    _g = (unsigned char) gr;
    _pixel = RGB(_r,_g,_b);
  }

//=======================>>> vColor::SetB <<<===========================
  void vColor::SetB(unsigned int bl)
  {
    _b = (unsigned char) bl;
    _pixel = RGB(_r,_g,_b);
  }
