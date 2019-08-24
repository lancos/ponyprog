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
	  gpio_no(0),
	  usb_vp(0),
	  interf_type(SIPROG_API)
{
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

	connect(&watcher, SIGNAL(notify(bool, quint16, quint16)),
			this, SLOT(onUSB(bool, quint16, quint16)));

	getSettings();
}

void e2Dialog::getSettings()
{
	E2Profile::readDialogSettings(this);

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
		{
			tabWidget->setCurrentIndex(INTERF_COM);
		}
		else
		{
			on_tabWidget_currentChanged(INTERF_COM);
		}
		ckControlOutCOM->setChecked((cmdWin->GetPolarity() & RESETINV) ? true : false);
		ckClockOutCOM->setChecked((cmdWin->GetPolarity() & CLOCKINV) ? true : false);
		ckDataInCOM->setChecked((cmdWin->GetPolarity() & DININV) ? true : false);
		ckDataOutCOM->setChecked((cmdWin->GetPolarity() & DOUTINV) ? true : false);
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
		{
			tabWidget->setCurrentIndex(INTERF_LPT);
		}
		else
		{
			on_tabWidget_currentChanged(INTERF_LPT);
		}
		ckControlOutLPT->setChecked((cmdWin->GetPolarity() & RESETINV) ? true : false);
		ckClockOutLPT->setChecked((cmdWin->GetPolarity() & CLOCKINV) ? true : false);
		ckDataInLPT->setChecked((cmdWin->GetPolarity() & DININV) ? true : false);
		ckDataOutLPT->setChecked((cmdWin->GetPolarity() & DOUTINV) ? true : false);
		//ckPowerOnLPT->setChecked((cmdWin->GetPolarity() & POWERINV) ? true : false);
		//ckEnaODLPT->setChecked((cmdWin->GetPolarity() & ENBUSINV) ? true : false);
		ckClockInLPT->setChecked((cmdWin->GetPolarity() & CLOCKININV) ? true : false);
		//ckControlInLPT->setChecked((cmdWin->GetPolarity() & CTRLININV) ? true : false);
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
		{
			tabWidget->setCurrentIndex(INTERF_USB);
		}
		else
		{
			on_tabWidget_currentChanged(INTERF_USB);
		}
		ckControlOutUSB->setChecked((cmdWin->GetPolarity() & RESETINV) ? true : false);
		ckClockOutUSB->setChecked((cmdWin->GetPolarity() & CLOCKINV) ? true : false);
		ckDataInUSB->setChecked((cmdWin->GetPolarity() & DININV) ? true : false);
		ckDataOutUSB->setChecked((cmdWin->GetPolarity() & DOUTINV) ? true : false);
		ckPowerOnUSB->setChecked((cmdWin->GetPolarity() & POWERINV) ? true : false);
		ckEnaODUSB->setChecked((cmdWin->GetPolarity() & ENBUSINV) ? true : false);
		ckClockInUSB->setChecked((cmdWin->GetPolarity() & CLOCKININV) ? true : false);
		ckControlInUSB->setChecked((cmdWin->GetPolarity() & CTRLININV) ? true : false);
	}
	else if (TypeToInterfVector(interf_type) == INTERF_GPIO)
	{
		gpio_no = port_no;
		if (gpio_no >= gpioList.count())
		{
			gpio_no = 0;
			port_no = 0;
		}
		cbxInterfGPIONum->setCurrentIndex(gpio_no);
		cbxInterfGPIO->setCurrentIndex(TypeToInterfIndex(interf_type));

		if (tabWidget->currentIndex() != INTERF_GPIO)
		{
			tabWidget->setCurrentIndex(INTERF_GPIO);
		}
		else
		{
			on_tabWidget_currentChanged(INTERF_GPIO);
		}
		ckControlOutGPIO->setChecked((cmdWin->GetPolarity() & RESETINV) ? true : false);
		ckClockOutGPIO->setChecked((cmdWin->GetPolarity() & CLOCKINV) ? true : false);
		ckDataInGPIO->setChecked((cmdWin->GetPolarity() & DININV) ? true : false);
		ckDataOutGPIO->setChecked((cmdWin->GetPolarity() & DOUTINV) ? true : false);
		ckPowerOnGPIO->setChecked((cmdWin->GetPolarity() & POWERINV) ? true : false);
		ckEnaODGPIO->setChecked((cmdWin->GetPolarity() & ENBUSINV) ? true : false);
		ckClockInGPIO->setChecked((cmdWin->GetPolarity() & CLOCKININV) ? true : false);
		ckControlInGPIO->setChecked((cmdWin->GetPolarity() & CTRLININV) ? true : false);
	}

	//TODO: A way to detect if run on RaspberryPi
	//if (QSysInfo::currentCpuArchitecture().startsWith("arm"))
	cbxInterfGPIONum->setEnabled(false);
}

