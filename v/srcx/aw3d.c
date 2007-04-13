/*
* $KK: ThreeD.c,v 0.3 92/11/04 xx:xx:xx keithley Exp $ 
*/

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.
Copyright 1992 by Kaleb Keithley

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital, MIT, or Kaleb 
Keithley not be used in advertising or publicity pertaining to distribution 
of the software without specific, written prior permission.  

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
I hate to do this, but I included the contents of <v/vctlclrs.h>
directly because of C++ vs C problems.
   
Bruce Wampler - March 1, 1996
******************************************************************/


#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include <v/aw3dp.h>
#include <X11/Xosdefs.h>

#define VSHADOWS
/*
#ifdef VSHADOWS
#include <v/vctlclrs.h>
#endif
*/
  extern Pixel _vStatusBarBG;	/* Background of status bar */
  extern Pixel _vDialogBG;       /* Background for dialogs/command bar */
  extern Pixel _vMenuBarBG;      /* Background for MenuBar */
  extern Pixel _vControlBG;	/* Background for Controls */
  extern Pixel _vControlFace;	/* Face of controls */
  extern Pixel _vLightControlShadow; /* light control shadow */
  extern Pixel _vDarkControlShadow;  /* dark control shadow */

/* Initialization of defaults */

#define XtNtopShadowPixmap "topShadowPixmap"
#define XtCTopShadowPixmap "TopShadowPixmap"
#define XtNbottomShadowPixmap "bottomShadowPixmap"
#define XtCBottomShadowPixmap "BottomShadowPixmap"

#define offset(field) XtOffsetOf(ThreeDRec, field)

static XtResource resources[] = {
    {XtNshadowWidth, XtCShadowWidth, XtRDimension, sizeof(Dimension),
	offset(threeD.shadow_width), XtRImmediate, (XtPointer) 2},
    {XtNtopShadowPixel, XtCTopShadowPixel, XtRPixel, sizeof(Pixel),
	offset(threeD.top_shadow_pixel), XtRString, XtDefaultForeground},
    {XtNbottomShadowPixel, XtCBottomShadowPixel, XtRPixel, sizeof(Pixel),
	offset(threeD.bot_shadow_pixel), XtRString, XtDefaultForeground},
    {XtNtopShadowPixmap, XtCTopShadowPixmap, XtRPixmap, sizeof(Pixmap),
	offset(threeD.top_shadow_pxmap), XtRImmediate, (XtPointer) NULL},
    {XtNbottomShadowPixmap, XtCBottomShadowPixmap, XtRPixmap, sizeof(Pixmap),
	offset(threeD.bot_shadow_pxmap), XtRImmediate, (XtPointer) NULL},
    {XtNtopShadowContrast, XtCTopShadowContrast, XtRInt, sizeof(int),
	offset(threeD.top_shadow_contrast), XtRImmediate, (XtPointer) 20},
    {XtNbottomShadowContrast, XtCBottomShadowContrast, XtRInt, sizeof(int),
	offset(threeD.bot_shadow_contrast), XtRImmediate, (XtPointer) 40},
    {XtNuserData, XtCUserData, XtRPointer, sizeof(XtPointer),
	offset(threeD.user_data), XtRPointer, (XtPointer) NULL},
    {XtNbeNiceToColormap, XtCBeNiceToColormap, XtRBoolean, sizeof(Boolean),
	offset(threeD.be_nice_to_cmap), XtRImmediate, (XtPointer) False},
    {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
	XtOffsetOf(RectObjRec,rectangle.border_width), XtRImmediate,
	(XtPointer)0},
    {XtNinvertBorder, XtCInvertBorder, XtRBoolean, sizeof(Boolean),
	offset(threeD.invert_border), XtRImmediate, (XtPointer) False},
};

#undef offset

static void ClassPartInitialize(), Initialize(), Destroy();
static void Redisplay(), Realize();
static void _Xaw3dDrawShadows(), _Xaw3dDrawShadowSurroundedBox();
static Boolean SetValues();

