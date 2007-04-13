/*
 * $XConsortium: Toggle.h,v 1.16 94/04/17 20:13:19 kaleb Exp $
 *
Copyright (c) 1989, 1994  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.
 */

/*
 * ToggleP.h - Private definitions for Toggle widget
 *
 * Author: Chris D. Peterson
 *         MIT X Consortium
 *         kit@expo.lcs.mit.edu
 *  
 * Date:   January 12, 1989
 */

#ifndef _XawToggle3d_h
#define _XawToggle3d_h

/***********************************************************************
 *
 * Toggle Widget
 *
 ***********************************************************************/

#include <v/awcmd3d.h>

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 state               State              Boolean         Off

 background	     Background		Pixel		XtDefaultBackground
 bitmap		     Pixmap		Pixmap		None
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 callback	     Callback		Pointer		NULL
 cursor		     Cursor		Cursor		None
 destroyCallback     Callback		Pointer		NULL
 font		     Font		XFontStructx*	XtDefaultFont
 foreground	     Foreground		Pixel		XtDefaultForeground
 height		     Height		Dimension	text height
 highlightThickness  Thickness		Dimension	2
 insensitiveBorder   Insensitive	Pixmap		Gray
 internalHeight	     Height		Dimension	2
 internalWidth	     Width		Dimension	4
 justify	     Justify		XtJustify	XtJustifyCenter
 label		     Label		String		NULL
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 resize		     Resize		Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	text width
 x		     Position		Position	0
 y		     Position		Position	0


*/

/*
 * These should be in StringDefs.h but aren't so we will define
 * them here if they are needed.
 */


#define XtCWidget "Widget"
#define XtCState "State"

#ifndef _XtStringDefs_h_
#define XtRWidget "Widget"
#endif

#define XtNstate "state"

extern WidgetClass               toggle3dWidgetClass;

typedef struct _Toggle3dClassRec   *Toggle3dWidgetClass;
typedef struct _Toggle3dRec        *Toggle3dWidget;

#endif /* _XawToggle3d_h */
