/* MODIFIED ATHENA SCROLLBAR3d (USING ARROWHEADS AT ENDS OF TRAVEL) */
/* Modifications Copyright 1992 by Mitch Trachtenberg             */
/* Rights, permissions, and disclaimer of warranty are as in the  */
/* DEC and MIT notice below.                                      */
/* $XConsortium: Scrollbar3d.c,v 1.72 94/04/17 20:12:40 kaleb Exp $ */

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

/* ScrollBar3d.c */
/* created by weissman, Mon Jul  7 13:20:03 1986 */
/* converted by swick, Thu Aug 27 1987 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <v/awscl3dp.h>

#include <X11/Xmu/Drawing.h>

/* Private definitions. */
#define V_TRANSLATIONS

#ifdef V_TRANSLATIONS
static char defaultTranslations[] =
    "<Btn1Down>:   NotifyScroll()\n\
     <Btn2Down>:   MoveThumb() NotifyThumb() \n\
     <Btn3Down>:   NotifyScroll()\n\
     <Btn1Motion>: MoveThumb() \n\
     <Btn3Motion>: MoveThumb() \n\
     <Btn2Motion>: MoveThumb() \n\
     <BtnUp>:      HandleThumb() EndScroll()";

/*     <BtnUp>:      NotifyThumb() EndScroll()"; */


#else
static char defaultTranslations[] =
    "<Btn1Down>:   NotifyScroll()\n\
     <Btn2Down>:   MoveThumb() NotifyThumb() \n\
     <Btn3Down>:   NotifyScroll()\n\
     <Btn1Motion>: HandleThumb() \n\
     <Btn3Motion>: HandleThumb() \n\
     <Btn2Motion>: MoveThumb() NotifyThumb() \n\
     <BtnUp>:      EndScroll()";
#endif

static float floatZero = 0.0;

#define Offset(field) XtOffsetOf(Scrollbar3dRec, field)


static XtResource resources[] = {
  {XtNscrollVCursor, XtCCursor, XtRCursor, sizeof(Cursor),
       Offset(scrollbar3d.verCursor), XtRString, "sb_v_double_arrow"},
  {XtNscrollHCursor, XtCCursor, XtRCursor, sizeof(Cursor),
       Offset(scrollbar3d.horCursor), XtRString, "sb_h_double_arrow"},
  {XtNlength, XtCLength, XtRDimension, sizeof(Dimension),
       Offset(scrollbar3d.length), XtRImmediate, (XtPointer) 1},
  {XtNthickness, XtCThickness, XtRDimension, sizeof(Dimension),
       Offset(scrollbar3d.thickness), XtRImmediate, (XtPointer) 14},
  {XtNorientation, XtCOrientation, XtROrientation, sizeof(XtOrientation),
      Offset(scrollbar3d.orientation), XtRImmediate, (XtPointer) XtorientVertical},
  {XtNscrollProc, XtCCallback, XtRCallback, sizeof(XtPointer),
       Offset(scrollbar3d.scrollProc), XtRCallback, NULL},
  {XtNthumbProc, XtCCallback, XtRCallback, sizeof(XtPointer),
       Offset(scrollbar3d.thumbProc), XtRCallback, NULL},
  {XtNjumpProc, XtCCallback, XtRCallback, sizeof(XtPointer),
       Offset(scrollbar3d.jumpProc), XtRCallback, NULL},
  {XtNthumb, XtCThumb, XtRBitmap, sizeof(Pixmap),
       Offset(scrollbar3d.thumb), XtRImmediate, (XtPointer) XtUnspecifiedPixmap},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
       Offset(scrollbar3d.foreground), XtRString, XtDefaultForeground},
  {XtNscrollbar3dBackground, XtCScrollbar3dBackground, XtRPixel, sizeof(Pixel),
       Offset(scrollbar3d.background), XtRString, XtDefaultForeground},
  {XtNshown, XtCShown, XtRFloat, sizeof(float),
       Offset(scrollbar3d.shown), XtRFloat, (XtPointer)&floatZero},
  {XtNtopOfThumb, XtCTopOfThumb, XtRFloat, sizeof(float),
       Offset(scrollbar3d.top), XtRFloat, (XtPointer)&floatZero},
  {XtNpickTop, XtCPickTop, XtRBoolean, sizeof(Boolean),
       Offset(scrollbar3d.pick_top), XtRImmediate, (XtPointer) False},
  {XtNminimumThumb, XtCMinimumThumb, XtRDimension, sizeof(Dimension),
       Offset(scrollbar3d.min_thumb), XtRImmediate, (XtPointer) 7},
  {XtNpushThumb, XtCPushThumb, XtRBoolean, sizeof(Boolean),
       Offset(scrollbar3d.push_thumb), XtRImmediate, (XtPointer) True}
};
#undef Offset

