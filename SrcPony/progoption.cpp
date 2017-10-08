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


#include "progoption.h"

#include <QDebug>

#include "e2profil.h"
#include "eeptypes.h"
#include "globals.h"



//======================>>> progOptionDialog::progOptionDialog <<<==================
progOptionDialog::progOptionDialog(QWidget *bw, long type, int &reload, int &reep, int &erase, int &flash, int &eeprom, int &lock, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);

	cmdw = static_cast<e2CmdWindow *>(bw);

	chk = (QVector<QCheckBox *>() << idRELOAD << idREADFLASH << idREADEEP << idBYTESWAP <<
		   idSETID << idREADOSCCAL << idERASE << idFLASH << idEEPROM << idLOCK);

	setTextWidgets();

	int prog_opt = E2Profile::GetProgramOptions();

	//Checked
	for (int k = 0; k < chk.count(); k++)
	{
		chk[k]->setChecked((prog_opt & (1 << k)) ? 1 : 0);
	}

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));

	qDebug() << "progOptionDialog::progOptionDialog()";
}

//===================>>> progOptionDialog::~progOptionDialog <<<====================
progOptionDialog::~progOptionDialog()
{
	qDebug() << "progOptionDialog::~progOptionDialog()";
}


void progOptionDialog::setTextWidgets()
{
	QStringList txList = (QStringList() << translate(STR_MSGRELOAD) << translate(STR_MSGREADPROG) << translate(STR_MSGREADDATA) << translate(STR_MSGBYTESWAP) <<
						  translate(STR_MSGSERNUM) << translate(STR_READOSCCALIB) << translate(STR_MSGERASE) << translate(STR_MSGWRITEPROG) << translate(STR_MSGWRITEDATA) << translate(STR_MSGWRITESEC));

	for (int i = 0; i < chk.size(); i++)
	{
		chk[i]->setText(txList.at(i));
	}

	pushOk->setText(translate(STR_BTNOK));
	pushCancel->setText(translate(STR_BTNCANC));
}

void progOptionDialog::onOk()
{
	int prog_opt = 0;

	for (int k = 0; k < chk.count(); k++)
	{
		prog_opt |= (chk[k]->isChecked()) ? (1 << k) : 0;
	}

	E2Profile::SetProgramOptions(prog_opt);

	accept();
}

