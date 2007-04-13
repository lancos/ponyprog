//===============================================================
// vprinter.h: Printer class .h file - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VPRINTER_H
#define VPRINTER_H

#include <v/vpsprntr.h>

    // For X version, this is a simple renaming of vPSPrinter

    class vPrinter : public vPSPrinter
      {
      public:		//---------------------------------------- public
	vPrinter() {}
	virtual ~vPrinter() {}

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

      };

#endif
