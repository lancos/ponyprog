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

#include "prefdialog.h"
#include "ui_prefdialog.h"

#include "e2profil.h"

//	ULTRASLOW = 0,
//	VERYSLOW,
//	SLOW,
//	NORMAL,
//	FAST,
//	TURBO

PrefDialog::PrefDialog(QWidget *parent, const QString title) :
	QDialog(parent),
	ui(new Ui::PrefDialog)
{
	ui->setupUi(this);
	setWindowTitle(title);

	QStringList speedItems = (QStringList() << "Slowest" << "Very Slow" << "Slow" << "Normal" << "Fast"  << "Fastest");
	ui->cbxBusSpeedSPI->addItems(speedItems);
	ui->cbxBusSpeedI2C->addItems(speedItems);
	ui->cbxBusSpeedIM->addItems(speedItems);
	ui->cbxBusSpeedPIC->addItems(speedItems);
	ui->cbxBusSpeedSDE->addItems(speedItems);
	ui->cbxBusSpeedMicroWire->addItems(speedItems);

	ui->lblBusSpeedSPI->setText("SPI Bus Speed");
	ui->lblBusSpeedI2C->setText("I2C Bus Speed");
	ui->lblBusSpeedIM->setText("IM Bus Speed");
	ui->lblBusSpeedPIC->setText("PIC Bus Speed");
	ui->lblBusSpeedSDE->setText("SDE Bus Speed");
	ui->lblBusSpeedMicroWire->setText("MicroWire Bus Speed");

	ui->cbxBusSpeedSPI->setCurrentIndex(E2Profile::GetSPISpeed());
	ui->cbxBusSpeedI2C->setCurrentIndex(E2Profile::GetI2CSpeed());
	ui->cbxBusSpeedIM->setCurrentIndex(E2Profile::GetIMBusSpeed());
	ui->cbxBusSpeedPIC->setCurrentIndex(E2Profile::GetPICSpeed());
	ui->cbxBusSpeedSDE->setCurrentIndex(E2Profile::GetSDESpeed());
	ui->cbxBusSpeedMicroWire->setCurrentIndex(E2Profile::GetMicroWireSpeed());
}

PrefDialog::~PrefDialog()
{
	delete ui;
}

void PrefDialog::on_buttonBox_accepted()
{
	E2Profile::SetSPISpeed(ui->cbxBusSpeedSPI->currentIndex());
	E2Profile::SetI2CSpeed(ui->cbxBusSpeedI2C->currentIndex());
	E2Profile::SetIMBusSpeed(ui->cbxBusSpeedIM->currentIndex());
	E2Profile::SetPICSpeed(ui->cbxBusSpeedPIC->currentIndex());
	E2Profile::SetSDESpeed(ui->cbxBusSpeedSDE->currentIndex());
	E2Profile::SetMicroWireSpeed(ui->cbxBusSpeedMicroWire->currentIndex());
}