void e2Dialog::setSettings()
{
	E2Profile::writeDialogSettings(this);
	InterfPins pins = {-1, -1, -1, -1, -1, -1, -1, -1};

	unsigned int pol = 0;
	switch (TypeToInterfVector(interf_type))
	{
	case INTERF_COM:
		if (ckControlOutCOM->isChecked())
		{
			pol |= RESETINV;
		}
		if (ckClockOutCOM->isChecked())
		{
			pol |= CLOCKINV;
		}
		if (ckDataInCOM->isChecked())
		{
			pol |= DININV;
		}
		if (ckDataOutCOM->isChecked())
		{
			pol |= DOUTINV;
		}
		break;
	case INTERF_LPT:
		if (ckControlOutLPT->isChecked())
		{
			pol |= RESETINV;
		}
		if (ckClockOutLPT->isChecked())
		{
			pol |= CLOCKINV;
		}
		if (ckDataInLPT->isChecked())
		{
			pol |= DININV;
		}
		if (ckDataOutLPT->isChecked())
		{
			pol |= DOUTINV;
		}
		if (ckClockInLPT->isChecked())
		{
			pol |= CLOCKININV;
		}
		break;
	case INTERF_USB:
		if (ckControlOutUSB->isChecked())
		{
			pol |= RESETINV;
		}
		if (ckClockOutUSB->isChecked())
		{
			pol |= CLOCKINV;
		}
		if (ckDataInUSB->isChecked())
		{
			pol |= DININV;
		}
		if (ckDataOutUSB->isChecked())
		{
			pol |= DOUTINV;
		}
		if (ckPowerOnUSB->isChecked())
		{
			pol |= POWERINV;
		}
		if (ckEnaODUSB->isChecked())
		{
			pol |= ENBUSINV;
		}
		if (ckClockInUSB->isChecked())
		{
			pol |= CLOCKININV;
		}
		if (ckControlInUSB->isChecked())
		{
			pol |= CTRLININV;
		}

		//Set pins configuration
		pins.clock = cbxClockOutUSB->currentIndex();
		pins.ctrl = cbxControlOutUSB->currentIndex();
		pins.datain = cbxDataInUSB->currentIndex();
		pins.dataout = cbxDataOutUSB->currentIndex();

		pins.clockin = cbxClockInUSB->currentIndex();
		pins.ctrlin = cbxControlInUSB->currentIndex();
		pins.enbus = cbxEnaODUSB->currentIndex();
		pins.poweron = cbxPowerOnUSB->currentIndex();

		E2Profile::SetInterfacePins(interf_type, pins);
		break;
	case INTERF_GPIO:
		if (ckControlOutGPIO->isChecked())
		{
			pol |= RESETINV;
		}
		if (ckClockOutGPIO->isChecked())
		{
			pol |= CLOCKINV;
		}
		if (ckDataInGPIO->isChecked())
		{
			pol |= DININV;
		}
		if (ckDataOutGPIO->isChecked())
		{
			pol |= DOUTINV;
		}
		if (ckPowerOnGPIO->isChecked())
		{
			pol |= POWERINV;
		}
		if (ckEnaODGPIO->isChecked())
		{
			pol |= ENBUSINV;
		}
		if (ckClockInGPIO->isChecked())
		{
			pol |= CLOCKININV;
		}
		if (ckControlInGPIO->isChecked())
		{
			pol |= CTRLININV;
		}

		pins.clock = cbxClockOutGPIO->currentIndex();
		pins.ctrl = cbxControlOutGPIO->currentIndex();
		pins.datain = cbxDataInGPIO->currentIndex();
		pins.dataout = cbxDataOutGPIO->currentIndex();

		pins.clockin = cbxClockInGPIO->currentIndex();
		pins.ctrlin = cbxControlInGPIO->currentIndex();
		pins.enbus = cbxEnaODGPIO->currentIndex();
		pins.poweron = cbxPowerOnGPIO->currentIndex();

		E2Profile::SetInterfacePins(interf_type, pins);
		break;
	}
	cmdWin->SetPolarity(pol);

	if (interf_type != cmdWin->GetInterfaceType())
	{
		cmdWin->ClosePort();
		cmdWin->SetInterfaceType(interf_type);
	}
	else
	{
		cmdWin->GetInterfPtr()->ConfigPins(pins);
	}

	cmdWin->SetPort(port_no);

	//Store values in the INI file
	E2Profile::SetParInterfType(interf_type);
	E2Profile::SetPortNumber(port_no);
	E2Profile::SetPolarityLines(cmdWin->GetPolarity());

	qDebug() << "PortNo: " << port_no;
}