ThreeDClassRec threeDClassRec = {
    { /* core fields */
    /* superclass		*/	(WidgetClass) &simple3dClassRec,
    /* class_name		*/	"ThreeD",
    /* widget_size		*/	sizeof(ThreeDRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	ClassPartInitialize,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* resource_count		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	XtInheritResize,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry           */	XtInheritQueryGeometry,
    /* display_accelerator      */	XtInheritDisplayAccelerator,
    /* extension                */	NULL
    },
    { /* simple fields */
    /* change_sensitive         */      XtInheritChangeSensitive
    },
    { /* threeD fields */
    /* shadowdraw                */      _Xaw3dDrawShadows
    }
};

WidgetClass threeDWidgetClass = (WidgetClass) &threeDClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

#define mbshadowpm_width 3
#define mbshadowpm_height 3
static unsigned char mbshadowpm_bits[] = {
	0xFF,0xFF,0xFF};
/*   0x05, 0x03, 0x06}; */
#define mtshadowpm_width 3
#define mtshadowpm_height 3
static char mtshadowpm_bits[] = {
   0x02, 0x04, 0x01};
#define shadowpm_width 2
#define shadowpm_height 2
static char shadowpm_bits[] = {
   0x02, 0x01};

#ifdef USEGRAY
static XColor Gray = {
  /* pixel */            0, 
  /* red, green, blue */ 0,0,0, 
  /* flags */            0,
  /* pad */              0
};
#define ALLOCGRAY { XColor exact; \
     (void) XAllocNamedColor(dpy, DefaultColormapOfScreen (scn), \
			     "gray", &Gray,&exact);}  /* Blindflug */
#endif


/* ARGSUSED */
static void AllocTopShadowGC (w)
    Widget w;
{
    ThreeDWidget	tdw = (ThreeDWidget) w;
    Screen		*scn = XtScreen (w);
    XtGCMask		valuemask;
    XGCValues		myXGCV;

    if (tdw->threeD.be_nice_to_cmap || DefaultDepthOfScreen (scn) == 1) {
	valuemask = GCTile | GCFillStyle;
	myXGCV.tile = tdw->threeD.top_shadow_pxmap;
	myXGCV.fill_style = FillTiled;
    } else {
	valuemask = GCForeground;
	myXGCV.foreground = tdw->threeD.top_shadow_pixel;
    }
    tdw->threeD.top_shadow_GC = XtGetGC(w, valuemask, &myXGCV);
}

/* ARGSUSED */
static void AllocBotShadowGC (w)
    Widget w;
{
    ThreeDWidget	tdw = (ThreeDWidget) w;
    Screen		*scn = XtScreen (w);
    XtGCMask		valuemask;
    XGCValues		myXGCV;

    if (tdw->threeD.be_nice_to_cmap || DefaultDepthOfScreen (scn) == 1) {
	valuemask = GCTile | GCFillStyle;
	myXGCV.tile = tdw->threeD.bot_shadow_pxmap;
	myXGCV.fill_style = FillTiled;
    } else {
	valuemask = GCForeground;
	myXGCV.foreground = tdw->threeD.bot_shadow_pixel;
    }
    tdw->threeD.bot_shadow_GC = XtGetGC(w, valuemask, &myXGCV);
}

/* ARGSUSED */
static void AllocTopShadowPixmap (new)
    Widget new;
{
    ThreeDWidget	tdw = (ThreeDWidget) new;
    Display		*dpy = XtDisplay (new);
    Screen		*scn = XtScreen (new);
    unsigned long	top_fg_pixel = 0, top_bg_pixel = 0;
    char		*pm_data = NULL;
    Boolean		create_pixmap = FALSE;
    unsigned int        pixmapSize = shadowpm_width;
 
    /*
     * I know, we're going to create two pixmaps for each and every
     * shadow'd widget.  Yeuck.  I'm semi-relying on server side
     * pixmap cacheing.
     */

    if (DefaultDepthOfScreen (scn) == 1)
      {
	top_fg_pixel = BlackPixelOfScreen (scn);
	top_bg_pixel = WhitePixelOfScreen (scn);
	pm_data = (char*)mtshadowpm_bits;
	create_pixmap = TRUE;
      }

    if (create_pixmap)
	tdw->threeD.top_shadow_pxmap = XCreatePixmapFromBitmapData (dpy,
			RootWindowOfScreen (scn),
			pm_data,
			pixmapSize,
			pixmapSize,
			top_fg_pixel,
			top_bg_pixel,
			DefaultDepthOfScreen (scn));
  }