static void ClassInitialize();
static void Initialize();
static void Destroy();
static void Realize();
static void Resize();
static void Redisplay();
static Boolean SetValues();

static void HandleThumb();
static void MoveThumb();
static void NotifyThumb();
static void NotifyScroll();
static void EndScroll();

static XtActionsRec actions[] = {
    {"HandleThumb",	HandleThumb},
    {"MoveThumb",	MoveThumb},
    {"NotifyThumb",	NotifyThumb},
    {"NotifyScroll",	NotifyScroll},
    {"EndScroll",	EndScroll}
};


Scrollbar3dClassRec scrollbar3dClassRec = {
  { /* core fields */
    /* superclass       */	(WidgetClass) &threeDClassRec,
    /* class_name       */	"Scrollbar3d",
    /* size             */	sizeof(Scrollbar3dRec),
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
  { /* scrollbar3d fields */
    /* ignore		*/	0
  }

};

WidgetClass scrollbar3dWidgetClass = (WidgetClass)&scrollbar3dClassRec;

#define NoButton -1
#define PICKLENGTH(widget, x, y) \
    ((widget->scrollbar3d.orientation == XtorientHorizontal) ? (x) : (y))
#define MIN(x,y)	((x) < (y) ? (x) : (y))
#define MAX(x,y)	((x) > (y) ? (x) : (y))

static void ClassInitialize()
{
    XtAddConverter( XtRString, XtROrientation, XmuCvtStringToOrientation,
		    (XtConvertArgList)NULL, (Cardinal)0 );
}

#define MARGIN(sbw) (sbw)->scrollbar3d.thickness

/*
   Used to swap X and Y coordinates when the scrollbar3d is horizontal.
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
   Draws the thumb "pressed in" when pressed = 1.
 */

static void PaintThumb (sbw, pressed)
    Scrollbar3dWidget sbw;
    int pressed;
{
    Dimension margin, tzl;
    Position  floor;
    Position  top, bot;
    Dimension x, y;             	   /* upper-left corner of rectangle */
    Dimension w, h;             	   /* size of rectangle */
    Dimension sw = sbw->threeD.shadow_width;
    Dimension th = sbw->scrollbar3d.thickness;
    XPoint    ipoint[4],opoint[4];	 /* inner and outer points of thumb */
    XPoint    point[4];           	 /* points used for drawing */
    GC        top_shadow_GC, bot_shadow_GC;
    double    thumb_len;

    margin = MARGIN (sbw);
    tzl = sbw->scrollbar3d.length - 2*margin;
    floor = sbw->scrollbar3d.length - margin;

    top = margin + (int)(tzl * sbw->scrollbar3d.top);
    thumb_len = tzl * sbw->scrollbar3d.shown;
    bot = top + (int)thumb_len;
    if ( thumb_len-(int)thumb_len > 0.5 ) ++bot;

    if (bot < top + (int)sbw->scrollbar3d.min_thumb +
              2 * (int)sbw->threeD.shadow_width) 
	bot = top + sbw->scrollbar3d.min_thumb +
	      2 * sbw->threeD.shadow_width;

    if ( bot >= floor ) {
	top = floor-(bot-top)+1;
	bot = floor;
    }

    sbw->scrollbar3d.topLoc = top;
    sbw->scrollbar3d.shownLength = bot - top;

/*
// This pressed in look looks like crap, and doesn't
// get fixed, so take it out. BEW
//    if (pressed && sbw->scrollbar3d.push_thumb) {
//	top_shadow_GC = sbw->threeD.bot_shadow_GC;
//	bot_shadow_GC = sbw->threeD.top_shadow_GC;
//    }
//    else {
*/
	top_shadow_GC = sbw->threeD.top_shadow_GC;
	bot_shadow_GC = sbw->threeD.bot_shadow_GC;
/*
//    }
*/

    if (XtIsRealized ((Widget) sbw)) {
	/* the space above the thumb */
	x = sw;
	y = margin;
	w = th - sw * 2;
	h = top - y;
	if (sbw->scrollbar3d.orientation == XtorientHorizontal) {
	    swap(&x, &y);
	    swap(&w, &h);
	}
	XFillRectangle(XtDisplay((Widget) sbw), XtWindow((Widget) sbw),
		       sbw->scrollbar3d.bgc, x, y,
		       (unsigned int) w, (unsigned int) h);
	/* the space below the thumb */
	x = sw;
	y = bot;
	w = th - sw * 2;
	h = tzl + margin - bot;
	if (sbw->scrollbar3d.orientation == XtorientHorizontal) {
	    swap(&x, &y);
	    swap(&w, &h);
	}
	XFillRectangle(XtDisplay((Widget) sbw), XtWindow((Widget) sbw),
		       sbw->scrollbar3d.bgc, x, y,
		       (unsigned int) w, (unsigned int) h);
	/* the thumb itself */
	x = sw * 2;
	y = top + sw;
	w = th - sw * 4;
	h = bot - top - 2 * sw;
	if (sbw->scrollbar3d.orientation == XtorientHorizontal) {
	    swap(&x, &y);
	    swap(&w, &h);
	}
	/* we can't use "w > 0" and "h > 0" because they are
	   usually unsigned quantities */
	if (th - sw * 4 > 0 && bot - top - 2 * sw > 0)
	    XFillRectangle(XtDisplay((Widget) sbw), XtWindow((Widget) sbw),
			   sbw->scrollbar3d.gc, x, y,
			   (unsigned int) w, (unsigned int) h);
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
	if (sbw->scrollbar3d.orientation == XtorientHorizontal) {
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
	XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
		      top_shadow_GC, point, 4, Convex, CoordModeOrigin);
	/* top */
	point[0] = opoint[0];
	point[1] = opoint[3];
	point[2] = ipoint[3];
	point[3] = ipoint[0];
	XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
		      top_shadow_GC, point, 4, Convex, CoordModeOrigin);
	/* bottom */
	point[0] = opoint[1];
	point[1] = opoint[2];
	point[2] = ipoint[2];
	point[3] = ipoint[1];
	XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
		      bot_shadow_GC, point, 4, Convex, CoordModeOrigin);
	/* right */
	point[0] = opoint[3];
	point[1] = opoint[2];
	point[2] = ipoint[2];
	point[3] = ipoint[3];
	XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
		      bot_shadow_GC, point, 4, Convex, CoordModeOrigin);
    }
}

