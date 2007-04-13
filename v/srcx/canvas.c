/* =============================================================
 * canvas.c - canvas composite widget
 *
 * Copyright (C) 1995  Bruce E. Wampler
 *
 * This file is part of the V C++ GUI Framework, and is covered
 * under the terms of the GNU Library General Public License,
 * Version 2. This library has NO WARRANTY. See the source file
 * vapp.cxx for more complete information about license terms.
 * ============================================================== */

#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>

#include	<v/canvasp.h>

#define INITIAL_WIDTH 200
#define INITIAL_HEIGHT 100

/****************************************************************
 *
 * canvas Resources
 *
 ****************************************************************/

  static XtResource resources[] =
  {
    {XtNhSpace, XtCHSpace, XtRDimension, sizeof(Dimension),
	 XtOffset(CanvasWidget, canvas.h_space), XtRImmediate, (XtPointer)4},

    {XtNvSpace, XtCVSpace, XtRDimension, sizeof(Dimension),
	 XtOffset(CanvasWidget, canvas.v_space), XtRImmediate, (XtPointer)4},

    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
	XtOffset(CanvasWidget, canvas.callback), XtRCallback, NULL},
  };

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

static void Initialize();
static void Resize();
static Boolean SetValues();
static void ChangeManaged();
static XtGeometryResult QueryGeometry(), GeometryManager();
static void RefigureLocations(CanvasWidget w);

CanvasClassRec canvasClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &compositeClassRec,
    /* class_name         */    "canvas",
    /* widget_size        */    sizeof(CanvasRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */	NULL,
    /* class_inited       */	FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */	NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions	  */	0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    Resize,
    /* expose             */    NULL,
    /* set_values         */    SetValues,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus       */    NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */	QueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },{
/* composite_class fields */
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension          */	NULL
  },{
/* canvas class fields */
    /* empty		  */	0,
  }
};

WidgetClass canvasWidgetClass = (WidgetClass)&canvasClassRec;


/****************************************************************
 *
 * Private Routines
 *
 ****************************************************************/

/*
 * Do a layout, either actually assigning positions, or just 
 * calculating size.
 */

/* ARGSUSED */
  static void DoLayout(CanvasWidget w, Boolean doit)
  {
    CanvasWidget sbw = w;

    Widget main, vscroll, hscroll;
    Dimension mw, mh;	/* main window */
    Dimension vh;	/* vertical scrollbar length (height) */
    Dimension hw;	/* horizontal scrollbar length (width) */
    Position vx;
    Position hy;

    int vManaged, hManaged;

    int i;

    if (sbw->composite.num_children != 3)
      {
	XtAppError(XtWidgetToApplicationContext((Widget)sbw),
	    "Canvas: must manage exactly three widgets.");
      }

    vManaged = 1;
    hManaged = 1;		/* assume all managed */

    /*
    * Child one is the main window, two is the vertical scrollbar,
    * and three is the horizontal scrollbar.
    */
    main = sbw->composite.children[0];
    vscroll = sbw->composite.children[1];
    hscroll = sbw->composite.children[2];

    if (!XtIsManaged(main))
      {
	XtAppError(XtWidgetToApplicationContext((Widget)sbw),
 	  "Canvas: Draw area must be managed.");
      }

    if (!XtIsManaged(vscroll))
	vManaged = 0;

    if (!XtIsManaged(hscroll))
	hManaged = 0;

    /* 
    * Size all three widgets so that space is fully utilized.
    */
    if (vManaged)
      {
	mw = sbw->core.width - (2 * sbw->canvas.h_space) - 
	   vscroll->core.width - (2 * vscroll->core.border_width) -
	   (2 * main->core.border_width);

	vx = main->core.x + mw + sbw->canvas.h_space + main->core.border_width +
	    vscroll->core.border_width; 

      }
    else
      {
	mw = sbw->core.width - 		/* (2 * sbw->canvas.h_space) -  */
	   (2 * main->core.border_width);
	vx = 0;
      }

    if (hManaged)
      {
	mh = sbw->core.height - (2 * sbw->canvas.v_space) -
	    hscroll->core.height - (2 * hscroll->core.border_width) -
	    (2 * main->core.border_width);

	hy = main->core.y + mh + sbw->canvas.v_space + main->core.border_width +
	    hscroll->core.border_width; 

      }
    else
      {
	mh = sbw->core.height - 		/* (2 * sbw->canvas.v_space) - */
	   (2 * main->core.border_width);

	hy = 0;
      }

    if (mh < 2) mh = 2;
    if (mw < 2) mw = 2;

    vh = mh;   /* scrollbars are always same length as main window */
    hw = mw;

    if (doit)
      {
	XtResizeWidget(main, mw, mh, 1);

	if (vManaged)
	  {
	    XtResizeWidget(vscroll, vscroll->core.width, vh, 1);
	    XtMoveWidget(vscroll, vx, vscroll->core.y);
	  }

	if (hManaged)
	  {
	    XtResizeWidget(hscroll, hw, hscroll->core.height, 1);
	    XtMoveWidget(hscroll, hscroll->core.x, hy);
	  }
      }
  }

