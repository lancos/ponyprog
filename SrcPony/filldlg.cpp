//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: filldlg.cpp,v 1.4 2007/05/03 10:13:54 lancos Exp $
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


#include <QDebug>
#include <QString>

#include "types.h"
#include "filldlg.h"
#include "string_table.h"


using namespace Translator;

//=========================>>> FillDialog::FillDialog <<<====================
FillDialog::FillDialog(QWidget* bw, long &cfrom, long &cto, int &cval, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);

	qDebug() << "FillDialog::FillDialog()";

	pFrom = &cfrom;
	pTo = &cto;
	pVal = &cval;

	mFrom = (cfrom < 0) ? 0 : cfrom;
	mTo = (cto < 0) ? 0xFFFFFF : cto;
	mVal = (cval < 0) ? 0xFF : cval;

	lblFrom->setText(STR_LBLFROM);
	lblTo->setText(STR_LBLTO);
	lblVal->setText(STR_LBLVALUE);

	pushOk->setText(STR_BTNOK);
	pushCancel->setText(STR_BTNCANC);

	QString str;
	str = QString().sprintf("0x%04lX", mFrom);
	txiFrom->setText(str);

	str = QString().sprintf("0x%04lX", mTo);
	txiTo->setText(str);

	str = QString().sprintf("0x%02X", mVal);
	txiVal->setText(str);

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


//======================>>> FillDialog::~FillDialog <<<======================
FillDialog::~FillDialog()
{
	qDebug() << "FillDialog::~FillDialog()";
}


void FillDialog::onOk()
{
	bool good;
	QString str = txiFrom->text();
	mFrom = str.toLong(&good, 16);

	if (good == false)  // TODO
	{
		QPalette *palette = new QPalette();
		palette->setColor(QPalette::Text, Qt::red);
		txiFrom->setPalette(*palette);

		return;
		//              reject();
	}

	str = txiTo->text();
	mTo = str.toLong(&good, 16);

	if (good == false)  // TODO
	{
		QPalette *palette = new QPalette();
		palette->setColor(QPalette::Text, Qt::red);
		txiTo->setPalette(*palette);

		return;
	}

	str = txiVal->text();
	mVal = str.toLong(&good, 16);

	if (good == false)  // TODO
	{
		QPalette *palette = new QPalette();
		palette->setColor(QPalette::Text, Qt::red);
		txiVal->setPalette(*palette);

		return;
	}

	*pFrom = mFrom;
	*pTo = mTo;
	*pVal = mVal;

	accept();
}

