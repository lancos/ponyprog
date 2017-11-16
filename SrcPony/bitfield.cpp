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
#include <QMessageBox>

#include "bitfield.h"


//=========================>>> BitFieldWidget::BitFieldWidget <<<====================
BitFieldWidget::BitFieldWidget(QWidget *parent, QVector<BitInfo> &vInfo, QVector<MaskDescr> &vMask, unsigned int field, const QString &chipNm) :
	QWidget(parent)
{
	setupUi(this);

	chipName = chipNm;

	vecInfo = &vInfo;

	vecDescr = &vMask;

	bField = field;

	lstComboBoxes = (QVector<QComboBox *>() << comboBox0 << comboBox1 << comboBox2 << comboBox3);

	for (int i = 0; i < lstComboBoxes.count(); i++)
	{
		lstComboBoxes.at(i)->setHidden(true);
	}

	initWidget();
}


//======================>>> BitFieldWidget::~BitFieldWidget <<<======================
BitFieldWidget::~BitFieldWidget()
{
	qDebug() << "BitFieldWidget::~BitFieldWidget()";
}


void BitFieldWidget::initWidget()
{

	if (vecInfo->count() > 0)
	{
		int lastBit = 0;
		for (int i = 0; i < vecInfo->count(); i++)
		{
			QTreeWidgetItem *itm = new QTreeWidgetItem();
			int bitOffset = vecInfo->at(i).bit;
			lastBit = bitOffset;
			QString sDes = vecInfo->at(i).ShortDescr;
			itm->setText(0, QString().sprintf("Bit %d, ", bitOffset) + sDes);
			if (vecInfo->at(i).LongDescr.length() > 0)
			{
				itm->setText(1, vecInfo->at(i).LongDescr);
			}

			itm->setFlags(itm->flags() | Qt::ItemIsUserCheckable);

			itm->setCheckState(0, Qt::Unchecked);

			if (bField & (1 << bitOffset))
			{
				itm->setCheckState(0, Qt::Checked);
			}
			if (sDes == "SPIEN")
			{
				itm->setFlags(itm->flags() ^ Qt::ItemIsEnabled);
				itm->setCheckState(0, Qt::Checked);
				bField |= (1 << bitOffset);
			}
			if (sDes == "RSTDISBL")
			{
				itm->setCheckState(0, Qt::Unchecked);
				bField &= ~(1 << bitOffset);
			}

			treeWidget->addTopLevelItem(itm);
		}

		// reset not used bits
		for (int i = (lastBit + 1); i < 32; i++)
		{
			bField &= ~(1 << i);
		}

		treeWidget->expandAll();
		treeWidget->resizeColumnToContents(0);

		connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(onBitClicked(QTreeWidgetItem *, int)));
	}

	scanMasks();

	createComboLists();

	// init of comboboxes
	for (int i = 0; i < treeWidget->topLevelItemCount(); i++)
	{
		if (treeWidget->topLevelItem(i)->text(1).length() == 0)
		{
			emit onBitClicked(treeWidget->topLevelItem(i), 0);
		}
	}
}


unsigned int BitFieldWidget::getBitfield()
{
	return bField;
}


void BitFieldWidget::createComboLists()
{
	for (int i = 0; i < maskList.count(); i++)
	{
		QStringList lst;
		QString currentMask = maskList.at(i);
		// two loops are not optimal!
		foreach (MaskDescr mdes, *vecDescr)
		{
			if (mdes.mask.indexOf(QRegExp(currentMask)) >= 0)
			{
				int posBeg = mdes.LongDescr.indexOf("=$n");
				if (posBeg > 0)
				{
					QString tStr = mdes.LongDescr;
					// TODO replace text with text from mdes.ExtDescr
					int posExtText = mdes.ExtDescr.indexOf(chipName + ": ");
					if (posExtText >= 0)
					{
						posExtText += chipName.length();
						QString addrTxt = mdes.ExtDescr.simplified().mid(posExtText + 2, 5);

						tStr.replace(posBeg + 1, 5, addrTxt);
					}

					lst << tStr;
				}
				else
				{
					lst << mdes.LongDescr;
				}
			}
		}

		// when not all combinations are descripted
// 		if (isExp(lst.count()) == false)
		if ((1 << maskBitSum.at(i)) != lst.count())
		{
			lst << "Undefined combination";
		}

		lstComboBoxes.at(i)->setHidden(false);
		lstComboBoxes.at(i)->addItems(lst);

		connect(lstComboBoxes.at(i), SIGNAL(activated(int)), this, SLOT(onComboSelected(int)));
	}
}


void BitFieldWidget::scanMasks()
{
	maskList.clear();
	maskBitSum.clear();

	// analyse from mask entries
	foreach (MaskDescr mdes, *vecDescr)
	{
		QString cMask = mdes.mask;
		cMask.replace(QRegExp("\\d+"), "\\d+");
		// at string begin only
		cMask = "^" + cMask;
		if (maskList.indexOf(cMask) == -1)
		{
			maskList << cMask;
			int numBits = 0;
			QRegExp rx("(\\d+)");
			int pos = 0;
			while ((pos = rx.indexIn(mdes.mask, pos)) != -1)
			{
				QString n =  rx.cap(1);
				numBits += n.length();

				pos += rx.matchedLength();
			}

			qDebug() << mdes.mask << "bits" << numBits;
			maskBitSum << numBits;
		}
	}

	qDebug() << maskList;
}