/* ARGSUSED */
  static XtGeometryResult GeometryManager(Widget w,
	XtWidgetGeometry* request, XtWidgetGeometry* reply)
  {
    XtWidgetGeometry allowed;

    if (request->request_mode & ~(XtCWQueryOnly | CWWidth | CWHeight))
	return XtGeometryNo;

    if (request->request_mode & CWWidth)
	allowed.width = request->width;
    else
	allowed.width = w->core.width;

    if (request->request_mode & CWHeight)
	allowed.height = request->height;
    else
	allowed.height = w->core.height;

    if (allowed.width == w->core.width && allowed.height == w->core.height)
	return XtGeometryNo;

    if (!(request->request_mode & XtCWQueryOnly))
      {
	RefigureLocations((CanvasWidget)w);
      }
    return XtGeometryYes;
  }

  static void RefigureLocations(CanvasWidget sbw)
  {
    DoLayout(sbw, False);
  }


/*
 * Calculate preferred size.  We can't just use the current sizes
 * of the children, because that calculation would always end up with 
 * our current size.  Could query each child, and use that size to 
 * recalculate a size for us, then if it ends up being larger
 * than width and height passed in, accept bounding box.  
 * However, we know our children and they don't have any particular
 * preferred geometry, except the bigger the better.
 * Therefore, if the parent suggested a size, we'll take it.
 */

/* ARGSUSED */
  static XtGeometryResult QueryGeometry(Widget w,
	XtWidgetGeometry* request, XtWidgetGeometry* reply_return)
  {
    XtGeometryResult result;

    request->request_mode &= CWWidth | CWHeight;

    /* parent isn't going to change w or h, so nothing to re-compute */
    if (request->request_mode == 0)
	return XtGeometryYes;

    /* if proposed size is large enough, accept it.  Otherwise,
    * suggest our arbitrary initial size. */

    result = XtGeometryYes;
    if (request->request_mode & CWHeight)
      {
	if (request->height < INITIAL_HEIGHT)
	  {
	    result = XtGeometryAlmost;
	    reply_return->height = INITIAL_HEIGHT;
	    reply_return->request_mode &= CWHeight;
	  }
	else
	    result = XtGeometryYes;
      }

    if (request->request_mode & CWWidth)
      {
	if (request->width < INITIAL_WIDTH)
	  {
	    result = XtGeometryAlmost;
	    reply_return->width = INITIAL_WIDTH;
	    reply_return->request_mode &= CWWidth;
	  }
	else
	    result = XtGeometryYes;
      }

    return(result);
  }

/*
 * Actually layout the canvas
 */

  static void Resize(Widget w)
  {
    CanvasWidget cw = (CanvasWidget) w;
    DoLayout(cw, True);
    XtCallCallbacks((Widget)cw, XtNcallback, NULL);
  }

  static void ChangeManaged(CanvasWidget w)
  {
    CanvasWidget sbw = (CanvasWidget) w;
    DoLayout(sbw, True);
  }

/* ARGSUSED */
  static void Initialize(Widget request, Widget new)
  {
    CanvasWidget newsbw = (CanvasWidget)new;

    if (newsbw->core.width == 0)
	newsbw->core.width = INITIAL_WIDTH;

    if (newsbw->core.height == 0)
	newsbw->core.height = INITIAL_HEIGHT;

  } /* Initialize */

/* ARGSUSED */
  static Boolean SetValues(Widget current, Widget request, Widget new,
	ArgList args, Cardinal* num_args)
  {
    CanvasWidget sbwcurrent = (CanvasWidget) current;
    CanvasWidget sbwnew = (CanvasWidget) new;

    /* need to relayout if h_space or v_space change */
    if ((sbwnew->canvas.h_space != sbwcurrent->canvas.h_space) ||
	(sbwnew->canvas.v_space != sbwcurrent->canvas.v_space))
      {
	DoLayout(sbwnew, True);
      }

    return False;
  }
