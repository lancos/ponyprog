
/* MODIFIED ATHENA SCROLLBAR (USING ARROWHEADS AT ENDS OF TRAVEL) */
/* Modifications Copyright 1992 by Mitch Trachtenberg             */
/* Rights, permissions, and disclaimer of warranty are as in the  */
/* DEC and MIT notice below.                                      */
/* $XConsortium: Slider3d.c,v 1.72 94/04/17 20:12:40 kaleb Exp $ */

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
   
This new widget is a combination slider and progress bar modified
from the original scrollbar. I'm combining them so that V will
still work with vanilla Athena widgets.

Bruce Wampler - March 1, 1996
******************************************************************/


#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <v/awsld3dp.h>

#include <X11/Xmu/Drawing.h>

/* Private definitions. */

static char defaultTranslations[] =
    "<BtnDown>:   StartScroll(Forward) \n\
     <BtnUp>:      NotifyScroll(Proportional) EndScroll()";

static float floatZero = 0.0;

#define Offset(field) XtOffsetOf(Slider3dRec, field)

static XtResource resources[] = {
  {XtNscrollVCursor, XtCCursor, XtRCursor, sizeof(Cursor),
       Offset(slider3d.verCursor), XtRString, "sb_v_double_arrow"},
  {XtNscrollHCursor, XtCCursor, XtRCursor, sizeof(Cursor),
       Offset(slider3d.horCursor), XtRString, "sb_h_double_arrow"},
  {XtNlength, XtCLength, XtRDimension, sizeof(Dimension),
       Offset(slider3d.length), XtRImmediate, (XtPointer) 1},
  {XtNthickness, XtCThickness, XtRDimension, sizeof(Dimension),
       Offset(slider3d.thickness), XtRImmediate, (XtPointer) 14},
  {XtNorientation, XtCOrientation, XtROrientation, sizeof(XtOrientation),
      Offset(slider3d.orientation), XtRImmediate, (XtPointer) XtorientVertical},
  {XtNscrollProc, XtCCallback, XtRCallback, sizeof(XtPointer),
       Offset(slider3d.scrollProc), XtRCallback, NULL},
  {XtNthumbProc, XtCCallback, XtRCallback, sizeof(XtPointer),
       Offset(slider3d.thumbProc), XtRCallback, NULL},
  {XtNjumpProc, XtCCallback, XtRCallback, sizeof(XtPointer),
       Offset(slider3d.jumpProc), XtRCallback, NULL},
  {XtNthumb, XtCThumb, XtRBitmap, sizeof(Pixmap),
       Offset(slider3d.thumb), XtRImmediate, (XtPointer) XtUnspecifiedPixmap},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
       Offset(slider3d.foreground), XtRString, XtDefaultForeground},
  {XtNslider3dBackground, XtCSlider3dBackground, XtRPixel, sizeof(Pixel),
       Offset(slider3d.background), XtRString, XtDefaultForeground},
  {XtNshown, XtCShown, XtRFloat, sizeof(float),
       Offset(slider3d.shown), XtRFloat, (XtPointer)&floatZero},
  {XtNtopOfThumb, XtCTopOfThumb, XtRFloat, sizeof(float),
       Offset(slider3d.top), XtRFloat, (XtPointer)&floatZero},
  {XtNminimumThumb, XtCMinimumThumb, XtRDimension, sizeof(Dimension),
       Offset(slider3d.min_thumb), XtRImmediate, (XtPointer) 0},
  {XtNprogressVal, XtCProgressVal, XtRDimension, sizeof(Dimension),
       Offset(slider3d.progressVal), XtRImmediate, (XtPointer) 1},
  {XtNisProgress, XtCIsProgress, XtRBoolean, sizeof(Boolean),
       Offset(slider3d.isProgress), XtRImmediate, (XtPointer) False},
  {XtNshowProgressVal, XtCShowProgressVal, XtRBoolean, sizeof(Boolean),
       Offset(slider3d.showProgressVal), XtRImmediate, (XtPointer) True},
  {XtNusePercent, XtCUsePercent, XtRBoolean, sizeof(Boolean),
       Offset(slider3d.usePercent), XtRImmediate, (XtPointer) True},
};
#undef Offset

