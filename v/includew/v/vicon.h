//===============================================================
// vicon.h - Icon class - Windows
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
#include <windows.h>
#include <v/v_defs.h>


    enum IconType {Normal, Transparent};	// types of icons

    class V_EXPORT vIcon	// an icon
      {
	//friend class vWinDC;
	//friend class vLabelCmd;
	//friend class vButtonCmd;

      public:		//---------------------------------------- public
	vIcon();
	vIcon(unsigned char* ic, int h, int w, int d = 1, IconType it = Normal);
	~vIcon();
	IconType iType;		// type of icon
	int height;		// height in pixels
	int width;		// width in pixels
	int depth;		// bits per pixel
	unsigned char* icon;	// ptr to icon array

	HBITMAP GetIconHBM(HDC iDC) VCONST 
	  {
	    if (_hbm == 0) vIconToBMP(iDC); return _hbm;
	  }
	HBITMAP GetIconMask(HDC iDC) VCONST
	  {
	    if ( _hmsk == 0 && depth > 1 ) vIconToBMP(iDC);
	    return _hmsk;
	  }

      protected:	//--------------------------------------- protected
      private:		//--------------------------------------- private

	int NeedBMPBytes() VCONST;
	void vIconToBMP(HDC iDC) VCONST;

	VMUTABLE HBITMAP _hbm, _hmsk;
      };
#endif
