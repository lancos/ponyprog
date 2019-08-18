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

#include <QMessageBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QStringList>
#include <QDebug>

#include "e2dlg.h"
#include "e2cmdw.h"
#include "eeptypes.h"
#include "types.h"
#include "i2cbus.h"
#include "interfconv.h"

class e2CmdWindow;

e2Dialog::e2Dialog(QWidget *bw, const QString title)
	: QDialog(bw),
	  port_no(0),
	  lpt_no(2),
	  com_no(3),
	  usb_no(0),
	  usb_vp(0),
	  interf_type(SIPROG_API)
{
	qDebug() << "e2Dialog::e2Dialog()";

	setupUi(this);

	setWindowTitle(title);

	e2CmdWindow *cmdw = static_cast<e2CmdWindow *>(bw);

	if (cmdw->getStyleSheet().length() > 0)
	{
		setStyleSheet(cmdw->getStyleSheet());
	}

	setWidgetsText();		//NB Must be called before getSettings()

	//connect(rdbComPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));
	//connect(rdbLptPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));

	//connect(cbxInterfCOM, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectCOM(int)));
	//connect(cbxInterfLPT, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectLPT(int)));

	//connect(cbxInterfCOMNum, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectCOMNum(int)));

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(pushTest, SIGNAL(clicked(bool)), this, SLOT(onTest()));

	getSettings();
}

e2Dialog::~e2Dialog()
{
	qDebug() << "e2Dialog::~e2Dialog()";
}

void e2Dialog::getSettings()
{
	interf_type = cmdWin->GetInterfaceType();

	if (interf_type < SIPROG_API || interf_type >= LAST_HT)
	{
		interf_type = SIPROG_API;
	}

	port_no = cmdWin->GetPort();

	if (TypeToInterfVector(interf_type) == INTERF_COM)		//COM
	{
		com_no = port_no;
		if (com_no >= comList.count())
		{
			com_no = 0;
			port_no = 0;
		}
		cbxInterfCOMNum->setCurrentIndex(com_no);
		cbxInterfCOM->setCurrentIndex(TypeToInterfIndex(interf_type));

		if (tabWidget->currentIndex() != INTERF_COM)
			tabWidget->setCurrentIndex(INTERF_COM);
		else
			on_tabWidget_currentChanged(INTERF_COM);
	}
	else if (TypeToInterfVector(interf_type) == INTERF_LPT)
	{
		lpt_no = port_no;
		if (lpt_no >= lptList.count())
		{
			lpt_no = 0;
			port_no = 0;
		}
		cbxInterfLPTNum->setCurrentIndex(lpt_no);
		cbxInterfLPT->setCurrentIndex(TypeToInterfIndex(interf_type));

		if (tabWidget->currentIndex() != INTERF_LPT)
			tabWidget->setCurrentIndex(INTERF_LPT);
		else
			on_tabWidget_currentChanged(INTERF_LPT);
	}
	else if (TypeToInterfVector(interf_type) == INTERF_USB)
	{
		usb_no = port_no;
		//usb_vp = TypeToInterfVidPid(interf_type);
		//watcher.hotplug_register(usb_vp.vid, usb_vp.pid);
		//usbList = MpsseInterface::find_all(usb_vp.vid, usb_vp.pid);

		cbxInterfUSB->setCurrentIndex(TypeToInterfIndex(interf_type));
		cbxInterfUSBNum->setCurrentIndex(usb_no);

		if (tabWidget->currentIndex() != INTERF_USB)
			tabWidget->setCurrentIndex(INTERF_USB);
		else
			on_tabWidget_currentChanged(INTERF_USB);
	}

	chkPol1->setChecked((cmdWin->GetPolarity() & RESETINV) ? 1 : 0);
	chkPol2->setChecked((cmdWin->GetPolarity() & CLOCKINV) ? 1 : 0);
	chkPol3->setChecked((cmdWin->GetPolarity() & DININV) ? 1 : 0);
	chkPol4->setChecked((cmdWin->GetPolarity() & DOUTINV) ? 1 : 0);
}

