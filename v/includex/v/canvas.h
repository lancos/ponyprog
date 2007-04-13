#ifndef CANVAS_H
#define CANVAS_H

/***********************************************************************

  canvas Widget (subclass of CompositeClass)

  Copyright (C) 1995  Bruce E. Wampler

  This file is part of the V C++ GUI Framework, and is covered
  under the terms of the GNU Library General Public License,
  Version 2. This library has NO WARRANTY. See the source file
  vapp.cxx for more complete information about license terms.

 ***********************************************************************/

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 hSpace 	     HSpace		Dimension	4
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 vSpace 	     VSpace		Dimension	4
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

*/


/* Class record constants */

    typedef struct _CanvasClassRec *CanvasWidgetClass;
    typedef struct _CanvasRec      *CanvasWidget;

    extern WidgetClass canvasWidgetClass;

#endif
