//===============================================================
// vpsprntr.h: PS Printer class .h file - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VPSPRNTR_H
#define VPSPRNTR_H

#include <v/v_defs.h>
#include <v/vmodald.h>
#include <fstream.h>

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

    class vPSPrinter : public vModalDialog
      {
      public:		//---------------------------------------- public
	vPSPrinter();
	~vPSPrinter();

	vPSPrinter& operator =(const vPSPrinter& pr);

	ofstream* OpenPrinter(); 
	void ClosePrinter();

	int GetHeight() {return _height;}
	int GetWidth() {return _width;}

	char* GetDocName() {return _name;}

	int GetPortrait(){return _portrait;}
	void SetPortrait(int p) {_portrait = p;}

	int GetUseColors() {return _useColor;}
	void SetUseColors(int c) {_useColor = c;}

	int GetPaper() {return _paperType;}
	char* GetPaperName();

	int GetCopies() {return _copies;}
	void SetCopies(int s) {_copies = s;}

	int GetToFile() {return _toFile;}

	int Setup(char* fn = 0);

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private
	virtual void DialogCommand(ItemVal id, ItemVal val, CmdType ctype);

	static int _instances;

	int _cmdsAdded;

	// Printer attributes
	char* _name;		// name of stream
	ofstream* _pstream;   // the stream

	int _width;		// width of printer
	int _height;		// height of printer
	int _portrait;	// true if portrait, else landscape
	int _useColor;	// true if printer supports colors

	int _paperType;
	int _copies;
	int _toFile;
      };

#endif
