/* $XConsortium: Command.c,v 1.79 94/04/17 20:11:58 kaleb Exp $ */

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
 * Command.c - Command button widget
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <v/awcmd3dp.h>
#include <X11/Xmu/Converters.h>
#include <X11/extensions/shape.h>

#define DEFAULT_HIGHLIGHT_THICKNESS 2
#define DEFAULT_SHAPE_HIGHLIGHT 32767

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static char defaultTranslations[] =
    "<EnterWindow>:	highlight()		\n\
     <LeaveWindow>:	reset()			\n\
     <Btn1Down>:	set()			\n\
     <Btn1Up>:		notify() unset()	";

#define offset(field) XtOffsetOf(Command3dRec, field)
static XtResource resources[] = { 
   {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer), 
      offset(command3d.callbacks), XtRCallback, (XtPointer)NULL},
   {XtNhighlightThickness, XtCThickness, XtRDimension, sizeof(Dimension),
      offset(command3d.highlight_thickness), XtRImmediate,
      (XtPointer) DEFAULT_SHAPE_HIGHLIGHT},
   {XtNshapeStyle, XtCShapeStyle, XtRShapeStyle, sizeof(int),
      offset(command3d.shape_style), XtRImmediate, (XtPointer)XawShapeRectangle},
   {XtNcornerRoundPercent, XtCCornerRoundPercent, 
	XtRDimension, sizeof(Dimension),
	offset(command3d.corner_round), XtRImmediate, (XtPointer) 25},
   {XtNshadowWidth, XtCShadowWidth, XtRDimension, sizeof(Dimension),
      offset(threeD.shadow_width), XtRImmediate, (XtPointer) 2},
   {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
      XtOffsetOf(RectObjRec,rectangle.border_width), XtRImmediate,
      (XtPointer)0}
};
#undef offset

static Boolean SetValues();
static void Initialize(), Redisplay(), Set(), Reset(), Notify(), Unset();
static void Highlight(), Unhighlight(), Destroy(), PaintCommand3dWidget();
static void ClassInitialize();
static Boolean ShapeButton();
static void Realize(), Resize();

static XtActionsRec actionsList[] = {
  {"set",		Set},
  {"notify",		Notify},
  {"highlight",		Highlight},
  {"reset",		Reset},
  {"unset",		Unset},
  {"unhighlight",	Unhighlight}
};

#define SuperClass ((Label3dWidgetClass)&label3dClassRec)

Command3dClassRec command3dClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "Command3d",				/* class_name		  */
    sizeof(Command3dRec),			/* size			  */
    ClassInitialize,			/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    Realize,				/* realize		  */
    actionsList,			/* actions		  */
    XtNumber(actionsList),		/* num_actions		  */
    resources,				/* resources		  */
    XtNumber(resources),		/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    FALSE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    Destroy,				/* destroy		  */
    Resize,				/* resize		  */
    Redisplay,				/* expose		  */
    SetValues,				/* set_values		  */
    NULL,				/* set_values_hook	  */
    XtInheritSetValuesAlmost,		/* set_values_almost	  */
    NULL,				/* get_values_hook	  */
    NULL,				/* accept_focus		  */
    XtVersion,				/* version		  */
    NULL,				/* callback_private	  */
    defaultTranslations,		/* tm_table		  */
    XtInheritQueryGeometry,		/* query_geometry	  */
    XtInheritDisplayAccelerator,	/* display_accelerator	  */
    NULL				/* extension		  */
  },  /* CoreClass fields initialization */
  {
    XtInheritChangeSensitive		/* change_sensitive	*/
  },  /* Simple3d3dClass fields initialization */
  {
    XtInheritXaw3dShadowDraw,           /* shadowdraw           */
  },  /* ThreeD Class fields initialization */
  {
    0,                                     /* field not used    */
  },  /* Label3dClass fields initialization */
  {
    0,                                     /* field not used    */
  },  /* Command3dClass fields initialization */
};

  /* for public consumption */
WidgetClass command3dWidgetClass = (WidgetClass) &command3dClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static GC 
Get_GC(cbw, fg, bg)
Command3dWidget cbw;
Pixel fg, bg;
{
  XGCValues	values;
  
  values.foreground   = fg;
  values.background	= bg;
  values.font		= cbw->label3d.font->fid;
  values.cap_style = CapProjecting;
  
  if (cbw->command3d.highlight_thickness > 1 )
    values.line_width   = cbw->command3d.highlight_thickness;
  else 
    values.line_width   = 0;
  
  if ( cbw->simple3d.international == True )
      return XtAllocateGC((Widget)cbw, 0, 
		 (GCForeground|GCBackground|GCLineWidth|GCCapStyle),
		 &values, GCFont, 0 );
  else
      return XtGetGC((Widget)cbw,
		 (GCForeground|GCBackground|GCFont|GCLineWidth|GCCapStyle),
		 &values);
}


