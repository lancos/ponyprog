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
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//


#include <QDebug>
#include <QString>


//=========================>>> BitFieldWidget::BitFieldWidget <<<====================
BitFieldWidget::BitFieldWidget(QWidget *bw, long &cfrom, long &cto, int &cval, long max_addr, const QString title) :
	QDialog(bw)
{
	setupUi(this);

// 	setWindowTitle(title);
//
// 	e2CmdWindow *cmdw = static_cast<e2CmdWindow *>(bw);
//
// 	if (cmdw->getStyleSheet().length() > 0)
// 	{
// 		setStyleSheet(cmdw->getStyleSheet());
// 	}
//
// 	qDebug() << "BitFieldWidget::BitFieldWidget()";
//
// 	pFrom = &cfrom;
// 	pTo = &cto;
// 	pVal = &cval;
//
// 	mMax = (max_addr > 0) ? max_addr : 0xFFFFFF;
// 	mFrom = (cfrom < 0) ? 0 : cfrom;
// 	mTo = (cto < 0) ? mMax : cto;
// 	mVal = (cval < 0) ? 0xFF : cval;
//
// 	lblFrom->setText(translate(STR_LBLFROM));
// 	lblTo->setText(translate(STR_LBLTO));
// 	lblVal->setText(translate(STR_LBLVALUE));
//
// 	pushOk->setText(translate(STR_BTNOK));
// 	pushCancel->setText(translate(STR_BTNCANC));
//
// 	//TODO: should get strings from E2Profile settings, not local variables
// 	QString str;
// 	str = QString().sprintf("0x%04lX", mFrom);
// 	txiFrom->setText(str);
//
// 	str = QString().sprintf("0x%04lX", mTo);
// 	txiTo->setText(str);
//
// 	str = QString().sprintf("0x%02X", mVal);
// 	txiVal->setText(str);
//
// 	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
// 	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


//======================>>> BitFieldWidget::~BitFieldWidget <<<======================
BitFieldWidget::~BitFieldWidget()
{
	qDebug() << "BitFieldWidget::~BitFieldWidget()";
}



/**
 * @brief
 */
void BitFieldWidget::initWidgets(const QString &msg, bool readonly)
{
	long type = cmdw->GetCurrentChipType();

	lstWidget = (QVector<QComboBox *>() << comboBox0 << comboBox1 << comboBox2 << comboBox3);

	for (int i = 0; i < 4; i++)
	{
		lstComboWidget.at(i)->setHidden(true);
	}

	currentChip = eepFindFuses(type);

	if (currentChip < 0)
	{
		return;
	}

	currentBitField = eep_bits.at(currentChip);

	if (currentBitField.lock.count() > 0)
	{
		unsigned int lock = awip->GetLockBits();
		lockBits = lock;

		for (int i = 0; i < currentBitField.lock.count(); i++)
		{
			QTreeWidgetItem *itm = new QTreeWidgetItem();
			int bitOffset = currentBitField.lock.at(i).bit;
			itm->setText(0, QString().sprintf("Bit %d, ", bitOffset) + currentBitField.lock.at(i).ShortDescr);
			if (currentBitField.lock.at(i).LongDescr.length() > 0)
			{
				itm->setText(1, currentBitField.lock.at(i).LongDescr);
			}
			itm->setFlags(itm->flags() | Qt::ItemIsUserCheckable);
			itm->setCheckState(0, Qt::Unchecked);

			if (lock & (1 << bitOffset))
			{
				itm->setCheckState(0, Qt::Checked);
			}
			treeWidget->addTopLevelItem(itm);
		}

		treeWidget->expandAll();
		treeWidget->resizeColumnToContents(0);

		connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(onBitClicked(QTreeWidgetItem *, int)));
	}

	scanMasks();

	// now working with maskList

	for (int i = 0; i < maskList.count(); i++)
	{
		QStringList lst;
		QString currentMask = maskList.at(i);
		// two loops are not optimal!
		foreach (MaskDescr mdes, currentBitField.lockDescr)
		{
			if (mdes.mask.indexOf(QRegExp(currentMask)) >= 0)
			{
				lst << mdes.LongDescr;
			}
		}

		// when not all combinations are descripted
		if (isExp(lst.count()) == false)
		{
			lst << "Undefined combination";
		}

		lstComboWidget.at(i)->setHidden(false);
		lstComboWidget.at(i)->addItems(lst);

		connect(lstComboWidget.at(i), SIGNAL(activated(int)), this, SLOT(onComboSelected(int)));
	}

	emit displayBitFields();
}


void BitFieldWidget::onComboSelected(int idx)
{
	QComboBox *s = static_cast<QComboBox *>(sender());
	int globIdx = 0;
	int nMask = -1;
	// we have only 4 comboboxes for lock
	for (int i = 0; i < 4; i++)
	{
		if (lstWidget.at(i) == s)
		{
			nMask = i;
			break;
		}
		globIdx += lstWidget.at(i)->count();

		// correcture for undefined
		if (lstWidget.at(i)->findText("Undefined combination") > 0)
		{
			globIdx--;
		}
	}

	if (nMask < 0)
	{
		return;
	}

	globIdx += idx;

	qDebug() << "onComboSelected index" << globIdx;

	if (currentBitField.lockDescr.at(globIdx).mask.length() == 0)
	{
		return;
	}

	// deactivate signal from checkbutton
	disconnect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(onBitClicked(QTreeWidgetItem *, int)));

	// possible list
	QStringList mskList = currentBitField.lockDescr.at(globIdx).mask.split(" ");
	qDebug() << mskList << currentBitField.lockDescr.at(globIdx).LongDescr;

	foreach (QString cMask, mskList)
	{
		setMaskBits(treeWidget, cMask);
	}

	// activate signal
	connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(onBitClicked(QTreeWidgetItem *, int)));

	emit displayBitFields();
}


