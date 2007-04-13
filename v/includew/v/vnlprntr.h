//===============================================================
// vnlprntr.h: null Printer class .h file - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VNLPRNTR_H
#define VNLPRNTR_H

#define vPaperLetter 0
#define vPaperLegal 1
#define vPaperTabloid 2
#define vPaperLedger 3
#define vPaperStatement 4
#define vPaperExecutive 5
#define vPaperA3 6
#define vPaperA4 7
#define vPaperA5 8
#define vPaperB4 9
#define vPaperB5 10
#define vPaperFolio 11
#define vPaperQuarto 12
#define vPaper10x14 13

#define vPaperDefault vPaperLetter		// Index to default paper
#include <iosfwd>

    class V_EXPORT vNullPrinter
      {
//	class ofstream;
      public:		//---------------------------------------- public
	vNullPrinter(){}
	~vNullPrinter(){}

	ofstream* OpenPrinter() VCONST {return 0;}
	void ClosePrinter() {}

	int GetHeight() VCONST {return _height;}
	int GetWidth() VCONST {return _width;}

	VCONST char* GetDocName() VCONST {return 0;}

	int GetPortrait() VCONST {return _portrait;}
	void SetPortrait(int p) {_portrait = p;}

	int GetUseColors() VCONST {return _useColor;}
	void SetUseColors(int c) {_useColor = c;}

	int GetPaper()  VCONST {return _paperType;}
	VCONST char* GetPaperName() VCONST {return 0;}

	int GetCopies() VCONST {return _copies;}
	void SetCopies(int s) {_copies = s;}

	int GetToFile() VCONST {return _toFile;}

	int Setup( VCONST char* fn = 0) VCONST {return 0;}

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

	int _width;		// width of printer
	int _height;		// height of printer
	int _portrait;	// true if portrait, else landscape
	int _useColor;	// true if printer supports colors

	int _paperType;
	int _copies;
	int _toFile;
      };

#endif
