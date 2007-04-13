//===============================================================
// vcolor.cxx: vColor class for drawing - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vapp.h>
#include <v/vcolor.h>

    // Define 16 standard colors for use by everyone

    vColor vStdColors[16] =
      {
	vColor( 0, 0, 0),       // vC_Black
        vColor( 255, 0, 0),     // vC_Red
        vColor( 127, 0, 0),     // vC_DimRed
        vColor( 0, 255, 0),     // vC_Green
        vColor( 0, 127, 0),     // vC_DimGreen
        vColor( 0, 0, 255),     // vC_Blue
        vColor( 0, 0, 127),     // vC_DimBlue
        vColor( 255, 255, 0),   // vC_Yellow
        vColor( 127, 127, 0),   // vC_DimYellow
        vColor( 255, 0, 255),   // vC_Magenta
        vColor( 127, 0, 127),   // vC_DimMagenta
        vColor( 0, 255, 255),   // vC_Cyan
        vColor( 0, 127, 127),   // vC_DimCyan
        vColor( 127, 127, 127), // vC_DarkGray
        vColor( 192, 192, 192), // vC_MedGray
        vColor( 255, 255, 255)  // vC_White
      };

    char* vColorNames[16] =
	{
	"Black", "Red", "Dim Red", "Green", "Dim Green", "Blue",
	"Dim Blue", "Yellow", "Dim Yellow", "Magenta", "Dim Magenta",
	"Cyan", "Dim Cyan", "Dark Gray", "Med Gray", "White"
	};

    static Pixel _firstPix = 0;

//=======================>>> vColor::vColor <<<===========================
  vColor::vColor(unsigned int rd, unsigned int gr, unsigned int bl)
  {
    _r = (unsigned char) rd;
    _g = (unsigned char) gr;
    _b = (unsigned char) bl;
    _pixel = 0; _mapped = 0;
  }

//=======================>>> vColor::vColor <<<===========================
  vColor::~vColor()
  {
  }

//=======================>>> vColor::vColor <<<===========================
  vColor::vColor(const vColor& color)
  {
    // Copy constructor
    _r = color._r;
    _g = color._g;
    _b = color._b;
    if (color._mapped)
	_pixel = color._pixel;
    else
	MapColor();
    _mapped = 1;
 
  }

//=======================>>> vColor::= <<<===========================
  vColor& vColor::operator =(const vColor& color)
  {
    if (this == &color)	// to self
      return *this;

    _r = color._r;
    _g = color._g;
    _b = color._b;
    if (color._mapped)
	_pixel = color._pixel;
    else
	MapColor();
    _mapped = 1;
    return *this;
  }

//=======================>>> vColor::BitsOfColor <<<===========================
  int vColor::BitsOfColor() VCONST
  {
    return theApp->Xdepth();	// How many bits of color supported
  }

//=======================>>> vColor::ResetColor <<<===========================
  void vColor::ResetColor(unsigned int rd, unsigned int gr, unsigned int bl)
  {
    // This method will try to free cells for more use!
    // Note: any other color objects that reference this same color
    // (because of =, copy, or coincidence) will have their color
    // changed, too.

    if (_r == rd && _g == gr && _b == bl)
	return;

    _r = (unsigned char) rd;
    _g = (unsigned char) gr;
    _b = (unsigned char) bl;

    if (_mapped && _pixel >= _firstPix)	 // Already mapped, so free
      {
	Pixel pix[1];
	pix[0] = _pixel;
	XFreeColors(theApp->display(), theApp->Xcolormap(), 
		pix, 1, 0);
      }
    MapColor();
  }

//=======================>>> vColor::ResetColor <<<===========================
  void vColor::ResetColor(VCONST vColor& c)
  {
    // This method will try to free cells for more use!
    // Note: any other color objects that reference this same color
    // (because of =, copy, or coincidence) will have their color
    // changed, too.

    if (_r == c.r() && _g == c.g() && _b == c.b())
	return;

    _r = c.r();
    _g = c.g();
    _b = c.b();

    if (_mapped && _pixel >= _firstPix)	 // Already mapped, so free
      {
	Pixel pix[1];
	pix[0] = _pixel;
	XFreeColors(theApp->display(), theApp->Xcolormap(), 
		pix, 1, 0);
      }
    MapColor();
  }

