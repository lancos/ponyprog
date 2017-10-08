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
#include "types.h"
#include "filldlg.h"

class e2CmdWindow;

//=========================>>> FillDialog::FillDialog <<<====================
FillDialog::FillDialog(QWidget *bw, long &cfrom, long &cto, int &cval, long max_addr, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);

	e2CmdWindow *cmdw = static_cast<e2CmdWindow *>(bw);

	if (cmdw->getStyleSheet().length() > 0)
	{
		setStyleSheet(cmdw->getStyleSheet());
	}

	qDebug() << "FillDialog::FillDialog()";

	pFrom = &cfrom;
	pTo = &cto;
	pVal = &cval;

	mMax = (max_addr > 0) ? max_addr : 0xFFFFFF;
	mFrom = (cfrom < 0) ? 0 : cfrom;
	mTo = (cto < 0) ? mMax : cto;
	mVal = (cval < 0) ? 0xFF : cval;

	lblFrom->setText(translate(STR_LBLFROM));
	lblTo->setText(translate(STR_LBLTO));
	lblVal->setText(translate(STR_LBLVALUE));

	pushOk->setText(translate(STR_BTNOK));
	pushCancel->setText(translate(STR_BTNCANC));

	//TODO: should get strings from E2Profile settings, not local variables
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
	bool bad = false;
	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Text, Qt::black);   //Color for good parameter
	txiFrom->setPalette(*palette);
	txiTo->setPalette(*palette);
	txiVal->setPalette(*palette);

	palette->setColor(QPalette::Text, Qt::red);             //Color for bad parameter

	QString str = txiFrom->text();
	mFrom = str.toLong(&good, 0);   //0 accept both decimal and hex format (with 0x prefix)

	//Check from field format
	if (good == false)
	{
		txiFrom->setPalette(*palette);
		bad = true;
	}

	str = txiTo->text();
	mTo = str.toLong(&good, 0);

	//Check to field format
	if (good == false)
	{
		txiTo->setPalette(*palette);
		bad = true;
	}

	str = txiVal->text();
	mVal = str.toLong(&good, 0);

	//Check val field format
	if (good == false)
	{
		txiVal->setPalette(*palette);
		bad = true;
	}

	if (bad)
	{
		return;
	}

	//Check from field range
	if (mFrom >= mMax)
	{
		txiFrom->setPalette(*palette);
		bad = true;
	}

	//Check to field range
	if (mTo < mFrom || mTo > mMax)
	{
		txiTo->setPalette(*palette);
		bad = true;
	}

	//Check value field range
	if (mVal < 0 || mVal > 0xff)
	{
		txiVal->setPalette(*palette);
		bad = true;
	}

	if (bad)
	{
		return;
	}

	*pFrom = mFrom;
	*pTo = mTo;
	*pVal = mVal;

	accept();
}

