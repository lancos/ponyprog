/* $XConsortium: ToggleP.h,v 1.9 94/04/17 20:13:19 converse Exp $ */

/*

Copyright (c) 1989  X Consortium

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
 *
 */

#ifndef _XawToggleP_h
#define _XawToggleP_h

#include <v/awtog3d.h>
#include <v/awcmd3dp.h>

/***********************************************************************
 *
 * Toggle Widget Private Data
 *
 ***********************************************************************/

#define streq(a, b) ( strcmp((a), (b)) == 0 )

/************************************
 *
 *  Class structure
 *
 ***********************************/

   /* New fields for the Toggle widget class record */
typedef struct _Toggle3dClass  {
    XtActionProc Set;
    XtActionProc Unset;
    XtPointer extension;
} Toggle3dClassPart;

   /* Full class record declaration */
typedef struct _Toggle3dClassRec {
    CoreClassPart	core_class;
    Simple3dClassPart	simple3d_class;
    ThreeDClassPart	threeD_class;
    Label3dClassPart	label3d_class;
    Command3dClassPart	command3d_class;
    Toggle3dClassPart     toggle3d_class;
} Toggle3dClassRec;

extern Toggle3dClassRec toggle3dClassRec;

/***************************************
 *
 *  Instance (widget) structure 
 *
 **************************************/

    /* New fields for the Toggle widget record */
typedef struct {
    /* resources */
    Widget      widget;
} Toggle3dPart;

   /* Full widget declaration */
typedef struct _Toggle3dRec {
    CorePart		core;
    Simple3dPart	simple3d;
    ThreeDPart		threeD;
    Label3dPart		label3d;
    Command3dPart	command3d;
    Toggle3dPart       toggle3d;
} Toggle3dRec;

#endif /* _XawToggleP_h */