/*
   Draw the arrows for scrollbar3d.
   Draw the corresponding arrow "pressed in" if
   toppressed or botpressed is 1.
 */
static void PaintArrows (sbw, toppressed, botpressed)
    Scrollbar3dWidget sbw;
    int toppressed, botpressed;
{
    XPoint	ipoint[6], opoint[6];	/* inner and outer points */
    XPoint	rpoint[4];		/* the rectangle around arrows */
    XPoint	tpoint[6];		/* temporary for args to XFillPolygon */
    Dimension	sw = sbw->threeD.shadow_width;
    Dimension	th = sbw->scrollbar3d.thickness;
    Dimension	len = sbw->scrollbar3d.length;
    GC		top_shadow_GC, bot_shadow_GC;

    if (XtIsRealized ((Widget) sbw)) {
	/* 3D arrows?
         */
	if (sw) {
	    /*
	       The points are numbered like this:

	       r0        r3
	       +---------+
	       |    ^ o2 |
	       |   /|\   |   a = i0
	       |  /c^ \  |   b = i1
	       | / / \ \ |   c = i2
	       |/a<--->b\|
	    o0 +---------+ o1
               |         |
               |         |
	    o3 +---------+ o4
	       |\d<--->e/|
	       | \ \ / / |   d = i3
	       |  \fv /  |   e = i4
	       |   \|/   |   f = i5
	       |    v o5 |
	       +---------+
	       r1        r2

	     */

	    rpoint[0].x = rpoint[1].x = opoint[0].x = opoint[3].x = sw;
	    ipoint[0].x = ipoint[3].x = sw * 2.5;
	    opoint[2].x = opoint[5].x = ipoint[2].x = ipoint[5].x = th / 2;
	    ipoint[1].x = ipoint[4].x = th - (int)(sw * 2.5);
	    rpoint[2].x = rpoint[3].x = opoint[1].x = opoint[4].x = th - sw;
	
	    rpoint[0].y = rpoint[3].y = opoint[2].y = sw;
	    ipoint[2].y = sw * 3.2;
	    ipoint[0].y = ipoint[1].y = th - sw;
	    opoint[0].y = opoint[1].y = th;
	    opoint[3].y = opoint[4].y = len - th;
	    ipoint[3].y = ipoint[4].y = len - th + sw;
	    ipoint[5].y = len - (int)(sw * 3.2);
	    rpoint[1].y = rpoint[2].y = opoint[5].y = len - sw;

	    /* some ugly kludges to make them look right */
	    opoint[2].y--;
	    opoint[0].x--;
	    ipoint[2].y--;
	    ipoint[0].x--;

	    /* make sure shades don't overlap */
	    if (ipoint[0].x > ipoint[1].x)
	        ipoint[4].x = ipoint[3].x = ipoint[1].x = ipoint[0].x
		    = (ipoint[0].x + ipoint[1].x) / 2;
	    if (ipoint[0].y < ipoint[2].y)	{
	        ipoint[2].y = ipoint[1].y = ipoint[0].y
		    = (ipoint[0].y + ipoint[2].y) / 2;
	        ipoint[5].y = ipoint[4].y = ipoint[3].y
		    = (ipoint[3].y + ipoint[5].y) / 2;
	    }

	    /* horizontal arrows require that x and y coordinates be swapped */
	    if (sbw->scrollbar3d.orientation == XtorientHorizontal) {
		int n;
 	    	for (n = 0; n < 6; n++) {
		    swap(&ipoint[n].x, &ipoint[n].y);
		    swap(&opoint[n].x, &opoint[n].y);
		}
		for (n = 0; n < 4; n++)
		swap(&rpoint[n].x, &rpoint[n].y);
	    }
 	    /* draw the up/left arrow */
	    if (toppressed) {
		top_shadow_GC = sbw->threeD.bot_shadow_GC;
		bot_shadow_GC = sbw->threeD.top_shadow_GC;
	    }
	    else {
		top_shadow_GC = sbw->threeD.top_shadow_GC;
		bot_shadow_GC = sbw->threeD.bot_shadow_GC;
	    }
	    /* top-left background */
	    tpoint[0] = rpoint[0];
	    tpoint[1] = opoint[0];
	    tpoint[2] = opoint[2];
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
			  sbw->scrollbar3d.bgc, 
			  tpoint, 3, 
			  Convex, CoordModeOrigin);
	    /* top-right background */
	    tpoint[0] = rpoint[3];
	    tpoint[1] = opoint[2];
	    tpoint[2] = opoint[1];
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
			  sbw->scrollbar3d.bgc, 
			  tpoint, 3, 
			  Convex, CoordModeOrigin);
	    /* the right shade */
	    tpoint[0] = opoint[1];
	    tpoint[1] = opoint[2];
	    tpoint[2] = ipoint[2];
	    tpoint[3] = ipoint[1];
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
			  bot_shadow_GC, 
			  tpoint, 4, 
			  Convex, CoordModeOrigin);
	    /* the left shade */
	    tpoint[0] = opoint[2];
	    tpoint[1] = opoint[0];
	    tpoint[2] = ipoint[0];
	    tpoint[3] = ipoint[2];
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
			  top_shadow_GC, 
			  tpoint, 4, 
			  Convex, CoordModeOrigin);
	    /* the bottom shade */
	    tpoint[0] = opoint[0];
	    tpoint[1] = opoint[1];
	    tpoint[2] = ipoint[1];
	    tpoint[3] = ipoint[0];
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
			  bot_shadow_GC, 
			  tpoint, 4, 
			  Convex, CoordModeOrigin);
	    /* the arrow itself */
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
			  sbw->scrollbar3d.gc, 
			  ipoint, 3, 
			  Convex, CoordModeOrigin);

 	    /* draw the down/right arrow */
	    if (botpressed) {
		top_shadow_GC = sbw->threeD.bot_shadow_GC;
		bot_shadow_GC = sbw->threeD.top_shadow_GC;
	    }
	    else {
		top_shadow_GC = sbw->threeD.top_shadow_GC;
		bot_shadow_GC = sbw->threeD.bot_shadow_GC;
	    }
	    /* bottom-left background */
	    tpoint[0] = rpoint[1];
	    tpoint[1] = opoint[5];
	    tpoint[2] = opoint[3];
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
			  sbw->scrollbar3d.bgc, 
			  tpoint, 3, 
			  Convex, CoordModeOrigin);
	    /* bottom-right background */
	    tpoint[0] = rpoint[2];
	    tpoint[1] = opoint[4];
	    tpoint[2] = opoint[5];
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
			  sbw->scrollbar3d.bgc, 
			  tpoint, 3, 
			  Convex, CoordModeOrigin);
	    /* the left shade */
	    tpoint[0] = opoint[3];
	    tpoint[1] = opoint[5];
	    tpoint[2] = ipoint[5];
	    tpoint[3] = ipoint[3];
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
			  top_shadow_GC, 
			  tpoint, 4, 
			  Convex, CoordModeOrigin);
	    /* the right shade */
	    tpoint[0] = opoint[5];
	    tpoint[1] = opoint[4];
	    tpoint[2] = ipoint[4];
	    tpoint[3] = ipoint[5];
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
			  bot_shadow_GC, 
			  tpoint, 4, 
			  Convex, CoordModeOrigin);
	    /* the top shade */
	    tpoint[0] = opoint[4];
	    tpoint[1] = opoint[3];
	    tpoint[2] = ipoint[3];
	    tpoint[3] = ipoint[4];
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
			  top_shadow_GC, 
			  tpoint, 4, 
			  Convex, CoordModeOrigin);
	    /* the arrow itself */
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
			  sbw->scrollbar3d.gc,
			  ipoint+3, 3, 
			  Convex, CoordModeOrigin);
	} else {
	    tpoint[0] = opoint[0];
	    tpoint[1] = opoint[1];
	    tpoint[2] = opoint[2];
	    tpoint[3] = opoint[3];
	    tpoint[4] = opoint[4];
	    tpoint[5] = opoint[5];

	    /* horizontal arrows require that x and y coordinates be swapped */
	    if (sbw->scrollbar3d.orientation == XtorientHorizontal) {
		int n;
 	    	for (n = 0; n < 6; n++)
		    swap(&tpoint[n].x, &tpoint[n].y);
	    }
	    /* draw the up/left arrow */
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
	    			  sbw->scrollbar3d.gc, 
					  tpoint, 3, 
					  Convex, CoordModeOrigin);
	    /* draw the down/right arrow */
	    XFillPolygon (XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw), 
					  sbw->scrollbar3d.gc, 
					  tpoint+3, 3, 
					  Convex, CoordModeOrigin);
	}
    }
}