static void ClassInitialize();
static void Initialize();
static void Destroy();
static void Realize();
static void Resize();
static void Redisplay();
static Boolean SetValues();

static void StartScroll();
static void MoveThumb();
static void NotifyThumb();
static void NotifyScroll();
static void EndScroll();

static XtActionsRec actions[] = {
    {"StartScroll",     StartScroll},
    {"MoveThumb",	MoveThumb},
    {"NotifyThumb",	NotifyThumb},
    {"NotifyScroll",	NotifyScroll},
    {"EndScroll",	EndScroll}
};


Slider3dClassRec slider3dClassRec = {
  { /* core fields */
    /* superclass       */	(WidgetClass) &threeDClassRec,
    /* class_name       */	"Slider3d",
    /* size             */	sizeof(Slider3dRec),
    /* class_initialize	*/	ClassInitialize,
    /* class_part_init  */	NULL,
    /* class_inited	*/	FALSE,
    /* initialize       */	Initialize,
    /* initialize_hook  */	NULL,
    /* realize          */	Realize,
    /* actions          */	actions,
    /* num_actions	*/	XtNumber(actions),
    /* resources        */	resources,
    /* num_resources    */	XtNumber(resources),
    /* xrm_class        */	NULLQUARK,
    /* compress_motion	*/	TRUE,
    /* compress_exposure*/	TRUE,
    /* compress_enterleave*/	TRUE,
    /* visible_interest */	FALSE,
    /* destroy          */	Destroy,
    /* resize           */	Resize,
    /* expose           */	Redisplay,
    /* set_values       */	SetValues,
    /* set_values_hook  */	NULL,
    /* set_values_almost */	XtInheritSetValuesAlmost,
    /* get_values_hook  */	NULL,
    /* accept_focus     */	NULL,
    /* version          */	XtVersion,
    /* callback_private */	NULL,
    /* tm_table         */	defaultTranslations,
    /* query_geometry	*/	XtInheritQueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension        */	NULL
  },
  { /* simple fields */
    /* change_sensitive	*/	XtInheritChangeSensitive
  },
  { /* threeD fields */
    /* shadowdraw	*/	XtInheritXaw3dShadowDraw
  },
  { /* slider3d fields */
    /* ignore		*/	0
  }

};

WidgetClass slider3dWidgetClass = (WidgetClass)&slider3dClassRec;

#define NoButton -1
#define PICKLENGTH(widget, x, y) \
    ((widget->slider3d.orientation == XtorientHorizontal) ? (x) : (y))
#define MIN(x,y)	((x) < (y) ? (x) : (y))
#define MAX(x,y)	((x) > (y) ? (x) : (y))

static void ClassInitialize()
{
    XtAddConverter( XtRString, XtROrientation, XmuCvtStringToOrientation,
		    (XtConvertArgList)NULL, (Cardinal)0 );
}

#define MARGIN(sbw) (sbw)->threeD.shadow_width

/*
   Used to swap X and Y coordinates when the slider3d is horizontal.
 */
static void swap(a, b)
    Dimension *a, *b;
{
    Dimension tmp = *a;
    *a = *b;
    *b = tmp;
}

/*
   Paint the thumb in the area specified by sbw->top and sbw->shown.
   The old area is erased.  No special tricks, draws the entire scollbar
   over every time but doesn't draw the same region twice during a
   single call to PaintThumb so there shouldn't be any flicker.
 */

