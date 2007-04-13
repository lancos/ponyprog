//===============================================================
// vbaseGLCanvas - a basic GL window canvas for drawing
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vbglcnv.h>		// our header
// #include "vbglcnv.h"		// our header

extern "C"
{
#ifndef Athena
#include <Xm/DrawingA.h>
#else
#include <X11/Xaw/Paned.h>
#endif
#include <v/canvas.h>
}

#include <v/vwindow.h>		// we are part of a vWindow

    XVisualInfo* vBaseGLCanvasPane::_visinfo = 0;	// one time visual
    int vBaseGLCanvasPane::_doubleBuffer = 0;		// if we have double buffering
    unsigned int vBaseGLCanvasPane::_vGLmode = 0;

//================>>> vBaseGLCanvasPane::vBaseGLCanvasPane <<<========================
  vBaseGLCanvasPane::vBaseGLCanvasPane(unsigned int vGLmode,
   PaneType pt) : vCanvasPane(pt)
  {
    SysDebug(Constructor,"vBaseGLCanvasPane::vBaseGLCanvasPane() constructor\n")

    const unsigned int defGLmode = (vGL_RGB | vGL_DoubleBuffer);

    // One shot initialization of X Visual

    if (_visinfo == 0)	// Haven't gotten a visual yet
      {
	_vGLmode = ((vGLmode == vGL_Default) ? defGLmode : vGLmode);

	if (_vGLmode & vGL_Indexed)	// Indexed mode *********************
	  {
	    _visinfo = chooseIndexed(_vGLmode);
	    if (_visinfo == 0 && (_vGLmode & vGL_DoubleBuffer))
	      {
		_vGLmode &= ~vGL_DoubleBuffer;	// try without double buffer
		_visinfo = chooseIndexed(_vGLmode);
	      }
	  }
	else				// RGB mode ********************
	  {
	    _visinfo = chooseRGB(_vGLmode);
	    if (_visinfo == 0 && (_vGLmode & vGL_Alpha))	// try to get rid of this
	      {
		_vGLmode &= ~vGL_Alpha;	// zap the alpha plane
		_visinfo = chooseRGB(_vGLmode);
	      }
	    if (_visinfo == 0 && (_vGLmode & vGL_DoubleBuffer))
	      {
		_vGLmode &= ~vGL_DoubleBuffer;	// try without double buffer
		_visinfo = chooseRGB(_vGLmode);
	      }
	  }

	if (_visinfo == 0)
	    XtAppError(theApp->appContext(), "Can't create Visual for GL.");
	else
	    _doubleBuffer = (_vGLmode & vGL_DoubleBuffer);
      }
  }

//================>>> vBaseGLCanvasPane::~vBaseGLCanvasPane <<<===================
  vBaseGLCanvasPane::~vBaseGLCanvasPane()
  {
    SysDebug(Destructor,"vBaseGLCanvasPane::~vBaseGLCanvasPane() destructor\n")
  }

//==================>>> vBaseGLCanvasPane::chooseIndexed <<<=======================
  XVisualInfo* vBaseGLCanvasPane::chooseIndexed(unsigned int glMode)
  {

    static int bufSizeList[] = {16, 12, 8, 4, 2, 1, 0};
    int list[32];
    int n = 0;
    XVisualInfo* vi;

    list[n++] = GLX_BUFFER_SIZE;
    list[n++] = 1;		// we will dynamically change this
    if (glMode & vGL_DoubleBuffer)
      {
	list[n++] = GLX_DOUBLEBUFFER;
      }
    if (glMode & vGL_Stereo)
      {
	list[n++] = GLX_STEREO;
      }
    if (glMode & vGL_Depth)
      {
	list[n++] = GLX_DEPTH_SIZE;
	list[n++] = 1;
      }
    if (glMode & vGL_Stencil)
      {
	list[n++] = GLX_STENCIL_SIZE;
	list[n++] = 1;
      }
    list[n] = (int) None; /* terminate list */

    /* glXChooseVisual specify GLX_BUFFER_SIZE prefers the
    "smallest index buffer of at least the specified size".
    This would be reasonable if GLUT allowed the user to
    specify the required buffe size, but GLUT's display mode
    is too simplistic (easy to use?). GLUT should try to find
    the "largest".  So start with a large buffer size and
    shrink until we find a matching one that exists. */

    for (int i = 0; bufSizeList[i]; i++)
      {
	// Assumes list[1] is where GLX_BUFFER_SIZE parameter is.
	list[1] = bufSizeList[i];
	vi = glXChooseVisual(theApp->display(), 
	    DefaultScreen(theApp->display()), list);
	if (vi)
	  {
	    return vi;
	  }
      }
    return 0;
  }

