//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: e2dlg.cpp,v 1.8 2016/05/27 11:22:51 lancos Exp $
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

class e2CmdWindow;

using namespace Translator;


//=========================>>> e2Dialog::e2Dialog <<<====================
e2Dialog::e2Dialog(QWidget* bw, const QString title)
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

	setWidgetsText();

	connect(rdbComPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));
	connect(rdbLptPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));

	connect (cbxInterfCOM, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectCOM(int)));
	connect (cbxInterfLPT, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectLPT(int)));

	connect (cbxInterfNum, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectNum(int)));

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(pushTest, SIGNAL(clicked(bool)), this, SLOT(onTest()));


	getSettings();
}


//======================>>> e2Dialog::~e2Dialog <<<======================
e2Dialog::~e2Dialog()
{
	qDebug() << "e2Dialog::~e2Dialog()";
}


void e2Dialog::onSelectLPT(int i)
{
	interf_type = (HInterfaceType)(i + 3);
}


void e2Dialog::onSelectCOM(int i)
{
	interf_type = (HInterfaceType)i;
}


void e2Dialog::onSelectNum(int i)
{
	if (rdbComPort->isChecked() == true)
	{
		lpt_no = i;
		port_no = lpt_no;
	}
	else
	{
		com_no = i;
		port_no = com_no;
	}
}

void e2Dialog::onChangePort(bool b)
{
	QRadioButton *s = static_cast<QRadioButton*>(sender());
	//      bool state = s->isChecked();

	disconnect(rdbComPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));
	disconnect(rdbLptPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));

	if (s == rdbComPort)
	{
		cbxInterfCOM->setEnabled(b);
		cbxInterfLPT->setEnabled(!b);

		disconnect (cbxInterfNum, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectNum(int)));
		cbxInterfNum->clear();
		cbxInterfNum->addItems(usbList);
		connect (cbxInterfNum, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectNum(int)));

		cbxInterfNum->setCurrentIndex(com_no);
	}
	else
	{
		cbxInterfCOM->setEnabled(!b);
		cbxInterfLPT->setEnabled(b);

		disconnect (cbxInterfNum, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectNum(int)));
		cbxInterfNum->clear();
		cbxInterfNum->addItems(lptList);
		connect (cbxInterfNum, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectNum(int)));

		cbxInterfNum->setCurrentIndex(lpt_no);
	}

	connect(rdbComPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));
	connect(rdbLptPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));
}


void e2Dialog::getSettings()
{
	interf_type = cmdWin->GetInterfaceType();

	if (interf_type < SIPROG_API || interf_type == LAST_HT)
	{
		interf_type = SIPROG_API;
	}

	port_no = cmdWin->GetPort();

	// init of radiobuttons
	if (interf_type >= SIPROG_API && interf_type <= JDM_API)  // COM
	{
		cbxInterfCOM->setCurrentIndex(interf_type);
		cbxInterfLPT->setCurrentIndex(0);

		com_no = port_no;
		rdbComPort->setChecked(true);

		if (com_no >= cbxInterfNum->count())
		{
			com_no = 0;
			port_no = 0;
		}

		cbxInterfNum->setCurrentIndex(com_no);
	}
	else
	{
		cbxInterfLPT->setCurrentIndex(interf_type - 3 );
		cbxInterfCOM->setCurrentIndex(0);

		lpt_no = port_no;
		rdbLptPort->setChecked(true);

		if (lpt_no >= cbxInterfNum->count())
		{
			lpt_no = 0;
			port_no = 0;
		}

		cbxInterfNum->setCurrentIndex(lpt_no);
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

	bool set_port = false;

	if (port_no != cmdWin->GetPort())
	{
		set_port = true;
	}

	if (interf_type != cmdWin->GetInterfaceType())
	{
		cmdWin->ClosePort();
		cmdWin->SetInterfaceType(interf_type);
		set_port = true;
	}

	if (set_port)
	{
		cmdWin->SetPort(port_no);
	}

	//Store values in the INI file
	E2Profile::SetParInterfType(interf_type);
	E2Profile::SetParPortNo(port_no);
	cmdWin->SetPolarity(cmdWin->GetPolarity());
}


void e2Dialog::setWidgetsText()
{
	//Serial interfaces list
	QStringList interfListC;
	interfListC <<
	            "SI Prog API" <<
	            "SI Prog I/O" <<
	            "  JDM API  ";
	cbxInterfCOM->addItems(interfListC);

	//Parallel interfaces list
	QStringList interfListL;
	interfListL <<
	            "Avr ISP API" <<
	            "Avr ISP I/O" <<
	            "DT-006 API " <<
	            "DT-006 I/O " <<
	            "EasyI2C API" <<
	            "EasyI2C I/O" <<
	            "SysFs GPIO ";
	cbxInterfLPT->addItems(interfListL);

	rdbComPort->setText(STR_LBLSERIAL);
	rdbLptPort->setText(STR_LBLPARALLEL);

	lptList << STR_LBLLPT1 << STR_LBLLPT2 << STR_LBLLPT3;
	comList << STR_LBLCOM1 << STR_LBLCOM2 << STR_LBLCOM3 << STR_LBLCOM4;
	usbList << STR_LBLUSB1 << STR_LBLUSB2 << STR_LBLUSB3 << STR_LBLUSB4 << STR_LBLUSB5 << STR_LBLUSB6 << STR_LBLUSB7 << STR_LBLUSB8;

	lblPol1->setText(STR_LBLSELPOLARITY);

	chkPol1->setText(STR_LBLINVRESET);
	chkPol2->setText(STR_LBLINVSCK);
	chkPol3->setText(STR_LBLINVDATAIN);
	chkPol4->setText(STR_LBLINVDATAOUT);

	pushOk->setText(STR_BTNOK);
	pushTest->setText(STR_BTNPROBE);
	pushCancel->setText(STR_BTNCANC);
}


void e2Dialog::onOk()
{
	setSettings();

	accept();
}

void e2Dialog::onTest()
{
	int test = Test();

	if ( test )
	{
		QMessageBox::critical(this, "Failed",  STR_MSGTEST + " " + STR_MSGFAILED);
	}
	else
	{
		QMessageBox::information(this, "Info", STR_MSGTEST + " " + STR_MSGOK);
	}
}


int e2Dialog::Test(int p, int open_only) const
{
	qDebug() << "e2Dialog::Test() IN *** p=" << p << ", open_only=" << open_only;

	HInterfaceType old_interf = cmdWin->GetInterfaceType();
	int test;

	if (p == 0)
	{
		p = port_no;
	}

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