static void PaintThumb (sbw, pressed)
    Slider3dWidget sbw;
    int pressed;
{
    Dimension margin, tzl;
    Position  floor;
    Position  top, bot;
    Dimension x, y;             	   /* upper-left corner of rectangle */
    Dimension w, h;             	   /* size of rectangle */
    Dimension sw = sbw->threeD.shadow_width;
    Dimension th = sbw->slider3d.thickness;
    XPoint    ipoint[4],opoint[4];	 /* inner and outer points of thumb */
    XPoint    point[4];           	 /* points used for drawing */
    GC        top_shadow_GC, bot_shadow_GC;
    double    thumb_len;

    int lblx, lbly, lblw, lblh;

    margin = MARGIN (sbw);
    tzl = sbw->slider3d.length - 2*margin;
    floor = sbw->slider3d.length - margin;

    top = margin + (int)(tzl * sbw->slider3d.top);
    thumb_len = tzl * sbw->slider3d.shown;
    bot = top + (int)thumb_len;
    if ( thumb_len-(int)thumb_len > 0.5 ) ++bot;

    if (bot < top + (int)sbw->slider3d.min_thumb +
              2 * (int)sbw->threeD.shadow_width) 
	bot = top + sbw->slider3d.min_thumb +
	      2 * sbw->threeD.shadow_width;

    if ( bot >= floor ) {
	top = floor-(bot-top)+1;
	bot = floor;
    }

    sbw->slider3d.topLoc = top;
    sbw->slider3d.shownLength = bot - top;

    top_shadow_GC = sbw->threeD.top_shadow_GC;
    bot_shadow_GC = sbw->threeD.bot_shadow_GC;

    if (XtIsRealized ((Widget) sbw)) {
	/* the space above the thumb */
	x = sw;
	y = margin;
	w = th - sw * 2;
	h = top - y;
	if (sbw->slider3d.orientation == XtorientHorizontal) {
	    swap(&x, &y);
	    swap(&w, &h);
	}
	lblx = x; lbly = y; lblw = w; lblh = tzl;
	XFillRectangle(XtDisplay((Widget) sbw), XtWindow((Widget) sbw),
		       sbw->slider3d.bgc, x, y,
		       (unsigned int) w, (unsigned int) h);
	/* the space below the thumb */
	x = sw;
	y = bot;
	w = th - sw * 2;
	h = tzl + margin - bot;
	if (sbw->slider3d.orientation == XtorientHorizontal) {
	    swap(&x, &y);
	    swap(&w, &h);
	}
	XFillRectangle(XtDisplay((Widget) sbw), XtWindow((Widget) sbw),
		       sbw->slider3d.bgc, x, y,
		       (unsigned int) w, (unsigned int) h);


	/* the thumb itself */
	x = sw * 2;
	y = top + sw;
	w = th - sw * 4;
	h = bot - top - 2 * sw;
	if (sbw->slider3d.orientation == XtorientHorizontal) {
	    swap(&x, &y);
	    swap(&w, &h);
	}
	/* we can't use "w > 0" and "h > 0" because they are
	   usually unsigned quantities */
	if (th - sw * 4 > 0 && bot - top - 2 * sw > 0)
	  {
	    XFillRectangle(XtDisplay((Widget) sbw), XtWindow((Widget) sbw),
			   sbw->slider3d.gc, x, y,
			   (unsigned int) w, (unsigned int) h);
	  }


	/* This code draws the little indent on sliders BEW 3/2/96 */

	if (!sbw->slider3d.isProgress)
	  {
	    if (sbw->slider3d.orientation == XtorientHorizontal)
	      {
		XFillRectangle(XtDisplay((Widget) sbw), XtWindow((Widget) sbw),
		bot_shadow_GC, x+(w/2)-1, y+1, 1, h-2);
		XFillRectangle(XtDisplay((Widget) sbw), XtWindow((Widget) sbw),
		top_shadow_GC, x+(w/2), y+1, 1, h-2);
	      }
	    else
	      {
		XFillRectangle(XtDisplay((Widget) sbw), XtWindow((Widget) sbw),
		bot_shadow_GC, x+1, y+(h/2)-1, w-2, 1);
		XFillRectangle(XtDisplay((Widget) sbw), XtWindow((Widget) sbw),
		top_shadow_GC, x+1, y+(h/2), w-2, 1);
	      }
	  }
#ifdef DRAWPROGRESSVALUES
	/* This is the start of the code needed to draw a text value
	   in a scroll bar, but it just doesn't look good yet. Maybe
	   someday I'll finish this... BEW 3/2/96 */
	else if (sbw->slider3d.showProgressVal)	/* is progress - show it? */
	  {
	    if (sbw->slider3d.orientation == XtorientHorizontal)
	      {
		XDrawImageString(XtDisplay((Widget) sbw), XtWindow((Widget) sbw),
		top_shadow_GC, lblx+(lblh/2)-10, lbly+10, "100%",4);
	      }
	    else
	      {
		XDrawImageString(XtDisplay((Widget) sbw), XtWindow((Widget) sbw),
		top_shadow_GC, lblx, lbly+(lblh/2)+5, "100%",4);
	      }
	  }
#endif

	/* the shades around the thumb

	   o0             o3
	   +--------------+
	   |\ i0      i3 /|
	   | +----------+ |
	   | |          | |
	   | |          | |
	   | |          | |
	   | +----------+ |
	   |/ i1      i2 \|
	   +--------------+
	   o1             o2

	 */

	opoint[0].x = opoint[1].x = sw;
	opoint[0].y = opoint[3].y = top;
	opoint[2].x = opoint[3].x = th - sw;
	opoint[2].y = opoint[1].y = bot;

	ipoint[0].x = ipoint[1].x = opoint[0].x + sw;
	ipoint[0].y = ipoint[3].y = opoint[0].y + sw;
	ipoint[2].x = ipoint[3].x = opoint[2].x - sw;
	ipoint[2].y = ipoint[1].y = opoint[2].y - sw;

	/* make sure shades don't overlap */
	if (ipoint[0].x > ipoint[3].x)
	    ipoint[3].x = ipoint[2].x = ipoint[1].x = ipoint[0].x
		= (ipoint[0].x + ipoint[3].x) / 2;
	if (ipoint[0].y > ipoint[1].y)
	    ipoint[3].y = ipoint[2].y = ipoint[1].y = ipoint[0].y
		= (ipoint[0].y + ipoint[1].y) / 2;
	if (sbw->slider3d.orientation == XtorientHorizontal) {
	    int n;
	    for (n = 0; n < 4; n++)	{
		swap(&ipoint[n].x, &ipoint[n].y);
		swap(&opoint[n].x, &opoint[n].y);
	    }
	}
	/* left */
	point[0] = opoint[0];
	point[1] = opoint[1];
	point[2] = ipoint[1];
	point[3] = ipoint[0];
	if (sbw->slider3d.shown == 0.0)
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
		       sbw->slider3d.bgc, point, 4, Convex, CoordModeOrigin);
	else
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
		      top_shadow_GC, point, 4, Convex, CoordModeOrigin);
	/* top */
	point[0] = opoint[0];
	point[1] = opoint[3];
	point[2] = ipoint[3];
	point[3] = ipoint[0];
	if (sbw->slider3d.shown == 0.0)
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
		       sbw->slider3d.bgc, point, 4, Convex, CoordModeOrigin);
	else
	XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
		      top_shadow_GC, point, 4, Convex, CoordModeOrigin);
	/* bottom */
	point[0] = opoint[1];
	point[1] = opoint[2];
	point[2] = ipoint[2];
	point[3] = ipoint[1];
	if (sbw->slider3d.shown == 0.0)
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
		       sbw->slider3d.bgc, point, 4, Convex, CoordModeOrigin);
	else
	XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
		      bot_shadow_GC, point, 4, Convex, CoordModeOrigin);
	/* right */
	point[0] = opoint[3];
	point[1] = opoint[2];
	point[2] = ipoint[2];
	point[3] = ipoint[3];
	if (sbw->slider3d.shown == 0.0)
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
		       sbw->slider3d.bgc, point, 4, Convex, CoordModeOrigin);
	else
	XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
		      bot_shadow_GC, point, 4, Convex, CoordModeOrigin);
    }
}

