/*
 *	Functions for drawing String's with tab characters in them
 */

#if (NeedFunctionPrototypes > 0)

extern void	XfwfDrawImageString(Display *display, Drawable drawable,
			GC gc, XFontStruct *font, int x, int y, String string, int length,
			int *tabs, char *underline);
extern void     XfwfDrawString(Display *display, Drawable drawable,
			GC gc, XFontStruct *font, int x, int y, String string, int length,
			int *tabs, char *underline);
extern void	XtabDrawString(Display *display, Drawable drawable, GC gc,
			int x, int y, String string, int length, int *tabs);
extern int *	XfwfTablist2Tabs(char *tablist);
extern int	XfwfTextWidth(XFontStruct *font, String str, int length,
			int *tabs);
extern char *	strnchr(char *s, int c, int n);

#else

extern void	XfwfDrawImageString();
extern void	XfwfDrawString();
extern void	XtabDrawString();
extern int *	XfwfTablist2Tabs();
extern int	XfwfTextWidth();
extern char *	strnchr();

#endif

