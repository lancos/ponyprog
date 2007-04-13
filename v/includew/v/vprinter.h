//===============================================================
// vprinter.h: Printer class .h file
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

#include <v/vwinprtr.h>

    // For Windows version, this is a simple renaming of vWinPrinter

    class V_EXPORT vPrinter : public vWinPrinter
      {
      public:		//---------------------------------------- public
	vPrinter() {}
	virtual ~vPrinter() {}

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

      };

#endif