static QStringList generatePinList(int min, int max)
{
	QStringList pinList;

	Q_ASSERT(min <= max);

	for (int k = min; k <= max; k++)
	{
		pinList << QString::number(k);
	}

	return pinList;
}

#define GPIO_PIN_MAX	31		//may be 27 for RaspberryPi
#define USB_PIN_MAX		15		//0-7 for ADBUS, 8-15 for ACBUS

void e2Dialog::setWidgetsText()
{
#ifndef Q_OS_LINUX
	//remove the tab on system different from Linux
	tabWidget->removeTab(INTERF_GPIO);
#endif
	//recurseCbxHide(groupBoxUSBPol);
	recurseCbxHide(groupBoxCOMPol);		//No pin select on legacy COM
	recurseCbxHide(groupBoxLPTPol);		//No pin select on legacy LPT
	//recurseCbxHide(groupBoxGPIOPol);

	QStringList pinlist = generatePinList(0, GPIO_PIN_MAX);
	cbxClockOutGPIO->addItems(pinlist);
	cbxDataOutGPIO->addItems(pinlist);
	cbxDataInGPIO->addItems(pinlist);
	cbxControlOutGPIO->addItems(pinlist);

	cbxClockInGPIO->addItems(pinlist);
	cbxControlInGPIO->addItems(pinlist);
	cbxEnaODGPIO->addItems(pinlist);
	cbxPowerOnGPIO->addItems(pinlist);

	pinlist = generatePinList(0, USB_PIN_MAX);
	cbxClockOutUSB->addItems(pinlist);
	cbxDataOutUSB->addItems(pinlist);
	cbxDataInUSB->addItems(pinlist);
	cbxControlOutUSB->addItems(pinlist);

	cbxClockInUSB->addItems(pinlist);
	cbxControlInUSB->addItems(pinlist);
	cbxEnaODUSB->addItems(pinlist);
	cbxPowerOnUSB->addItems(pinlist);

	cbxInterfCOM->addItems(GetInterfList(INTERF_COM));
	cbxInterfLPT->addItems(GetInterfList(INTERF_LPT));
	cbxInterfUSB->addItems(GetInterfList(INTERF_USB));
	cbxInterfGPIO->addItems(GetInterfList(INTERF_GPIO));

	//rdbComPort->setText(translate(STR_LBLSERIAL));
	//rdbLptPort->setText(translate(STR_LBLPARALLEL));

	lptList = E2Profile::GetLPTDevList();
	comList = E2Profile::GetCOMDevList();
	usbList.clear();
	gpioList = QString("0 1 2 3 4 5 6 7").split(' ');

	cbxInterfCOMNum->clear();
	cbxInterfCOMNum->addItems(comList);
	cbxInterfLPTNum->clear();
	cbxInterfLPTNum->addItems(lptList);
	cbxInterfUSBNum->clear();
	cbxInterfUSBNum->addItems(usbList);
	cbxInterfGPIONum->clear();
	cbxInterfGPIONum->addItems(gpioList);

	groupBoxCOMPol->setTitle(translate(STR_LBLSELPOLARITY));
	groupBoxLPTPol->setTitle(translate(STR_LBLSELPOLARITY));
	groupBoxUSBPol->setTitle(translate(STR_LBLSELPOLARITY));
	groupBoxGPIOPol->setTitle(translate(STR_LBLSELPOLARITY));

	ckControlOutCOM->setToolTip(translate(STR_LBLINVRESET));
	ckControlOutLPT->setToolTip(translate(STR_LBLINVRESET));
	ckControlOutUSB->setToolTip(translate(STR_LBLINVRESET));
	ckControlOutGPIO->setToolTip(translate(STR_LBLINVRESET));

	ckClockOutCOM->setToolTip(translate(STR_LBLINVSCK));
	ckClockOutLPT->setToolTip(translate(STR_LBLINVSCK));
	ckClockOutUSB->setToolTip(translate(STR_LBLINVSCK));
	ckClockOutGPIO->setToolTip(translate(STR_LBLINVSCK));

	ckDataInCOM->setToolTip(translate(STR_LBLINVDATAIN));
	ckDataInLPT->setToolTip(translate(STR_LBLINVDATAIN));
	ckDataInUSB->setToolTip(translate(STR_LBLINVDATAIN));
	ckDataInGPIO->setToolTip(translate(STR_LBLINVDATAIN));

	ckDataOutCOM->setToolTip(translate(STR_LBLINVDATAOUT));
	ckDataOutLPT->setToolTip(translate(STR_LBLINVDATAOUT));
	ckDataOutUSB->setToolTip(translate(STR_LBLINVDATAOUT));
	ckDataOutGPIO->setToolTip(translate(STR_LBLINVDATAOUT));

	cbxControlOutGPIO->setToolTip("Select Control-Out Pin");
	cbxClockOutGPIO->setToolTip("Select Clock-Out Pin");
	cbxDataOutGPIO->setToolTip("Select Data-Out Pin");
	cbxDataInGPIO->setToolTip("Select Data-In Pin");

	cbxControlOutUSB->setToolTip("Select Control-Out Pin");
	cbxClockOutUSB->setToolTip("Select Clock-Out Pin");
	cbxDataOutUSB->setToolTip("Select Data-Out Pin");
	cbxDataInUSB->setToolTip("Select Data-In Pin");

	//lblPol1->setText(translate(STR_LBLSELPOLARITY));

	//chkPol1->setText(translate(STR_LBLINVRESET));
	//chkPol2->setText(translate(STR_LBLINVSCK));
	//chkPol3->setText(translate(STR_LBLINVDATAIN));
	//chkPol4->setText(translate(STR_LBLINVDATAOUT));

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

		//Get pins configuration
		InterfPins pins = E2Profile::GetInterfacePins(interf_type);

		cbxClockOutUSB->setCurrentIndex(pins.clock);
		cbxControlOutUSB->setCurrentIndex(pins.ctrl);
		cbxDataInUSB->setCurrentIndex(pins.datain);
		cbxDataOutUSB->setCurrentIndex(pins.dataout);

		cbxClockInUSB->setCurrentIndex(pins.clockin);
		cbxControlInUSB->setCurrentIndex(pins.ctrlin);
		cbxEnaODUSB->setCurrentIndex(pins.enbus);
		cbxPowerOnUSB->setCurrentIndex(pins.poweron);
	}
	else if (index == INTERF_GPIO)
	{
		qDebug() << "GPIO Port selected: " << gpio_no;

		port_no = gpio_no;
		cbxInterfGPIONum->setCurrentIndex(gpio_no);

		interf_type = VindexToInterfType(INTERF_GPIO, cbxInterfGPIO->currentIndex());

		//Get pins configuration
		InterfPins pins = E2Profile::GetInterfacePins(interf_type);

		cbxClockOutGPIO->setCurrentIndex(pins.clock);
		cbxControlOutGPIO->setCurrentIndex(pins.ctrl);
		cbxDataInGPIO->setCurrentIndex(pins.datain);
		cbxDataOutGPIO->setCurrentIndex(pins.dataout);

		cbxClockInGPIO->setCurrentIndex(pins.clockin);
		cbxControlInGPIO->setCurrentIndex(pins.ctrlin);
		cbxEnaODGPIO->setCurrentIndex(pins.enbus);
		cbxPowerOnGPIO->setCurrentIndex(pins.poweron);
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

void e2Dialog::on_cbxInterfGPIO_currentIndexChanged(int index)
{
	interf_type = VindexToInterfType(INTERF_GPIO, index);
	qDebug() << "Selected IntefType: " << (int)interf_type << ", index = " << index << ", GPIO";
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
		qDebug() << __PRETTY_FUNCTION__ << "Selected USB port " << port_no;
	}
}

