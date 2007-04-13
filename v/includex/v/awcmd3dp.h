/*
* $XConsortium: CommandP.h,v 1.31 94/04/17 20:12:01 rws Exp $
*/


/***********************************************************

Copyright (c) 1987, 1988  X Consortium

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


Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/******************************************************************
This code was taken directly from the Xaw3d distribution.
Several classes were used to make V look better.
The names of the classes have been changed to allow
standard Athena widgets to be used as well.

I've not marked any of the changes specifically.
   
Bruce Wampler - March 1, 1996
******************************************************************/


/* 
 * CommandP.h - Private definitions for Command widget
 * 
 */

#ifndef _XawCommandP3d_h
#define _XawCommandP3d_h

#include <v/awcmd3d.h>
#include <v/awlbl3dp.h>

/***********************************************************************
 *
 * Command Widget Private Data
 *
 ***********************************************************************/

typedef enum {
  HighlightNone,		/* Do not highlight. */
  HighlightWhenUnset,		/* Highlight only when unset, this is
				   to preserve current command3d widget 
				   functionality. */
  HighlightAlways		/* Always highlight, lets the toggle widget
				   and other subclasses do the right thing. */
} XtCommand3dHighlight;

/************************************
 *
 *  Class structure
 *
 ***********************************/


   /* New fields for the Command3d widget class record */
typedef struct _Command3dClass 
  {
    int makes_compiler_happy;  /* not used */
  } Command3dClassPart;

   /* Full class record declaration */
typedef struct _Command3dClassRec {
    CoreClassPart	core_class;
    Simple3dClassPart	simple3d_class;
    ThreeDClassPart	threeD_class;
    Label3dClassPart	label3d_class;
    Command3dClassPart    command3d_class;
} Command3dClassRec;

extern Command3dClassRec command3dClassRec;

/***************************************
 *
 *  Instance (widget) structure 
 *
 **************************************/

    /* New fields for the Command3d widget record */
typedef struct {
    /* resources */
    Dimension   highlight_thickness;
    XtCallbackList callbacks;

    /* private state */
    Pixmap      	gray_pixmap;
    GC          	normal_GC;
    GC          	inverse_GC;
    Boolean     	set;
    XtCommand3dHighlight	highlighted;
    /* more resources */
    int			shape_style;    
    Dimension		corner_round;
} Command3dPart;


/*    XtEventsPtr eventTable;*/


   /* Full widget declaration */
typedef struct _Command3dRec {
    CorePart         core;
    Simple3dPart	     simple3d;
    ThreeDPart       threeD;
    Label3dPart	     label3d;
    Command3dPart      command3d;
} Command3dRec;

#endif /* _XawCommand3dP_h */