/*	Function Name: Destroy
 *	Description: Called as the slider3d is going away...
 *	Arguments: w - the slider3d.
 *	Returns: nonw
 */
static void Destroy (w)
    Widget w;
{
    Slider3dWidget sbw = (Slider3dWidget) w;
    XtReleaseGC (w, sbw->slider3d.gc);
    XtReleaseGC (w, sbw->slider3d.bgc);
}

/*	Function Name: CreateGC
 *	Description: Creates the GC.
 *	Arguments: w - the slider3d widget.
 *	Returns: none. 
 */

static void CreateGC (w)
    Widget w;
{
    Slider3dWidget sbw = (Slider3dWidget) w;
    XGCValues gcValues;
    XtGCMask mask;
    unsigned int depth = 1;

    /* make GC for slider3d background */
    if (sbw->threeD.be_nice_to_cmap ||
	DefaultDepthOfScreen (XtScreen(w)) == 1) {
	mask = GCTile | GCFillStyle;
	gcValues.tile = sbw->threeD.bot_shadow_pxmap;
	gcValues.fill_style = FillTiled;
    } else {
	mask = GCForeground;
	gcValues.foreground = sbw->slider3d.background;
    }
    sbw->slider3d.bgc = XtGetGC(w, mask, &gcValues);

    /* make GC for slider3d foreground */
    if (sbw->slider3d.thumb == XtUnspecifiedPixmap) {
        sbw->slider3d.thumb = XmuCreateStippledPixmap (XtScreen(w), 
					(Pixel) 0, (Pixel) 0, depth);
    } else if (sbw->slider3d.thumb != None) {
	Window root;
	int x, y;
	unsigned int width, height, bw;
	if (XGetGeometry (XtDisplay(w), sbw->slider3d.thumb, &root, &x, &y,
			 &width, &height, &bw, &depth) == 0) {
	    XtAppError (XtWidgetToApplicationContext (w),
	       "Slider3d Widget: Could not get geometry of thumb pixmap.");
	}
    }

    gcValues.foreground = sbw->slider3d.foreground;
    gcValues.background = sbw->core.background_pixel;
    mask = GCForeground | GCBackground;

    if (sbw->slider3d.thumb != None) {
	if (depth == 1) {
	    gcValues.fill_style = FillOpaqueStippled;
	    gcValues.stipple = sbw->slider3d.thumb;
	    mask |= GCFillStyle | GCStipple;
	}
	else {
	    gcValues.fill_style = FillTiled;
	    gcValues.tile = sbw->slider3d.thumb;
	    mask |= GCFillStyle | GCTile;
	}
    }
    /* the creation should be non-caching, because */
    /* we now set and clear clip masks on the gc returned */
    sbw->slider3d.gc = XtGetGC (w, mask, &gcValues);
    gcValues.foreground = sbw->slider3d.foreground;
    gcValues.background = sbw->core.background_pixel;
    mask = GCForeground | GCBackground;
}