void e2Dialog::on_cbxInterfGPIONum_currentIndexChanged(int index)
{
	if (index >= 0)
	{
		port_no = gpio_no = index;
		qDebug() << __PRETTY_FUNCTION__ << "Selected GPIO port " << port_no;
	}
}

void e2Dialog::onUSB(bool connected, quint16 vid, quint16 pid)
{
	qDebug() << __PRETTY_FUNCTION__ << " " << connected << " " << vid << " " << pid;

	usbList.clear();
	usbList = MpsseInterface::find_all(usb_vp.vid, usb_vp.pid);
	cbxInterfUSBNum->clear();
	cbxInterfUSBNum->addItems(usbList);
}

void e2Dialog::on_pushDefaultsUSB_clicked()
{
	ckControlOutUSB->setChecked(false);
	ckClockOutUSB->setChecked(false);
	ckDataInUSB->setChecked(false);
	ckDataOutUSB->setChecked(false);
	ckPowerOnUSB->setChecked(false);
	ckEnaODUSB->setChecked(false);
	ckClockInUSB->setChecked(false);
	ckControlInUSB->setChecked(false);

	InterfPins dpins;	//default pins
	if (!TypeToInterfPins(interf_type, dpins))
	{
		qWarning() << __PRETTY_FUNCTION__ << " TypeToInterfPins(" << interf_type << ") Failed";
	}
	cbxClockOutUSB->setCurrentIndex(dpins.clock);
	cbxControlOutUSB->setCurrentIndex(dpins.ctrl);
	cbxDataInUSB->setCurrentIndex(dpins.datain);
	cbxDataOutUSB->setCurrentIndex(dpins.dataout);
	cbxClockInUSB->setCurrentIndex(dpins.clockin);
	cbxControlInUSB->setCurrentIndex(dpins.ctrlin);
	cbxPowerOnUSB->setCurrentIndex(dpins.poweron);
	cbxEnaODUSB->setCurrentIndex(dpins.enbus);
}

