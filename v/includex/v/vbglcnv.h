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

#include <v/vapp.h>
#include <v/vcanvas.h>

extern "C"
{

  static void CGLKeyInEV(Widget w, XtPointer client_data, XEvent* event, char *x);

// OpenGL stuff here

    void CgraphicsInit(Widget w, XtPointer clientData, XtPointer call);

#include <X11/GLw/GLwDrawA.h>	// Xt OpenGL drawing area widget.
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

}

const unsigned int vGL_Default = 0;
const unsigned int vGL_RGB = 0x0001;
const unsigned int vGL_Alpha = 0x0002;
const unsigned int vGL_Indexed = 0x0004;
const unsigned int vGL_DoubleBuffer = 0x0008;
const unsigned int vGL_Stereo = 0x0010;
const unsigned int vGL_Stencil = 0x0020;
const unsigned int vGL_Accum = 0x0040;
const unsigned int vGL_Depth = 0x0080;


    class vWindow;		// we are part of a window

    class vBaseGLCanvasPane : public vCanvasPane
      {

	friend void CgraphicsInit(Widget w, XtPointer clientData, XtPointer call);
	friend void CGLKeyInEV(Widget w, XtPointer client_data, XEvent* event, char *x);
      public:		//---------------------------------------- public

	vBaseGLCanvasPane(unsigned int vGLmode = vGL_Default, PaneType pt = P_Canvas);

 	virtual ~vBaseGLCanvasPane();


	// Expose/redraw event
	virtual void Resize(int newW, int newH);  // Resize event

	// OpenGL specific stuff

	virtual void graphicsInit(void);

	void vglMakeCurrent(void)
	  {
	    glXMakeCurrent(theApp->display(), XtWindow(DrawingWindow()), glxContext);
	  }

 	virtual void vglFlush();

	
	XVisualInfo* GetXVisualInfo() { return _visinfo; }

	// Platform dependent


      protected:	//--------------------------------------- protected

	virtual void initialize(vWindow* pWindow, Widget pWidget,
	    Widget drawCanvas = (Widget)0, Widget drawArea=(Widget)0);

	void GLKeyInEV(unsigned int keycode, unsigned int state);

	XVisualInfo* chooseIndexed(unsigned int glMode);
	XVisualInfo* chooseRGB(unsigned int glMode);

	GLXContext glxContext;

      private:		//--------------------------------------- private

	static XVisualInfo* _visinfo;
	static int _doubleBuffer;		// if we have double buffering
	static unsigned int _vGLmode;

      };
#endif