static void SetDimensions (sbw)
    Slider3dWidget sbw;
{
    if (sbw->slider3d.orientation == XtorientVertical) {
	sbw->slider3d.length = sbw->core.height;
	sbw->slider3d.thickness = sbw->core.width;
    } else {
	sbw->slider3d.length = sbw->core.width;
	sbw->slider3d.thickness = sbw->core.height;
    }
}

/* ARGSUSED */
static void Initialize( request, new, args, num_args )
    Widget request;		/* what the client asked for */
    Widget new;			/* what we're going to give him */
    ArgList args;
    Cardinal *num_args;
{
    Slider3dWidget sbw = (Slider3dWidget) new;

    CreateGC (new);

    if (sbw->core.width == 0)
	sbw->core.width = (sbw->slider3d.orientation == XtorientVertical)
	    ? sbw->slider3d.thickness : sbw->slider3d.length;

    if (sbw->core.height == 0)
	sbw->core.height = (sbw->slider3d.orientation == XtorientHorizontal)
	    ? sbw->slider3d.thickness : sbw->slider3d.length;

    SetDimensions (sbw);
    sbw->slider3d.topLoc = 0;
    sbw->slider3d.shownLength = sbw->slider3d.min_thumb;
}

static void Realize (w, valueMask, attributes)
    Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    Slider3dWidget sbw = (Slider3dWidget) w;

    sbw->slider3d.inactiveCursor =
      (sbw->slider3d.orientation == XtorientVertical)
	? sbw->slider3d.verCursor
	: sbw->slider3d.horCursor;

    XtVaSetValues (w, XtNcursor, sbw->slider3d.inactiveCursor, NULL);
    /* 
     * The Simple3d widget actually stuffs the value in the valuemask. 
     */
    
    (*slider3dWidgetClass->core_class.superclass->core_class.realize)
	(w, valueMask, attributes);
}

