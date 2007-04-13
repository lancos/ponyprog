//===============================================================
// vWinPrinterC - a basic canvas for drawing
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


#include <v/vwinprdc.h>
#include <v/vapp.h>		// need access to the app

//-----------------------------------------------------------------------

//================>>> vWinPrinterDC::vWinPrinterDC <<<========================
  vWinPrinterDC::vWinPrinterDC(const int scalePrinting) : vWinDC()
  {
    SysDebug(Constructor,"vWinPrinterDC::vWinPrinterDC() constructor\n")

    // _isPrinterDC is < 0 if we are scaling. For scaled printing, fonts
    // get treated differently. If we aren't scaling, then we use the old
    // default mode, which lets the GDI handle scaling. This is kind of
    // messy, but it seemed the easiest way to patch things given that
    // _isPrinterDC is really used by vWinDC.

    _isPrinterDC = scalePrinting ? -1 : 1;
    _scalePrinting = scalePrinting;
  }

//================>>> vWinPrinterDC::~vWinPrinterDC <<<========================
  vWinPrinterDC::~vWinPrinterDC()
  {

    SysDebug(Destructor,"vWinPrinterDC::~vWinPrinterDC() destructor\n")

  }

//================>>> vWinPrinterDC::SetBackground <<<==========================
  void vWinPrinterDC::SetBackground(VCONST vColor& color)
  {

   _canvasBG = color.pixel();		// retrieve X pixel value

    GetHDC();
    ::SetBkColor(_hdc, _canvasBG);
    ReleaseHDC();
  }

//======================>>> vWinPrinterDC::SetFont <<<===========================
  void vWinPrinterDC::SetFont(VCONST vFont& vf)
  {
    // Change the font associated with this window.

    _font = vf;
  }

//================>>> vWinPrinterDC::BeginPrinting <<<========================
  int vWinPrinterDC::BeginPrinting()
  {

    DOCINFO docinfo;

    if (!_printer.GetHDC())		// should not happen...
      {
	return 0;
      }

    _pages = 0;

    docinfo.cbSize = sizeof(DOCINFO);	// set this size...
    docinfo.lpszDocName = "VPrintJob";
    docinfo.lpszOutput = 0;		// no file output...
#ifndef VWIN16
    docinfo.lpszDatatype = 0;
    docinfo.fwType = 0;
#endif

    if (::StartDoc(_printer.GetHDC(), &docinfo) <= 0)
      {
        DWORD retcode = ::GetLastError();
	return (retcode == 0);
      }


    BeginPage();

    return 1;

  }

//================>>> vWinPrinterDC::BeginPage <<<========================
  void vWinPrinterDC::BeginPage()
  {
    HDC pHDC = _printer.GetHDC();
    if (!pHDC)
	return;
    ++_pages;		// bump number of pages so far

    ::StartPage(pHDC);

    // We will support two modes of printing - default and scaled.
    // In scaled, it is up to the user to find the page size and scale
    // things accordingly to make them fit. The fonts will be loaded differently
    // depending on the mode.

    // The default mode produces output that closely matches the postscript
    // printer output, but also scales line thickness, which can be bad!
    // Scaled printing does not scale line thickness.

    if (!_scalePrinting)
      {
        int pixw = ::GetDeviceCaps(pHDC, HORZRES);
        int pixh = ::GetDeviceCaps(pHDC, VERTRES);
        // This scaling gives about the same proportions as the X version
        int width = ::GetDeviceCaps(pHDC, HORZSIZE) * 3;
        int height = ::GetDeviceCaps(pHDC,VERTSIZE) * 3;

        ::SetMapMode(pHDC, MM_ISOTROPIC);
        ::SetWindowExt(pHDC,width,height);
        ::SetViewportExt(pHDC,pixw,pixh);
      }
    else
      {
        ::SetMapMode(pHDC, MM_TEXT);
      }
  }

//================>>> vWinPrinterDC::EndPage <<<========================
  void vWinPrinterDC::EndPage()
  {
    if (!_printer.GetHDC())
	return;
    ::EndPage(_printer.GetHDC());
  }

//================>>> vWinPrinterDC::EndPrinting <<<========================
  void vWinPrinterDC::EndPrinting()
  {
    if (!_printer.GetHDC())
	return;

    EndPage();

    ::EndDoc(_printer.GetHDC());

  }

//=====================>>> vWinPrinterDC::SetPrinter <<<============================
 void vWinPrinterDC::SetPrinter(vPrinter& printer)
  {
    _printer = printer;
    HDC pHDC = _printer.GetHDC();
    if (!pHDC)
	return;
    if (_scalePrinting)
      {
      	_physHeight = ::GetDeviceCaps(pHDC, VERTRES);
	_physWidth = ::GetDeviceCaps(pHDC, HORZRES);
      }
    else
      {
        _physHeight = _printer.GetHeight();
        _physWidth = _printer.GetWidth();
      }

  }