void BitFieldWidget::setMaskBits(QTreeWidget *w, const QString &cMask)
{
	int p = cMask.indexOf("=");
	if (p < 0)
	{
		// not correct format
		return;
	}

	QString mskName = cMask.left(p);
	QString bitStr = cMask.mid(p + 1);
	bool ok;

	unsigned int bField = bitStr.toInt(&ok, 2);

	int idx;
	// at begin of string only
	mskName = "^" + mskName + "\\d+";

	qDebug() << cMask <<  "converted to" << mskName << (bin) << bField << (dec);

	// search in QTreeWidget the names
	for (idx = 0; idx < w->topLevelItemCount(); idx++)
	{
		QString t = w->topLevelItem(idx)->text(0);
		int pos = t.indexOf(", ");

		if (pos > 0)
		{
			QString nm = t.mid(pos + 2);

			// first element found
			if (nm.indexOf(QRegExp(mskName)) == 0)
			{
				break;
			}
		}
	}

	if (idx >= w->topLevelItemCount())
	{
		qDebug() << "setMaskBits is wrong";
		return;
	}

	for (int i = 0; i < bitStr.length(); i++)
	{
		Qt::CheckState st;
		if (bField & 0x01)
		{
			st = Qt::Checked;
			if (w == treeWidgetFuse)
			{
				fuseBits |= (1 << idx);
			}
			else
			{
				lockBits |= (1 << idx);
			}
		}
		else
		{
			st = Qt::Unchecked;
			if (w == treeWidgetFuse)
			{
				fuseBits &= ~(1 << idx);
			}
			else
			{
				lockBits &= ~(1 << idx);
			}
		}

		bField >>= 1;
		w->topLevelItem(idx)->setCheckState(0, st);

		QString t = w->topLevelItem(idx)->text(0);
		int pos = t.indexOf(", ");

		if (pos > 0)
		{
			QString nm = t.mid(pos + 2);
			if (nm.indexOf(QRegExp(mskName)) < 0)
			{
				break;
			}
		}
		else
		{
			break;
		}

		idx++;
	}
}



void BitFieldWidget::onBitClicked(QTreeWidgetItem *itm, int col)
{
	Qt::CheckState st = itm->checkState(0);
	QString t =  itm->text(0);
	int pos = t.indexOf(",");

	if (pos < 0)
	{
		return;
	}

	QString fuseName = t.mid(pos + 2);

	t = t.left(pos);
	t.remove("Bit ");

	int bOffset = t.toInt();

	if (st == Qt::Checked)
	{
		fuseBits |= (1 << bOffset);
	}
	else
	{
		fuseBits &= ~(1 << bOffset);
	}

	displayBitFields();

	qDebug() << fuseName << maskListFuse;

	int idxCombo = -1;

	for (int i = 0; i < maskListFuse.count(); i++)
	{
		QString m = maskListFuse.at(i);
		QStringList mSplitted = m.split(" ");

		for (int r = 0; r < mSplitted.count(); r++)
		{
			QString msplt = mSplitted.at(r);
			msplt.remove("=");
			if (fuseName.indexOf(QRegExp(msplt)) >= 0)
			{
				idxCombo = i;
				qDebug() << "found " << msplt << i;
				break;
			}
		}
		if (idxCombo != -1)
		{
			break;
		}
	}

	// maskListFuse.at(idxCombo) has mask to search
	// and idxCombo is the number of list
	if (idxCombo == -1)
	{
		return;
	}

	QString completeMask = maskListFuse.at(idxCombo);

	// now replace the \\d+ with bit information from treewidget
	QStringList mSplitted = completeMask.split(" ");
	for (int r = 0; r < mSplitted.count(); r++)
	{
		QString orig = mSplitted.at(r);
		QString msplt = orig;
		msplt.remove("=");
		qDebug() << msplt;

		QString bitString;
		for (int iTree = 0; iTree < treeWidgetFuse->topLevelItemCount(); ++iTree)
		{
			QString t = treeWidgetFuse->topLevelItem(iTree)->text(0);
			int pos = t.indexOf(", ");
			if (pos <= 0)
			{
				continue;
			}

			t = t.mid(pos + 2);

			if (t.indexOf(QRegExp(msplt)) >= 0)
			{
				if (treeWidgetFuse->topLevelItem(iTree)->checkState(0) == Qt::Unchecked)
				{
					bitString = "0" + bitString;
				}
				else
				{
					bitString = "1" + bitString;
				}
			}
		}

		if (bitString.length() > 0)
		{
			msplt.replace("\\d+", QString("=" + bitString));
			completeMask.replace(orig, msplt);

		}
	}

	qDebug() << "converted: " << completeMask << "idx" << idxCombo;

	if (completeMask.length() > 0)
	{
		int i;
		for (i = 0; i < currentBitField.fuseDescr.count(); i++)
		{
			if (currentBitField.fuseDescr.at(i).mask.indexOf(QRegExp(completeMask)) == 0)
			{
				QString comboText = currentBitField.fuseDescr.at(i).LongDescr;
				int c = lstFuseWidget.at(idxCombo)->findText(comboText);
				if (c >= 0)
				{
					lstFuseWidget.at(idxCombo)->setCurrentIndex(c);
				}

				break;
			}
		}
		if (i == currentBitField.fuseDescr.count())
		{
			int p = lstFuseWidget.at(idxCombo)->findText("Undefined combination");
			if (p >= 0)
			{
				lstFuseWidget.at(idxCombo)->setCurrentIndex(p);
			}
		}
	}
}



