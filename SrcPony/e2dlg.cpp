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

//=========================>>> e2Dialog::e2Dialog <<<====================
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

	setWidgetsText();

	connect(rdbComPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));
	connect(rdbLptPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));

	connect(cbxInterfCOM, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectCOM(int)));
	connect(cbxInterfLPT, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectLPT(int)));

	connect(cbxInterfNum, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectNum(int)));

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

extern HInterfaceType VindexToInterfType(int vector, int index);

void e2Dialog::onSelectLPT(int i)
{
	interf_type = VindexToInterfType(1, i);
	qDebug() << "IntefType: " << (int)interf_type << ", index = " << i << ", LPT";
//	interf_type = (HInterfaceType)(i + 3);
}


void e2Dialog::onSelectCOM(int i)
{
	interf_type = VindexToInterfType(0, i);
	qDebug() << "IntefType: " << (int)interf_type << ", index = " << i << ", COM";
//	interf_type = (HInterfaceType)i;
}


void e2Dialog::onSelectNum(int i)
{
	if (rdbLptPort->isChecked() == true)
	{
		lpt_no = i;
		port_no = lpt_no;
		qDebug() << "LPT port " << port_no;
	}
	else
	{
		com_no = i;
		port_no = com_no;
		qDebug() << "COM port " << port_no;
	}
}

void e2Dialog::onChangePort(bool b)
{
	if (b)
	{
		QRadioButton *s = static_cast<QRadioButton *>(sender());
		//bool state = s->isChecked();

		//disconnect(rdbComPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));
		//disconnect(rdbLptPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));

		if (s == rdbComPort)
		{
			qDebug() << "COM Port selected";

			cbxInterfCOM->setEnabled(true);
			cbxInterfLPT->setEnabled(false);

			disconnect(cbxInterfNum, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectNum(int)));
			cbxInterfNum->clear();
			cbxInterfNum->addItems(comList);
			connect(cbxInterfNum, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectNum(int)));

			port_no = com_no;
			cbxInterfNum->setCurrentIndex(com_no);

			interf_type = VindexToInterfType(0, cbxInterfCOM->currentIndex());
		}
		else
		{
			qDebug() << "LPT Port selected";

			cbxInterfCOM->setEnabled(false);
			cbxInterfLPT->setEnabled(true);

			disconnect(cbxInterfNum, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectNum(int)));
			cbxInterfNum->clear();
			cbxInterfNum->addItems(lptList);
			connect(cbxInterfNum, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectNum(int)));

			port_no = lpt_no;
			cbxInterfNum->setCurrentIndex(lpt_no);

			interf_type = VindexToInterfType(1, cbxInterfLPT->currentIndex());
		}

		//connect(rdbComPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));
		//connect(rdbLptPort, SIGNAL(toggled(bool)), this, SLOT(onChangePort(bool)));
	}
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

	// init of radiobuttons
	if (TypeToInterfVector(interf_type) == 0)		//COM
	{
		cbxInterfCOM->setCurrentIndex(TypeToInterfIndex(interf_type));
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
		cbxInterfLPT->setCurrentIndex(TypeToInterfIndex(interf_type));
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

extern QStringList GetInterfList(int vector);

void e2Dialog::setWidgetsText()
{
	//Serial interfaces list
	QStringList interfListC = GetInterfList(0);
	cbxInterfCOM->addItems(interfListC);

	//Parallel interfaces list
	QStringList interfListL = GetInterfList(1);
	cbxInterfLPT->addItems(interfListL);

	rdbComPort->setText(translate(STR_LBLSERIAL));
	rdbLptPort->setText(translate(STR_LBLPARALLEL));

	lptList = E2Profile::GetLPTDevList();
	comList = E2Profile::GetCOMDevList();

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