bool BitFieldWidget::isExp(unsigned int a)
{
	return (a > 0 && (a & (a - 1)) == 0);
}


void BitFieldWidget::onComboSelected(int idx)
{
	QComboBox *s = static_cast<QComboBox *>(sender());
	int globIdx = 0;
	int nMask = -1;

	// no changes in this case
	if (s->currentText() == "Undefined combination")
	{
		return;
	}

	// we have only 4 comboboxes for lock
	for (int i = 0; i < 4; i++)
	{
		if (lstComboBoxes.at(i) == s)
		{
			nMask = i;
			break;
		}
		globIdx += lstComboBoxes.at(i)->count();

		// correcture for undefined
		if (lstComboBoxes.at(i)->findText("Undefined combination") > 0)
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

	if (vecDescr->at(globIdx).mask.length() == 0)
	{
		return;
	}

	// deactivate signal from checkbutton
	disconnect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(onBitClicked(QTreeWidgetItem *, int)));

	// possible list
	QStringList mskList = vecDescr->at(globIdx).mask.split(" ");
	qDebug() << mskList << vecDescr->at(globIdx).LongDescr;

	foreach (QString cMask, mskList)
	{
		setMaskBits(cMask);
	}

	// activate signal
	connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(onBitClicked(QTreeWidgetItem *, int)));

	emit displayBitFields(bField);
}


void BitFieldWidget::setMaskBits(const QString &cMask)
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

	unsigned int localField = bitStr.toInt(&ok, 2);



	int idx;
	// at begin of string only
	mskName = "^" + mskName + "\\d+";

	qDebug() << cMask <<  "converted to" << mskName << (bin) << localField << (dec);

	// search in QTreeWidget the names
	for (idx = 0; idx < treeWidget->topLevelItemCount(); idx++)
	{
		QString t = treeWidget->topLevelItem(idx)->text(0);
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

	if (idx >= treeWidget->topLevelItemCount())
	{
		qDebug() << "setMaskBits is wrong";
		return;
	}

	int bitOffset = vecInfo->at(idx).bit;

	for (int i = 0; i < bitStr.length(); i++)
	{
		Qt::CheckState st;
		if (localField & 0x01)
		{
			st = Qt::Unchecked;

			bField |= (1 << bitOffset);
		}
		else
		{
			st = Qt::Checked;

			bField &= ~(1 << bitOffset);
		}

		localField >>= 1;

		treeWidget->topLevelItem(idx)->setCheckState(0, st);

		QString t = treeWidget->topLevelItem(idx)->text(0);
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

		bitOffset++;
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

	if (fuseName == "RSTDISBL")
	{
		QMessageBox msgBox(QMessageBox::Warning, "Warning", "Attention! If you disable Reset pin you can't access the the chip anymore", QMessageBox::Yes | QMessageBox::No);
// 		msgBox.setStyleSheet(programStyleSheet);
		msgBox.setButtonText(QMessageBox::Yes, "Yes");
		msgBox.setButtonText(QMessageBox::No, "No");
		int res = msgBox.exec();
		if (res == QMessageBox::No)
		{
			return;
		}
	}

	t = t.left(pos);
	t.remove("Bit ");

	int bOffset = t.toInt();

	if (st == Qt::Checked)
	{
		bField |= (1 << bOffset);
	}
	else
	{
		bField &= ~(1 << bOffset);
	}

	displayBitFields(bField);

	qDebug() << fuseName << maskList;

	int idxCombo = -1;

	for (int i = 0; i < maskList.count(); i++)
	{
		QString m = maskList.at(i);
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

	QString completeMask = maskList.at(idxCombo);

	// now replace the \\d+ with bit information from treewidget
	QStringList mSplitted = completeMask.split(" ");
	for (int r = 0; r < mSplitted.count(); r++)
	{
		QString orig = mSplitted.at(r);
		QString msplt = orig;
		msplt.remove("=");
		qDebug() << msplt;

		QString bitString;
		for (int iTree = 0; iTree < treeWidget->topLevelItemCount(); ++iTree)
		{
			QString t = treeWidget->topLevelItem(iTree)->text(0);
			int pos = t.indexOf(", ");
			if (pos <= 0)
			{
				continue;
			}

			t = t.mid(pos + 2);

			if (t.indexOf(QRegExp(msplt)) >= 0)
			{
				if (treeWidget->topLevelItem(iTree)->checkState(0) == Qt::Checked)
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
		for (i = 0; i < vecDescr->count(); i++)
		{
			if (vecDescr->at(i).mask.indexOf(QRegExp(completeMask)) == 0)
			{
				QString comboText = vecDescr->at(i).LongDescr;
				int c = lstComboBoxes.at(idxCombo)->findText(comboText);
				if (c >= 0)
				{
					lstComboBoxes.at(idxCombo)->setCurrentIndex(c);
				}

				break;
			}
		}
		if (i == vecDescr->count())
		{
			int p = lstComboBoxes.at(idxCombo)->findText("Undefined combination");
			if (p >= 0)
			{
				lstComboBoxes.at(idxCombo)->setCurrentIndex(p);
			}
		}
	}
}



