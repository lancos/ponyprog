//===============================================================
// vglcnv.h GL Canvas C++ class .h file - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VGLCNV_H
#define VGLCNV_H

#include <v/vcanvas.h>

#ifndef WIN32
#define WIN32
#endif

extern "C"
{
// OpenGL stuff here
#include <GL/gl.h>
#include <GL/glu.h>
}

const unsigned int vGL_Default = 0;    // Note for Win32 :         
const unsigned int vGL_RGB = 0x0001;   // RGBA is used (with alpha)
const unsigned int vGL_Alpha = 0x0002; // No independant alphamode on Win32
                                           // if Alpha is set, RGBA is used
const unsigned int vGL_Indexed = 0x0004;
const unsigned int vGL_DoubleBuffer = 0x0008;
const unsigned int vGL_Stereo = 0x0010;
const unsigned int vGL_Stencil = 0x0020;
const unsigned int vGL_Accum = 0x0040;
const unsigned int vGL_Depth = 0x0080;


    class vWindow;		// we are part of a window

    class vBaseGLCanvasPane : public vCanvasPane
      {

      public:		//---------------------------------------- public

	vBaseGLCanvasPane(unsigned int vGLmode = vGL_Default, PaneType pt = P_Canvas);

 	virtual ~vBaseGLCanvasPane();


	// Expose/redraw event
	virtual void Resize(int newW, int newH);  // Resize event

	// OpenGL specific stuff

	virtual void graphicsInit(void);

	void vglMakeCurrent(void);

 	virtual void vglFlush();


	// Platform dependent


      protected:	//--------------------------------------- protected

	virtual void initialize(vWindow* pWindow, HWND pWidget);
//	virtual void CreateDC(void);

	// Drawing and OpenGL for Win32
	HDC      _hDC;	  // device context
	HGLRC    _hGLRC;        // OpenGL context
	HPALETTE _hPalette;     // Palette


      private:		//--------------------------------------- private

	// OpenGL initialization of the drawings
	void _setUpPixelFormat();
	void _setUpPalette();

	// OpenGL infos
	static int _pixelFormat;
	static int _doubleBuffer;		// if we have double buffering
	static unsigned int _vGLmode;
      };
#endif
