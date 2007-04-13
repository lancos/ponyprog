//===============================================================
// vcpdc.h: CanvasPaneDC class .h file - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VCPDC_H
#define VCPDC_H

#include <v/vxdc.h>

    class vCanvasPane;

    class vCanvasPaneDC : public vXDC
      {

      public:		//---------------------------------------- public

	vCanvasPaneDC(vCanvasPane* parentPane);

 	virtual ~vCanvasPaneDC();

	virtual void Clear(void);
	virtual void SetBackground(VCONST vColor& color);
	virtual void SetFont(VCONST vFont& vf);

      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private

	virtual Drawable GetDrawable();

 	vCanvasPane* _parentPane;

	Widget _drawWidget;

	GC makeGC(Widget w);		// make a GC to use
	GC makeXorGC(Widget w);		// make a GC to use

      };
#endif
