//===============================================================
// vcolor.h: vColor class header for drawing - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VCOLOR_H
#define VCOLOR_H

#include <v/v_defs.h>

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

// For use in Color buttons
const int M_Black = 32200;
const int M_Red = 32201;
const int M_DimRed = 32202;
const int M_Green = 32203;
const int M_DimGreen = 32204;
const int M_Blue = 32205;
const int M_DimBlue = 32206;
const int M_Yellow = 32207;
const int M_DimYellow = 32208;
const int M_Magenta = 32209;
const int M_DimMagenta = 32210;
const int M_Cyan = 32211;
const int M_DimCyan = 32212;
const int M_DarkGray = 32213;
const int M_MedGray = 32214;
const int M_White = 32215;
const int M_ColorFrame = 32216;

// Index into array
const int vC_Black = 0;
const int vC_Red = 1;
const int vC_DimRed = 2;
const int vC_Green = 3;
const int vC_DimGreen = 4;
const int vC_Blue = 5;
const int vC_DimBlue = 6;
const int vC_Yellow = 7;
const int vC_DimYellow = 8;
const int vC_Magenta = 9;
const int vC_DimMagenta = 10;
const int vC_Cyan = 11;
const int vC_DimCyan = 12;
const int vC_DarkGray = 13;
const int vC_MedGray = 14;
const int vC_White = 15;

    class vPen;
    class vBrush;

    class vColor
      {
	friend class vPen;
	friend class vBrush;

      public:		//---------------------------------------- public
	vColor(unsigned int rd = 0, unsigned int gr = 0, unsigned int bl = 0);
	~vColor();

	vColor(const vColor& color);		// copy constructor
	vColor& operator =(const vColor& color); // = operator
#ifdef DONT_INLINE_COLOR_OPS
	int operator ==(const vColor& c2)
	  {return (_r == c2._r && _g == c2._g && _b == c2._b);}
	int operator !=(const vColor& c2)
	  {return (_r != c2._r || _g != c2._g || _b != c2._b);}
#endif

	int BitsOfColor() VCONST;
	void ResetColor(unsigned int rd= 0, unsigned int gr = 0, unsigned int bl = 0);
	void ResetColor(VCONST vColor& c);
	void Set(unsigned int rd = 0, unsigned int gr = 0, unsigned int bl = 0);
	void Set(VCONST vColor& c);
	void SetR(unsigned int rd = 0);
	void SetG(unsigned int gr = 0);
	void SetB(unsigned int bl = 0);

	unsigned int r() const { return (unsigned int) _r;}
	unsigned int g() const { return (unsigned int) _g;}
	unsigned int b() const { return (unsigned int) _b;}
	int IsMapped() VCONST { return _mapped != 0; }
	void MapColor() VCONST;;

	Pixel pixel() VCONST { if (!_mapped) MapColor(); return _pixel; }

      protected:	//--------------------------------------- protected

	unsigned char _r;
	unsigned char _g;
	unsigned char _b;

      private:		//--------------------------------------- private
	VMUTABLE unsigned char _mapped;
	VMUTABLE Pixel _pixel;
      };

#ifndef hpux
    inline int operator==(const vColor &x, const vColor &y)
	{return (x.r() == y.r() && x.g() == y.g() && x.b() == y.b());}
    inline int operator != (const vColor& x, const vColor& y)
	{return !(x == y);}
#endif
    extern Pixel vXAllocColor(unsigned int r, unsigned int g, unsigned int b);
    extern vColor vStdColors[16];
    extern char* vColorNames[16];
#endif