/* ARGSUSED */
static void 
Initialize(request, new, args, num_args)
Widget request, new;
ArgList args;			/* unused */
Cardinal *num_args;		/* unused */
{
  Command3dWidget cbw = (Command3dWidget) new;
  int shape_event_base, shape_error_base;

  if (cbw->command3d.shape_style != XawShapeRectangle
      && !XShapeQueryExtension(XtDisplay(new), &shape_event_base, 
			       &shape_error_base))
      cbw->command3d.shape_style = XawShapeRectangle;
  if (cbw->command3d.highlight_thickness == DEFAULT_SHAPE_HIGHLIGHT) {
      if (cbw->command3d.shape_style != XawShapeRectangle)
	  cbw->command3d.highlight_thickness = 0;
      else
	  cbw->command3d.highlight_thickness = DEFAULT_HIGHLIGHT_THICKNESS;
  }
  if (cbw->command3d.shape_style != XawShapeRectangle) {
    cbw->threeD.shadow_width = 0;
    cbw->core.border_width = 1;
  }

  cbw->command3d.normal_GC = Get_GC(cbw, cbw->label3d.foreground, 
				  cbw->core.background_pixel);
  cbw->command3d.inverse_GC = Get_GC(cbw, cbw->core.background_pixel, 
				   cbw->label3d.foreground);
  XtReleaseGC(new, cbw->label3d.normal_GC);
  cbw->label3d.normal_GC = cbw->command3d.normal_GC;

  cbw->command3d.set = FALSE;
  cbw->command3d.highlighted = HighlightNone;
}

static Region 
HighlightRegion(cbw)
Command3dWidget cbw;
{
  static Region outerRegion = NULL, innerRegion, emptyRegion;
  XRectangle rect;

  if (cbw->command3d.highlight_thickness == 0 ||
      cbw->command3d.highlight_thickness >
      (Dimension) ((Dimension) Min(cbw->core.width, cbw->core.height)/2))
    return(NULL);

  if (outerRegion == NULL) {
    /* save time by allocating scratch regions only once. */
    outerRegion = XCreateRegion();
    innerRegion = XCreateRegion();
    emptyRegion = XCreateRegion();
  }

  rect.x = rect.y = 0;
  rect.width = cbw->core.width;
  rect.height = cbw->core.height;
  XUnionRectWithRegion( &rect, emptyRegion, outerRegion );
  rect.x = rect.y = cbw->command3d.highlight_thickness;
  rect.width -= cbw->command3d.highlight_thickness * 2;
  rect.height -= cbw->command3d.highlight_thickness * 2;
  XUnionRectWithRegion( &rect, emptyRegion, innerRegion );
  XSubtractRegion( outerRegion, innerRegion, outerRegion );
  return outerRegion;
}

/***************************
*
*  Action Procedures
*
***************************/

/* ARGSUSED */
static void 
Set(w,event,params,num_params)
Widget w;
XEvent *event;
String *params;		/* unused */
Cardinal *num_params;	/* unused */
{
  Command3dWidget cbw = (Command3dWidget)w;

  if (cbw->command3d.set)
    return;

  cbw->command3d.set= TRUE;
  if (XtIsRealized(w))
    PaintCommand3dWidget(w, event, (Region) NULL, TRUE);
}

/* ARGSUSED */
static void
Unset(w,event,params,num_params)
Widget w;
XEvent *event;
String *params;		/* unused */
Cardinal *num_params;
{
  Command3dWidget cbw = (Command3dWidget)w;

  if (!cbw->command3d.set)
    return;

  cbw->command3d.set = FALSE;
  if (XtIsRealized(w)) {
    XClearWindow(XtDisplay(w), XtWindow(w));
    PaintCommand3dWidget(w, event, (Region) NULL, TRUE);
  }
}

/* ARGSUSED */
static void 
Reset(w,event,params,num_params)
Widget w;
XEvent *event;
String *params;		/* unused */
Cardinal *num_params;   /* unused */
{
  Command3dWidget cbw = (Command3dWidget)w;

  if (cbw->command3d.set) {
    cbw->command3d.highlighted = HighlightNone;
    Unset(w, event, params, num_params);
  } else
    Unhighlight(w, event, params, num_params);
}

