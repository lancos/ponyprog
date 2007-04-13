/* $XConsortium: Toggle.c,v 1.28 94/04/17 20:13:17 kaleb Exp $ */

/*

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
 * Toggle.c - Toggle button widget
 *
 * Author: Chris D. Peterson
 *         MIT X Consortium 
 *         kit@expo.lcs.mit.edu
 *  
 * Date:   January 12, 1989
 *
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Converters.h>
#include <X11/Xmu/Misc.h>
#include <v/awtog3dp.h>

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

/* 
 * The order of toggle and notify are important, as the state has
 * to be set when we call the notify proc.
 */

static char defaultTranslations[] =
    "<EnterWindow>:	    highlight(Always)	\n\
     <LeaveWindow>:	    unhighlight()	\n\
     <Btn1Down>,<Btn1Up>:   toggle() notify()";

#define offset(field) XtOffsetOf(Toggle3dRec, field)

static XtResource resources[] = { 
   {XtNstate, XtCState, XtRBoolean, sizeof(Boolean), 
      offset(command3d.set), XtRString, "off"},
};

#undef offset


static void Toggle(), Initialize(), Notify(), ToggleSet();
static void ToggleDestroy(), ClassInit();
static Boolean SetValues();

static XtActionsRec actionsList[] =
{
  {"toggle",	        Toggle},
  {"notify",	        Notify},
  {"set",	        ToggleSet},
};

#define SuperClass ((Command3dWidgetClass)&command3dClassRec)

Toggle3dClassRec toggle3dClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "Toggle3d",				/* class_name		  */
    sizeof(Toggle3dRec),			/* size			  */
    ClassInit,				/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    XtInheritRealize,			/* realize		  */
    actionsList,			/* actions		  */
    XtNumber(actionsList),		/* num_actions		  */
    resources,				/* resources		  */
    XtNumber(resources),		/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    FALSE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    NULL,         			/* destroy		  */
    XtInheritResize,			/* resize		  */
    XtInheritExpose,			/* expose		  */
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
    XtInheritChangeSensitive		/* change_sensitive	  */ 
  },  /* SimpleClass fields initialization */
  {
    XtInheritXaw3dShadowDraw            /* shadowdraw          */
  },  /* ThreeDClass fields initialization */
  {
    0                                     /* field not used    */
  },  /* LabelClass fields initialization */
  {
    0                                     /* field not used    */
  },  /* CommandClass fields initialization */
  {
      NULL,			        /* Set Procedure. */
      NULL,			        /* Unset Procedure. */
      NULL			        /* extension. */
  }  /* ToggleClass fields initialization */
};

  /* for public consumption */
WidgetClass toggle3dWidgetClass = (WidgetClass) &toggle3dClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void
ClassInit()
{
  XtActionList actions;
  Cardinal num_actions;
  Cardinal i;
  Toggle3dWidgetClass class = (Toggle3dWidgetClass) toggle3dWidgetClass;
  static XtConvertArgRec parentCvtArgs[] = {
      {XtBaseOffset, (XtPointer)XtOffsetOf(WidgetRec, core.parent),
	   sizeof(Widget)}
  };

  XawInitializeWidgetSet();
  XtSetTypeConverter(XtRString, XtRWidget, XmuNewCvtStringToWidget,
		     parentCvtArgs, XtNumber(parentCvtArgs), XtCacheNone,
		     (XtDestructor)NULL);
/* 
 * Find the set and unset actions in the command widget's action table. 
 */

  XtGetActionList(command3dWidgetClass, &actions, &num_actions);

  for (i = 0 ; i < num_actions ; i++) {
    if (streq(actions[i].string, "set"))
	class->toggle3d_class.Set = actions[i].proc;
    if (streq(actions[i].string, "unset")) 
	class->toggle3d_class.Unset = actions[i].proc;

    if ( (class->toggle3d_class.Set != NULL) &&
	 (class->toggle3d_class.Unset != NULL) ) {
	XtFree((char *) actions);
	return;
    }
  }  

/* We should never get here. */
  XtError("Aborting, due to errors resolving bindings in the Toggle widget.");
}

/*ARGSUSED*/
static void Initialize(request, new, args, num_args)
 Widget request, new;
 ArgList args;
 Cardinal *num_args;
{
    Toggle3dWidget tw = (Toggle3dWidget) new;
    Toggle3dWidget tw_req = (Toggle3dWidget) request;

    XtAddCallback(new, XtNdestroyCallback, ToggleDestroy, (XtPointer)NULL);

/*
 * Command widget assumes that the widget is unset, so we only 
 * have to handle the case where it needs to be set.
 *
 * If this widget is in a radio group then it may cause another
 * widget to be unset, thus calling the notify proceedure.
 *
 * I want to set the toggle if the user set the state to "On" in 
 * the resource group, reguardless of what my ancestors did.
 */

    if (tw_req->command3d.set)
      ToggleSet(new, (XEvent *)NULL, (String *)NULL, (Cardinal *)0);
}

/************************************************************
 *
 *  Action Procedures
 *
 ************************************************************/

/* ARGSUSED */
static void 
ToggleSet(w,event,params,num_params)
Widget w;
XEvent *event;
String *params;		/* unused */
Cardinal *num_params;	/* unused */
{
    Toggle3dWidgetClass class = (Toggle3dWidgetClass) w->core.widget_class;

    class->toggle3d_class.Set(w, event, NULL, 0);
}

/* ARGSUSED */
static void 
Toggle(w,event,params,num_params)
Widget w;
XEvent *event;
String *params;		/* unused */
Cardinal *num_params;	/* unused */
{
  Toggle3dWidget tw = (Toggle3dWidget)w;
  Toggle3dWidgetClass class = (Toggle3dWidgetClass) w->core.widget_class;

  if (tw->command3d.set) 
    class->toggle3d_class.Unset(w, event, NULL, 0);
  else 
    ToggleSet(w, event, params, num_params);
}

/* ARGSUSED */
static void Notify(w,event,params,num_params)
Widget w;
XEvent *event;
String *params;		/* unused */
Cardinal *num_params;	/* unused */
{
  Toggle3dWidget tw = (Toggle3dWidget) w;
  long antilint = tw->command3d.set;

  XtCallCallbacks( w, XtNcallback, (XtPointer) antilint );
}

/************************************************************
 *
 * Set specified arguments into widget
 *
 ***********************************************************/

/* ARGSUSED */
static Boolean 
SetValues (current, request, new, args, num_args)
Widget current, request, new;
ArgList args;
Cardinal *num_args;
{
    Toggle3dWidget oldtw = (Toggle3dWidget) current;
    Toggle3dWidget tw = (Toggle3dWidget) new;
    Toggle3dWidget rtw = (Toggle3dWidget) request;

    if (!tw->core.sensitive && oldtw->core.sensitive && rtw->command3d.set)
	tw->command3d.set = True;

    if (oldtw->command3d.set != tw->command3d.set) {
	tw->command3d.set = oldtw->command3d.set;
	Toggle(new, (XEvent *)NULL, (String *)NULL, (Cardinal *)0);
    }
    return(FALSE);
}

/*	Function Name: ToggleDestroy
 *	Description: Destroy Callback for toggle widget.
 *	Arguments: w - the toggle widget that is being destroyed.
 *                 junk, grabage - not used.
 *	Returns: none.
 */

/* ARGSUSED */
static void
ToggleDestroy(w, junk, garbage)
Widget w;
XtPointer junk, garbage;
{
}
