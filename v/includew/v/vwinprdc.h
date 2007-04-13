//===============================================================
// vwinprdc.h: WinPrinterDC class .h file - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VWINPRDC_H
#define VWINPRDC_H

#include <v/vwindc.h>
#include <v/vprinter.h>

    class V_EXPORT vWinPrinterDC : public vWinDC
      {

      public:		//---------------------------------------- public

	vWinPrinterDC(const int scalePrinting = 0);

	virtual ~vWinPrinterDC();

	// Drawing

	virtual void SetBackground(VCONST vColor& color);
	virtual void SetFont(VCONST vFont& vf);

        virtual int BeginPrinting();
	virtual void EndPrinting();
	virtual void BeginPage();
	virtual void EndPage();
	virtual void SetPrinter(vPrinter& printer);


      protected:	//--------------------------------------- protected

	// These methods are to be used only internally and are assumed to
	// be used correctly - there is no error checking. The idea is to have
	// the canvas use BeginPaint/EndPaint for Redraw, and the drawing
	// rotuines to use GetHDC and ReleaseHDC.

	virtual void GetHDC() VCONST {  _hdc = _printer.GetHDC(); }
	virtual void BeginPaint() {}
	virtual void EndPaint() {}
	virtual void ReleaseHDC() VCONST  { }
        int _scalePrinting;

      private:		//--------------------------------------- private
        int _pages;
	vPrinter _printer;	// the printer
      };
#endif