/* ARGSUSED */
static void AllocBotShadowPixmap (new)
    Widget new;
{
    ThreeDWidget	tdw = (ThreeDWidget) new;
    Display		*dpy = XtDisplay (new);
    Screen		*scn = XtScreen (new);
    unsigned long	bot_fg_pixel = 0, bot_bg_pixel = 0;
    char		*pm_data = NULL;
    Boolean		create_pixmap = FALSE;
    unsigned int        pixmapSize = shadowpm_width;

    if (DefaultDepthOfScreen (scn) == 1) {
	bot_fg_pixel = BlackPixelOfScreen (scn);
	bot_bg_pixel = WhitePixelOfScreen (scn);
	pm_data = (char*)mbshadowpm_bits;
	create_pixmap = TRUE;
    }
    if (create_pixmap)
	tdw->threeD.bot_shadow_pxmap = XCreatePixmapFromBitmapData (dpy,
			RootWindowOfScreen (scn),
			pm_data,
			pixmapSize,
			pixmapSize,
			bot_fg_pixel,
			bot_bg_pixel,
			DefaultDepthOfScreen (scn));
}

#ifndef VSHADOWS
/* ARGSUSED */
void Xaw3dComputeTopShadowRGB (new, xcol_out)
    Widget new;
    XColor *xcol_out;
{
    if (XtIsSubclass (new, threeDWidgetClass)) {
	ThreeDWidget tdw = (ThreeDWidget) new;
	XColor get_c;
	double contrast;
	Display *dpy = XtDisplay (new);
	Screen *scn = XtScreen (new);
	Colormap cmap = DefaultColormapOfScreen (scn);

	get_c.pixel = tdw->core.background_pixel;
	if (get_c.pixel == WhitePixelOfScreen (scn) ||
	    get_c.pixel == BlackPixelOfScreen (scn)) {
	    contrast = (100 - tdw->threeD.top_shadow_contrast) / 100.0;
	    xcol_out->red   = contrast * 65535.0;
	    xcol_out->green = contrast * 65535.0;
	    xcol_out->blue  = contrast * 65535.0;
	} else {
	    contrast = 1.0 + tdw->threeD.top_shadow_contrast / 100.0;
	    XQueryColor (dpy, cmap, &get_c);
#define MIN(x,y) (unsigned short) (x < y) ? x : y
	    xcol_out->red   = MIN (65535, (int) (contrast * (double) get_c.red));
	    xcol_out->green = MIN (65535, (int) (contrast * (double) get_c.green));
	    xcol_out->blue  = MIN (65535, (int) (contrast * (double) get_c.blue));
#undef MIN
	}
    } else
	xcol_out->red = xcol_out->green = xcol_out->blue = 0;
}
#endif

/* ARGSUSED */
static void AllocTopShadowPixel (new)
    Widget new;
{
    XColor set_c;
    ThreeDWidget tdw = (ThreeDWidget) new;

#ifdef VSHADOWS
    tdw->threeD.top_shadow_pixel = _vLightControlShadow;
#else

    Display *dpy = XtDisplay (new);
    Screen *scn = XtScreen (new);
    Colormap cmap = DefaultColormapOfScreen (scn);

    Xaw3dComputeTopShadowRGB (new, &set_c);
    (void) XAllocColor (dpy, cmap, &set_c);
    tdw->threeD.top_shadow_pixel = set_c.pixel;
#endif
}

