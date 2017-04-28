//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: progoption.h,v 1.2 2007/04/20 10:58:23 lancos Exp $
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

#ifndef progOPTION_H
#define progOPTION_H

#include <QObject>
#include <QDialog>
#include <QCheckBox>
#include <QVector>
#include <QString>

#include "ui_progoption.h"

#include "e2cmdw.h"
#include "string_table.h"

using namespace Translator;

// TODO to check modal
class progOptionDialog : public QDialog, public Ui::ProgOptDialog
{
	Q_OBJECT
  public:               //---------------------------------------- public
	progOptionDialog(QWidget *bw, long type, int &reload, int &reep, int &erase, int &flash, int &eeprom, int &lock, const QString title = STR_MSGPROGOPT);
	virtual ~progOptionDialog();            // Destructor

  private slots:
	void onOk();

  protected:    //--------------------------------------- protected

  private:
	void setTextWidgets();

  private:              //--------------------------------------- private
	QVector <QCheckBox *> chk;
	e2CmdWindow *cmdw;
};
#endif

