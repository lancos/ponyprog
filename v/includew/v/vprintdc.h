//===============================================================
// vpsdc.h: Print DC class .h file - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VPRINTDC_H
#define VPRINTDC_H

#include <v/vwinprdc.h>

    // This is a simple renamimg of the NullDC

    class /* V_EXPORT */ vPrintDC : public vWinPrinterDC
      {

      public:		//---------------------------------------- public

	vPrintDC(int scale = 0) : vWinPrinterDC(scale) {}

 	virtual ~vPrintDC() {}
      };
#endif
