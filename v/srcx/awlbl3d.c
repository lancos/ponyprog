/* $XConsortium: Label.c,v 1.97 94/04/17 20:12:12 kaleb Exp $ */

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
 * Label.c - Label widget
 *
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <v/awlbl3dp.h>
#include <X11/Xmu/Converters.h>
#include <X11/Xmu/Drawing.h>
#include <ctype.h>

/* needed for abs() */
#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#else
int abs();
#endif

#define streq(a,b) (strcmp( (a), (b) ) == 0)

#define MULTI_LINE_LABEL3d 32767

#ifdef CRAY
#define WORD64
#endif

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffsetOf(Label3dRec, field)
static XtResource resources[] = {
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(label3d.foreground), XtRString, XtDefaultForeground},
    {XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	offset(label3d.font),XtRString, XtDefaultFont},
    {XtNfontSet,  XtCFontSet, XtRFontSet, sizeof(XFontSet ),
        offset(label3d.fontset),XtRString, XtDefaultFontSet},
    {XtNlabel,  XtCLabel, XtRString, sizeof(String),
	offset(label3d.label3d), XtRString, NULL},
    {XtNencoding, XtCEncoding, XtRUnsignedChar, sizeof(unsigned char),
	offset(label3d.encoding), XtRImmediate, (XtPointer)XawTextEncoding8bit},
    {XtNjustify, XtCJustify, XtRJustify, sizeof(XtJustify),
	offset(label3d.justify), XtRImmediate, (XtPointer)XtJustifyCenter},
    {XtNinternalWidth, XtCWidth, XtRDimension,  sizeof(Dimension),
	offset(label3d.internal_width), XtRImmediate, (XtPointer)4},
    {XtNinternalHeight, XtCHeight, XtRDimension, sizeof(Dimension),
	offset(label3d.internal_height), XtRImmediate, (XtPointer)2},
    {XtNleftBitmap, XtCLeftBitmap, XtRBitmap, sizeof(Pixmap),
       offset(label3d.left_bitmap), XtRImmediate, (XtPointer) None},
    {XtNbitmap, XtCPixmap, XtRBitmap, sizeof(Pixmap),
	offset(label3d.pixmap), XtRImmediate, (XtPointer)None},
    {XtNresize, XtCResize, XtRBoolean, sizeof(Boolean),
	offset(label3d.resize), XtRImmediate, (XtPointer)True},
    {XtNshadowWidth, XtCShadowWidth, XtRDimension, sizeof(Dimension),
	offset(threeD.shadow_width), XtRImmediate, (XtPointer) 0},
    {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
         XtOffsetOf(RectObjRec,rectangle.border_width), XtRImmediate,
         (XtPointer)1}
};
#undef offset

static void Initialize();
static void Resize();
static void Redisplay();
static Boolean SetValues();
static void ClassInitialize();
static void Destroy();
static XtGeometryResult QueryGeometry();