/* ARGSUSED */
static void 
Highlight(w,event,params,num_params)
Widget w;
XEvent *event;
String *params;		
Cardinal *num_params;	
{
  Command3dWidget cbw = (Command3dWidget)w;

  if ( *num_params == (Cardinal) 0) 
    cbw->command3d.highlighted = HighlightWhenUnset;
  else {
    if ( *num_params != (Cardinal) 1) 
      XtWarning("Too many parameters passed to highlight action table.");
    switch (params[0][0]) {
    case 'A':
    case 'a':
      cbw->command3d.highlighted = HighlightAlways;
      break;
    default:
      cbw->command3d.highlighted = HighlightWhenUnset;
      break;
    }
  }

  if (XtIsRealized(w))
    PaintCommand3dWidget(w, event, HighlightRegion(cbw), TRUE);
}

/* ARGSUSED */
static void 
Unhighlight(w,event,params,num_params)
Widget w;
XEvent *event;
String *params;		/* unused */
Cardinal *num_params;	/* unused */
{
  Command3dWidget cbw = (Command3dWidget)w;

  cbw->command3d.highlighted = HighlightNone;
  if (XtIsRealized(w))
    PaintCommand3dWidget(w, event, HighlightRegion(cbw), TRUE);
}

/* ARGSUSED */
static void 
Notify(w,event,params,num_params)
Widget w;
XEvent *event;
String *params;		/* unused */
Cardinal *num_params;	/* unused */
{
  Command3dWidget cbw = (Command3dWidget)w; 

  /* check to be sure state is still Set so that user can cancel
     the action (e.g. by moving outside the window, in the default
     bindings.
  */
  if (cbw->command3d.set)
    XtCallCallbackList(w, cbw->command3d.callbacks, (XtPointer) NULL);
}

/*
 * Repaint the widget window
 */

/************************
*
*  REDISPLAY (DRAW)
*
************************/

/* ARGSUSED */
static void 
Redisplay(w, event, region)
Widget w;
XEvent *event;
Region region;
{
  PaintCommand3dWidget(w, event, region, FALSE);
}

/*	Function Name: PaintCommand3dWidget
 *	Description: Paints the command3d widget.
 *	Arguments: w - the command3d widget.
 *                 region - region to paint (passed to the superclass).
 *                 change - did it change either set or highlight state?
 *	Returns: none
 */

static void 
PaintCommand3dWidget(w, event, region, change)
Widget w;
XEvent *event;
Region region;
Boolean change;
{
  Command3dWidget cbw = (Command3dWidget) w;
  Command3dWidgetClass cwclass = (Command3dWidgetClass) XtClass (w);
  Boolean very_thick;
  GC norm_gc, rev_gc;
  Dimension	s = cbw->threeD.shadow_width;
   
  very_thick = cbw->command3d.highlight_thickness >
               (Dimension)((Dimension) Min(cbw->core.width, cbw->core.height)/2);

  if (cbw->command3d.set) {
    cbw->label3d.normal_GC = cbw->command3d.inverse_GC;
    XFillRectangle(XtDisplay(w), XtWindow(w), cbw->command3d.normal_GC,
		   s, s, cbw->core.width - 2 * s, cbw->core.height - 2 * s);
    region = NULL;		/* Force label3d to repaint text. */
  }
  else
    cbw->label3d.normal_GC = cbw->command3d.normal_GC;

  if (cbw->command3d.highlight_thickness <= 0)
  {
    (*SuperClass->core_class.expose) (w, event, region);
    (*cwclass->threeD_class.shadowdraw) (w, event, region, !cbw->command3d.set);
    return;
  }

/*
 * If we are set then use the same colors as if we are not highlighted. 
 */

  if (cbw->command3d.set == (cbw->command3d.highlighted == HighlightNone)) {
    norm_gc = cbw->command3d.inverse_GC;
    rev_gc = cbw->command3d.normal_GC;
  }
  else {
    norm_gc = cbw->command3d.normal_GC;
    rev_gc = cbw->command3d.inverse_GC;
  }

  if ( !( (!change && (cbw->command3d.highlighted == HighlightNone)) ||
	  ((cbw->command3d.highlighted == HighlightWhenUnset) &&
	   (cbw->command3d.set))) ) {
    if (very_thick) {
      cbw->label3d.normal_GC = norm_gc; /* Give the label3d the right GC. */
      XFillRectangle(XtDisplay(w),XtWindow(w), rev_gc,
		     s, s, cbw->core.width - 2 * s, cbw->core.height - 2 * s);
    }
    else {
      /* wide lines are centered on the path, so indent it */
      int offset = cbw->command3d.highlight_thickness/2;
      XDrawRectangle(XtDisplay(w),XtWindow(w), rev_gc, s + offset, s + offset, 
		     cbw->core.width - cbw->command3d.highlight_thickness - 2 * s,
		     cbw->core.height - cbw->command3d.highlight_thickness - 2 * s);
    }
  }
  (*SuperClass->core_class.expose) (w, event, region);
  (*cwclass->threeD_class.shadowdraw) (w, event, region, !cbw->command3d.set);
}

