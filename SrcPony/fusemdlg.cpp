//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: fusemdlg.cpp,v 1.10 2009/11/16 23:40:43 lancos Exp $
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


#include "fusemdlg.h"

#include <QDebug>


//======================>>> fuseModalDialog::fuseModalDialog <<<==================
fuseModalDialog::fuseModalDialog(e2CmdWindow *bw, e2AppWinInfo *p, bool readonly, const QString &msg) :
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

	long type = bw->GetCurrentChipType();

	qDebug() << "fuseModalDialog::fuseModalDialog()";

	chkFuse = (QVector<QCheckBox *>() << chk0_F0 << chk1_F0 << chk2_F0 << chk3_F0 << chk4_F0 << chk5_F0 << chk6_F0 << chk7_F0
			   << chk0_F1 << chk1_F1 << chk2_F1 << chk3_F1 << chk4_F1 << chk5_F1 << chk6_F1 << chk7_F1
			   << chk0_F2 << chk1_F2 << chk2_F2 << chk3_F2 << chk4_F2 << chk5_F2 << chk6_F2 << chk7_F2
			   << chk0_F3 << chk1_F3 << chk2_F3 << chk3_F3 << chk4_F3 << chk5_F3 << chk6_F3 << chk7_F3
			  );

	chkLock = (QVector<QCheckBox *>() << chk0_L0 << chk1_L0 << chk2_L0 << chk3_L0 << chk4_L0 << chk5_L0 << chk6_L0 << chk7_L0
			   << chk0_L1 << chk1_L1 << chk2_L1 << chk3_L1 << chk4_L1 << chk5_L1 << chk6_L1 << chk7_L1
			   << chk0_L2 << chk1_L2 << chk2_L2 << chk3_L2 << chk4_L2 << chk5_L2 << chk6_L2 << chk7_L2
			   << chk0_L3 << chk1_L3 << chk2_L3 << chk3_L3 << chk4_L3 << chk5_L3 << chk6_L3 << chk7_L3
			  );

	read = write = false;

	frmLock->setTitle("Lock");
	frmFuses->setTitle("Fuse");

	initWidgets(msg, type, readonly);

	connect(pushRead, SIGNAL(clicked()), this, SLOT(onRead()));
	connect(pushWrite, SIGNAL(clicked()), this, SLOT(onProg()));
	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


//===================>>> fuseModalDialog::~fuseModalDialog <<<====================
fuseModalDialog::~fuseModalDialog()
{
	qDebug() << "fuseModalDialog::~fuseModalDialog()";
}

void fuseModalDialog::setTextWidgets()
{
	int i = 0;

	foreach (QCheckBox *c, chkFuse)
	{
		c->setText(QString().sprintf(" %d", i));
		i++;

		if (i == 8)
		{
			i = 0;
		}
	}

	i = 0;

	foreach (QCheckBox *c, chkLock)
	{
		c->setText(QString().sprintf(" %d", i));
		i++;

		if (i == 8)
		{
			i = 0;
		}
	}

	pushRead->setText(translate(STR_BTNREAD));
	pushWrite->setText(translate(STR_BTNWRITE));
	pushOk->setText(translate(STR_BTNOK));
	pushCancel->setText(translate(STR_BTNCANC));
}


void fuseModalDialog::onOk()
{
	unsigned int lock = 0;
	unsigned int fuse = 0;

	for (int k = 0; k < LOCKPACKSIZE; k++)
	{
		lock |= (chkLock.at(0 * 8 + k)->isChecked()) ? (0x80000000 >> k) : 0;
		lock |= (chkLock.at(1 * 8 + k)->isChecked()) ? (0x800000 >> k) : 0;
		lock |= (chkLock.at(2 * 8 + k)->isChecked()) ? (0x8000 >> k) : 0;
		lock |= (chkLock.at(3 * 8 + k)->isChecked()) ? (0x80 >> k) : 0;

		fuse |= (chkFuse.at(0 * 8 + k)->isChecked()) ? (0x80000000 >> k) : 0;
		fuse |= (chkFuse.at(1 * 8 + k)->isChecked()) ? (0x800000 >> k) : 0;
		fuse |= (chkFuse.at(2 * 8 + k)->isChecked()) ? (0x8000 >> k) : 0;
		fuse |= (chkFuse.at(3 * 8 + k)->isChecked()) ? (0x80 >> k) : 0;
	}

	//      lock = lock;
	//      fuse = fuse;

	if (read == true)
	{
		emit done(2);
	}
	else
	{
		awip->SetLockBits(lock);
		awip->SetFuseBits(fuse);

		if (write == true)
		{
			emit done(3);
		}
		else
		{
			accept();
		}
	}
}