/* ARGSUSED */
static Boolean SetValues (current, request, desired, args, num_args)
    Widget  current,		/* what I am */
	    request,		/* what he wants me to be */
	    desired;		/* what I will become */
    ArgList args;
    Cardinal *num_args;
{
    Slider3dWidget sbw = (Slider3dWidget) current;
    Slider3dWidget dsbw = (Slider3dWidget) desired;
    Boolean redraw = FALSE;

/*
 * If these values are outside the acceptable range ignore them...
 */

    if (dsbw->slider3d.top < 0.0 || dsbw->slider3d.top > 1.0)
        dsbw->slider3d.top = sbw->slider3d.top;

    if (dsbw->slider3d.shown < 0.0 || dsbw->slider3d.shown > 1.0)
        dsbw->slider3d.shown = sbw->slider3d.shown;

/*
 * Change colors and stuff...
 */
    if (XtIsRealized (desired)) {
	if (sbw->slider3d.foreground != dsbw->slider3d.foreground ||
	    sbw->core.background_pixel != dsbw->core.background_pixel ||
	    sbw->slider3d.background != dsbw->slider3d.background) {
	    XtReleaseGC (desired, sbw->slider3d.gc);
	    XtReleaseGC (desired, sbw->slider3d.bgc);
	    CreateGC (desired);
	    redraw = TRUE;
	}
	if (sbw->slider3d.top != dsbw->slider3d.top ||
	    sbw->slider3d.shown != dsbw->slider3d.shown)
	    redraw = TRUE;
    }
    return redraw;
}

static void Resize (w)
    Widget w;
{
    /* ForgetGravity has taken care of background, but thumb may
     * have to move as a result of the new size. */
    SetDimensions ((Slider3dWidget) w);
    Redisplay (w, (XEvent*) NULL, (Region)NULL);
}


/* ARGSUSED */
static void Redisplay (w, event, region)
    Widget w;
    XEvent *event;
    Region region;
{
    Slider3dWidget sbw = (Slider3dWidget) w;
    Slider3dWidgetClass swclass = (Slider3dWidgetClass) XtClass (w);
    int x, y;
    unsigned int width, height;

    (*swclass->threeD_class.shadowdraw) (w, event, region, FALSE);
    x = y = 1;
    width = sbw->core.width - 2;
    height = sbw->core.height - 2;
    if (region == NULL ||
	XRectInRegion (region, x, y, width, height) != RectangleOut) {
	/* Forces entire thumb to be painted. */
	sbw->slider3d.topLoc = -(sbw->slider3d.length + 1);
	PaintThumb (sbw, 0); 
    }
}

static Boolean CompareEvents (oldEvent, newEvent)
    XEvent *oldEvent, *newEvent;
{
#define Check(field) if (newEvent->field != oldEvent->field) return False;

    Check(xany.display);
    Check(xany.type);
    Check(xany.window);

    switch (newEvent->type) {
    case MotionNotify:
	Check(xmotion.state); 
	break;
    case ButtonPress:
    case ButtonRelease:
	Check(xbutton.state);
	Check(xbutton.button); 
	break;
    case KeyPress:
    case KeyRelease:
	Check(xkey.state);
	Check(xkey.keycode); 
	break;
    case EnterNotify:
    case LeaveNotify:
	Check(xcrossing.mode);
	Check(xcrossing.detail);
	Check(xcrossing.state); 
	break;
    }
#undef Check

    return True;
}