static void 
Destroy(w)
Widget w;
{
  Command3dWidget cbw = (Command3dWidget) w;

  /* so Label3d can release it */
  if (cbw->label3d.normal_GC == cbw->command3d.normal_GC)
    XtReleaseGC( w, cbw->command3d.inverse_GC );
  else
    XtReleaseGC( w, cbw->command3d.normal_GC );
}

/*
 * Set specified arguments into widget
 */

/* ARGSUSED */
static Boolean 
SetValues (current, request, new, args, num_args)
Widget current, request, new;
ArgList args;
Cardinal *num_args;
{
  Command3dWidget oldcbw = (Command3dWidget) current;
  Command3dWidget cbw = (Command3dWidget) new;
  Boolean redisplay = False;

  if ( oldcbw->core.sensitive != cbw->core.sensitive && !cbw->core.sensitive) {
    /* about to become insensitive */
    cbw->command3d.set = FALSE;
    cbw->command3d.highlighted = HighlightNone;
    redisplay = TRUE;
  }
  
  if ( (oldcbw->label3d.foreground != cbw->label3d.foreground)           ||
       (oldcbw->core.background_pixel != cbw->core.background_pixel) ||
       (oldcbw->command3d.highlight_thickness != 
                                   cbw->command3d.highlight_thickness) ||
       (oldcbw->label3d.font != cbw->label3d.font) ) 
  {
    if (oldcbw->label3d.normal_GC == oldcbw->command3d.normal_GC)
	/* Label3d has release one of these */
      XtReleaseGC(new, cbw->command3d.inverse_GC);
    else
      XtReleaseGC(new, cbw->command3d.normal_GC);

    cbw->command3d.normal_GC = Get_GC(cbw, cbw->label3d.foreground, 
				    cbw->core.background_pixel);
    cbw->command3d.inverse_GC = Get_GC(cbw, cbw->core.background_pixel, 
				     cbw->label3d.foreground);
    XtReleaseGC(new, cbw->label3d.normal_GC);
    cbw->label3d.normal_GC = (cbw->command3d.set
			    ? cbw->command3d.inverse_GC
			    : cbw->command3d.normal_GC);
    
    redisplay = True;
  }

  if ( XtIsRealized(new)
       && oldcbw->command3d.shape_style != cbw->command3d.shape_style
       && !ShapeButton(cbw, TRUE))
  {
      cbw->command3d.shape_style = oldcbw->command3d.shape_style;
  }

  return (redisplay);
}

static void ClassInitialize()
{
    XtSetTypeConverter( XtRString, XtRShapeStyle, XmuCvtStringToShapeStyle,
		        (XtConvertArgList)NULL, 0, XtCacheNone, (XtDestructor)NULL );
}


static Boolean
ShapeButton(cbw, checkRectangular)
Command3dWidget cbw;
Boolean checkRectangular;
{
    Dimension corner_size = 0;

    if ( (cbw->command3d.shape_style == XawShapeRoundedRectangle) ) {
	corner_size = (cbw->core.width < cbw->core.height) ? cbw->core.width 
	                                                   : cbw->core.height;
	corner_size = (int) (corner_size * cbw->command3d.corner_round) / 100;
    }

    if (checkRectangular || cbw->command3d.shape_style != XawShapeRectangle) {
	if (!XmuReshapeWidget((Widget) cbw, cbw->command3d.shape_style,
			      corner_size, corner_size)) {
	    cbw->command3d.shape_style = XawShapeRectangle;
	    return(False);
	}
    }
    return(TRUE);
}

static void Realize(w, valueMask, attributes)
    Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    (*command3dWidgetClass->core_class.superclass->core_class.realize)
	(w, valueMask, attributes);

    ShapeButton( (Command3dWidget) w, FALSE);
}

static void Resize(w)
    Widget w;
{
    if (XtIsRealized(w)) 
	ShapeButton( (Command3dWidget) w, FALSE);

    (*command3dWidgetClass->core_class.superclass->core_class.resize)(w);
}
