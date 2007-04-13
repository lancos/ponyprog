//===============================================================
// vbaseGLCanvas - a basic GL window canvas for drawing
//
// Copyright (C) 1995-1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>
#include <v/vbglcnv.h>		// our header

#include <v/vapp.h>
#include <v/vwindow.h>

#include <stdlib.h>

// OpenGL init
    int vBaseGLCanvasPane::_pixelFormat = 0;
    int vBaseGLCanvasPane::_doubleBuffer = 0;	// if we have double buffering
    unsigned int vBaseGLCanvasPane::_vGLmode = 0;

//================>>> vBaseGLCanvasPane::vBaseGLCanvasPane <<<========================
  vBaseGLCanvasPane::vBaseGLCanvasPane(unsigned int vGLmode,
   PaneType pt) : vCanvasPane(pt)
  {
    SysDebug(Constructor,"vBaseGLCanvasPane::vBaseGLCanvasPane() constructor\n")

    const unsigned int defGLmode = (vGL_RGB | vGL_DoubleBuffer | vGL_Depth);

    _vGLmode = ((vGLmode == vGL_Default) ? defGLmode : vGLmode);
    _hDC = 0;
    _hGLRC = 0;
    _hPalette = 0;

  }

//================>>> vBaseGLCanvasPane::~vBaseGLCanvasPane <<<===================
  vBaseGLCanvasPane::~vBaseGLCanvasPane()
  {
    SysDebug(Destructor,"vBaseGLCanvasPane::~vBaseGLCanvasPane() destructor\n")
    // OpenGL: finish OpenGL rendering

    if (_hDC)
	::ReleaseDC(theApp->winHwnd(),_hDC);
    if (_hGLRC)
      {
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(_hGLRC);
      }
    if (_hPalette)
      {
	DeleteObject(_hPalette);
      }
  }

//==================>>> vBaseGLCanvasPane::initialize <<<==========================
  void vBaseGLCanvasPane::initialize(vWindow* pWindow, HWND pWidget)
  {
    vCanvasPane::initialize(pWindow, pWidget);	// init std canvas
	// the initialize will call CreateDC, which we override to
	// create the DC we need for OpenGL

    if (!_hDC)
      {
        // Create the GL DC for it to use.
        // Initialize OpenGL rendering

        _hDC = ::GetDC(_drawWindow);

	// Set up the rendering pixel format
	_setUpPixelFormat();
	// Set up Palette
	_setUpPalette();
	_hGLRC = wglCreateContext(_hDC);
      }

    // initialize graphic
    graphicsInit();
  }

#ifdef USECREATEDC
//==================>>> vBaseGLCanvasPane::CreateDC <<<========================
  void vBaseGLCanvasPane::CreateDC(void)
  {
    // This overrides the CreateDC of vCanvasPane and creates
    // an OpenGL compatible DC, and then sets up the rendering stuff

    vCanvasPane::CreateDC();		// we can have both regular DC
    					// and a OpenGL DC.
  }
#endif

