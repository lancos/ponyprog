//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2025   Claudio Lanconelli                           //
//                                                                         //
//  https://github.com/lancos/ponyprog                                        //
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

#include <QDebug>

#include "types.h"
#include "sernumdlg.h"

SerNumDialog::SerNumDialog(QWidget *bw, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);
	setTextWidgets();

	E2Profile::readDialogSettings(this, false);

	loc = 0;
	val = 0;
	memtype = false;
	autoinc = false;
	size = 1;
	fmt = FMT_BIG_ENDIAN;

	val = E2Profile::GetSerialNumVal();
	E2Profile::GetSerialNumAddress(loc, size, memtype);
	autoinc = E2Profile::GetSerialNumAutoInc();
	fmt = E2Profile::GetSerialNumFormat();

	if (loc < 0)
	{
		loc = 0;
	}
	if (size < 0 || size > 4)
	{
		size = 4;
	}

	txiLoc->setText(QString("0x%1").arg(loc, 4, 16, QLatin1Char('0')));	//sprintf("0x%04lX", loc));
	txiLen->setText(QString("%1").arg(size));	//sprintf("%d", size));
	txiVal->setText(QString("%1").arg(val));	//sprintf("%lu", val));

	chkMemOffset->setChecked(memtype);
	chkAutoInc->setChecked(autoinc);

	if (fmt == FMT_LITTLE_ENDIAN)
	{
		rdbLittleEnd->setChecked(true);
		rdbBigEnd->setChecked(false);
	}
	else
	{
		rdbLittleEnd->setChecked(false);
		rdbBigEnd->setChecked(true);
	}

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));

	qDebug() << Q_FUNC_INFO;
}

SerNumDialog::~SerNumDialog()
{
	qDebug() << Q_FUNC_INFO;
}

void SerNumDialog::setTextWidgets()
{
	pushOk->setText(translate(STR_BTNOK));
	pushCancel->setText(translate(STR_BTNCANC));

	lblLoc->setText(translate(STR_MSGADDRESS));
	lblLen->setText(translate(STR_MSGSIZE2));
	lblVal->setText(translate(STR_MSGVALUE));

	chkMemOffset->setText(translate(STR_MSGOFFSET));
	chkAutoInc->setText(translate(STR_MSGSNAUTOINC));

	frmFormat->setTitle(translate(STR_MSGFORMAT));

	rdbLittleEnd->setText(translate(STR_MSGLITTLEEND));
	rdbBigEnd->setText(translate(STR_MSGBIGENDIAN));
}

void SerNumDialog::onOk()
{
	bool ok;
	long l = txiLoc->text().toLong(&ok, 0);

	if (ok == true)
	{
		loc = l;
	}
	else
	{
		QPalette *palette = new QPalette();
		palette->setColor(QPalette::Text, Qt::red);
		txiLoc->setPalette(*palette);

		return;
	}

	l = txiLen->text().toLong(&ok, 0);

	if (ok == true)
	{
		size = l;
	}
	else
	{
		QPalette *palette = new QPalette();
		palette->setColor(QPalette::Text, Qt::red);
		txiLen->setPalette(*palette);

		return;
	}

	l = txiVal->text().toLong(&ok, 0);

	if (ok == true)
	{
		val = l;
	}
	else
	{
		QPalette *palette = new QPalette();
		palette->setColor(QPalette::Text, Qt::red);
		txiVal->setPalette(*palette);

		return;
	}

	if (rdbLittleEnd->isChecked() == true)
	{
		fmt = FMT_LITTLE_ENDIAN;
	}
	else
	{
		fmt = FMT_BIG_ENDIAN;
	}

	memtype = chkMemOffset->isChecked();
	autoinc = chkAutoInc->isChecked();

	E2Profile::SetSerialNumAddress(loc, size, memtype);
	E2Profile::SetSerialNumVal(val);
	E2Profile::SetSerialNumFormat(fmt);
	E2Profile::SetSerialNumAutoInc(autoinc);

	E2Profile::writeDialogSettings(this, false);

	accept();
}


OscCalibDialog::OscCalibDialog(QWidget *bw, e2AppWinInfo *aw, const QString title) :
	QDialog(bw)
{
	setupUi(this);
	setWindowTitle(title);

	loc = 0;
	val = 0;
	memtype = false;
	enabled = false;
	size = 1;

	qDebug() << Q_FUNC_INFO;

	E2Profile::readDialogSettings(this, false);

	E2Profile::GetCalibrationAddress(enabled, loc, size, memtype);

	lblLoc->setText(translate(STR_MSGADDRESS));
	lblVal->setText(translate(STR_MSGVALUE));

	chkMemOffset->setText(translate(STR_MSGOFFSET));
	chkEnabled->setText(translate(STR_CALIBRENABLED));

	loc = (loc < 0) ? 0 : loc;

	txiLoc->setText(QString("0x%1").arg(loc, 4, 16, QLatin1Char('0')));	//sprintf("0x%04lX", loc));
	txiVal->setText(QString("%1").arg(val));	//sprintf("%d", val));

	chkMemOffset->setChecked(memtype);
	chkEnabled->setChecked(enabled);

	pushOk->setText(translate(STR_BTNOK));
	pushCancel->setText(translate(STR_BTNCANC));
	pushRead->setText(translate(STR_BTNREAD));

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(pushRead, SIGNAL(clicked()), this, SLOT(onRead()));

	awip = aw;
}

OscCalibDialog::~OscCalibDialog()
{
	qDebug() << Q_FUNC_INFO;
}

void OscCalibDialog::onOk()
{
	bool ok1, ok2;
	long i = txiLoc->text().toLong(&ok1, 0);

	if (ok1)
	{
		loc = i;
	}
	else
	{
		QPalette *palette = new QPalette();
		palette->setColor(QPalette::Text, Qt::red);
		txiLoc->setPalette(*palette);
	}

	int v = txiVal->text().toInt(&ok2, 0);

	if (ok2)
	{
		val = v;
	}
	else
	{
		QPalette *palette = new QPalette();
		palette->setColor(QPalette::Text, Qt::red);
		txiVal->setPalette(*palette);
	}

	if (ok1 && ok2)
	{
		memtype = chkMemOffset->isChecked();
		enabled = chkEnabled->isChecked();

		E2Profile::SetCalibrationAddress(enabled, loc, size, memtype);
		E2Profile::writeDialogSettings(this, false);

		accept();
	}
}

void OscCalibDialog::onRead()
{
	int val = awip->ReadOscCalibration();
	if (val >= 0)
	{
		txiVal->setText(QString("0x%1").arg(val, 2, 16, QLatin1Char('0')));	//sprintf("0x%02X", val));
	}
	else
	{
		qWarning() << "Error " << val;
		//if (verbose != verboseNo)
		//{
		//	rval = OnError(rval);
		//}
		QPalette *palette = new QPalette();
		palette->setColor(QPalette::Text, Qt::red);
		txiVal->setPalette(*palette);
		txiVal->setText(QString("Error %1").arg(val));
	}
}