/*	Function Name: Destroy
 *	Description: Called as the scrollbar3d is going away...
 *	Arguments: w - the scrollbar3d.
 *	Returns: nonw
 */
static void Destroy (w)
    Widget w;
{
    Scrollbar3dWidget sbw = (Scrollbar3dWidget) w;
    if(sbw->scrollbar3d.timer_id != (XtIntervalId) 0)
	XtRemoveTimeOut (sbw->scrollbar3d.timer_id);
    XtReleaseGC (w, sbw->scrollbar3d.gc);
    XtReleaseGC (w, sbw->scrollbar3d.bgc);
}

/*	Function Name: CreateGC
 *	Description: Creates the GC.
 *	Arguments: w - the scrollbar3d widget.
 *	Returns: none. 
 */

static void CreateGC (w)
    Widget w;
{
    Scrollbar3dWidget sbw = (Scrollbar3dWidget) w;
    XGCValues gcValues;
    XtGCMask mask;
    unsigned int depth = 1;

    /* make GC for scrollbar3d background */
    if (sbw->threeD.be_nice_to_cmap ||
	DefaultDepthOfScreen (XtScreen(w)) == 1) {
	mask = GCTile | GCFillStyle;
	gcValues.tile = sbw->threeD.bot_shadow_pxmap;
	gcValues.fill_style = FillTiled;
    } else {
	mask = GCForeground;
	gcValues.foreground = sbw->scrollbar3d.background;
    }
    sbw->scrollbar3d.bgc = XtGetGC(w, mask, &gcValues);

    /* make GC for scrollbar3d foreground */
    if (sbw->scrollbar3d.thumb == XtUnspecifiedPixmap) {
        sbw->scrollbar3d.thumb = XmuCreateStippledPixmap (XtScreen(w), 
					(Pixel) 0, (Pixel) 0, depth);
    } else if (sbw->scrollbar3d.thumb != None) {
	Window root;
	int x, y;
	unsigned int width, height, bw;
	if (XGetGeometry (XtDisplay(w), sbw->scrollbar3d.thumb, &root, &x, &y,
			 &width, &height, &bw, &depth) == 0) {
	    XtAppError (XtWidgetToApplicationContext (w),
	       "Scrollbar3d Widget: Could not get geometry of thumb pixmap.");
	}
    }

    gcValues.foreground = sbw->scrollbar3d.foreground;
    gcValues.background = sbw->core.background_pixel;
    mask = GCForeground | GCBackground;

    if (sbw->scrollbar3d.thumb != None) {
	if (depth == 1) {
	    gcValues.fill_style = FillOpaqueStippled;
	    gcValues.stipple = sbw->scrollbar3d.thumb;
	    mask |= GCFillStyle | GCStipple;
	}
	else {
	    gcValues.fill_style = FillTiled;
	    gcValues.tile = sbw->scrollbar3d.thumb;
	    mask |= GCFillStyle | GCTile;
	}
    }
    /* the creation should be non-caching, because */
    /* we now set and clear clip masks on the gc returned */
    sbw->scrollbar3d.gc = XtGetGC (w, mask, &gcValues);
    gcValues.foreground = sbw->scrollbar3d.foreground;
    gcValues.background = sbw->core.background_pixel;
    mask = GCForeground | GCBackground;
}