//==================>>> vBaseGLCanvasPane::_setUpPixelFormat <<<==========================
  void vBaseGLCanvasPane::_setUpPixelFormat()
  {
    // default pfd values
    PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),  /* size */
      1,                              /* version */
      PFD_SUPPORT_OPENGL |
      PFD_DRAW_TO_WINDOW |
      PFD_DOUBLEBUFFER,               /* support double-buffering */
      PFD_TYPE_RGBA,                  /* color type */
      16,                             /* prefered color depth */
      0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
      0,                              /* no alpha buffer */
      0,                              /* alpha bits (ignored) */
      0,                              /* no accumulation buffer */
      0, 0, 0, 0,                     /* accum bits (ignored) */
      24,                             /* depth buffer */
      0,                              /* no stencil buffer */
      0,                              /* no auxiliary buffers */
      PFD_MAIN_PLANE,                 /* main layer */
      0,                              /* reserved */
      0, 0, 0,                        /* no layer, visible, damage masks */
    };

    _doubleBuffer = 1;

    // prepare the mode  
    if (_vGLmode & vGL_Indexed)	// Indexed mode *********************
      {
	pfd.iPixelType = PFD_TYPE_COLORINDEX;

	_pixelFormat = ChoosePixelFormat(_hDC, &pfd);

	// try without double buffer
	if (_pixelFormat == 0 && (_vGLmode & vGL_DoubleBuffer))
	  {
	    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	    _pixelFormat = ChoosePixelFormat(_hDC, &pfd);
	    _doubleBuffer = 0;
	  }
      }
    else		// RGBA mode ********************
      {
	_pixelFormat = ChoosePixelFormat(_hDC, &pfd);
	UserDebug(WindowEvents,"vBaseGLCanvasPane::_setUpPixelFormat: OpenGL starts in RGB, Double buffering and Depth mode");

	// try to get rid of this (i.e. try without double buffer)
	// As Alpha mode is not independant on Win32 platform
	// if vGL_Alpha is set, it is considered as vGL_RGB
	if ( (_pixelFormat == 0) && (_vGLmode & vGL_RGB))
	  {
	    UserDebug(WindowEvents,"without Double buffering");

	    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	    _pixelFormat = ChoosePixelFormat(_hDC, &pfd);
	    _doubleBuffer = 0;
	  }
	else
	    UserDebug(WindowEvents,"with Double buffering");
      }
  
    if (_pixelFormat == 0)
      {
	::MessageBox(WindowFromDC(_hDC), "ChoosePixelFormat failed.", "Error",
	MB_ICONERROR | MB_OK);
	exit(1);
      }

    if (SetPixelFormat(_hDC, _pixelFormat, &pfd) != TRUE)
      {
	::MessageBox(WindowFromDC(_hDC), "SetPixelFormat failed.", "Error",
	       MB_ICONERROR | MB_OK);
	exit(1);
      }
  
    UserDebug(WindowEvents,"\n");
  }

//==================>>> vBaseGLCanvasPane::_setUpPalette <<<==========================
  void vBaseGLCanvasPane::_setUpPalette()
  {
    //  int pixelFormat = GetPixelFormat(_hDC);
    PIXELFORMATDESCRIPTOR pfd;
    LOGPALETTE* pPal;
    int paletteSize;

    DescribePixelFormat(_hDC, _pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    if (pfd.dwFlags & PFD_NEED_PALETTE) 
      {
	paletteSize = 1 << pfd.cColorBits;
      }
    else
      {
	return;
      }

    pPal = (LOGPALETTE*)
    	malloc(sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY));
    pPal->palVersion = 0x300;
    pPal->palNumEntries = paletteSize;

    /* build a simple RGB color palette */
      {
	int redMask = (1 << pfd.cRedBits) - 1;
	int greenMask = (1 << pfd.cGreenBits) - 1;
	int blueMask = (1 << pfd.cBlueBits) - 1;
	int i;

	for (i=0; i<paletteSize; ++i)
	  {
	    pPal->palPalEntry[i].peRed =
	     (((i >> pfd.cRedShift) & redMask) * 255) / redMask;
	    pPal->palPalEntry[i].peGreen =
	      (((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
	    pPal->palPalEntry[i].peBlue =
	      (((i >> pfd.cBlueShift) & blueMask) * 255) / blueMask;
	    pPal->palPalEntry[i].peFlags = 0;
	  }
      }
  
    _hPalette = CreatePalette(pPal);
    free(pPal);

    if (_hPalette) 
      {
	SelectPalette(_hDC, _hPalette, FALSE);
	RealizePalette(_hDC);
      }
  }

//==================>>> vBaseGLCanvasPane::graphicsInit <<<========================
  void vBaseGLCanvasPane::graphicsInit(void)
  {
    // Perform only the most basic operations, including getting the context

    vglMakeCurrent();
  }

//================>>> vBaseGLCanvasPane::vBaseGLMakeCurrent <<<===============
  void vBaseGLCanvasPane::vglMakeCurrent(void)
  {
    wglMakeCurrent(_hDC, _hGLRC);
  }

//==================>>> vBaseGLCanvasPane::vglFlush <<<========================
  void vBaseGLCanvasPane::vglFlush(void)
  {
    if (_doubleBuffer)
	SwapBuffers(_hDC);
    else
	glFlush();
  }

//=====================>>> vBaseGLCanvasPane::Resize <<<============================
  void vBaseGLCanvasPane::Resize(int newW, int newH)
  {
    // This is the routine the user will override to intercept size changes
    vCanvasPane::Resize(newW,newH);

    vglMakeCurrent();
    glViewport(0,0,newW,newH);

  }