/* ARGSUSED */
#ifndef VSHADOWS
void Xaw3dComputeBottomShadowRGB (new, xcol_out)
    Widget new;
    XColor *xcol_out;
{
    if (XtIsSubclass (new, threeDWidgetClass)) {
	ThreeDWidget tdw = (ThreeDWidget) new;
	XColor get_c;
	double contrast;
	Display *dpy = XtDisplay (new);
	Screen *scn = XtScreen (new);
	Colormap cmap = DefaultColormapOfScreen (scn);

	get_c.pixel = tdw->core.background_pixel;
	if (get_c.pixel == WhitePixelOfScreen (scn) ||
	    get_c.pixel == BlackPixelOfScreen (scn)) {
	    contrast = tdw->threeD.bot_shadow_contrast / 100.0;
	    xcol_out->red   = contrast * 65535.0;
	    xcol_out->green = contrast * 65535.0;
	    xcol_out->blue  = contrast * 65535.0;
	} else {
	    XQueryColor (dpy, cmap, &get_c);
	    contrast = (100 - tdw->threeD.bot_shadow_contrast) / 100.0;
	    xcol_out->red   = contrast * get_c.red;
	    xcol_out->green = contrast * get_c.green;
	    xcol_out->blue  = contrast * get_c.blue;
	}
    } else
	xcol_out->red = xcol_out->green = xcol_out->blue = 0;
}
#endif

/* ARGSUSED */
static void AllocBotShadowPixel (new)
    Widget new;
{
    XColor set_c;
    ThreeDWidget tdw = (ThreeDWidget) new;

#ifdef VSHADOWS
    tdw->threeD.bot_shadow_pixel = _vDarkControlShadow;
#else
    Display *dpy = XtDisplay (new);
    Screen *scn = XtScreen (new);
    Colormap cmap = DefaultColormapOfScreen (scn);

    Xaw3dComputeBottomShadowRGB (new, &set_c);
    (void) XAllocColor (dpy, cmap, &set_c);
    tdw->threeD.bot_shadow_pixel = set_c.pixel;
#endif
}


/* ARGSUSED */
static void ClassPartInitialize (wc)
    WidgetClass	wc;

{
    ThreeDClassRec *tdwc = (ThreeDClassRec*) wc;
    ThreeDClassRec *super = (ThreeDClassRec*) tdwc->core_class.superclass;

    if (tdwc->threeD_class.shadowdraw == XtInheritXaw3dShadowDraw)
	tdwc->threeD_class.shadowdraw = super->threeD_class.shadowdraw;
}


/* ARGSUSED */
static void Initialize (request, new, args, num_args)
    Widget request, new;
    ArgList args;
    Cardinal *num_args;
{
    ThreeDWidget 	tdw = (ThreeDWidget) new;
    Screen		*scr = XtScreen (new);

    if (tdw->threeD.be_nice_to_cmap || DefaultDepthOfScreen (scr) == 1) {
	AllocTopShadowPixmap (new);
	AllocBotShadowPixmap (new);
    } else {
	if (tdw->threeD.top_shadow_pixel == tdw->threeD.bot_shadow_pixel) {
	    /* 
		Eeek.  We're probably going to XQueryColor() twice 
		for each widget.  Necessary because you can set the
		top and bottom shadows independent of each other in
		SetValues.  Some cacheing would certainly help...
	    */
	    AllocTopShadowPixel (new);
	    AllocBotShadowPixel (new);
	}
	tdw->threeD.top_shadow_pxmap = tdw->threeD.bot_shadow_pxmap = (Pixmap) 0;
    }
    AllocTopShadowGC (new);
    AllocBotShadowGC (new);
}

static void Realize (gw, valueMask, attrs)
    Widget gw;
    XtValueMask *valueMask;
    XSetWindowAttributes *attrs;
{
 /* 
  * This is necessary because Simple3d doesn't have a realize method
  * XtInheritRealize in the ThreeD class record doesn't work.  This
  * daisychains through Simple3d to the Core class realize method
  */
    (*threeDWidgetClass->core_class.superclass->core_class.realize)
	 (gw, valueMask, attrs);
}

