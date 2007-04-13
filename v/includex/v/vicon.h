//===============================================================
// vicon.h - Class for Icons
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VICON_H
#define VICON_H

#include <v/vapp.h>

    enum IconType {Normal, Transparent};        // types of icons

    class vIcon	// an icon
      {
	friend class vCanvasPaneDC;
	friend class vLabelCmd;
        friend class vButtonCmd;

      public:		//---------------------------------------- public
	vIcon(unsigned char* ic, int h, int w, int d = 1, IconType it = Normal);
	~vIcon();

	IconType iType;		// type of icon
	int height;		// height in pixels
	int width;		// width in pixels
	int depth;		// bits per pixel
	unsigned char* icon;	// ptr to icon array

	Pixmap GetXPM(int cmd = 0, unsigned long pbg = 0) VCONST;	// X version
	Pixmap GetInsensXPM(int cmd = 0, unsigned long pbg = 0) VCONST;	// X version

      protected:	//--------------------------------------- protected
      private:		//--------------------------------------- private
	VMUTABLE Pixmap _pm;
	VMUTABLE Pixmap _inspm;
	static GC _GC;
	static GC _insGC;
	int _cmd;

      };
#endif