void e2Dialog::setSettings()
{
	// *** Add code to process dialog values here
	if (chkPol1->isChecked())
	{
		cmdWin->SetPolarity(cmdWin->GetPolarity() | (uint8_t)RESETINV);
	}
	else
	{
		cmdWin->SetPolarity(cmdWin->GetPolarity() & (uint8_t)~RESETINV);
	}

	if (chkPol2->isChecked())
	{
		cmdWin->SetPolarity(cmdWin->GetPolarity() | (uint8_t)CLOCKINV);
	}
	else
	{
		cmdWin->SetPolarity(cmdWin->GetPolarity() & (uint8_t)~CLOCKINV);
	}

	if (chkPol3->isChecked())
	{
		cmdWin->SetPolarity(cmdWin->GetPolarity() | (uint8_t)DININV);
	}
	else
	{
		cmdWin->SetPolarity(cmdWin->GetPolarity() & (uint8_t)~DININV);
	}

	if (chkPol4->isChecked())
	{
		cmdWin->SetPolarity(cmdWin->GetPolarity() | (uint8_t)DOUTINV);
	}
	else
	{
		cmdWin->SetPolarity(cmdWin->GetPolarity() & (uint8_t)~DOUTINV);
	}

	if (interf_type != cmdWin->GetInterfaceType())
	{
		cmdWin->ClosePort();
		cmdWin->SetInterfaceType(interf_type);
	}
	cmdWin->SetPort(port_no);

	//Store values in the INI file
	E2Profile::SetParInterfType(interf_type);
	E2Profile::SetPortNumber(port_no);
	E2Profile::SetPolarityControl(cmdWin->GetPolarity());

	qDebug() << "PortNo: " << port_no;
}

void e2Dialog::setWidgetsText()
{
	//Serial interfaces list
	QStringList interfListC = GetInterfList(INTERF_COM);
	cbxInterfCOM->addItems(interfListC);

	//Parallel interfaces list
	QStringList interfListL = GetInterfList(INTERF_LPT);
	cbxInterfLPT->addItems(interfListL);

	QStringList interfListU = GetInterfList(INTERF_USB);
	cbxInterfUSB->addItems(interfListU);

	//rdbComPort->setText(translate(STR_LBLSERIAL));
	//rdbLptPort->setText(translate(STR_LBLPARALLEL));

	lptList = E2Profile::GetLPTDevList();
	comList = E2Profile::GetCOMDevList();
	usbList.clear();

	cbxInterfCOMNum->clear();
	cbxInterfCOMNum->addItems(comList);
	cbxInterfLPTNum->clear();
	cbxInterfLPTNum->addItems(lptList);
	cbxInterfUSBNum->clear();
	cbxInterfUSBNum->addItems(usbList);

	lblPol1->setText(translate(STR_LBLSELPOLARITY));

	chkPol1->setText(translate(STR_LBLINVRESET));
	chkPol2->setText(translate(STR_LBLINVSCK));
	chkPol3->setText(translate(STR_LBLINVDATAIN));
	chkPol4->setText(translate(STR_LBLINVDATAOUT));

	pushOk->setText(translate(STR_BTNOK));
	pushTest->setText(translate(STR_BTNPROBE));
	pushCancel->setText(translate(STR_BTNCANC));
}


void e2Dialog::onOk()
{
	setSettings();

	accept();
}

void e2Dialog::onTest()
{
	int test = Test();

	if (test)
	{
		QMessageBox msgBox(QMessageBox::Critical, "Failed", translate(STR_TEST) + " " + translate(STR_MSGFAILED), QMessageBox::Ok);
		msgBox.setStyleSheet(cmdWin->getStyleSheet());
		msgBox.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
		msgBox.exec();
	}
	else
	{
		QMessageBox msgBox(QMessageBox::Information, "Info",  translate(STR_TEST) + " " + translate(STR_OK), QMessageBox::Ok);
		msgBox.setStyleSheet(cmdWin->getStyleSheet());
		msgBox.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
		msgBox.exec();
	}
}


