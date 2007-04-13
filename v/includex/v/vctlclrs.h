/*===============================================================
// v3dcolrs.h -- Centarl definitions of colors used to draw 3d stuff
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
// 
// Note: this code is used by come C code, so needs old style comments
//=============================================================== */

#ifndef V3DCOLRS_H
#define V3DCOLRS_H

extern "C"
{
#include <X11/Intrinsic.h>
}

  extern Pixel _vStatusBarBG;	/* Background of status bar */
  extern Pixel _vDialogBG;       /* Background for dialogs/command bar */
  extern Pixel _vMenuBarBG;      /* Background for MenuBar */
  extern Pixel _vControlBG;	/* Background for Controls */
  extern Pixel _vControlFace;	/* Face of controls */
  extern Pixel _vLightControlShadow; /* light control shadow */
  extern Pixel _vDarkControlShadow;  /* dark control shadow */
#endif