struct EventData {
    XEvent *oldEvent;
    int count;
};

static Bool PeekNotifyEvent (dpy, event, args)
    Display *dpy;
    XEvent *event;
    char *args;
{
    struct EventData *eventData = (struct EventData*)args;

    return ((++eventData->count == QLength(dpy)) /* since PeekIf blocks */
	    || CompareEvents(event, eventData->oldEvent));
}


static Boolean LookAhead (w, event)
    Widget w;
    XEvent *event;
{
    XEvent newEvent;
    struct EventData eventData;

    if (QLength (XtDisplay (w)) == 0) return False;

    eventData.count = 0;
    eventData.oldEvent = event;

    XPeekIfEvent (XtDisplay (w), &newEvent, PeekNotifyEvent, (char*)&eventData);

    return CompareEvents (event, &newEvent);
}


static void ExtractPosition (event, x, y)
    XEvent *event;
    Position *x, *y;		/* RETURN */
{
    switch( event->type ) {
    case MotionNotify:
	*x = event->xmotion.x;	 
	*y = event->xmotion.y;	  
	break;
    case ButtonPress:
    case ButtonRelease:
	*x = event->xbutton.x;   
	*y = event->xbutton.y;   
	break;
    case KeyPress:
    case KeyRelease:
	*x = event->xkey.x;      
	*y = event->xkey.y;	  
	break;
    case EnterNotify:
    case LeaveNotify:
	*x = event->xcrossing.x; 
	*y = event->xcrossing.y; 
	break;
    default:
	*x = 0; *y = 0;
    }
}

/* ARGSUSED */
static void StartScroll (w, event, params, num_params )
    Widget w;
    XEvent *event;
    String *params;		/* direction: Back|Forward|Smooth */
    Cardinal *num_params;	/* we only support 1 */
{
    Slider3dWidget sbw = (Slider3dWidget) w;
    Cursor cursor;
    char direction;

    if (sbw->slider3d.direction != 0) return; /* if we're already scrolling */
    if (*num_params > 0) 
	direction = *params[0];
    else
	direction = 'C';

    sbw->slider3d.direction = direction;

    cursor = (sbw->slider3d.orientation == XtorientVertical)
	? sbw->slider3d.verCursor
	: sbw->slider3d.horCursor;

    XtVaSetValues (w, XtNcursor, cursor, NULL);
    XFlush (XtDisplay (w));
}

/*
 * Make sure the first number is within the range specified by the other
 * two numbers.
 */

static int InRange(num, small, big)
    int num, small, big;
{
    return (num < small) ? small : ((num > big) ? big : num);
}

/*
 * Same as above, but for floating numbers. 
 */

static float FloatInRange(num, small, big)
    float num, small, big;
{
    return (num < small) ? small : ((num > big) ? big : num);
}