Label3dClassRec label3dClassRec = {
  {
/* core_class fields */	
    /* superclass	  	*/	(WidgetClass) &threeDClassRec,
    /* class_name	  	*/	"Label3d",
    /* widget_size	  	*/	sizeof(Label3dRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	XtInheritRealize,
    /* actions		  	*/	NULL,
    /* num_actions	  	*/	0,
    /* resources	  	*/	resources,
    /* num_resources	  	*/	XtNumber(resources),
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	TRUE,
    /* compress_exposure  	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	Destroy,
    /* resize		  	*/	Resize,
    /* expose		  	*/	Redisplay,
    /* set_values	  	*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
/* Simple3d class fields initialization */
  {
    /* change_sensitive		*/	XtInheritChangeSensitive
  },
/* ThreeD class fields initialization */
  {
    /* shadowdraw 		*/	XtInheritXaw3dShadowDraw
  },
/* Label3d class fields initialization */
  {
    /* ignore 			*/	0
  }
};
WidgetClass label3dWidgetClass = (WidgetClass)&label3dClassRec;
/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void ClassInitialize()
{
    XtAddConverter( XtRString, XtRJustify, XmuCvtStringToJustify, 
		    (XtConvertArgList)NULL, 0 );
}

#ifndef WORD64

#define TXT16 XChar2b

#else

#define TXT16 char

static XChar2b *buf2b;
static int buf2blen = 0;

_XawLabel3dWidth16(fs, str, n)
    XFontStruct *fs;
    char *str;
    int	n;
{
    int i;
    XChar2b *ptr;

    if (n > buf2blen) {
	buf2b = (XChar2b *)XtRealloc((char *)buf2b, n * sizeof(XChar2b));
	buf2blen = n;
    }
    for (ptr = buf2b, i = n; --i >= 0; ptr++) {
	ptr->byte1 = *str++;
	ptr->byte2 = *str++;
    }
    return XTextWidth16(fs, buf2b, n);
}

_XawLabel3dDraw16(dpy, d, gc, x, y, str, n)
    Display *dpy;
    Drawable d;
    GC gc;
    int x, y;
    char *str;
    int n;
{
    int i;
    XChar2b *ptr;

    if (n > buf2blen) {
	buf2b = (XChar2b *)XtRealloc((char *)buf2b, n * sizeof(XChar2b));
	buf2blen = n;
    }
    for (ptr = buf2b, i = n; --i >= 0; ptr++) {
	ptr->byte1 = *str++;
	ptr->byte2 = *str++;
    }
    XDrawString16(dpy, d, gc, x, y, buf2b, n);
}

#define XTextWidth16 _XawLabel3dWidth16
#define XDrawString16 _XawLabel3dDraw16

#endif /* WORD64 */

/*
 * Calculate width and height of displayed text in pixels
 */

static void SetTextWidthAndHeight(lw)
    Label3dWidget lw;
{
    XFontStruct	*fs = lw->label3d.font;

    char *nl;

    if (lw->label3d.pixmap != None) {
	Window root;
	int x, y;
	unsigned int width, height, bw, depth;
	if (XGetGeometry(XtDisplay(lw), lw->label3d.pixmap, &root, &x, &y,
			 &width, &height, &bw, &depth)) {
	    lw->label3d.label3d_height = height;
	    lw->label3d.label3d_width = width;
	    lw->label3d.label3d_len = depth;
	    return;
	}
    }
    if ( lw->simple3d.international == True ) {

      XFontSet	fset = lw->label3d.fontset;
      XFontSetExtents *ext = XExtentsOfFontSet(fset);

      lw->label3d.label3d_height = ext->max_ink_extent.height;
      if (lw->label3d.label3d == NULL) {
	  lw->label3d.label3d_len = 0;
	  lw->label3d.label3d_width = 0;
      }
      else if ((nl = index(lw->label3d.label3d, '\n')) != NULL) {
	  char *label3d;
	  lw->label3d.label3d_len = MULTI_LINE_LABEL3d;
	  lw->label3d.label3d_width = 0;
	  for (label3d = lw->label3d.label3d; nl != NULL; nl = index(label3d, '\n')) {
	      int width = XmbTextEscapement(fset, label3d, (int)(nl - label3d));

	      if (width > (int)lw->label3d.label3d_width)
		  lw->label3d.label3d_width = width;
	      label3d = nl + 1;
	      if (*label3d)
		  lw->label3d.label3d_height +=
		      ext->max_ink_extent.height;
	  }
	  if (*label3d) {
	      int width = XmbTextEscapement(fset, label3d, strlen(label3d));

	      if (width > (int) lw->label3d.label3d_width)
		  lw->label3d.label3d_width = width;
	  }
      } else {
	  lw->label3d.label3d_len = strlen(lw->label3d.label3d);
	  lw->label3d.label3d_width =
	      XmbTextEscapement(fset, lw->label3d.label3d, (int) lw->label3d.label3d_len);
      }

    } else {

        lw->label3d.label3d_height = fs->max_bounds.ascent + fs->max_bounds.descent;
        if (lw->label3d.label3d == NULL) {
            lw->label3d.label3d_len = 0;
            lw->label3d.label3d_width = 0;
        }
        else if ((nl = index(lw->label3d.label3d, '\n')) != NULL) {
	    char *label3d;
            lw->label3d.label3d_len = MULTI_LINE_LABEL3d;
            lw->label3d.label3d_width = 0;
            for (label3d = lw->label3d.label3d; nl != NULL; nl = index(label3d, '\n')) {
	        int width;

	        if (lw->label3d.encoding)
		    width = XTextWidth16(fs, (TXT16*)label3d, (int)(nl - label3d)/2);
	        else
		    width = XTextWidth(fs, label3d, (int)(nl - label3d));
	        if (width > (int)lw->label3d.label3d_width)
		    lw->label3d.label3d_width = width;
	        label3d = nl + 1;
	        if (*label3d)
		    lw->label3d.label3d_height +=
		        fs->max_bounds.ascent + fs->max_bounds.descent;
	    }
	    if (*label3d) {
	        int width = XTextWidth(fs, label3d, strlen(label3d));

	        if (lw->label3d.encoding)
		    width = XTextWidth16(fs, (TXT16*)label3d, (int)strlen(label3d)/2);
	        else
		    width = XTextWidth(fs, label3d, strlen(label3d));
	        if (width > (int) lw->label3d.label3d_width)
		    lw->label3d.label3d_width = width;
	    }
        } else {
	    lw->label3d.label3d_len = strlen(lw->label3d.label3d);
	    if (lw->label3d.encoding)
	        lw->label3d.label3d_width =
		    XTextWidth16(fs, (TXT16*)lw->label3d.label3d,
			         (int) lw->label3d.label3d_len/2);
	    else
	        lw->label3d.label3d_width =
		    XTextWidth(fs, lw->label3d.label3d, (int) lw->label3d.label3d_len);
        }

    }
}

static void GetnormalGC(lw)
    Label3dWidget lw;
{
    XGCValues	values;

    values.foreground	= lw->label3d.foreground;
    values.background	= lw->core.background_pixel;
    values.font		= lw->label3d.font->fid;
    values.graphics_exposures = False;

    if ( lw->simple3d.international == True )
        /* Since Xmb/wcDrawString eats the font, I must use XtAllocateGC. */
        lw->label3d.normal_GC = XtAllocateGC(
                (Widget)lw, 0,
	(unsigned) GCForeground | GCBackground | GCGraphicsExposures,
	&values, GCFont, 0 );
    else
        lw->label3d.normal_GC = XtGetGC(
	(Widget)lw,
	(unsigned) GCForeground | GCBackground | GCFont | GCGraphicsExposures,
	&values);
}

static void GetgrayGC(lw)
    Label3dWidget lw;
{
    XGCValues	values;

    values.foreground = lw->label3d.foreground;
    values.background = lw->core.background_pixel;
    values.font	      = lw->label3d.font->fid;
    values.fill_style = FillTiled;
    values.tile       = XmuCreateStippledPixmap(XtScreen((Widget)lw),
						lw->label3d.foreground, 
						lw->core.background_pixel,
						lw->core.depth);
    values.graphics_exposures = False;

    lw->label3d.stipple = values.tile;
    if ( lw->simple3d.international == True )
        /* Since Xmb/wcDrawString eats the font, I must use XtAllocateGC. */
        lw->label3d.gray_GC = XtAllocateGC((Widget)lw,  0,
				(unsigned) GCForeground | GCBackground |
					   GCTile | GCFillStyle |
					   GCGraphicsExposures,
				&values, GCFont, 0);
    else
        lw->label3d.gray_GC = XtGetGC((Widget)lw, 
				(unsigned) GCForeground | GCBackground |
					   GCFont | GCTile | GCFillStyle |
					   GCGraphicsExposures,
				&values);
}

static void compute_bitmap_offsets (lw)
    Label3dWidget lw;
{
    /*
     * bitmap will be eventually be displayed at 
     * (internal_width, internal_height + lbm_y)
     */
    if (lw->label3d.lbm_height != 0) {
	lw->label3d.lbm_y = (lw->core.height -
			  (lw->threeD.shadow_width * 2 + 
			   lw->label3d.internal_height * 2 +
			   lw->label3d.lbm_height)) / 2;
    } else {
	lw->label3d.lbm_y = 0;
    }
}


static void set_bitmap_info (lw)
    Label3dWidget lw;
{
    Window root;
    int x, y;
    unsigned int bw, depth;

    if (!(lw->label3d.left_bitmap &&
	  XGetGeometry (XtDisplay(lw), lw->label3d.left_bitmap, &root, &x, &y,
			&lw->label3d.lbm_width, &lw->label3d.lbm_height,
			&bw, &depth))) {
	lw->label3d.lbm_width = lw->label3d.lbm_height = 0;
    }
    compute_bitmap_offsets (lw);
}



/* ARGSUSED */
static void Initialize(request, new, args, num_args)
    Widget request, new;
    ArgList args;
    Cardinal *num_args;
{
    Label3dWidget lw = (Label3dWidget) new;

    if (lw->label3d.label3d == NULL) 
        lw->label3d.label3d = XtNewString(lw->core.name);
    else {
        lw->label3d.label3d = XtNewString(lw->label3d.label3d);
    }

    GetnormalGC(lw);
    GetgrayGC(lw);

    SetTextWidthAndHeight(lw);

    if (lw->core.height == 0)
        lw->core.height = lw->label3d.label3d_height +
			    2 * lw->label3d.internal_height +
			    2 * lw->threeD.shadow_width;

    set_bitmap_info (lw);		/* need core.height */

    if (lw->core.width == 0)		/* need label3d.lbm_width */
        lw->core.width = (lw->label3d.label3d_width +
			    2 * lw->label3d.internal_width +
			    2 * lw->threeD.shadow_width +
			    LEFT_OFFSET(lw));

    lw->label3d.label3d_x = lw->label3d.label3d_y = 0;
    (*XtClass(new)->core_class.resize) ((Widget)lw);

} /* Initialize */

/*
 * Repaint the widget window
 */

/* ARGSUSED */
static void Redisplay(gw, event, region)
    Widget gw;
    XEvent *event;
    Region region;
{
    extern WidgetClass command3dWidgetClass;
    Label3dWidget w = (Label3dWidget) gw;
    Label3dWidgetClass lwclass = (Label3dWidgetClass) XtClass (gw);
    GC gc;

    /* 
     * Don't draw shadows if command is going to redraw them.
     * The shadow draw method is region aware, but since 99% of
     * all label3ds don't have shadows, we'll check for a shadow
     * before we incur the function call overhead.
     */
    if (!XtIsSubclass (gw, command3dWidgetClass) && w->threeD.shadow_width > 0)
	(*lwclass->threeD_class.shadowdraw) (gw, event, region, True);

    /*
     * now we'll see if we need to draw the rest of the label3d
     */
    if (region != NULL) {
	int x = w->label3d.label3d_x;
	unsigned int width = w->label3d.label3d_width;
	if (w->label3d.lbm_width) {
	    if (w->label3d.label3d_x > (x = w->label3d.internal_width))
		width += w->label3d.label3d_x - x;
	}
	if (XRectInRegion(region, x, w->label3d.label3d_y,
			 width, w->label3d.label3d_height) == RectangleOut){
	    return;
	}
    }

    gc = XtIsSensitive(gw) ? w->label3d.normal_GC : w->label3d.gray_GC;
#ifdef notdef
    if (region != NULL)
	XSetRegion(XtDisplay(gw), gc, region);
#endif /*notdef*/

    if (w->label3d.pixmap == None) {
	int len = w->label3d.label3d_len;
	char *label3d = w->label3d.label3d;
	Position y = w->label3d.label3d_y + w->label3d.font->max_bounds.ascent;
        Position ksy = w->label3d.label3d_y;

	/* display left bitmap */
	if (w->label3d.left_bitmap && w->label3d.lbm_width != 0) {
	    XCopyPlane (XtDisplay(gw), w->label3d.left_bitmap, XtWindow(gw), gc,
		       0, 0, w->label3d.lbm_width, w->label3d.lbm_height,
		       (int) w->label3d.internal_width
			+ w->threeD.shadow_width,
		       (int) w->label3d.internal_height 
			+ w->threeD.shadow_width
			+ w->label3d.lbm_y, 
		       (unsigned long) 1L);
	}

        if ( w->simple3d.international == True ) {

	    XFontSetExtents *ext = XExtentsOfFontSet(w->label3d.fontset);

	    ksy += abs(ext->max_ink_extent.y);

            if (len == MULTI_LINE_LABEL3d) {
	        char *nl;
	        while ((nl = index(label3d, '\n')) != NULL) {
	            XmbDrawString(XtDisplay(w), XtWindow(w), w->label3d.fontset, gc,
	  		        w->label3d.label3d_x, ksy, label3d, (int)(nl - label3d));
	            ksy += ext->max_ink_extent.height;
	            label3d = nl + 1;
	        }
	        len = strlen(label3d);
            }
            if (len)
	        XmbDrawString(XtDisplay(w), XtWindow(w), w->label3d.fontset, gc,
			      w->label3d.label3d_x, ksy, label3d, len);

        } else { /*international false, so use R5 routine */

	    if (len == MULTI_LINE_LABEL3d) {
	        char *nl;
	        while ((nl = index(label3d, '\n')) != NULL) {
		    if (w->label3d.encoding)
		        XDrawString16(XtDisplay(gw), XtWindow(gw), gc,
				 		w->label3d.label3d_x, y,
				 		(TXT16*)label3d, (int)(nl - label3d)/2);
		    else
		        XDrawString(XtDisplay(gw), XtWindow(gw), gc,
			       		w->label3d.label3d_x, y, label3d, (int)(nl - label3d));
		    y += w->label3d.font->max_bounds.ascent + 
		                        w->label3d.font->max_bounds.descent;
		    label3d = nl + 1;
	        }
	        len = strlen(label3d);
	    }
	    if (len) {
	        if (w->label3d.encoding)
		    XDrawString16(XtDisplay(gw), XtWindow(gw), gc,
			     w->label3d.label3d_x, y, (TXT16*)label3d, len/2);
	        else
		    XDrawString(XtDisplay(gw), XtWindow(gw), gc,
			   w->label3d.label3d_x, y, label3d, len);
	    }

        } /*endif international*/

    } else if (w->label3d.label3d_len == 1) { /* depth */
	XCopyPlane(XtDisplay(gw), w->label3d.pixmap, XtWindow(gw), gc,
				0, 0, w->label3d.label3d_width, w->label3d.label3d_height,
				w->label3d.label3d_x, w->label3d.label3d_y, 1L);
    } else {
	XCopyArea(XtDisplay(gw), w->label3d.pixmap, XtWindow(gw), gc,
				0, 0, w->label3d.label3d_width, w->label3d.label3d_height,
				w->label3d.label3d_x, w->label3d.label3d_y);
    }

#ifdef notdef
    if (region != NULL)
	XSetClipMask(XtDisplay(gw), gc, (Pixmap)None);
#endif /* notdef */
}

static void _Reposition(lw, width, height, dx, dy)
    Label3dWidget lw;
    Dimension width, height;
    Position *dx, *dy;
{
    Position newPos;
    Position leftedge = lw->label3d.internal_width + LEFT_OFFSET(lw);

    switch (lw->label3d.justify) {

	case XtJustifyLeft   :
	    newPos = leftedge;
	    break;

	case XtJustifyRight  :
	    newPos = width -
		(lw->label3d.label3d_width 
			+ lw->label3d.internal_width 
			+ lw->threeD.shadow_width);
	    break;

	case XtJustifyCenter :
	default:
	    newPos = (int)(width - lw->label3d.label3d_width) / 2;
	    break;
    }
    if (newPos < (Position)leftedge)
	newPos = leftedge;
    *dx = newPos - lw->label3d.label3d_x;
    lw->label3d.label3d_x = newPos;
    *dy = (newPos = (int)(height - lw->label3d.label3d_height) / 2)
	  - lw->label3d.label3d_y;
    lw->label3d.label3d_y = newPos;
    return;
}

static void Resize(w)
    Widget w;
{
    Label3dWidget lw = (Label3dWidget)w;
    Position dx, dy;

    _Reposition(lw, w->core.width, w->core.height, &dx, &dy);
    compute_bitmap_offsets (lw);
}

/*
 * Set specified arguments into widget
 */

#define PIXMAP 0
#define WIDTH 1
#define HEIGHT 2
#define NUM_CHECKS 3

static Boolean SetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
    Label3dWidget curlw = (Label3dWidget) current;
    Label3dWidget reqlw = (Label3dWidget) request;
    Label3dWidget newlw = (Label3dWidget) new;
    int i;
    Boolean was_resized = False, redisplay = False, checks[NUM_CHECKS];

    for (i = 0; i < NUM_CHECKS; i++)
	checks[i] = FALSE;

    for (i = 0; i < *num_args; i++) {
	if (streq(XtNbitmap, args[i].name))
	    checks[PIXMAP] = TRUE;
	if (streq(XtNwidth, args[i].name))
	    checks[WIDTH] = TRUE;
	if (streq(XtNheight, args[i].name))
	    checks[HEIGHT] = TRUE;
    }

    if (newlw->label3d.label3d == NULL) {
	newlw->label3d.label3d = newlw->core.name;
    }

    /*
     * resize on bitmap change
     */
    if (curlw->label3d.left_bitmap != newlw->label3d.left_bitmap) {
	was_resized = True;
    }

    if (curlw->label3d.encoding != newlw->label3d.encoding)
	was_resized = True;

    if ( (curlw->label3d.fontset != newlw->label3d.fontset) &&
					 curlw->simple3d.international ){
	was_resized = True;
    }
    if (curlw->label3d.label3d != newlw->label3d.label3d) {
        if (curlw->label3d.label3d != curlw->core.name)
	    XtFree( (char *)curlw->label3d.label3d );

	if (newlw->label3d.label3d != newlw->core.name) {
	    newlw->label3d.label3d = XtNewString( newlw->label3d.label3d );
	}
	was_resized = True;
    }

    if (was_resized || (curlw->label3d.font != newlw->label3d.font) ||
	(curlw->label3d.justify != newlw->label3d.justify) || checks[PIXMAP]) {

	SetTextWidthAndHeight(newlw);
	was_resized = True;
    }

    /* recalculate the window size if something has changed. */
    if (newlw->label3d.resize && was_resized) {
	if ((curlw->core.height == reqlw->core.height) && !checks[HEIGHT])
	    newlw->core.height = (newlw->label3d.label3d_height +
				    2 * newlw->label3d.internal_height +
				    2 * newlw->threeD.shadow_width);

	set_bitmap_info (newlw);

	if ((curlw->core.width == reqlw->core.width) && !checks[WIDTH])
	    newlw->core.width = (newlw->label3d.label3d_width +
				    LEFT_OFFSET(newlw) +
				    2 * newlw->label3d.internal_width +
				    2 * newlw->threeD.shadow_width);
    }

    if (curlw->label3d.foreground		!= newlw->label3d.foreground
	|| curlw->core.background_pixel != newlw->core.background_pixel
	|| curlw->label3d.font->fid	!= newlw->label3d.font->fid	  ) {

        /* The Fontset is not in the GC - don't make a new GC if FS changes! */

	XtReleaseGC(new, curlw->label3d.normal_GC);
	XtReleaseGC(new, curlw->label3d.gray_GC);
	XmuReleaseStippledPixmap( XtScreen(current), curlw->label3d.stipple );
	GetnormalGC(newlw);
	GetgrayGC(newlw);
	redisplay = True;
    }

    if ((curlw->label3d.internal_width != newlw->label3d.internal_width)
        || (curlw->label3d.internal_height != newlw->label3d.internal_height)
	|| was_resized) {
	/* Resize() will be called if geometry changes succeed */
	Position dx, dy;
	_Reposition(newlw, curlw->core.width, curlw->core.height, &dx, &dy);
    }

    return was_resized || redisplay ||
	   XtIsSensitive(current) != XtIsSensitive(new);
}

static void Destroy(w)
    Widget w;
{
    Label3dWidget lw = (Label3dWidget)w;

    if ( lw->label3d.label3d != lw->core.name )
	XtFree( lw->label3d.label3d );
    XtReleaseGC( w, lw->label3d.normal_GC );
    XtReleaseGC( w, lw->label3d.gray_GC);
    XmuReleaseStippledPixmap( XtScreen(w), lw->label3d.stipple );
}


static XtGeometryResult QueryGeometry(w, intended, preferred)
    Widget w;
    XtWidgetGeometry *intended, *preferred;
{
    Label3dWidget lw = (Label3dWidget)w;

    preferred->request_mode = CWWidth | CWHeight;
    preferred->width = (lw->label3d.label3d_width + 
			    2 * lw->label3d.internal_width +
			    2 * lw->threeD.shadow_width +
			    LEFT_OFFSET(lw));
    preferred->height = lw->label3d.label3d_height + 
			    2 * lw->label3d.internal_height +
			    2 * lw->threeD.shadow_width;
    if (  ((intended->request_mode & (CWWidth | CWHeight))
	   	== (CWWidth | CWHeight)) &&
	  intended->width == preferred->width &&
	  intended->height == preferred->height)
	return XtGeometryYes;
    else if (preferred->width == w->core.width &&
	     preferred->height == w->core.height)
	return XtGeometryNo;
    else
	return XtGeometryAlmost;
}
