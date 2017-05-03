//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: e2cnv.h,v 1.2 2007/04/20 10:58:23 lancos Exp $
//-------------------------------------------------------------------------//
//                                                                         //
// This program is free software; you can redistribute it and/or           //
// modify it under the terms of the GNU  General Public License            //
// as published by the Free Software Foundation; either version2 of        //
// the License, or (at your option) any later version.                     //
//                                                                         //
// This program is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
// General Public License for more details.                                //
//                                                                         //
// You should have received a copy of the GNU  General Public License      //
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//

#ifndef e2TCNV_H
#define e2TCNV_H

// #include <v/vtextcnv.h>

class e2CmdWindow;

class e2TextCanvasPane : public vTextCanvasPane
{
  public:               //---------------------------------------- public
	e2TextCanvasPane(e2CmdWindow *win);
	virtual ~e2TextCanvasPane();

	// Scrolling
	virtual void HPage(int, int);
	virtual void VPage(int, int);

	virtual void HScroll(int);
	virtual void VScroll(int);

	// Events
	virtual void FontChanged(vFont &newFont);
	virtual void ResizeText(const int rows, const int cols);
	virtual void Redraw(int x, int y, int w , int h);
	virtual void TextMouseDown(int row, int col, int button);
	virtual void TextMouseUp(int row, int col, int button);
	virtual void TextMouseMove(int row, int col, int button);

  protected:    //--------------------------------------- protected
  private:              //--------------------------------------- private
	e2CmdWindow *cmdWin;
};
#endif