int e2Dialog::Test(int p, bool open_only) const
{
	qDebug() << "e2Dialog::Test() IN *** p=" << p << ", open_only=" << open_only;

	HInterfaceType old_interf = cmdWin->GetInterfaceType();
	int test;

	if (p < 0)
	{
		p = port_no;
	}

	qDebug() << "e2Dialog::Test() old_interf = " << (int)old_interf << ", interf = " << (int)interf_type;

	if (interf_type != old_interf)
	{
		cmdWin->SetInterfaceType(interf_type);
		test = cmdWin->TestPort(p, open_only);
		cmdWin->SetInterfaceType(old_interf);
	}
	else
	{
		test = cmdWin->TestPort(p, open_only);
	}


	qDebug() << "e2Dialog::Test() = " << test << " *** OUT";

	return test;
}

void e2Dialog::on_tabWidget_currentChanged(int index)
{
	if (index == INTERF_COM)
	{
		qDebug() << "COM Port selected: " << com_no;

		port_no = com_no;
		cbxInterfCOMNum->setCurrentIndex(com_no);

		interf_type = VindexToInterfType(INTERF_COM, cbxInterfCOM->currentIndex());
	}
	else if (index == INTERF_LPT)
	{
		qDebug() << "LPT Port selected: " << lpt_no;

		port_no = lpt_no;
		cbxInterfLPTNum->setCurrentIndex(lpt_no);

		interf_type = VindexToInterfType(INTERF_LPT, cbxInterfLPT->currentIndex());
	}
	else if (index == INTERF_USB)
	{
		qDebug() << "USB Port selected: " << usb_no;

		//usb_vp = TypeToInterfVidPid(interf_type);
		//watcher.hotplug_register(usb_vp.vid, usb_vp.pid);
		//usbList = MpsseInterface::find_all(usb_vp.vid, usb_vp.pid);

		int idx = cbxInterfUSB->currentIndex();
		on_cbxInterfUSB_currentIndexChanged(idx);
		port_no = usb_no;
		cbxInterfUSBNum->setCurrentIndex(port_no);
	}
}

void e2Dialog::on_cbxInterfCOM_currentIndexChanged(int index)
{
	interf_type = VindexToInterfType(INTERF_COM, index);
	qDebug() << "Selected IntefType: " << (int)interf_type << ", index = " << index << ", COM";
}

void e2Dialog::on_cbxInterfLPT_currentIndexChanged(int index)
{
	interf_type = VindexToInterfType(INTERF_LPT, index);
	qDebug() << "Selected IntefType: " << (int)interf_type << ", index = " << index << ", LPT";
}

void e2Dialog::on_cbxInterfUSB_currentIndexChanged(int index)
{
	interf_type = VindexToInterfType(INTERF_USB, index);
	qDebug() << "Selected IntefType: " << (int)interf_type << ", index = " << index << ", USB";
	usb_vp = TypeToInterfVidPid(interf_type);
	watcher.hotplug_register(usb_vp.vid, usb_vp.pid);
	usbList = MpsseInterface::find_all(usb_vp.vid, usb_vp.pid);
	cbxInterfUSBNum->clear();
	cbxInterfUSBNum->addItems(usbList);
}

void e2Dialog::on_cbxInterfCOMNum_currentIndexChanged(int index)
{
	if (index >= 0)
	{
		port_no = com_no = index;
		qDebug() << "Selected COM port " << port_no;
	}
}

void e2Dialog::on_cbxInterfLPTNum_currentIndexChanged(int index)
{
	if (index >= 0)
	{
		port_no = lpt_no = index;
		qDebug() << "Selected LPT port " << port_no;
	}
}

void e2Dialog::on_cbxInterfUSBNum_currentIndexChanged(int index)
{
	if (index >= 0)
	{
		port_no = usb_no = index;
		qDebug() << "Selected USB port " << port_no;
	}
}
