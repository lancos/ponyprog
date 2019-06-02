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

class e2CmdWindow;

e2Dialog::e2Dialog(QWidget *bw, const QString title)
	: QDialog(bw),
	  lpt_no(2),
	  com_no(3)
{
	setupUi(this);

	setWindowTitle(title);

	e2CmdWindow *cmdw = static_cast<e2CmdWindow *>(bw);

	if (cmdw->getStyleSheet().length() > 0)
	{
		setStyleSheet(cmdw->getStyleSheet());
	}

	qDebug() << "e2Dialog::e2Dialog()";

	usbspeedItems = (QStringList() << "Slow" << "Normal" << "Fast"  << "High" << "Highest");
	cbxUSBspeed->addItems(usbspeedItems);

	setWidgetsText();

	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onChangeMain(int)));

	// selection of subtype
	connect(cbxInterfType, SIGNAL(currentIndexChanged(int)), this, SLOT(onChangeType(int)));

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(pushTest, SIGNAL(clicked(bool)), this, SLOT(onTest()));

	getSettings();
}


e2Dialog::~e2Dialog()
{
	qDebug() << "e2Dialog::~e2Dialog()";
}

extern HInterfaceType VindexToInterfType(int vector, int index);
extern QStringList GetInterfList(int vector);

/**
 * @brief   slot for selection of main interface type: TTY/LPT/CH341A
 *
 */
void e2Dialog::onChangeMain(int i)
{
	interf_type = VindexToInterfType(i, 0);
	qDebug() << "IntefType: " << (int)interf_type << ", index = " << i;

	disconnect(cbxInterfType, SIGNAL(currentIndexChanged(int)), this, SLOT(onChangeType(int)));
	// rebuild the list
	cbxInterfType->clear();

	QStringList interfList = GetInterfList(i);
	cbxInterfType->addItems(interfList);

	connect(cbxInterfType, SIGNAL(currentIndexChanged(int)), this, SLOT(onChangeType(int)));

	onChangeType(0);
}
/**
 * @brief   slot for selection of subtype
 *
 */
void e2Dialog::onChangeType(int i)
{
	int m = tabWidget->currentIndex();

	interf_type = VindexToInterfType(m, i);
	qDebug() << "IntefType: " << (int)interf_type << ", subindex = " << i;
}

extern int TypeToInterfVector(HInterfaceType type);
extern int TypeToInterfIndex(HInterfaceType type);

void e2Dialog::getSettings()
{
	interf_type = cmdWin->GetInterfaceType();

	if (interf_type < SIPROG_API || interf_type >= LAST_HT)
	{
		interf_type = SIPROG_API;
	}

	port_no = cmdWin->GetPort();

	int m = TypeToInterfVector(interf_type);

	int t = TypeToInterfIndex(interf_type);

	tabWidget->setCurrentIndex(m);

	switch (m)
	{
	case 0:		//COM
	{
		com_no = port_no;

		if (com_no >= cbxCOMNum->count())
		{
			com_no = 0;
			port_no = 0;
		}

		cbxCOMNum->setCurrentIndex(com_no);
		break;
	}

	case 1:
	{
		lpt_no = port_no;

		if (lpt_no >= cbxLPTNum->count())
		{
			lpt_no = 0;
			port_no = 0;
		}
		cbxLPTNum->setCurrentIndex(lpt_no);
		break;
	}

	case 2: // USB
	{
		port_no = 0; // auto
		int usb_speed = E2Profile::GetUSBSpeed();
		cbxUSBspeed->setCurrentIndex(usb_speed);
		break;
	}
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

	int usb_speed = cbxUSBspeed->currentIndex();
	E2Profile::SetUSBSpeed(usb_speed);

	E2Profile::SetPolarityControl(cmdWin->GetPolarity());

	qDebug() << "PortNo: " << port_no;
}


void e2Dialog::setWidgetsText()
{
	labelType->setText(translate(STR_LBLINTERFTYPE));
	labelCOMNum->setText(translate(STR_LBLNUM));
	labelLPTNum->setText(translate(STR_LBLNUM));
	labelUSBspeed->setText(translate(STR_USBSPEED));

	QStringList mainList;
#ifdef Q_OS_WIN32
	mainList << "COM" << "LPT" << "USB";
#else
	mainList << "TTY" << "ParPort" << "USB";
#endif
	for (int i = 0; i < mainList.count(); i++)
	{
		tabWidget->setTabText(i, mainList.at(i));
	}

	lptList = E2Profile::GetLPTDevList();
	comList = E2Profile::GetCOMDevList();

	cbxCOMNum->addItems(comList);
	cbxLPTNum->addItems(lptList);

	int idx = tabWidget->currentIndex();
	QStringList interfList = GetInterfList(idx);
	cbxInterfType->addItems(interfList);

	grpPolsel->setTitle(translate(STR_LBLSELPOLARITY));

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