void fuseModalDialog::onProg()
{
	write = true;

	emit onOk();
}


void fuseModalDialog::onRead()
{
	read = true;

	emit onOk();
}


int fuseModalDialog::eep_FindFuses(long type)
{
	for (int k = 0; eep_fusebits.count(); k++)
	{
		if (eep_fusebits.at(k).type == type)
		{
			return k;
		}
	}

	return -1;
}


void fuseModalDialog::initWidgets(const QString &msg, long int type, bool readonly)
{
	int j = eep_FindFuses(type);

	if (j >= 0)
	{
		FuseBit fBit = eep_fusebits.at(j);

		frmLock0->setHidden(fBit.lockenable3 == 0);
		frmLock1->setHidden(fBit.lockenable2 == 0);
		frmLock2->setHidden(fBit.lockenable1 == 0);
		frmLock3->setHidden(fBit.lockenable0 == 0);

		frmFuses0->setHidden(fBit.fuseenable3 == 0);
		frmFuses1->setHidden(fBit.fuseenable2 == 0);
		frmFuses2->setHidden(fBit.fuseenable1 == 0);
		frmFuses3->setHidden(fBit.fuseenable0 == 0);

		if (fBit.lockenable3 == 0 &&
				fBit.lockenable2 == 0 &&
				fBit.lockenable1 == 0 &&
				fBit.lockenable0 == 0)
		{
			frmLock->setHidden(true);
		}
		else
		{
			frmLock->setHidden(false);
		}

		if (fBit.fuseenable3 == 0 &&
				fBit.fuseenable2 == 0 &&
				fBit.fuseenable1 == 0 &&
				fBit.fuseenable0 == 0)
		{
			frmFuses->setHidden(true);
		}
		else
		{
			frmFuses->setHidden(false);
		}


		//Label
		for (int k = 0; k < LOCKPACKSIZE; k++)
		{
			QString sp;
			QString def = QString().sprintf(" %d", k);

			sp = fBit.locklabel3[k];

			if (!sp.length())
			{
				sp = def;
			}

			chkLock.at(0 * 8 + k)->setText(sp);

			sp = fBit.locklabel2[k];

			if (!sp.length())
			{
				sp = def;
			}

			chkLock.at(1 * 8 + k)->setText(sp);

			sp = fBit.locklabel1[k];

			if (!sp.length())
			{
				sp = def;
			}

			chkLock.at(2 * 8 + k)->setText(sp);

			sp = fBit.locklabel0[k];

			if (!sp.length())
			{
				sp = def;
			}

			chkLock.at(3 * 8 + k)->setText(sp);


			sp = fBit.fuselabel3[k];

			if (!sp.length())
			{
				sp = def;
			}

			chkFuse.at(0 * 8 + k)->setText(sp);

			sp = fBit.fuselabel2[k];

			if (!sp.length())
			{
				sp = def;
			}

			chkFuse.at(1 * 8 + k)->setText(sp);

			sp = fBit.fuselabel1[k];

			if (!sp.length())
			{
				sp = def;
			}

			chkFuse.at(2 * 8 + k)->setText(sp);

			sp = fBit.fuselabel0[k];

			if (!sp.length())
			{
				sp = def;
			}

			chkFuse.at(3 * 8 + k)->setText(sp);
		}

		//Sensitive
		for (int k = 0; k < LOCKPACKSIZE; k++)
		{
			chkLock.at(0 * 8 + k)->setEnabled((fBit.lockenable3 & (0x80 >> k)) ? true : false);
			chkLock.at(1 * 8 + k)->setEnabled((fBit.lockenable2 & (0x80 >> k)) ? true : false);
			chkLock.at(2 * 8 + k)->setEnabled((fBit.lockenable1 & (0x80 >> k)) ? true : false);
			chkLock.at(3 * 8 + k)->setEnabled((fBit.lockenable0 & (0x80 >> k)) ? true : false);

			chkFuse.at(0 * 8 + k)->setEnabled((fBit.fuseenable3 & (0x80 >> k)) ? true : false);
			chkFuse.at(1 * 8 + k)->setEnabled((fBit.fuseenable2 & (0x80 >> k)) ? true : false);
			chkFuse.at(2 * 8 + k)->setEnabled((fBit.fuseenable1 & (0x80 >> k)) ? true : false);
			chkFuse.at(3 * 8 + k)->setEnabled((fBit.fuseenable0 & (0x80 >> k)) ? true : false);
		}
	}
	else
	{
		//Default (disable all)
		for (int k = 0; k < LOCKPACKSIZE; k++)
		{
			QString def = QString().sprintf(" %d", k);
			//Label
			chkLock.at(0 * 8 + k)->setText(def);
			chkLock.at(1 * 8 + k)->setText(def);
			chkLock.at(2 * 8 + k)->setText(def);
			chkLock.at(3 * 8 + k)->setText(def);

			chkFuse.at(0 * 8 + k)->setText(def);
			chkFuse.at(1 * 8 + k)->setText(def);
			chkFuse.at(2 * 8 + k)->setText(def);
			chkFuse.at(3 * 8 + k)->setText(def);

			//Sensitive
			chkLock.at(0 * 8 + k)->setEnabled(false);
			chkLock.at(1 * 8 + k)->setEnabled(false);
			chkLock.at(2 * 8 + k)->setEnabled(false);
			chkLock.at(3 * 8 + k)->setEnabled(false);

			chkFuse.at(0 * 8 + k)->setEnabled(false);
			chkFuse.at(1 * 8 + k)->setEnabled(false);
			chkFuse.at(2 * 8 + k)->setEnabled(false);
			chkFuse.at(3 * 8 + k)->setEnabled(false);
		}
	}

	//Checked
	for (int k = 0; k < LOCKPACKSIZE; k++)
	{
		chkLock.at(0 * 8 + k)->setChecked(((lock >> 24) & (0x80 >> k)) ? true : false); // L0
		chkLock.at(1 * 8 + k)->setChecked(((lock >> 16) & (0x80 >> k)) ? true : false); // L1
		chkLock.at(2 * 8 + k)->setChecked(((lock >>  8) & (0x80 >> k)) ? true : false); // L2
		chkLock.at(3 * 8 + k)->setChecked(((lock) & (0x80 >> k)) ? true : false);     // L3

		chkFuse.at(0 * 8 + k)->setChecked(((fuse >> 24) & (0x80 >> k)) ? true : false); // F0
		chkFuse.at(1 * 8 + k)->setChecked(((fuse >> 16) & (0x80 >> k)) ? true : false); // F1
		chkFuse.at(2 * 8 + k)->setChecked(((fuse >>  8) & (0x80 >> k)) ? true : false); // F2
		chkFuse.at(3 * 8 + k)->setChecked(((fuse) & (0x80 >> k)) ? true : false);     // F3
	}

	chkHlp1->setText(translate(STR_FUSEDLGNOTESET) + " (bit = 0)");
	chkHlp1->setEnabled(false);
	chkHlp0->setText(translate(STR_FUSEDLGNOTECLR) + " (bit = 1)");
	chkHlp0->setEnabled(false);

	if (readonly)
	{
		pushOk->setEnabled(false);
		pushWrite->setEnabled(false);
	}
	else
	{
		pushOk->setEnabled(true);
		pushWrite->setEnabled(true);
	}
}


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

