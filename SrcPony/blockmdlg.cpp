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


#include "blockmdlg.h"

#include <QDebug>


//=========================>>> blockDialog::blockDialog <<<====================
blockDialog::blockDialog(e2CmdWindow *bw, e2AppWinInfo *p, bool readonly, const QString &msg) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(translate(STR_MSGFUSEDLG));

	awip = p;

	lock = awip->GetLockBits();
	fuse = awip->GetFuseBits();

	e2CmdWindow *cmdw = static_cast<e2CmdWindow *>(bw);

	if (cmdw->getStyleSheet().length() > 0)
	{
		setStyleSheet(cmdw->getStyleSheet());
	}

	qDebug() << "blockDialog::blockDialog()";


	lblFrom->setText(translate(STR_MSGFIRSTBLK));
	lblTo->setText(translate(STR_MSGNUMBLOCK));
	lblVal->setText(translate(STR_MSGHIGHENDBLK));

	QString str1 = QString().sprintf("%d", (int)((lock >> 4) & 0x0F));
	QString str2 = QString().sprintf("%d", (int)(lock & 0x0F));
	QString str3 = QString().sprintf("%d", (int)(fuse & 0x0F));

	txiFrom->setText(str1);
	txiTo->setText(str2);
	txiVal->setText(str3);

	pushOk->setText(translate(STR_BTNOK));
	pushCancel->setText(translate(STR_BTNCANC));

	if (readonly)
	{
		pushOk->setEnabled(false);
	}
	else
	{
		pushOk->setEnabled(true);
	}

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


//======================>>> blockDialog::~blockDialog <<<======================
blockDialog::~blockDialog()
{
	qDebug() << "blockDialog::~blockDialog()";
}



void blockDialog::onOk()
{
	bool good;
	QString str = txiFrom->text();

	fuse = 0;
	lock = 0;

	int From = str.toInt(&good);

	if (good == false)
	{
		reject();
	}

	str = txiTo->text();
	int To = str.toInt(&good);

	if (good == false)
	{
		reject();
	}

	str = txiVal->text();
	int Val = str.toInt(&good);

	if (good == false)
	{
		reject();
	}

	lock = ((From << 4) & 0xF0) | (To & 0x0F);
	fuse = Val & 0x0F;

	awip->SetLockBits(lock);
	awip->SetFuseBits(fuse);

	accept();
}