static void Destroy (w)
     Widget w;
{
    ThreeDWidget tdw = (ThreeDWidget) w;
    XtReleaseGC (w, tdw->threeD.top_shadow_GC);
    XtReleaseGC (w, tdw->threeD.bot_shadow_GC);
    if (tdw->threeD.top_shadow_pxmap)
	XFreePixmap (XtDisplay (w), tdw->threeD.top_shadow_pxmap);
    if (tdw->threeD.bot_shadow_pxmap)
	XFreePixmap (XtDisplay (w), tdw->threeD.bot_shadow_pxmap);
}

/* ARGSUSED */
static void Redisplay (w, event, region)
    Widget w;
    XEvent *event;		/* unused */
    Region region;		/* unused */
{
    _Xaw3dDrawShadows (w, event, region, True);
}

/* ARGSUSED */
static Boolean SetValues (gcurrent, grequest, gnew, args, num_args)
    Widget gcurrent, grequest, gnew;
    ArgList args;
    Cardinal *num_args;
{
    ThreeDWidget current = (ThreeDWidget) gcurrent;
    ThreeDWidget new = (ThreeDWidget) gnew;
    Boolean redisplay = FALSE;
    Boolean alloc_top_pixel = FALSE;
    Boolean alloc_bot_pixel = FALSE;
    Boolean alloc_top_pxmap = FALSE;
    Boolean alloc_bot_pxmap = FALSE;

    (*threeDWidgetClass->core_class.superclass->core_class.set_values) 
	(gcurrent, grequest, gnew, NULL, 0);
    if (new->threeD.shadow_width != current->threeD.shadow_width)
	redisplay = TRUE;
    if (new->threeD.be_nice_to_cmap != current->threeD.be_nice_to_cmap) {
	if (new->threeD.be_nice_to_cmap) {
	    alloc_top_pxmap = TRUE;
	    alloc_bot_pxmap = TRUE;
	} else {
	    alloc_top_pixel = TRUE;
	    alloc_bot_pixel = TRUE;
	}
	redisplay = TRUE;
    }
    if (!new->threeD.be_nice_to_cmap &&
	new->threeD.top_shadow_contrast != current->threeD.top_shadow_contrast)
	alloc_top_pixel = TRUE;
    if (!new->threeD.be_nice_to_cmap &&
	new->threeD.bot_shadow_contrast != current->threeD.bot_shadow_contrast)
	alloc_bot_pixel = TRUE;
    if (alloc_top_pixel)
	AllocTopShadowPixel (gnew);
    if (alloc_bot_pixel)
	AllocBotShadowPixel (gnew);
    if (alloc_top_pxmap)
	AllocTopShadowPixmap (gnew);
    if (alloc_bot_pxmap)
	AllocBotShadowPixmap (gnew);
    if (!new->threeD.be_nice_to_cmap &&
	new->threeD.top_shadow_pixel != current->threeD.top_shadow_pixel)
	alloc_top_pixel = TRUE;
    if (!new->threeD.be_nice_to_cmap &&
	new->threeD.bot_shadow_pixel != current->threeD.bot_shadow_pixel)
	alloc_bot_pixel = TRUE;
    if (new->threeD.be_nice_to_cmap) {
	if (alloc_top_pxmap) {
	    XtReleaseGC (gcurrent, current->threeD.top_shadow_GC);
	    AllocTopShadowGC (gnew);
	    redisplay = True;
	}
	if (alloc_bot_pxmap) {
	    XtReleaseGC (gcurrent, current->threeD.bot_shadow_GC);
	    AllocBotShadowGC (gnew);
	    redisplay = True;
	}
    } else {
	if (alloc_top_pixel) {
	    if (new->threeD.top_shadow_pxmap) {
		XFreePixmap (XtDisplay (gnew), new->threeD.top_shadow_pxmap);
		new->threeD.top_shadow_pxmap = (Pixmap) NULL;
	    }
	    XtReleaseGC (gcurrent, current->threeD.top_shadow_GC);
	    AllocTopShadowGC (gnew);
	    redisplay = True;
	}
	if (alloc_bot_pixel) {
	    if (new->threeD.bot_shadow_pxmap) {
		XFreePixmap (XtDisplay (gnew), new->threeD.bot_shadow_pxmap);
		new->threeD.bot_shadow_pxmap = (Pixmap) NULL;
	    }
	    XtReleaseGC (gcurrent, current->threeD.bot_shadow_GC);
	    AllocBotShadowGC (gnew);
	    redisplay = True;
	}
    }
    return (redisplay);
}

