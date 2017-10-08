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


#include <QDebug>
#include <QString>

#include "e2cmdw.h"
#include "globals.h"
#include "infomdlg.h"

class e2CmdWindow;


//======================>>> infoModalDialog::infoModalDialog <<<==================
e24xx_infoModalDialog::e24xx_infoModalDialog(QWidget *bw, int rlv, uint16_t crc, long size, const QString title) :
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


//===================>>> infoModalDialog::~infoModalDialog <<<====================
e24xx_infoModalDialog::~e24xx_infoModalDialog()
{
	qDebug() << "infoModalDialog::~infoModalDialog()";
}


void e24xx_infoModalDialog::onOk()
{
	accept();
}


//======================>>> infoModalDialog::infoModalDialog <<<==================
other_infoModalDialog::other_infoModalDialog(QWidget *bw, long fsize, long esize, uint16_t crc, const QString title) :
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


//===================>>> infoModalDialog::~infoModalDialog <<<====================
other_infoModalDialog::~other_infoModalDialog()
{
	qDebug() << "infoModalDialog::~infoModalDialog()";
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

	qDebug() << "notesModalDialog::notesModalDialog()";

	id_ptr = &id;
	cm_ptr = &cm;

	lblStrID->setText(translate(STR_MSGCHIPID));
	lblCommnt->setText(translate(STR_MSGNOTE));

	if (id.length() && cm.length())
	{
		id_txt = id;
		txiStrID->setText(id_txt);
		cmt_txt = cm;
		txiCommnt->setText(cmt_txt);
	}

	pushOk->setText(translate(STR_BTNOK));
	pushCancel->setText(translate(STR_BTNCANC));

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


notesModalDialog::~notesModalDialog()
{
	qDebug() << "notesModalDialog::~notesModalDialog()";
}


void notesModalDialog::onOk()
{
	cmt_txt = txiCommnt->text();
	id_txt = txiStrID->text();

	*id_ptr = id_txt;
	*cm_ptr = cmt_txt;

	accept();
}

#if 0
editModalDialog::editModalDialog(QWidget *bw, int curval, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);

	e2CmdWindow *cmdw = static_cast<e2CmdWindow *>(bw);

	if (cmdw->getStyleSheet().length() > 0)
	{
		setStyleSheet(cmdw->getStyleSheet());
	}

	qDebug() << "editModalDialog::editModalDialog()";

	if (curval < 0)
	{
		curval = 0;
	}

	lblFrom->setText(translate(STR_MSGHEX));
	lblTo->setText(translate(STR_MSGDECIMAL));
	lblVal->setText(translate(STR_MSGCHAR));

	oldval = curval;

	QString str;
	str = QString().sprintf("%02X", curval);
	txiFrom->setText(str);

	connect(txiFrom, SIGNAL(changed()), this, SLOT(onEdit()));

	str = QString().sprintf("%d", curval);
	txiTo->setText(str);
	connect(txiTo, SIGNAL(changed()), this, SLOT(onEdit()));

	str = QString().sprintf("%c", curval);
	txiVal->setText(str);
	connect(txiVal, SIGNAL(changed()), this, SLOT(onEdit()));

	pushOk->setText(translate(STR_BTNOK));
	pushCancel->setText(translate(STR_BTNCANC));

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


editModalDialog::~editModalDialog()
{
	qDebug() << "editModalDialog::~editModalDialog()";
}


void editModalDialog::onOk()
{
	accept();
}

int editModalDialog::GetVal()
{
	return oldval;
}


void editModalDialog::onEdit()
{
	int newval;
	bool ok;

	QLineEdit *l = static_cast<QLineEdit *>(sender());
	disconnect(txiFrom, SIGNAL(changed()), this, SLOT(onEdit())); // hex
	disconnect(txiTo, SIGNAL(changed()), this, SLOT(onEdit())); // dec
	disconnect(txiVal, SIGNAL(changed()), this, SLOT(onEdit()));  // char

	if (l == txiFrom)  // hex
	{
		QString n = txiFrom->text();
		newval = n.toInt(&ok, 16);

		if (ok == true)
		{
			QPalette *palette = new QPalette();
			palette->setColor(QPalette::Text, Qt::black);
			txiFrom->setPalette(*palette);
		}
		else
		{
			QPalette *palette = new QPalette();
			palette->setColor(QPalette::Text, Qt::red);
			txiFrom->setPalette(*palette);
		}
	}

	if (l == txiTo)  // dec
	{
		QString n = txiTo->text();
		newval = n.toInt(&ok, 10);

		if (ok == true)
		{
			QPalette *palette = new QPalette();
			palette->setColor(QPalette::Text, Qt::black);
			txiTo->setPalette(*palette);
		}
		else
		{
			QPalette *palette = new QPalette();
			palette->setColor(QPalette::Text, Qt::red);
			txiTo->setPalette(*palette);
		}
	}

	if (l == txiVal)  // char
	{
		QString n = txiVal->text();

		if (n.length() != 1)
		{
			ok = false;
			QPalette *palette = new QPalette();
			palette->setColor(QPalette::Text, Qt::red);
			txiVal->setPalette(*palette);
		}
		else
		{
			newval = static_cast<unsigned char>(n.at(0).toLatin1());
			ok = true;
			QPalette *palette = new QPalette();
			palette->setColor(QPalette::Text, Qt::black);
			txiVal->setPalette(*palette);
		}
	}

	if (ok == true)
	{
		if (oldval != newval)
		{
			oldval = newval;
			txiFrom->setText(QString().sprintf("%02X", newval));
			txiTo->setText(QString().sprintf("%d", newval));
			txiVal->setText(QString().sprintf("%c", newval));
		}
	}

	connect(txiFrom, SIGNAL(changed()), this, SLOT(onEdit()));
	connect(txiTo, SIGNAL(changed()), this, SLOT(onEdit()));
	connect(txiVal, SIGNAL(changed()), this, SLOT(onEdit()));
}

const int MAXLEN = 80;

editModalDialog2::editModalDialog2(QWidget *bw, const QString curval, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);

	e2CmdWindow *cmdw = static_cast<e2CmdWindow *>(bw);

	if (cmdw->getStyleSheet().length() > 0)
	{
		setStyleSheet(cmdw->getStyleSheet());
	}

	lblFrom->setHidden(true);
	txiFrom->setHidden(true);

	lblTo->setHidden(true);
	txiTo->setHidden(true);

	lblVal->setText(translate(STR_MSGCHAR));

	qDebug() << "editModalDialog::editModalDialog()";

	val = curval;

	if (val.length() > MAXLEN)
	{
		val = val.left(MAXLEN);
	}

	txiVal->setText(val);

	pushOk->setText(translate(STR_BTNOK));
	pushCancel->setText(translate(STR_BTNCANC));

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


editModalDialog2::~editModalDialog2()
{
	qDebug() << "editModalDialog::~editModalDialog()";
}


QString editModalDialog2::GetVal()
{
	return val;
}

void editModalDialog2::onOk()
{
	val = txiVal->text();

	if (val.length() > MAXLEN)
	{
		val = val.left(MAXLEN);
	}

	accept();
}
#endif
