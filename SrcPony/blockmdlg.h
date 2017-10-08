//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
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
// along with this program (see LICENSE);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//=========================================================================//

#ifndef BLOCKMDLG_H
#define BLOCKMDLG_H

// #include <QDialog>
#include <QObject>
#include <QString>
// #include <QVector>
// #include <QCheckBox>

#include "ui_editdlg.h"

#include "e2cmdw.h"


class blockDialog : public QDialog, public cTranslator, public Ui::EditDialog
{
	Q_OBJECT
  public:               //---------------------------------------- public
	blockDialog(e2CmdWindow *bw, e2AppWinInfo *p, bool readonly = false, const QString &msg = "");
	virtual ~blockDialog();          // Destructor

  protected:    //--------------------------------------- protected

  private slots:
	void onOk();

  private:              //--------------------------------------- private
	e2AppWinInfo *awip;
	uint32_t lock, fuse;
};

#endif
