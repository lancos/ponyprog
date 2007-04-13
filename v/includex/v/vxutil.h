//===============================================================
// vxutil.h - utility macros for Motif/Athena duality
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VXUTIL_H
#define VXUTIL_H

#ifndef Athena	// ---------------------------------- Motif ----

#define Nbelow(x) XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, x,
#define NrightOf(x) XmNleftAttachment, XmATTACH_WIDGET, XmNleftWidget, x,

#else	//------------------------------------------ Athena ----

#define Nbelow(x) XtNfromVert, x,
#define NrightOf(x)  XtNfromHoriz, x,

#endif	// -----------------------------------------------------

#endif
