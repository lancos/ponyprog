//===============================================================
// vcpdc.h: CanvasPaneDC class .h file - Windows
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

#include <v/vwindc.h>


    class V_EXPORT vWindow;		// we are part of a window
    class V_EXPORT vCanvasPane;
    class V_EXPORT vIcon;

    class V_EXPORT vCanvasPaneDC : public vWinDC
      {

      public:		//---------------------------------------- public

	vCanvasPaneDC(vCanvasPane* parentPane);

 	virtual ~vCanvasPaneDC();

	// Drawing

	virtual void Clear(void);
	virtual void ClearRect(int left, int top, int width, int height);
	virtual void SetBackground(VCONST vColor& color);
	virtual void SetFont(VCONST vFont& vf);

      protected:	//--------------------------------------- protected

	// These methods are to be used only internally and are assumed to
	// be used correctly - there is no error checking. The idea is to have
	// the canvas use BeginPaint/EndPaint for Redraw, and the drawing
	// rotuines to use GetHDC and ReleaseHDC.

	virtual void GetHDC() VCONST { if (_hdcPaint != 0) _hdc = _hdcPaint;
	     else {::HideCaret(_drawWidget); _hdc = ::GetDC(_drawWidget);} }
	virtual void BeginPaint() {_hdcPaint = ::BeginPaint(_drawWidget, &_ps);}
	virtual void EndPaint() { ::EndPaint(_drawWidget, &_ps); _hdcPaint = 0;}
	virtual void ReleaseHDC() VCONST { if (_hdc != _hdcPaint)
	  {::ShowCaret(_drawWidget); ::ReleaseDC(_drawWidget, _hdc);}}

      private:		//--------------------------------------- private

	vCanvasPane* _parentPane;
	HWND _drawWidget;
      };
#endif
