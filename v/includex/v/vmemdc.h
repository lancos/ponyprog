//===============================================================
// vcpdc.h: MemoryDC class .h file - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VMEMDC_H
#define VMEMDC_H

#include <v/vxdc.h>

    class vMemory;

    class vMemoryDC : public vXDC
      {

      public:		//---------------------------------------- public

	vMemoryDC(int width = 256, int height = 256);

 	virtual ~vMemoryDC();

	virtual void Clear(void);
	virtual void SetBackground(VCONST vColor& color);

	int CreatedOK() VCONST {return _pixmap != 0;}

	virtual Drawable GetDrawable() VCONST;

      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private

	Pixmap	_pixmap;

	GC makeGC();		// make a GC to use
	GC makeXorGC();		// make a GC to use

      };
#endif
