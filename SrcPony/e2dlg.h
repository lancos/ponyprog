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
#include "usbwatcher.h"

class e2Dialog : public QDialog, public cTranslator, public Ui::E2Dialog
{
	Q_OBJECT

  public:
	e2Dialog(QWidget *bw, const QString title = translate(STR_MSGINTSETUP));
	//virtual ~e2Dialog();            // Destructor

  protected:

  public slots:
	void onUSB(bool connected, quint16 vid, quint16 pid);

  private slots:
	void onOk();
	void onTest();
	void on_tabWidget_currentChanged(int index);
	void on_cbxInterfCOM_currentIndexChanged(int index);
	void on_cbxInterfCOMNum_currentIndexChanged(int index);
	void on_cbxInterfLPT_currentIndexChanged(int index);
	void on_cbxInterfLPTNum_currentIndexChanged(int index);
	void on_cbxInterfUSB_currentIndexChanged(int index);
	void on_cbxInterfUSBNum_currentIndexChanged(int index);
	void on_cbxInterfGPIO_currentIndexChanged(int index);
	void on_cbxInterfGPIONum_currentIndexChanged(int index);
	void on_pushDefaultsUSB_clicked();
	void on_pushDefaultsCOM_clicked();
	void on_pushDefaultsLPT_clicked();
	void on_pushDefaultsGPIO_clicked();

private:
	int Test(int p = -1, bool open_only = false) const;
	void setWidgetsText();
	void getSettings();
	void setSettings();

	void recurseCbxHide(QObject *object);

	USBWatcher watcher;
	QStringList lptList;
	QStringList comList;
	QStringList usbList;
	QStringList gpioList;

	int port_no;                    //Index of selected port in portList
	int lpt_no, com_no, usb_no, gpio_no;
	VidPid usb_vp;
	HInterfaceType interf_type;
};

#endif
