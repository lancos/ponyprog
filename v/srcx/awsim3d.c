/* $XConsortium: Simple.c,v 1.36 94/04/17 20:12:43 kaleb Exp $ */

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

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <v/awsim3dp.h>
#include <X11/Xmu/Drawing.h>

#define offset(field) XtOffsetOf(Simple3dRec, simple3d.field)

static XtResource resources[] = {
  {XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
     offset(cursor), XtRImmediate, (XtPointer) None},
  {XtNinsensitiveBorder, XtCInsensitive, XtRPixmap, sizeof(Pixmap),
     offset(insensitive_border), XtRImmediate, (XtPointer) NULL},
  {XtNpointerColor, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(pointer_fg), XtRString, XtDefaultForeground},
  {XtNpointerColorBackground, XtCBackground, XtRPixel, sizeof(Pixel),
     offset(pointer_bg), XtRString, XtDefaultBackground},
  {XtNcursorName, XtCCursor, XtRString, sizeof(String),
     offset(cursor_name), XtRString, NULL},
  {XtNinternational, XtCInternational, XtRBoolean, sizeof(Boolean),
     offset(international), XtRImmediate, (XtPointer) FALSE},
#undef offset
};

static void ClassPartInitialize(), ClassInitialize(),Realize(),ConvertCursor();
static Boolean SetValues(), ChangeSensitive();

Simple3dClassRec simple3dClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"Simple3d",
    /* widget_size		*/	sizeof(Simple3dRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	ClassPartInitialize,
    /* class_inited		*/	FALSE,
    /* initialize		*/	NULL,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	NULL,
    /* resize			*/	NULL,
    /* expose			*/	NULL,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* simple3d fields */
    /* change_sensitive		*/	ChangeSensitive
  }
};

WidgetClass simple3dWidgetClass = (WidgetClass)&simple3dClassRec;

static void ClassInitialize()
{
    static XtConvertArgRec convertArg[] = {
        {XtWidgetBaseOffset, (XtPointer) XtOffsetOf(WidgetRec, core.screen),
	     sizeof(Screen *)},
        {XtResourceString, (XtPointer) XtNpointerColor, sizeof(Pixel)},
        {XtResourceString, (XtPointer) XtNpointerColorBackground, 
	     sizeof(Pixel)},
        {XtWidgetBaseOffset, (XtPointer) XtOffsetOf(WidgetRec, core.colormap),
	     sizeof(Colormap)}
    };

    XtSetTypeConverter( XtRString, XtRColorCursor, XmuCvtStringToColorCursor,
		       convertArg, XtNumber(convertArg), 
		       XtCacheByDisplay, (XtDestructor)NULL);
}

static void ClassPartInitialize(class)
    WidgetClass class;
{
    Simple3dWidgetClass c     = (Simple3dWidgetClass) class;
    Simple3dWidgetClass super = (Simple3dWidgetClass)
      c->core_class.superclass;

    if (c->simple3d_class.change_sensitive == NULL) {
	char buf[256];

	(void) sprintf(buf,
		"%s Widget: The Simple3d Widget class method 'change_sensitive' is undefined.\nA function must be defined or inherited.",
		c->core_class.class_name);
	XtWarning(buf);
	c->simple3d_class.change_sensitive = ChangeSensitive;
    }

    if (c->simple3d_class.change_sensitive == XtInheritChangeSensitive)
	c->simple3d_class.change_sensitive = super->simple3d_class.change_sensitive;
}