static void SetDimensions (sbw)
    Scrollbar3dWidget sbw;
{
    if (sbw->scrollbar3d.orientation == XtorientVertical) {
	sbw->scrollbar3d.length = sbw->core.height;
	sbw->scrollbar3d.thickness = sbw->core.width;
    } else {
	sbw->scrollbar3d.length = sbw->core.width;
	sbw->scrollbar3d.thickness = sbw->core.height;
    }
}

/* ARGSUSED */
static void Initialize( request, new, args, num_args )
    Widget request;		/* what the client asked for */
    Widget new;			/* what we're going to give him */
    ArgList args;
    Cardinal *num_args;
{
    Scrollbar3dWidget sbw = (Scrollbar3dWidget) new;

    CreateGC (new);

    if (sbw->core.width == 0)
	sbw->core.width = (sbw->scrollbar3d.orientation == XtorientVertical)
	    ? sbw->scrollbar3d.thickness : sbw->scrollbar3d.length;

    if (sbw->core.height == 0)
	sbw->core.height = (sbw->scrollbar3d.orientation == XtorientHorizontal)
	    ? sbw->scrollbar3d.thickness : sbw->scrollbar3d.length;

    SetDimensions (sbw);
    sbw->scrollbar3d.scroll_mode = 0;
    sbw->scrollbar3d.timer_id = (XtIntervalId)0;
    sbw->scrollbar3d.topLoc = 0;
    sbw->scrollbar3d.shownLength = sbw->scrollbar3d.min_thumb;
}

