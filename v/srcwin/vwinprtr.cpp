//===============================================================
// vwinprtr.cpp - Windows Printer class
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
#include <v/vwinprtr.h>
#include <commdlg.h>


   int vWinPrinter::_instances = 0;	// reference counter for copy / assignment

//================>>> vWinPrinter::vWinPrinter <<<========================
  vWinPrinter::vWinPrinter()
  {
    _name = 0;
    _paperType = vPaperDefault;
    _copies = 1;
    _toFile = 1;
    _width = 1;
    _height = 1;
    _portrait = 1;
    _useColor = 0;
    _printhDC = 0;

    ++_instances;		// bump reference counter
  }

//================>>> vWinPrinter::vWinPrinter <<<========================
  vWinPrinter::vWinPrinter(const vWinPrinter& pr)
  {
    // copy constructor
    _name = pr._name;
    _width = pr._width;
    _height = pr._height;
    _portrait = pr._portrait;
    _useColor = pr._useColor;
    _paperType = pr._paperType;
    _copies = pr._copies;
    _toFile = pr._toFile;
    _printhDC = pr._printhDC;

    ++_instances;		// bump reference counter
  }

//================>>> vWinPrinter::= <<<========================
  vWinPrinter& vWinPrinter::operator =(const vWinPrinter& pr)
  {
    if (this == &pr)	// to self?
	return *this;

    if (_name)
	delete [] _name;	// free space before overwrite

    _name = pr._name;
    _width = pr._width;
    _height = pr._height;
    _portrait = pr._portrait;
    _useColor = pr._useColor;
    _paperType = pr._paperType;
    _copies = pr._copies;
    _toFile = pr._toFile;
    _printhDC = pr._printhDC;

    return *this;
  }

//================>>> vWinPrinter::~vWinPrinter <<<========================
  vWinPrinter::~vWinPrinter()
  {
    // We are using a reference count to track instances of printers.
    // We want to allow the user to pass copies of the single printer
    // around, but there should only be one name and one stream;

    --_instances;		// not reference any more

    if (_instances > 0)
	return;			// don't do anything else

    if (_name)
	delete [] _name;

  }

//================>>> vWinPrinter::GetPaperName <<<========================
  char* vWinPrinter::GetPaperName()
  {
    if (_paperType == vPaperLetter)	// we have (8.5x11) added...
	return "Letter";
    else
	return "Unknown";
  }

//================>>> vWinPrinter::Setup <<<========================
  int vWinPrinter::Setup(VCONST char* fn)
  {
    // fn is an override default name

    PRINTDLG pd;

    if (_printhDC != 0)
	return 0;
	
    pd.lStructSize = sizeof( PRINTDLG );
    pd.hwndOwner=NULL;
    pd.hDevMode=(HANDLE)NULL;
    pd.hDevNames=(HANDLE)NULL;
    pd.Flags = PD_RETURNDC | PD_NOSELECTION | PD_NOPAGENUMS |
	PD_HIDEPRINTTOFILE;
    pd.nFromPage=0;
    pd.nToPage=0;
    pd.nMinPage=0;
    pd.nMaxPage=0;
    pd.nCopies=1;
    pd.hInstance=(HINSTANCE)NULL;
    pd.lCustData = (LPARAM)0;
    pd.lpfnPrintHook = 0;
    pd.lpfnSetupHook = 0;
    pd.lpPrintTemplateName = (LPCSTR) 0;
    pd.lpSetupTemplateName = (LPCSTR) 0;
    pd.hPrintTemplate = (HGLOBAL)0;
    pd.hSetupTemplate = (HGLOBAL)0;
    if ( ::PrintDlg( &pd ) != 0 )
      {
        int pixw, pixh;
	_printhDC = pd.hDC;
	pixw = ::GetDeviceCaps(_printhDC, HORZRES);
	pixh = ::GetDeviceCaps(_printhDC, VERTRES);
        // This scaling gives about the same proportions as the X version
	_width = ::GetDeviceCaps(_printhDC, HORZSIZE) * 3;
	_height = ::GetDeviceCaps(_printhDC,VERTSIZE) * 3;

//	::SetMapMode(_printhDC, MM_ISOTROPIC);
//        ::SetWindowExt(_printhDC,_width,_height);
//      	::SetViewportExt(_printhDC,pixw,pixh);
      }


    return _printhDC != 0;
  }