//==================>>> vBaseGLCanvasPane::chooseRGB <<<=======================
  XVisualInfo* vBaseGLCanvasPane::chooseRGB(unsigned int glMode)
  {

    int list[32];
    int n = 0;
    XVisualInfo* vi;

    list[n++] = GLX_RGBA;
    list[n++] = GLX_RED_SIZE;
    list[n++] = 1;
    list[n++] = GLX_GREEN_SIZE;
    list[n++] = 1;
    list[n++] = GLX_BLUE_SIZE;
    list[n++] = 1;
    if (glMode & vGL_Alpha)
      {
	list[n++] = GLX_ALPHA_SIZE;
	list[n++] = 1;
      }

    if (glMode & vGL_DoubleBuffer)
      {
	list[n++] = GLX_DOUBLEBUFFER;
      }
    if (glMode & vGL_Stereo)
      {
	list[n++] = GLX_STEREO;
      }
    if (glMode & vGL_Depth)
      {
	list[n++] = GLX_DEPTH_SIZE;
	list[n++] = 16;			// fixed for 1.25: 5/17/00
      }
    if (glMode & vGL_Stencil)
      {
	list[n++] = GLX_STENCIL_SIZE;
	list[n++] = 1;
      }
    if (glMode & vGL_Accum)
      {
	list[n++] = GLX_ACCUM_RED_SIZE;
	list[n++] = 1;
	list[n++] = GLX_ACCUM_GREEN_SIZE;
	list[n++] = 1;
	list[n++] = GLX_ACCUM_BLUE_SIZE;
	list[n++] = 1;
	if (glMode & vGL_Alpha)
	  {
	    list[n++] = GLX_ACCUM_ALPHA_SIZE;
	    list[n++] = 1;
	  }
      }

    list[n] = (int) None; /* terminate list */

    vi = glXChooseVisual(theApp->display(), 
	    DefaultScreen(theApp->display()), list);
    return vi;
  }

extern "C"
{
//============================>>> CGLKeyInEV <<<==========================
  void CGLKeyInEV(Widget w, XtPointer client_data, XEvent* event, char *x)
  { 
    //	KeyPress Event Handler
    //
    // client_data will have the this pointer of our object

    XKeyPressedEvent *kp;

    kp = (XKeyPressedEvent*)event;
    vBaseGLCanvasPane* thisWindow = (vBaseGLCanvasPane*) client_data;	// get back this

    thisWindow->GLKeyInEV(kp->keycode, kp->state);
  }
}

//==================>>> vBaseGLCanvasPane::initialize <<<==========================
  void vBaseGLCanvasPane::initialize(vWindow* pWindow, Widget pWidget, 
	Widget drawCanvas, Widget drawArea)
  {

    Widget drwCnv, drwArea;

    drwCnv = XtVaCreateManagedWidget(
	"vBaseGLCanvasPane",
	canvasWidgetClass,		// widget class 
	pWidget,			// parent widget
#ifndef Athena
	XmNallowResize,1,
#else
	XtNshowGrip, FALSE,
	XtNallowResize,1,
#endif
	XtNhSpace, 1,
	XtNvSpace, 1,
	XtNheight,(pWindow->getWinHeight())+16,
	XtNwidth,(pWindow->getWinWidth())+16,
	NULL);			// argument list

     
    drwArea = XtVaCreateManagedWidget(
	"vCanvasDrawGL",		// widget name
	glwDrawingAreaWidgetClass,	// widget class 
	drwCnv,				// parent widget
	GLwNvisualInfo, _visinfo,
	XtNheight, pWindow->getWinHeight(),
	XtNwidth, pWindow->getWinWidth(),
	NULL);			// argument list

    // now we can handle everything in the standard canvas

    vCanvasPane::initialize(pWindow, pWidget, drwCnv, drwArea);

    // The graphics init event
    XtAddCallback(drwArea, GLwNginitCallback, CgraphicsInit,(XtPointer)this);

    glxContext = 0;

#ifdef Athena
    XtAddEventHandler(drwArea,KeyPressMask, 0,
	(XtEventHandler)CGLKeyInEV,(XtPointer)this);
#endif

  }

//=======================>>> vBaseGLCanvasPane::GLKeyInEV <<<==========================
  void vBaseGLCanvasPane::GLKeyInEV(unsigned int keycode, unsigned int state)
  {
    CnvKeyInEv(keycode, state);		// pass up
  }

//==================>>> vBaseGLCanvasPane::graphicsInit <<<========================
  void vBaseGLCanvasPane::graphicsInit(void)
  {
    XVisualInfo *visinfo;

    // Perform only the most basic operations, including getting the context

    XtVaGetValues(DrawingWindow(), GLwNvisualInfo, &visinfo, NULL);

    glxContext = glXCreateContext(theApp->display(), visinfo,
        0,					// No sharing.
        1);					// Direct rendering if possible.

    vglMakeCurrent();
  }

//==================>>> vBaseGLCanvasPane::graphicsInit <<<========================
  void vBaseGLCanvasPane::vglFlush(void)
  {
    
    if (_doubleBuffer)
	glXSwapBuffers(theApp->display(), XtWindow(DrawingWindow()));
    else
	glFlush();

    /* Avoid indirect rendering latency from queuing. */
    if (!glXIsDirect(theApp->display(), glxContext))
	glFinish();


  }

//=====================>>> vBaseGLCanvasPane::Resize <<<============================
  void vBaseGLCanvasPane::Resize(int newW, int newH)
  {
    // This is the routine the user will override to intercept size changes

    vglMakeCurrent();
    glXWaitX();
    glViewport(0,0,newW,newH);

  }


extern "C"
{
//==============================>>> CgraphicsInit <<<=============================
  void CgraphicsInit(Widget w, XtPointer clientData, XtPointer call)
  {
    ((vBaseGLCanvasPane*)clientData)->graphicsInit();
  }

}