static void Realize (w, valueMask, attributes)
    Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    Scrollbar3dWidget sbw = (Scrollbar3dWidget) w;
    sbw->scrollbar3d.inactiveCursor =
      (sbw->scrollbar3d.orientation == XtorientVertical)
	? sbw->scrollbar3d.verCursor
	: sbw->scrollbar3d.horCursor;

    XtVaSetValues (w, XtNcursor, sbw->scrollbar3d.inactiveCursor, NULL);
    /* 
     * The Simple3d widget actually stuffs the value in the valuemask. 
     */
    
    (*scrollbar3dWidgetClass->core_class.superclass->core_class.realize)
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
    Scrollbar3dWidget sbw = (Scrollbar3dWidget) current;
    Scrollbar3dWidget dsbw = (Scrollbar3dWidget) desired;
    Boolean redraw = FALSE;

/*
 * If these values are outside the acceptable range ignore them...
 */

    if (dsbw->scrollbar3d.top < 0.0 || dsbw->scrollbar3d.top > 1.0)
        dsbw->scrollbar3d.top = sbw->scrollbar3d.top;

    if (dsbw->scrollbar3d.shown < 0.0 || dsbw->scrollbar3d.shown > 1.0)
        dsbw->scrollbar3d.shown = sbw->scrollbar3d.shown;

/*
 * Change colors and stuff...
 */
    if (XtIsRealized (desired)) {
	if (sbw->scrollbar3d.foreground != dsbw->scrollbar3d.foreground ||
	    sbw->core.background_pixel != dsbw->core.background_pixel ||
	    sbw->scrollbar3d.background != dsbw->scrollbar3d.background) {
	    XtReleaseGC (desired, sbw->scrollbar3d.gc);
	    XtReleaseGC (desired, sbw->scrollbar3d.bgc);
	    CreateGC (desired);
	    redraw = TRUE;
	}
	if (sbw->scrollbar3d.top != dsbw->scrollbar3d.top ||
	    sbw->scrollbar3d.shown != dsbw->scrollbar3d.shown)
	    redraw = TRUE;
    }
    return redraw;
}

static void Resize (w)
    Widget w;
{
    /* ForgetGravity has taken care of background, but thumb may
     * have to move as a result of the new size. */
    SetDimensions ((Scrollbar3dWidget) w);
    Redisplay (w, (XEvent*) NULL, (Region)NULL);
}


