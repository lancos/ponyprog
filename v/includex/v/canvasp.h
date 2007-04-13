/* ===============================================================
 canvasp.h - Private definitions for canvas widget

 Copyright (C) 1995,1996  Bruce E. Wampler

 This file is part of the V C++ GUI Framework, and is covered
 under the terms of the GNU Library General Public License,
 Version 2. This library has NO WARRANTY. See the source file
 vapp.cxx for more complete information about license terms.
=============================================================== */

#ifndef CANVASP_H
#define CANVASP_H

#include <v/canvas.h>

#include <X11/CompositeP.h>

/* New fields for the canvas widget class record */
  typedef struct _CanvasClass
    {
	int empty;		/* need something */
    } CanvasClassPart;

/* Full class record declaration */
    typedef struct _CanvasClassRec
      {
	CoreClassPart	core_class;
	CompositeClassPart  composite_class;
	CanvasClassPart	canvas_class;
      } CanvasClassRec;

  extern CanvasClassRec canvasClassRec;

/* New fields for the canvas widget record */
    typedef struct
      {
	XtCallbackList callback;		/* app installed callbacks */
	Dimension	h_space, v_space;
	Dimension	preferred_width, preferred_height;
	Dimension	last_query_width, last_query_height;
	XtGeometryMask	last_query_mode;
      } CanvasPart;


/****************************************************************

	Full instance record declaration

 ****************************************************************/

  typedef struct _CanvasRec
    {
	CorePart	core;
	CompositePart	composite;
	CanvasPart	canvas;
    } CanvasRec;

#endif
