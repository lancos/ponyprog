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


#include <QDebug>
#include <QString>

#include "e2cmdw.h"
#include "globals.h"
#include "infomdlg.h"

class e2CmdWindow;


e24xx_infoModalDialog::e24xx_infoModalDialog(QWidget *bw, int rlv, quint16 crc, long size, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);

	e2CmdWindow *cmdw = static_cast<e2CmdWindow *>(bw);

	if (cmdw->getStyleSheet().length() > 0)
	{
		setStyleSheet(cmdw->getStyleSheet());
	}

	if (rlv == 1)
	{
		txiFrom->setText(translate(STR_YES));
	}
	else if (rlv == 2)
	{
		txiFrom->setText(translate(STR_NO));
	}
	else
	{
		txiFrom->setText(translate(STR_MSGUNKNOWN));
	}

	lblFrom->setText(translate(STR_MSGCRC)); // crc label
	lblTo->setText(translate(STR_MSGEEPSIZE)); // size label
	lblVal->setText(translate(STR_MSGFLASHSIZE)); // flash size label

	QString str;
	str = QString().sprintf("%04Xh", crc);
	txiTo->setText(str);
	txiTo->setReadOnly(true);

	str = QString().sprintf("%ld Byte", size);
	txiVal->setText(str);
	txiVal->setReadOnly(true);

	lblVal->setHidden(true);
	txiVal->setHidden(true);

	pushOk->setText(translate(STR_BTNCLOSE));
	pushCancel->setHidden(true);

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	//      connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


e24xx_infoModalDialog::~e24xx_infoModalDialog()
{
	qDebug() << Q_FUNC_INFO;
}


void e24xx_infoModalDialog::onOk()
{
	accept();
}


other_infoModalDialog::other_infoModalDialog(QWidget *bw, long fsize, long esize, quint16 crc, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);

	e2CmdWindow *cmdw = static_cast<e2CmdWindow *>(bw);

	if (cmdw->getStyleSheet().length() > 0)
	{
		setStyleSheet(cmdw->getStyleSheet());
	}

	QString str;

	lblFrom->setText(translate(STR_MSGCRC)); // crc label
	lblTo->setText(translate(STR_MSGEEPSIZE)); // size label
	lblVal->setText(translate(STR_MSGFLASHSIZE)); // flash size label

	str = QString().sprintf("%04Xh", crc);
	txiFrom->setText(str);
	txiFrom->setReadOnly(true);

	str = QString().sprintf("%ld Byte", fsize);
	txiTo->setText(str);
	txiTo->setReadOnly(true);

	str = QString().sprintf("%ld Byte", esize);
	txiVal->setText(str);
	txiVal->setReadOnly(true);

	pushOk->setText(translate(STR_BTNCLOSE));
	pushCancel->setHidden(true);

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	//      connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


other_infoModalDialog::~other_infoModalDialog()
{
	qDebug() << Q_FUNC_INFO;
}


void other_infoModalDialog::onOk()
{
	accept();
}


notesModalDialog::notesModalDialog(QWidget *bw, QString &id, QString &cm, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);

	e2CmdWindow *cmdw = static_cast<e2CmdWindow *>(bw);

	if (cmdw->getStyleSheet().length() > 0)
	{
		setStyleSheet(cmdw->getStyleSheet());
	}

	qDebug() << Q_FUNC_INFO;

	id_ptr = &id;
	cm_ptr = &cm;

	lblStrID->setText(translate(STR_MSGCHIPID));
	lblCommnt->setText(translate(STR_MSGNOTE));

	txiStrID->setText(id);
	txiCommnt->setText(cm);

	pushOk->setText(translate(STR_BTNOK));
	pushCancel->setText(translate(STR_BTNCANC));

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


notesModalDialog::~notesModalDialog()
{
	qDebug() << Q_FUNC_INFO;
}


void notesModalDialog::onOk()
{
	*id_ptr = txiStrID->text();
	*cm_ptr = txiCommnt->text();

	accept();
}