static void NotifyScroll (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;		/* style: Proportional|FullLength */
    Cardinal *num_params;	/* we only support 1 */
{
    Slider3dWidget sbw = (Slider3dWidget) w;
    int call_data;
    char style;
    Position x, y;

    if (sbw->slider3d.direction == 0) return; /* if no StartScroll */
    if (LookAhead (w, event)) return;
    if (*num_params > 0) 
	style = *params[0];
    else
	style = 'P';

    switch (style) {
    case 'P':    /* Proportional */
    case 'p':
	ExtractPosition (event, &x, &y);
	call_data = 
	    InRange (PICKLENGTH (sbw, x, y), 0, (int) sbw->slider3d.length); 
	break;

    case 'F':    /* FullLength */
    case 'f':    
	call_data = sbw->slider3d.length; 
	break;
    }
    switch (sbw->slider3d.direction) {
    case 'B':
    case 'b':    
	call_data = -call_data;
	/* fall through */

    case 'F':
    case 'f':    
	XtCallCallbacks (w, XtNscrollProc, (XtPointer)call_data);
	break;

    case 'C':
    case 'c':
	/* NotifyThumb has already called the thumbProc(s) */
	break;
    }
}

/* ARGSUSED */
static void EndScroll(w, event, params, num_params )
    Widget w;
    XEvent *event;		/* unused */
    String *params;		/* unused */
    Cardinal *num_params;	/* unused */
{
    Slider3dWidget sbw = (Slider3dWidget) w;

    XtVaSetValues (w, XtNcursor, sbw->slider3d.inactiveCursor, NULL);
    XFlush (XtDisplay (w));
    sbw->slider3d.direction = 0;
}

static float FractionLoc (sbw, x, y)
    Slider3dWidget sbw;
    int x, y;
{
    float   result;
    int margin;
    float   height, width;

    margin = MARGIN (sbw);
    x -= margin;
    y -= margin;
    height = sbw->core.height - 2 * margin;
    width = sbw->core.width - 2 * margin;
    result = PICKLENGTH (sbw, x / width, y / height);
    return FloatInRange(result, 0.0, 1.0);
}


static void MoveThumb (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;		/* unused */
    Cardinal *num_params;	/* unused */
{
    Slider3dWidget sbw = (Slider3dWidget) w;
    Position x, y;
    float loc, t, s;

    if (sbw->slider3d.direction == 0) return; /* if no StartScroll */

    if (LookAhead (w, event)) return;

    if (!event->xmotion.same_screen) return;

    ExtractPosition (event, &x, &y);
    loc = FractionLoc (sbw, x, y);
    t = sbw->slider3d.top;
    s = sbw->slider3d.shown;
    sbw->slider3d.picked = 0.5 * s;
    sbw->slider3d.top = loc - sbw->slider3d.picked;
    if (sbw->slider3d.top < 0.0) sbw->slider3d.top = 0.0;

    /* don't allow slider3d to shrink at end */
    if (sbw->slider3d.top + sbw->slider3d.shown > 1.0)
       sbw->slider3d.top = 1.0 - sbw->slider3d.shown + 0.001;
    PaintThumb (sbw, 1);
    XFlush (XtDisplay (w));	/* re-draw it before Notifying */
}

/* ARGSUSED */
static void NotifyThumb (w, event, params, num_params )
    Widget w;
    XEvent *event;
    String *params;		/* unused */
    Cardinal *num_params;	/* unused */
{
    Slider3dWidget sbw = (Slider3dWidget) w;
    float top = sbw->slider3d.top;

    if (sbw->slider3d.direction == 0) return; /* if no StartScroll */

    if (LookAhead (w, event)) return;

    /* thumbProc is not pretty, but is necessary for backwards
       compatibility on those architectures for which it work{s,ed};
       the intent is to pass a (truncated) float by value. */
    XtCallCallbacks (w, XtNthumbProc, *(XtPointer*)&top);
    XtCallCallbacks (w, XtNjumpProc, (XtPointer)&top);
}



/************************************************************
 *
 *  Public routines. 
 *
 ************************************************************/

/* Set the scroll bar to the given location. */

#if NeedFunctionPrototypes
void XawSlider3dSetThumb (Widget w, 
#if NeedWidePrototypes
			double top, double shown)
#else
			float top, float shown)
#endif
#else
void XawSlider3dSetThumb (w, top, shown)
    Widget w;
    float top, shown;
#endif
{
    Slider3dWidget sbw = (Slider3dWidget) w;

    if (sbw->slider3d.direction == 'c') return; /* if still thumbing */

    sbw->slider3d.top = (top > 1.0) ? 1.0 :
				(top >= 0.0) ? top : sbw->slider3d.top;

    sbw->slider3d.shown = (shown > 1.0) ? 1.0 :
				(shown >= 0.0) ? shown : sbw->slider3d.shown;

    /* don't allow slider3d to shrink at end */
    if (sbw->slider3d.top + sbw->slider3d.shown > 1.0)
	sbw->slider3d.top = 1.0 - sbw->slider3d.shown + 0.001;

    PaintThumb (sbw, 0);
}
