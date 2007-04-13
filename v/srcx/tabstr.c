/*
 *	Functions for drawing String's with tab characters in them
 */

#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "tabstr.h"

/*
 *	Like DrawImageString, except it takes an additional  "tabs"
 *	argument, used to specify what horizontal pixel position to
 *	move to when tab characters are present in the string.  If
 *	the "tabs" argument is NULL, works exactly like its
 *	counterpart.
 */
void
XfwfDrawImageString(display, drawable, gc, font, x, y, string, length, tabs, underline)
     Display *display;
     Drawable drawable;
     GC gc;
     XFontStruct *font;
     int x;
     int y;
     String string;
     int length;
     int *tabs;
     char *underline;
{
    register char	*p, *ep;
    register int	tx, tab;

    tab = tx = 0;
    for (p = string; length; ) {
	ep = strnchr(p, '\t', length);
	if (underline && p <= underline && underline < ((ep && tabs) ? ep : p+length)) {
	    int _x = 0, _wd = 0;
	    if (p != underline) {
		int direction, ascent, descent; XCharStruct overall;
		XTextExtents(font, p, underline - p, &direction, &ascent, &descent, &overall);
		_x = overall.width;
	    }
	    _wd = XTextWidth(font, underline, 1);
	    XDrawLine(display, drawable, gc,
		      x+tx + _x      , y + font->max_bounds.descent - 1,
		      x+tx + _x + _wd, y + font->max_bounds.descent - 1);
	}
	if (ep && tabs) {
	    XDrawImageString(display, drawable, gc, x+tx, y, p, ep - p);
	    tx = tabs[tab++];
	    length -= ep - p + 1;
	    p = ep + 1;
	} else {
	    XDrawImageString(display, drawable, gc, x+tx, y, p, length);
	    break;
	}
    }
}

/*
 *	Like DrawString, except it takes an additional  "tabs"
 *	argument, used to specify what horizontal pixel position to
 *	move to when tab characters are present in the string.  If
 *	the "tabs" argument is NULL, works exactly like its
 *	counterpart.
 */
void XfwfDrawString(display, drawable, gc, font, x, y, string, length, tabs, underline)
     Display *display;
     Drawable drawable;
     GC gc;
     XFontStruct *font;
     int x;
     int y;
     String string;
     int length;
     int *tabs;
     char *underline;
{
    register char *p, *ep;
    register int  tx, tab;
    
    tab = tx = 0;
    for (p = string; length; ) {
	ep = strnchr(p, '\t', length);
	if (underline && p <= underline && underline < ((ep && tabs) ? ep : p+length)) {
	    int _x = 0, _wd = 0;
	    if (p != underline) {
		int direction, ascent, descent; XCharStruct overall;
		XTextExtents(font, p, underline - p, &direction, &ascent, &descent, &overall);
		_x = overall.width;
	    }
	    _wd = XTextWidth(font, underline, 1);
	    XDrawLine(display, drawable, gc,
		      x+tx + _x      , y + font->max_bounds.descent - 1,
		      x+tx + _x + _wd, y + font->max_bounds.descent - 1);
	}
	if (ep && tabs) {
	    XDrawString(display, drawable, gc, x+tx, y, p, ep - p);
	    tx = tabs[tab++];
	    length -= ep - p + 1;
	    p = ep + 1;
	} else {
	    XDrawString(display, drawable, gc, x+tx, y, p, length);
	    break;
	}
    }
}

/*
 *	Converts a string list of tabs to an array of tabs
 */
int *XfwfTablist2Tabs(tablist)
    char *tablist;
{
	register int	*tabs = NULL;
	register int	ntabs = 0;

	if (!tablist)
		return NULL;
	for (;;)
	{
		/* Skip leading blanks */
		while (*tablist && *tablist == ' ') ++tablist;
		if (!*tablist) break;

		/* Allocate space for the new tab */
		if (ntabs)
			tabs = (int *) XtRealloc( (char *) tabs,
						(ntabs+1) * sizeof(int));
		else
			tabs = (int *) XtMalloc( (ntabs + 1) * sizeof(int));
		/* Add it to the list */
		tabs[ntabs++] = atoi(tablist);
		/* Skip to the next blank */
		while (*tablist && *tablist != ' ') ++tablist;
	}
	return (tabs);
}

/*
 *	Like TextWidth, except it takes an additional  "tabs"
 *	argument, used to specify what horizontal pixel position to
 *	move to when tab characters are present in the string.  If
 *	the "tabs" argument is NULL, works exactly like its
 *	counterpart.
 */
int
XfwfTextWidth(font, str, length, tabs)
     XFontStruct *font;
     String str;
     int length;
     int *tabs;
{
	register char	*p, *ep;
	register int	tx, tab, rc;

	tab = tx = 0;
 	if (length == 0) return 0;
	for (p = str; length; )
	{
		ep = strnchr(p, '\t', length);
		if (ep && tabs)
		{
			tx = tabs[tab++];
			length -= ep - p + 1;
			p = ep + 1;
		}
		else
		{
			rc = XTextWidth(font, p, length);
			if (rc < 0) return rc; else return rc + tx;
		}
	}
	return -1;
}

/*
 *	Like strchr, except has a length limit.
 */
char *
strnchr(s, c, n)
     char *s;
     int c;
     int n;
{
	while (n--)
		if (*s == c) return s; else ++s;
	return NULL;
}