/* ARGSUSED */
static void Redisplay (w, event, region)
    Widget w;
    XEvent *event;
    Region region;
{
    Scrollbar3dWidget sbw = (Scrollbar3dWidget) w;
    Scrollbar3dWidgetClass swclass = (Scrollbar3dWidgetClass) XtClass (w);
    int x, y;
    unsigned int width, height;

    (*swclass->threeD_class.shadowdraw) (w, event, region, FALSE);
    x = y = 1;
    width = sbw->core.width - 2;
    height = sbw->core.height - 2;
    if (region == NULL ||
	XRectInRegion (region, x, y, width, height) != RectangleOut) {
	/* Forces entire thumb to be painted. */
	sbw->scrollbar3d.topLoc = -(sbw->scrollbar3d.length + 1);
	PaintThumb (sbw, 0); 
    }
    /* we'd like to be region aware here!!!! */
    PaintArrows (sbw, 0, 0);
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
static void HandleThumb (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;		/* unused */
    Cardinal *num_params;	/* unused */
{
    Position x,y;
    Scrollbar3dWidget sbw = (Scrollbar3dWidget) w;

    ExtractPosition( event, &x, &y );
    /* if the motion event puts the pointer in thumb, call Move and Notify */
    /* also call Move and Notify if we're already in continuous scroll mode */
    if (sbw->scrollbar3d.scroll_mode == 2 ||
	(PICKLENGTH (sbw,x,y) >= sbw->scrollbar3d.topLoc &&
	 PICKLENGTH (sbw,x,y) <= (sbw->scrollbar3d.topLoc +
				  sbw->scrollbar3d.shownLength))){
	XtCallActionProc(w, "MoveThumb", event, params, *num_params);
	XtCallActionProc(w, "NotifyThumb", event, params, *num_params);
    }
}

static void RepeatNotify (client_data, idp)
    XtPointer client_data;
    XtIntervalId *idp;
{
#define A_FEW_PIXELS 5
    Scrollbar3dWidget sbw = (Scrollbar3dWidget) client_data;
    int call_data;

    if (sbw->scrollbar3d.scroll_mode != 1 && sbw->scrollbar3d.scroll_mode != 3) {
	sbw->scrollbar3d.timer_id = (XtIntervalId) 0;
	return;
    }
    call_data = MAX (A_FEW_PIXELS, sbw->scrollbar3d.length / 20);
    if (sbw->scrollbar3d.scroll_mode == 1)
	call_data = -call_data;
    XtCallCallbacks((Widget)sbw, XtNscrollProc, (XtPointer) call_data);
    sbw->scrollbar3d.timer_id = 
    XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)sbw),
		    (unsigned long) 150,
		    RepeatNotify,
		    client_data);
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
    String *params;		
    Cardinal *num_params;	
{
    Scrollbar3dWidget sbw = (Scrollbar3dWidget) w;
    int call_data;
    Position x, y;

    if (sbw->scrollbar3d.scroll_mode == 2) return; /* if scroll continuous */

    if (LookAhead (w, event)) return;

    ExtractPosition (event, &x, &y);

    if (PICKLENGTH (sbw,x,y) < sbw->scrollbar3d.thickness) {
	/* handle first arrow zone */
	call_data = MAX (A_FEW_PIXELS, sbw->scrollbar3d.length / 20);
	XtCallCallbacks (w, XtNscrollProc, (XtPointer)(-call_data));
	/* establish autoscroll */
	sbw->scrollbar3d.timer_id = 
	    XtAppAddTimeOut (XtWidgetToApplicationContext (w),
			     (unsigned long) 300, RepeatNotify, (XtPointer)w);
	sbw->scrollbar3d.scroll_mode = 1;
	PaintArrows (sbw, 1, 0);
	return;
    } else if (PICKLENGTH (sbw,x,y) > sbw->scrollbar3d.length -
				      sbw->scrollbar3d.thickness) {
	/* handle last arrow zone */
	call_data = MAX (A_FEW_PIXELS, sbw->scrollbar3d.length / 20);
	XtCallCallbacks (w, XtNscrollProc, (XtPointer)(call_data));
	/* establish autoscroll */
	sbw->scrollbar3d.timer_id =
	    XtAppAddTimeOut (XtWidgetToApplicationContext (w), 
			     (unsigned long) 300, RepeatNotify, (XtPointer)w);
	sbw->scrollbar3d.scroll_mode = 3;
	PaintArrows (sbw, 0, 1);
	return;
    }
    else if (PICKLENGTH (sbw, x, y) < sbw->scrollbar3d.topLoc) {
	/* handle zone "above" the thumb */
	call_data = -(sbw->scrollbar3d.length);
/*	XtCallCallbacks (w, XtNscrollProc, (XtPointer)(call_data)) */
	return;
    } else if (PICKLENGTH (sbw, x, y) > sbw->scrollbar3d.topLoc +
					sbw->scrollbar3d.shownLength) {
	/* handle zone "below" the thumb */
	call_data = sbw->scrollbar3d.length;
/* 	XtCallCallbacks (w, XtNscrollProc, (XtPointer)(call_data)) */
	return;
    } else {
	/* handle the thumb in the motion notify action */
	/* but we need to re-paint it "pressed in" here */
	PaintThumb (sbw, 1);
	return;
    }
}

