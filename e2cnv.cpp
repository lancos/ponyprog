//=========================================================================//
//-------------------------------------------------------------------------//
// e2cnv.cpp -- Source for e2TextCanvasPane class                          //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997, 1998  Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lanconel@cs.unibo.it                                           //
//  http://www.cs.unibo.it/~lanconel                                       //
//                                                                         //
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

#include "e2cnv.h"
#include "e2cmdw.h"

//=================>>> e2TextCanvasPane::e2TextCanvasPane <<<================
e2TextCanvasPane::e2TextCanvasPane(e2CmdWindow* win)
{
	cmdWin = win;
}

//=================>>> e2TextCanvasPane::~e2TextCanvasPane <<<================
e2TextCanvasPane::~e2TextCanvasPane()
{
}

//=====================>>> e2TextCanvasPane::HPage <<<========================
void e2TextCanvasPane::HPage(int shown, int top)
{
	vTextCanvasPane::HPage(shown, top);
}

//=====================>>> e2TextCanvasPane::VPage <<<========================
void e2TextCanvasPane::VPage(int shown, int top)
{
	cmdWin->ScrollV(top);

	vTextCanvasPane::VPage(shown, top);
}

//====================>>> e2TextCanvasPane::HScroll <<<======================
void e2TextCanvasPane::HScroll(int step)
{
	vTextCanvasPane::HScroll(step);
}

//====================>>> e2TextCanvasPane::VScroll <<<======================
void e2TextCanvasPane::VScroll(int step)
{
	if (step > 0)
		cmdWin->NextLine();
	else
		cmdWin->PrevLine();

	vTextCanvasPane::VScroll(step);
}

//===================>>> e2TextCanvasPane::FontChanged <<<==================
void e2TextCanvasPane::FontChanged(vFont& newFont)
{
	vTextCanvasPane::FontChanged(newFont);
}

//===================>>> e2TextCanvasPane::TextMouseDown <<<==================
void e2TextCanvasPane::TextMouseDown(int row, int col, int button)
{
	if (button == 1)
	{
		cmdWin->CharEdit(row, col);
	}
	vTextCanvasPane::TextMouseDown(row,col,button);
}

//======================>>> e2TextCanvasPane::TextMouseUp <<<==================
void e2TextCanvasPane::TextMouseUp(int row, int col, int button)
{
	vTextCanvasPane::TextMouseUp(row,col,button);
}

//======================>>> e2TextCanvasPane::TextMouseMove <<<=================
void e2TextCanvasPane::TextMouseMove(int row, int col, int button)
{
	vTextCanvasPane::TextMouseMove(row,col,button);
}

//=========================>>> e2TextCanvasPane::Redraw <<<======================
void e2TextCanvasPane::Redraw(int x, int y, int w, int h)
{
	cmdWin->Draw();		//???

	vTextCanvasPane::Redraw(x,y,w,h);
}

//======================>>> e2TextCanvasPane::ResizeText <<<======================
void e2TextCanvasPane::ResizeText(const int rows, const int cols)
{
	cmdWin->Draw(rows, cols);

	vTextCanvasPane::ResizeText(rows,cols);
}

