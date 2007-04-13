/*
* $XConsortium: LabelP.h,v 1.29 94/04/17 20:12:14 kaleb Exp $
*/


/***********************************************************

Copyright (c) 1987, 1988, 1994  X Consortium

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
 * LabelP.h - Private definitions for Label widget
 * 
 */

#ifndef _XawLabelP3d_h
#define _XawLabelP3d_h

/***********************************************************************
 *
 * Label Widget Private Data
 *
 ***********************************************************************/

#include <v/awlbl3d.h>
#include <v/aw3dp.h>

/* New fields for the Label widget class record */

typedef struct {int foo;} Label3dClassPart;

/* Full class record declaration */
typedef struct _Label3dClassRec {
    CoreClassPart	core_class;
    Simple3dClassPart	simple3d_class;
    ThreeDClassPart     threeD_class;
    Label3dClassPart	label3d_class;
} Label3dClassRec;

extern Label3dClassRec label3dClassRec;

/* New fields for the Label3d widget record */
typedef struct {
    /* resources */
    Pixel	foreground;
    XFontStruct	*font;
    XFontSet	fontset;
    char	*label3d;
    XtJustify	justify;
    Dimension	internal_width;
    Dimension	internal_height;
    Pixmap	pixmap;
    Boolean	resize;
    unsigned char encoding;
    Pixmap	left_bitmap;

    /* private state */
    GC		normal_GC;
    GC          gray_GC;
    Pixmap	stipple;
    Position	label3d_x;
    Position	label3d_y;
    Dimension	label3d_width;
    Dimension	label3d_height;
    Dimension	label3d_len;
    int		lbm_y;			/* where in label3d */
    unsigned int lbm_width, lbm_height;	 /* size of pixmap */
} Label3dPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _Label3dRec {
    CorePart	core;
    Simple3dPart	simple3d;
    ThreeDPart  threeD;
    Label3dPart	label3d;
} Label3dRec;

#define LEFT_OFFSET(lw) ((lw)->label3d.left_bitmap \
			 ? (lw)->label3d.lbm_width + (lw)->label3d.internal_width \
			 : 0)

#endif /* _XawLabel3dP_h */
