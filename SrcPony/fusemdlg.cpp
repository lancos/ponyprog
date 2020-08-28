//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2021   Claudio Lanconelli                           //
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


#include "fusemdlg.h"

#include <QDebug>
#include <QStringList>
#include <QTreeWidget>


//=========================================================================//
//
// E. Kalinowski
// description for fuse/lock bits is under:
// http://eleccelerator.com/fusecalc/
// http://www.engbedded.com/cgi-bin/fcx.cgi?
//
//=========================================================================//

fuseModalDialog::fuseModalDialog(e2CmdWindow *bw, e2AppWinInfo *p, bool readonly, const QString &msg) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(translate(STR_MSGFUSEDLG));

	awip = p;

	fuseWidget = NULL;
	lockWidget = NULL;

	currentBitField = NULL;

	cmdw = static_cast<e2CmdWindow *>(bw);

	if (cmdw->getStyleSheet().length() > 0)
	{
		setStyleSheet(cmdw->getStyleSheet());
	}

	qDebug() << Q_FUNC_INFO;

	read = write = false;

	setTextWidgets();

	initWidgets(msg, readonly);

	connect(pushRead, SIGNAL(clicked()), this, SLOT(onRead()));
	connect(pushWrite, SIGNAL(clicked()), this, SLOT(onProg()));
	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


fuseModalDialog::~fuseModalDialog()
{
	qDebug() << Q_FUNC_INFO;

	if (currentBitField)
	{
		// TODO: to solve this currentBitField with class, not with structure
		currentBitField->chNames.clear();
		currentBitField->fuse.clear();
		currentBitField->fuseDescr.clear();
		currentBitField->lock.clear();
		currentBitField->lockDescr.clear();
	}
}


void fuseModalDialog::setTextWidgets()
{
	pushRead->setText(translate(STR_BTNREAD));
	pushWrite->setText(translate(STR_BTNWRITE));
	pushOk->setText(translate(STR_BTNOK));
	pushCancel->setText(translate(STR_BTNCANC));
}


void fuseModalDialog::onOk()
{
	if (read == true)
	{
		emit done(2);
	}
	else
	{
		awip->SetLockBits(lockBits);
		awip->SetFuseBits(fuseBits);

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


void fuseModalDialog::getLock(int l)
{
	lockBits = l;
	displayBitFields();
}


void fuseModalDialog::getFuse(int l)
{
	fuseBits = l;
	displayBitFields();
}


/**
 * @brief
 */
void fuseModalDialog::initWidgets(const QString &msg, bool readonly)
{
	quint32 type = cmdw->GetCurrentChipType();

	fuseEnabled = true;
	lockEnabled = true;

	QString nm = awip->GetTypeString(type);

	chkHlp1->setText(translate(STR_FUSEDLGNOTECLR) + " (bit = 1)");
	chkHlp1->setEnabled(false);
	chkHlp0->setText(translate(STR_FUSEDLGNOTESET) + " (bit = 0)");
	chkHlp0->setChecked(true);
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

	currentBitField = awip->GetTypeFuses(type);

	if (currentBitField == NULL)
	{
		return;
	}

	unsigned int f = awip->GetFuseBits();
	if (currentBitField->fuse.count() > 0)
	{
		fuseWidget = new BitFieldWidget(this, currentBitField->fuse, currentBitField->fuseDescr, f, nm);
		tabWidget->addTab(fuseWidget, "Fuse");
		connect(fuseWidget, SIGNAL(displayBitFields(int)), this, SLOT(getFuse(int)));

		fuseBits = fuseWidget->getBitfield();
	}
	else
	{
		fuseEnabled = false;
	}

	unsigned int l = awip->GetLockBits();
	if (currentBitField->lock.count() > 0)
	{
		lockWidget = new BitFieldWidget(this, currentBitField->lock, currentBitField->lockDescr, l, nm);
		tabWidget->addTab(lockWidget, "Lock");
		connect(lockWidget, SIGNAL(displayBitFields(int)), this, SLOT(getLock(int)));

		lockBits = lockWidget->getBitfield();
	}
	else
	{
		lockEnabled = false;
	}

	displayBitFields();
}


void fuseModalDialog::displayBitFields()
{
	QString s;
	if (fuseEnabled == true)
	{
		s = QString().sprintf("Fuse: 0x%08X  ", fuseBits);
	}
	if (lockEnabled == true)
	{
		s +=  QString().sprintf("Lock: 0x%08X", lockBits);
	}
	labelFuseLock->setText(s);
}