static void Realize(w, valueMask, attributes)
    Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    Pixmap border_pixmap = 0;
    if (!XtIsSensitive(w)) {
	/* change border to gray; have to remember the old one,
	 * so XtDestroyWidget deletes the proper one */
	if (((Simple3dWidget)w)->simple3d.insensitive_border == None)
	    ((Simple3dWidget)w)->simple3d.insensitive_border =
		XmuCreateStippledPixmap(XtScreen(w),
					w->core.border_pixel, 
					w->core.background_pixel,
					w->core.depth);
        border_pixmap = w->core.border_pixmap;
	attributes->border_pixmap =
	  w->core.border_pixmap = ((Simple3dWidget)w)->simple3d.insensitive_border;

	*valueMask |= CWBorderPixmap;
	*valueMask &= ~CWBorderPixel;
    }

    ConvertCursor(w);

    if ((attributes->cursor = ((Simple3dWidget)w)->simple3d.cursor) != None)
	*valueMask |= CWCursor;

    XtCreateWindow( w, (unsigned int)InputOutput, (Visual *)CopyFromParent,
		    *valueMask, attributes );

    if (!XtIsSensitive(w))
	w->core.border_pixmap = border_pixmap;
}

/*	Function Name: ConvertCursor
 *	Description: Converts a name to a new cursor.
 *	Arguments: w - the simple3d widget.
 *	Returns: none.
 */

static void
ConvertCursor(w)
Widget w;
{
    Simple3dWidget simple3d = (Simple3dWidget) w;
    XrmValue from, to;
    Cursor cursor;
   
    if (simple3d->simple3d.cursor_name == NULL)
	return;

    from.addr = (XPointer) simple3d->simple3d.cursor_name;
    from.size = strlen((char *) from.addr) + 1;

    to.size = sizeof(Cursor);
    to.addr = (XPointer) &cursor;

    if (XtConvertAndStore(w, XtRString, &from, XtRColorCursor, &to)) {
	if ( cursor !=  None) 
	    simple3d->simple3d.cursor = cursor;
    } 
    else {
	XtAppErrorMsg(XtWidgetToApplicationContext(w),
		      "convertFailed","ConvertCursor","XawError",
		      "Simple3d: ConvertCursor failed.",
		      (String *)NULL, (Cardinal *)NULL);
    }
}


/* ARGSUSED */
static Boolean SetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
    Simple3dWidget s_old = (Simple3dWidget) current;
    Simple3dWidget s_new = (Simple3dWidget) new;
    Boolean new_cursor = FALSE;

    /* this disables user changes after creation*/
    s_new->simple3d.international = s_old->simple3d.international;

    if ( XtIsSensitive(current) != XtIsSensitive(new) )
	(*((Simple3dWidgetClass)XtClass(new))->
	     simple3d_class.change_sensitive) ( new );

    if (s_old->simple3d.cursor != s_new->simple3d.cursor) {
	new_cursor = TRUE;
    }
	
/*
 * We are not handling the string cursor_name correctly here.
 */

    if ( (s_old->simple3d.pointer_fg != s_new->simple3d.pointer_fg) ||
	(s_old->simple3d.pointer_bg != s_new->simple3d.pointer_bg) ||
	(s_old->simple3d.cursor_name != s_new->simple3d.cursor_name) ) {
	ConvertCursor(new);
	new_cursor = TRUE;
    }

    if (new_cursor && XtIsRealized(new))
        XDefineCursor(XtDisplay(new), XtWindow(new), s_new->simple3d.cursor);

    return False;   
}


static Boolean ChangeSensitive(w)
    Widget w;
{
    if (XtIsRealized(w)) {
	if (XtIsSensitive(w))
	    if (w->core.border_pixmap != XtUnspecifiedPixmap)
		XSetWindowBorderPixmap( XtDisplay(w), XtWindow(w),
				        w->core.border_pixmap );
	    else
		XSetWindowBorder( XtDisplay(w), XtWindow(w), 
				  w->core.border_pixel );
	else {
	    if (((Simple3dWidget)w)->simple3d.insensitive_border == None)
		((Simple3dWidget)w)->simple3d.insensitive_border =
		    XmuCreateStippledPixmap(XtScreen(w),
					    w->core.border_pixel, 
					    w->core.background_pixel,
					    w->core.depth);
	    XSetWindowBorderPixmap( XtDisplay(w), XtWindow(w),
				    ((Simple3dWidget)w)->
				        simple3d.insensitive_border );
	}
    }
    return False;
}