//=======================>>> vColor::Set <<<===========================
  void vColor::Set(unsigned int rd, unsigned int gr, unsigned int bl)
  {
    if (_r == rd && _g == gr && _b == bl)
	return;
    _r = (unsigned char) rd;
    _g = (unsigned char) gr;
    _b = (unsigned char) bl;
    MapColor();
  }

//=======================>>> vColor::Set <<<===========================
  void vColor::Set(VCONST vColor& c)
  {
    if (_r == c.r() && _g == c.g() && _b == c.b())
	return;
    _r = c.r();
    _g = c.g();
    _b = c.b();
    MapColor();
  }

//=======================>>> vColor::SetR <<<===========================
  void vColor::SetR(unsigned int rd)
  {
    if (_r == rd)
	return;
    _r = (unsigned char) rd;
    MapColor();
  }

//=======================>>> vColor::SetG <<<===========================
  void vColor::SetG(unsigned int gr)
  {
    if (_g == gr)
	return;
    _g = (unsigned char) gr;
    MapColor();
  }

//=======================>>> vColor::SetB <<<===========================
  void vColor::SetB(unsigned int bl)
  {
    if (_b == bl)
	return;
    _b = (unsigned char) bl;
    MapColor();
  }

//=======================>>> vColor::MapColor <<<===========================
  void vColor::MapColor() VCONST
  {
    // This will normally be used only internally. There is some
    // chance a user overriding a drawing canvas will need to
    // know about this method. The purpose of MapColor (and IsMapped)
    // is to map the V RGB value to whatever internal value the host
    // system requires. In the case of X, this is an X Pixel, which
    // is really a long index into the X display's colormap. Rather
    // than mapping the RGB to a Pixel every time, this will happen
    // only when the color is use to change a brush or font or something.
    // Then the Pixel value can be used directly for X. This mapping
    // should only have to be done whenever a change happens, and
    // the methods that allow changes are the ones to use MapColor.
    // It is harmeless (other than inefficient) to MapColor when it
    // is not needed. IsMapped returns true if the color has already
    // been mapped. This need for mapping is really why vColor is
    // real class rather than a simple struct. The act of setting
    // a new color also causes the color to be mapped.

    // Now, map the RGB to a Pixmap index

    _pixel = vXAllocColor(_r,_g,_b);
    _mapped = 1;
  }

//=======================>>> vXAllocColor <<<===========================
  Pixel vXAllocColor(unsigned int r, unsigned int g, unsigned int b)
  {
    // Global function to alloacate a R/W color cell if possible.
    // Use by vColor and by vIcon.

    XColor xc;
    int gotPix = 0;	// No Pixel mapped yet

    // Convert from 0-255 scale of V to 0-65535 scale of X
    xc.red = r * 256;
    xc.green = g * 256;
    xc.blue = b * 256;
    xc.flags = DoRed | DoBlue | DoGreen;
    xc.pixel = 0;

    if (theApp->Xdepth() > 1)		// A color display
      {
	gotPix = XAllocColor(theApp->display(), theApp->Xcolormap(), &xc);
      }

    if (gotPix)			// Got one, return it!
      {
	if (_firstPix == 0 && !(r == 0 && g == 0 && b == 0) && 
		!(r == 255 && g == 255 && b == 255) )
	  {
	    // Need to track first pixel so don't free
	    // colors allocated by V controls, etc.
	    _firstPix = xc.pixel;
	  }
	return xc.pixel;	// recover pixel value
      }
    else			// Couldn't get an R/W Pixel
      {
	// Can't get a color pixel, so as a backup position (or for b/w)
	// convert to gray scale (based on NTSC grayscale (using
	// a scale of 0 to 1.0: (ingore some rounding)
	//   gray = .3*red + .59*green + .11*blue

	unsigned int gray = ((xc.red/10)*3) + ((xc.green/100)*59)
	       + ((xc.blue/100)*11);

	// I arbitrarily chose the cutoff value...
	if (gray <= 29000)		// use either black or white
	    return XBlackPixel(theApp->display(),0);
	else
	    return XWhitePixel(theApp->display(),0);
      } 
  }