/* ARGSUSED */
static void
_Xaw3dDrawShadowSurroundedBox (gw, event, region, xtl, ytl, xbr, ybr, out)
    Widget gw;
    XEvent *event;
    Region region;
    Dimension  xtl, ytl, xbr, ybr; /* INNER corners */
    Boolean out;
{
    XPoint	 pt[6];
    ThreeDWidget tdw = (ThreeDWidget) gw;
    Dimension	 s   = tdw->threeD.shadow_width;

    /* 
     *	no point to do anything if the shadow_width is 0 or the
     *	widget has not been realized.
     */ 
    if((s > 0) && XtIsRealized (gw)){

	Dimension	xms = xtl - s;
	Dimension	yms = ytl - s;
	Dimension	xps = xbr + s;
	Dimension	yps = ybr + s;
	Display		*dpy = XtDisplay (gw);
	Window		win = XtWindow (gw);
	GC		top, bot;

	if (out) {
	    top = tdw->threeD.top_shadow_GC;
	    bot = tdw->threeD.bot_shadow_GC;
	} else {
	    top = tdw->threeD.bot_shadow_GC;
	    bot = tdw->threeD.top_shadow_GC;
	}
	/* Draw shadows. Points are numbered as follows:
	 *
	 *   1_________________________________ 2
	 *   |                               / /|
	 *   |                              / / |
	 *   |   4_________________________/ /  |
	 *   |   |                         3|   |
	 *   |   |                          |   |
	 *   |   |                          |   |
	 *   |   |                          |   |
	 *   |   |5_________________________|   |
	 *   |  / /                         4   |
	 *   | / /                              |
	 *   |/ /_______________________________|
	 *   3                                  1
	 */

	/* points common to top and bottom shadow */
	pt[0].x = xms;	pt[0].y = yps;
	pt[2].x = xps;	pt[2].y = yms;
	pt[3].x = xbr;	pt[3].y = ytl;
	pt[5].x = xtl;	pt[5].y = ybr;

	/* top-left shadow */
	if ((region == NULL) ||
	    (XRectInRegion (region, xms, yms, xps, ytl) != RectangleOut) ||
	    (XRectInRegion (region, xms, yms, xtl, yps) != RectangleOut)) {

	    pt[1].x = xms;	pt[1].y = yms;
	    pt[4].x = xtl;	pt[4].y = ytl;
	    XFillPolygon (dpy, win, top, pt, 6,Complex,CoordModeOrigin);
	}

	/* bottom-right shadow */
	if ((region == NULL) ||
	    (XRectInRegion (region, xms, ybr, xps, yps) != RectangleOut) ||
	    (XRectInRegion (region, xbr, yms, xps, yps) != RectangleOut)) {

	    /* swap points from top left to bottom right */
	    pt[1].x = xps;	pt[1].y = yps;
	    pt[4].x = xbr;	pt[4].y = ybr;
	    XFillPolygon (dpy, win, bot, pt,6, Complex,CoordModeOrigin);
	}
    }
}

/* ARGSUSED */
static void
_Xaw3dDrawShadows (gw, event, region, out)
    Widget gw;
    XEvent *event;
    Region region;
    Boolean out;
{
    ThreeDWidget tdw = (ThreeDWidget) gw;
    Dimension	 s   = tdw->threeD.shadow_width;
    /* 
     * draw the shadows using the core part width and height, 
     * and the threeD part shadow_width.
     *
     *	no point to do anything if the shadow_width is 0 or the
     *	widget has not been realized.
     */ 
    if(( s > 0) && XtIsRealized (gw)){
	_Xaw3dDrawShadowSurroundedBox (gw, event, region,
		      s, s,
		      tdw->core.width - s, tdw->core.height - s,
		      (out ^ tdw->threeD.invert_border) );
    }
}