/* ARGSUSED */
static void EndScroll(w, event, params, num_params )
    Widget w;
    XEvent *event;		/* unused */
    String *params;		/* unused */
    Cardinal *num_params;	/* unused */
{
    Scrollbar3dWidget sbw = (Scrollbar3dWidget) w;

    sbw->scrollbar3d.scroll_mode = 0;
    /* no need to remove any autoscroll timeout; it will no-op */
    /* because the scroll_mode is 0 */
    /* but be sure to remove timeout in destroy proc */
    /* release all buttons */
    PaintArrows (sbw, 0, 0);
    PaintThumb (sbw, 0);
}

static float FractionLoc (sbw, x, y)
    Scrollbar3dWidget sbw;
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
    Scrollbar3dWidget sbw = (Scrollbar3dWidget) w;
    Position x, y;
    float loc, t, s;

    if (LookAhead (w, event)) return;

    if (!event->xmotion.same_screen) return;

    ExtractPosition (event, &x, &y);
    loc = FractionLoc (sbw, x, y);

    /* This mod prevents motions in the arrows from doing anything */
    if (PICKLENGTH (sbw,x,y) < sbw->scrollbar3d.thickness) {
	/* handle first arrow zone */
	return;
    } else if (PICKLENGTH (sbw,x,y) > sbw->scrollbar3d.length -
				      sbw->scrollbar3d.thickness) {
	/* handle last arrow zone */
	return;
    }

    t = sbw->scrollbar3d.top;
    s = sbw->scrollbar3d.shown;
    if (sbw->scrollbar3d.scroll_mode != 2 )
      /* initialize picked position */
      sbw->scrollbar3d.picked = (FloatInRange(loc, t, t+s) - t);
    if (sbw->scrollbar3d.pick_top)
      sbw->scrollbar3d.top = loc;
    else {
      sbw->scrollbar3d.top = loc - sbw->scrollbar3d.picked;
      if (sbw->scrollbar3d.top < 0.0) sbw->scrollbar3d.top = 0.0;
    }
    /* don't allow scrollbar3d to shrink at end */
    if (sbw->scrollbar3d.top + sbw->scrollbar3d.shown > 1.0)
       sbw->scrollbar3d.top = 1.0 - sbw->scrollbar3d.shown + 0.001;
    sbw->scrollbar3d.scroll_mode = 2; /* indicate continuous scroll */
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
    Scrollbar3dWidget sbw = (Scrollbar3dWidget) w;
    float top = sbw->scrollbar3d.top;


    if (LookAhead (w, event)) return;

    /* thumbProc is not pretty, but is necessary for backwards
       compatibility on those architectures for which it work{s,ed};
       the intent is to pass a (truncated) float by value. */
    /* This corrects for rounding errors: If the thumb is moved to the end of
       the scrollable area sometimes the last line/column is not displayed.
       This can happen when the integer number of the top line or leftmost
       column to be be displayed is calculated from the float value
       sbw->scrollbar3d.top. The numerical error of this rounding problem is
       very small. We therefore add a small value which then forces the
       next line/column (the correct one) to be used. Since we can expect
       that the resolution of display screens will not be higher then
       10000 text lines/columns we add 1/10000 to the top position. The
       intermediate variable `top' is used to avoid erroneous summing up
       corrections (can this happen at all?). If the arrows are not displayed
       there is no problem since in this case there is always a constant
       integer number of pixels the thumb must be moved in order to scroll
       to the next line/column. */
    top += 0.0001;
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
void XawScrollbar3dSetThumb (Widget w, 
#if NeedWidePrototypes
			double top, double shown)
#else
			float top, float shown)
#endif
#else
void XawScrollbar3dSetThumb (w, top, shown)
    Widget w;
    float top, shown;
#endif
{
    Scrollbar3dWidget sbw = (Scrollbar3dWidget) w;

    if (sbw->scrollbar3d.scroll_mode == (char) 2) return; /* if still thumbing */

    sbw->scrollbar3d.top = (top > 1.0) ? 1.0 :
				(top >= 0.0) ? top : sbw->scrollbar3d.top;

    sbw->scrollbar3d.shown = (shown > 1.0) ? 1.0 :
				(shown >= 0.0) ? shown : sbw->scrollbar3d.shown;

    /* don't allow scrollbar3d to shrink at end */
    if (sbw->scrollbar3d.top + sbw->scrollbar3d.shown > 1.0)
	sbw->scrollbar3d.top = 1.0 - sbw->scrollbar3d.shown + 0.001;

    PaintThumb (sbw, 0);
}