void e2Dialog::on_pushDefaultsCOM_clicked()
{
	ckControlOutCOM->setChecked(false);
	ckClockOutCOM->setChecked(false);
	ckDataInCOM->setChecked(false);
	ckDataOutCOM->setChecked(false);
}

void e2Dialog::on_pushDefaultsLPT_clicked()
{
	ckControlOutLPT->setChecked(false);
	ckClockOutLPT->setChecked(false);
	ckDataInLPT->setChecked(false);
	ckDataOutLPT->setChecked(false);
	//ckPowerOnLPT->setChecked(false);
	//ckEnaODLPT->setChecked(false);
	ckClockInLPT->setChecked(false);
	//ckControlInLPT->setChecked(false);
}

void e2Dialog::on_pushDefaultsGPIO_clicked()
{
	ckControlOutGPIO->setChecked(false);
	ckClockOutGPIO->setChecked(false);
	ckDataInGPIO->setChecked(false);
	ckDataOutGPIO->setChecked(false);
	ckPowerOnGPIO->setChecked(false);
	ckEnaODGPIO->setChecked(false);
	ckClockInGPIO->setChecked(false);
	ckControlInGPIO->setChecked(false);

	InterfPins dpins;	//default pins
	if (!TypeToInterfPins(interf_type, dpins))
	{
		qWarning() << __PRETTY_FUNCTION__ << " TypeToInterfPins(" << interf_type << ") Failed";
	}
	cbxClockOutGPIO->setCurrentIndex(dpins.clock);
	cbxControlOutGPIO->setCurrentIndex(dpins.ctrl);
	cbxDataInGPIO->setCurrentIndex(dpins.datain);
	cbxDataOutGPIO->setCurrentIndex(dpins.dataout);

	//cbxClockOutGPIO->setCurrentIndex(DEF_GPIO_CLOCK);
	//cbxControlOutGPIO->setCurrentIndex(DEF_GPIO_CTRL);
	//cbxDataInGPIO->setCurrentIndex(DEF_GPIO_DATAIN);
	//cbxDataOutGPIO->setCurrentIndex(DEF_GPIO_DATAOUT);
}

#include <QComboBox>

void e2Dialog::recurseCbxHide(QObject *object)
{
	QComboBox *combobox = dynamic_cast<QComboBox *>(object);
	if (0 != combobox)
	{
		combobox->hide();
	}

	foreach (QObject *child, object->children())
	{
		recurseCbxHide(child);
	}
}
