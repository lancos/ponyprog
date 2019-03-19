//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2019   Claudio Lanconelli                           //
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

#ifndef e2DIALOG_H
#define e2DIALOG_H

#include <QDialog>
#include <QWidget>
#include <QObject>


#include "Translator.h"
#include "globals.h"

#include "ui_e2dlg.h"

class e2Dialog : public QDialog, public cTranslator, public Ui::E2Dialog
{
	Q_OBJECT

  public:
	e2Dialog(QWidget *bw, const QString title = translate(STR_MSGINTSETUP));
	virtual ~e2Dialog();            // Destructor

  protected:

  private slots:
	void onOk();
	void onTest();
	void onChangeMain(int i);
	void onChangeType(int i);
	void onChangePortNum(int i);

  private:
	int Test(int p = -1, bool open_only = false) const;
	void setWidgetsText();
	void getSettings();
	void setSettings();
	void activateTTYSettings(bool set);

	QStringList lptList;
	QStringList comList;

	int port_no;                    //Idex of selected port in portList
	int lpt_no, com_no;
	HInterfaceType interf_type;
};

#endif
