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


#include "fusemdlg.h"

#include <QDebug>
#include <QStringList>
#include <QTreeWidget>
#include <QRadioButton>



//======================>>> fuseModalDialog::fuseModalDialog <<<==================
fuseModalDialog::fuseModalDialog(e2CmdWindow *bw, e2AppWinInfo *p, bool readonly, const QString &msg) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(translate(STR_MSGFUSEDLG));

	awip = p;

	cmdw = static_cast<e2CmdWindow *>(bw);

	if (cmdw->getStyleSheet().length() > 0)
	{
		setStyleSheet(cmdw->getStyleSheet());
	}

	qDebug() << "fuseModalDialog::fuseModalDialog()";

	read = write = false;

	setTextWidgets();

	initWidgets(msg, readonly);

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
	// EK 2017
	// TODO add translations for header of view
	treeWidget->setHeaderLabels(QStringList() << "Bit" << "Description");

	pushRead->setText(translate(STR_BTNREAD));
	pushWrite->setText(translate(STR_BTNWRITE));
	pushOk->setText(translate(STR_BTNOK));
	pushCancel->setText(translate(STR_BTNCANC));
}


void fuseModalDialog::onOk()
{
	unsigned int l = 0;
	unsigned int f = 0;

	for (int j = 0; j < treeWidget->topLevelItemCount(); ++j)
	{
		QTreeWidgetItem *topItem = treeWidget->topLevelItem(j);
		QString topName = topItem->text(0);

		for (int i = 0; i < topItem->childCount(); ++i)
		{
			QString t = topItem->child(i)->text(0);
			if (topItem->child(i)->checkState(0) == false)
			{
				continue;
			}

			int pos = t.indexOf(",");
			if (pos > 0)
			{
				t = t.left(pos);
				t = t.remove("Bit ");
				int bOffset = t.toInt();
				if (topName == "Lock")
				{
					l |= (1 << bOffset);
				}
				if (topName == "Fuse")
				{
					f |= (1 << bOffset);
				}
// 				qDebug() << t << topItem->child(i)->checkState(0);
			}

		}
	}

	// EK 2017
	// TODO to work with chkHlp1/chkHlp0: invert or not bits

	if (read == true)
	{
		emit done(2);
	}
	else
	{
		awip->SetLockBits(l);
		awip->SetFuseBits(f);

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
	for (int k = 0; eep_bits.count(); k++)
	{
		if (eep_bits.at(k).type == type)
		{
			return k;
		}
	}

	return -1;
}

/**
 * @brief
 */
void fuseModalDialog::initWidgets(const QString &msg, bool readonly)
{
	long type = cmdw->GetCurrentChipType();

	int j = eep_FindFuses(type);

	if (j >= 0)
	{
		ChipBits fBit = eep_bits.at(j);
		if (fBit.lock.count() > 0)
		{
			unsigned int lock = awip->GetLockBits();

			QTreeWidgetItem *topItem = new QTreeWidgetItem();
			topItem->setText(0, "Lock");
			treeWidget->addTopLevelItem(topItem);

			for (int i = 0; i < fBit.lock.count(); i++)
			{
				QTreeWidgetItem *itm = new QTreeWidgetItem();
				int bitOffset = fBit.lock.at(i).bit;
				itm->setText(0, QString().sprintf("Bit %d, ", bitOffset) + fBit.lock.at(i).ShortDescr);
				if (fBit.lock.at(i).LongDescr.length() > 0)
				{
					itm->setText(1, fBit.lock.at(i).LongDescr);
				}
				itm->setFlags(itm->flags() | Qt::ItemIsUserCheckable);
				itm->setCheckState(0, Qt::Unchecked);

				if (lock & (1 << bitOffset))
				{
					itm->setCheckState(0, Qt::Checked);
				}
				topItem->addChild(itm);
			}

			treeWidget->expandAll();
			treeWidget->resizeColumnToContents(0);
		}

		if (fBit.fuse.count() > 0)
		{
			unsigned int fuse = awip->GetFuseBits();

			QTreeWidgetItem *topItem = new QTreeWidgetItem();
			topItem->setText(0, "Fuse");
			treeWidget->addTopLevelItem(topItem);

			for (int i = 0; i < fBit.fuse.count(); i++)
			{
				QTreeWidgetItem *itm = new QTreeWidgetItem();
				int bitOffset = fBit.fuse.at(i).bit;
				itm->setText(0, QString().sprintf("Bit %d, ", bitOffset) + fBit.fuse.at(i).ShortDescr);
				if (fBit.fuse.at(i).LongDescr.length() > 0)
				{
					itm->setText(1, fBit.fuse.at(i).LongDescr);
				}
				itm->setFlags(itm->flags() | Qt::ItemIsUserCheckable);
				itm->setCheckState(0, Qt::Unchecked);
				if (fuse & (1 << bitOffset))
				{
					itm->setCheckState(0, Qt::Checked);
				}
				topItem->addChild(itm);
			}

			treeWidget->expandAll();
			treeWidget->resizeColumnToContents(0);
		}
	}

	chkHlp1->setText(translate(STR_FUSEDLGNOTESET) + " (bit = 0)");
	chkHlp1->setEnabled(false);
	chkHlp0->setText(translate(STR_FUSEDLGNOTECLR) + " (bit = 1)");
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
}



QVector<ChipBits> fuseModalDialog::eep_bits =
{

	{
		AT89S8252,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 5, "LB3", ""},
			{ 6, "LB2", ""},
			{ 7, "LB1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		AT89S8253,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "LB3", ""},
			{ 1, "LB2", ""},
			{ 2, "LB1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		AT89S51,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 2, "LB3", ""},
			{ 3, "LB2", ""},
			{ 4, "LB1", "Verify disabled"}
		},
		{
			// lock mask description
			{ 0x00000, "External execution disabled"},
			{ 0x00010, "Verify disabled"},
			{ 0x00018, "MOVC disabled"},
			{ 0x0001c, "No memory lock features enabled"}
		}
	},

	{
		AT89S52,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 2, "LB3", ""},
			{ 3, "LB2", ""},
			{ 4, "LB1", ""}
		},
		{
			// lock mask description
                        { 0x00000, "External execution disabled"},
			{ 0x00010, "Verify disabled"},
			{ 0x00018, "MOVC disabled"},
			{ 0x0001c, "No memory lock features enabled"}
		}
	},

	{
		AT89S53,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 5, "LB3", ""},
			{ 6, "LB2", ""},
			{ 7, "LB1", ""}
		},
		{
			// lock mask description
                        { 0x00000, "External execution disabled"},
			{ 0x00010, "Verify disabled"},
			{ 0x00018, "MOVC disabled"},
			{ 0x0001c, "No memory lock features enabled"}
		}
	},

	{
		AT90S1200,
		{
			// fuse
                        { 0, "Fuse0", "External clock enabled"},
			{ 5, "Fuse5", "Serial program downloading (SPI) enabled"}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
                        { 0x00000, "No memory lock features enabled"},
                        { 0x00002, "Further programming disabled"},
                        { 0x00006, "Further programming and verification disabled"}
		}
	},
	{
		AT90S2313,
		{
			// fuse
                        { 0, "Fuse0", "Short start-up time enabled"},
			{ 5, "Fuse5", "Serial program downloading (SPI) enabled"}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
                        { 0x00000, "No memory lock features enabled"},
                        { 0x00002, "Further programming disabled"},
                        { 0x00006, "Further programming and verification disabled"}
		}
	},
	{
		AT90S4414,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},
	{
		AT90S8515,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},
	{
		AT90S4434,
		{
			// fuse
			{ 0, "FSTRT", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},
	{
		AT90S8535,
		{
			// fuse
			{ 0, "FSTRT", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},
	{
		AT90S2333,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "BODEN", ""},
			{ 4, "BODLEVEL", ""},
			{ 5, "SPIEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		AT90S4433,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "BODEN", ""},
			{ 4, "BODLEVEL", ""},
			{ 5, "SPIEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		AT90S2323,
		{
			// fuse
			{ 0, "FSTRT", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		AT90S2343,
		{
			// fuse
			{0, "RCEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		AT90S8534,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		AT90CAN32,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "TA0SEL", ""},
			{ 17, "BODLEVEL0", ""},
			{ 18, "BODLEVEL1", ""},
			{ 19, "BODLEVEL2", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		AT90CAN64,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "TA0SEL", ""},
			{ 17, "BODLEVEL0", ""},
			{ 18, "BODLEVEL1", ""},
			{ 19, "BODLEVEL2", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		AT90CAN128,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "TA0SEL", ""},
			{ 17, "BODLEVEL0", ""},
			{ 18, "BODLEVEL1", ""},
			{ 19, "BODLEVEL2", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATtiny12,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "RSTDISBL", ""},
			{ 5, "SPIEN", ""},
			{ 6, "BODEN", ""},
			{ 7, "BODLEVEL", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATtiny13,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "SUT0", ""},
			{ 3, "SUT1", ""},
			{ 4, "CKDIV8", ""},
			{ 5, "WDTON", ""},
			{ 6, "EESAVE", ""},
			{ 7, "SPIEN", ""},
			{ 8, "RSTDISBL", ""},
			{ 9, "BODLEVEL0", ""},
			{ 10, "BODLEVEL1", ""},
			{ 11, "DWEN", ""},
			{ 12, "SELFPRGEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},
	{
		ATtiny15,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 4, "RSTDISBL", ""},
			{ 5, "SPIEN", ""},
			{ 6, "BODEN", ""},
			{ 7, "BODLEVEL", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATtiny22,
		{
			// fuse
			{ 0, "RCEN", ""},
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},
	{
		ATtiny26,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOPT", ""},
			{ 7, "PLLCK", ""},
			{ 8, "BODEN", ""},
			{ 9, "BODLEVEL", ""},
			{ 10, "EESAVE", ""},
			{ 11, "SPIEN", ""},
			{ 12, "RSTDISBL", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATtiny2313,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "RSTDISBL", ""},
			{ 9, "BODLEVEL0", ""},
			{ 10, "BODLEVEL1", ""},
			{ 11, "BODLEVEL2", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "EESAVE", ""},
			{ 15, "DWEN", ""},
			{ 16, "SPMEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},
	{
		ATtiny25,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BODLEVEL0", ""},
			{ 9, "BODLEVEL1", ""},
			{ 10, "BODLEVEL2", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "DWEN", ""},
			{ 15, "RSTDISBL", ""},
			{ 16, "SELFPRGEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATtiny45,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BODLEVEL0", ""},
			{ 9, "BODLEVEL1", ""},
			{ 10, "BODLEVEL2", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "DWEN", ""},
			{ 15, "RSTDISBL", ""},
			{ 16, "SELFPRGEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},
	{
		ATtiny85,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BODLEVEL0", ""},
			{ 9, "BODLEVEL1", ""},
			{ 10, "BODLEVEL2", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "DWEN", ""},
			{ 15, "RSTDISBL", ""},
			{ 16, "SELFPRGEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATtiny261,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BODLEVEL0", ""},
			{ 9, "BODLEVEL1", ""},
			{ 10, "BODLEVEL2", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "DWEN", ""},
			{ 15, "RSTDISBL", ""},
			{ 16, "SELFPRGEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATtiny461,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BODLEVEL0", ""},
			{ 9, "BODLEVEL1", ""},
			{ 10, "BODLEVEL2", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "DWEN", ""},
			{ 15, "RSTDISBL", ""},
			{ 16, "SELFPRGEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATtiny861,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BODLEVEL0", ""},
			{ 9, "BODLEVEL1", ""},
			{ 10, "BODLEVEL2", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "DWEN", ""},
			{ 15, "RSTDISBL", ""},
			{ 16, "SELFPRGEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega48,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BODLEVEL0", ""},
			{ 9, "BODLEVEL1", ""},
			{ 10, "BODLEVEL2", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "DWEN", ""},
			{ 15, "RSTDISBL", ""},
			{ 16, "SELFPRGEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega88,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BODLEVEL0", ""},
			{ 9, "BODLEVEL1", ""},
			{ 10, "BODLEVEL2", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "DWEN", ""},
			{ 15, "RSTDISBL", ""},
			{ 16, "BOOTRST", ""},
			{ 17, "BOOTSZ0", ""},
			{ 18, "BOOTSZ1", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega168,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BODLEVEL0", ""},
			{ 9, "BODLEVEL1", ""},
			{ 10, "BODLEVEL2", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "DWEN", ""},
			{ 15, "RSTDISBL", ""},
			{ 16, "BOOTRST", ""},
			{ 17, "BOOTSZ0", ""},
			{ 18, "BOOTSZ1", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega328,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BODLEVEL0", ""},
			{ 9, "BODLEVEL1", ""},
			{ 10, "BODLEVEL2", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "DWEN", ""},
			{ 15, "RSTDISBL", ""},
			{ 16, "BOOTRST", ""},
			{ 17, "BOOTSZ0", ""},
			{ 18, "BOOTSZ1", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega164,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "BODLEVEL0", ""},
			{ 17, "BODLEVEL1", ""},
			{ 18, "BODLEVEL2", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega324,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "BODLEVEL0", ""},
			{ 17, "BODLEVEL1", ""},
			{ 18, "BODLEVEL2", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega644,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "BODLEVEL0", ""},
			{ 17, "BODLEVEL1", ""},
			{ 18, "BODLEVEL2", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega640,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "BODLEVEL0", ""},
			{ 17, "BODLEVEL1", ""},
			{ 18, "BODLEVEL2", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega1280,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "BODLEVEL0", ""},
			{ 17, "BODLEVEL1", ""},
			{ 18, "BODLEVEL2", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega1281,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "BODLEVEL0", ""},
			{ 17, "BODLEVEL1", ""},
			{ 18, "BODLEVEL2", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega2560,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "BODLEVEL0", ""},
			{ 17, "BODLEVEL1", ""},
			{ 18, "BODLEVEL2", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega2561,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "BODLEVEL0", ""},
			{ 17, "BODLEVEL1", ""},
			{ 18, "BODLEVEL2", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega603,
		{
			// fuse
			{ 0, "SUT0", ""},
			{ 1, "SUT1", ""},
			{ 3, "EESAVE", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega103,
		{
			// fuse
			{ 0, "SUT0", ""},
			{ 1, "SUT1", ""},
			{ 3, "EESAVE", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 1, "Lock1", ""},
			{ 2, "Lock2", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega8,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "BODEN", ""},
			{ 7, "BODLEVEL", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "CKOPT", ""},
			{ 13, "SPIEN", ""},
			{ 14, "WDTON", ""},
			{ 15, "RSTDISBL", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega8515,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "BODEN", ""},
			{ 7, "BODLEVEL", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "CKOPT", ""},
			{ 13, "SPIEN", ""},
			{ 14, "WDTON", ""},
			{ 15, "S8515C", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega8535,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "BODEN", ""},
			{ 7, "BODLEVEL", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "CKOPT", ""},
			{ 13, "SPIEN", ""},
			{ 14, "WDTON", ""},
			{ 15, "S8535C", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega64,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "BODEN", ""},
			{ 7, "BODLEVEL", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "CKOPT", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "WDTON", ""},
			{ 17, "M103C", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega128,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "BODEN", ""},
			{ 7, "BODLEVEL", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "CKOPT", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "WDTON", ""},
			{ 17, "M103C", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega16,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "BODEN", ""},
			{ 7, "BODLEVEL", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "CKOPT", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega161,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "BODEN", ""},
			{ 4, "BODLEVEL", ""},
			{ 5, "SPIEN", ""},
			{ 6, "BOOTRST", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega163,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 6, "BODEN", ""},
			{ 7, "BODLEVEL", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega162,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 17, "BOD0LEVEL", ""},
			{ 18, "BOD1LEVEL", ""},
			{ 19, "BOD2LEVEL", ""},
			{ 20, "M161C", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega169,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "CKOUT", ""},
			{ 7, "CKDIV8", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "WDTON", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""},
			{ 16, "RSTDISBL", ""},
			{ 17, "BOD0LEVEL", ""},
			{ 18, "BOD1LEVEL", ""},
			{ 19, "BOD2LEVEL", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega323,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 6, "BODEN", ""},
			{ 7, "BODLEVEL", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		ATmega32,
		{
			// fuse
			{ 0, "CKSEL0", ""},
			{ 1, "CKSEL1", ""},
			{ 2, "CKSEL2", ""},
			{ 3, "CKSEL3", ""},
			{ 4, "SUT0", ""},
			{ 5, "SUT1", ""},
			{ 6, "BODEN", ""},
			{ 7, "BODLEVEL", ""},
			{ 8, "BOOTRST", ""},
			{ 9, "BOOTSZ0", ""},
			{ 10, "BOOTSZ1", ""},
			{ 11, "EESAVE", ""},
			{ 12, "CKOPT", ""},
			{ 13, "SPIEN", ""},
			{ 14, "JTAGEN", ""},
			{ 15, "OCDEN", ""}
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "Lock1", ""},
			{ 1, "Lock2", ""},
			{ 2, "BootLock01", ""},
			{ 3, "BootLock02", ""},
			{ 4, "BootLock11", ""},
			{ 5, "BootLock12", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC12508,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "CP", ""},
			{ 4, "MCLRE", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC12509,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "CP", ""},
			{ 4, "MCLRE", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC12508A,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "CP", ""},
			{ 4, "MCLRE", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC12509A,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "CP", ""},
			{ 4, "MCLRE", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC12E518,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "CP", ""},
			{ 4, "MCLRE", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC12E519,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "CP", ""},
			{ 4, "MCLRE", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC12671,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "FOSC2", ""},
			{ 3, "WDTE", ""},
			{ 4, "PWRTE", ""},
			{ 5, "CP0", ""},
			{ 6, "CP1", ""},
			{ 7, "MCLRE", ""},
			{ 8, "CP0", ""},
			{ 9, "CP1", ""},
			{ 10, "CP0", ""},
			{ 11, "CP1", ""},
			{ 12, "CP0", ""},
			{ 13, "CP1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC12672,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "FOSC2", ""},
			{ 3, "WDTE", ""},
			{ 4, "PWRTE", ""},
			{ 5, "CP0", ""},
			{ 6, "CP1", ""},
			{ 7, "MCLRE", ""},
			{ 8, "CP0", ""},
			{ 9, "CP1", ""},
			{ 10, "CP0", ""},
			{ 11, "CP1", ""},
			{ 12, "CP0", ""},
			{ 13, "CP1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC12673,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "FOSC2", ""},
			{ 3, "WDTE", ""},
			{ 4, "PWRTE", ""},
			{ 5, "CP0", ""},
			{ 6, "CP1", ""},
			{ 7, "MCLRE", ""},
			{ 8, "CP0", ""},
			{ 9, "CP1", ""},
			{ 10, "CP0", ""},
			{ 11, "CP1", ""},
			{ 12, "CP0", ""},
			{ 13, "CP1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC12674,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "FOSC2", ""},
			{ 3, "WDTE", ""},
			{ 4, "PWRTE", ""},
			{ 5, "CP0", ""},
			{ 6, "CP1", ""},
			{ 7, "MCLRE", ""},
			{ 8, "CP0", ""},
			{ 9, "CP1", ""},
			{ 10, "CP0", ""},
			{ 11, "CP1", ""},
			{ 12, "CP0", ""},
			{ 13, "CP1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC1683,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "PWRTE", ""},
			{ 4, "CP", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC1684,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "PWRTE", ""},
			{ 4, "CP", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC1684A,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "PWRTE", ""},
			{ 4, "CP", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC16870,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "PWRTE", ""},
			{ 4, "CP0", ""},
			{ 5, "CP1", ""},
			{ 6, "BODEN", ""},
			{ 7, "LVP", ""},
			{ 8, "CPD", ""},
			{ 9, "WRT", ""},
			{ 11, "RESV", ""},
			{ 12, "CP0", ""},
			{ 13, "CP1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC16871,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "PWRTE", ""},
			{ 4, "CP0", ""},
			{ 5, "CP1", ""},
			{ 6, "BODEN", ""},
			{ 7, "LVP", ""},
			{ 8, "CPD", ""},
			{ 9, "WRT", ""},
			{ 11, "RESV", ""},
			{ 12, "CP0", ""},
			{ 13, "CP1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC16872,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "PWRTE", ""},
			{ 4, "CP0", ""},
			{ 5, "CP1", ""},
			{ 6, "BODEN", ""},
			{ 7, "LVP", ""},
			{ 8, "CPD", ""},
			{ 9, "WRT", ""},
			{ 11, "RESV", ""},
			{ 12, "CP0", ""},
			{ 13, "CP1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC16873,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "PWRTE", ""},
			{ 4, "CP0", ""},
			{ 5, "CP1", ""},
			{ 6, "BODEN", ""},
			{ 7, "LVP", ""},
			{ 8, "CPD", ""},
			{ 9, "WRT", ""},
			{ 11, "RESV", ""},
			{ 12, "CP0", ""},
			{ 13, "CP1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC16874,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "PWRTE", ""},
			{ 4, "CP0", ""},
			{ 5, "CP1", ""},
			{ 6, "BODEN", ""},
			{ 7, "LVP", ""},
			{ 8, "CPD", ""},
			{ 9, "WRT", ""},
			{ 11, "RESV", ""},
			{ 12, "CP0", ""},
			{ 13, "CP1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC16876,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "PWRTE", ""},
			{ 4, "CP0", ""},
			{ 5, "CP1", ""},
			{ 6, "BODEN", ""},
			{ 7, "LVP", ""},
			{ 8, "CPD", ""},
			{ 9, "WRT", ""},
			{ 11, "RESV", ""},
			{ 12, "CP0", ""},
			{ 13, "CP1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC16877,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTE", ""},
			{ 3, "PWRTE", ""},
			{ 4, "CP0", ""},
			{ 5, "CP1", ""},
			{ 6, "BODEN", ""},
			{ 7, "LVP", ""},
			{ 8, "CPD", ""},
			{ 9, "WRT", ""},
			{ 11, "RESV", ""},
			{ 12, "CP0", ""},
			{ 13, "CP1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC16873A,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTEN", ""},
			{ 3, "/PWRTEN", ""},
			{ 6, "BOREN", ""},
			{ 7, "LVP", ""},
			{ 8, "CPD", ""},
			{ 9, "WRT0", ""},
			{ 11, "WRT1", ""},
			{ 12, "DEBUG", ""},
			{ 14, "CP", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC16874A,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTEN", ""},
			{ 3, "/PWRTEN", ""},
			{ 6, "BOREN", ""},
			{ 7, "LVP", ""},
			{ 8, "CPD", ""},
			{ 9, "WRT0", ""},
			{ 11, "WRT1", ""},
			{ 12, "DEBUG", ""},
			{ 14, "CP", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC16876A,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTEN", ""},
			{ 3, "/PWRTEN", ""},
			{ 6, "BOREN", ""},
			{ 7, "LVP", ""},
			{ 8, "CPD", ""},
			{ 9, "WRT0", ""},
			{ 11, "WRT1", ""},
			{ 12, "DEBUG", ""},
			{ 14, "CP", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC16877A,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTEN", ""},
			{ 3, "/PWRTEN", ""},
			{ 6, "BOREN", ""},
			{ 7, "LVP", ""},
			{ 8, "CPD", ""},
			{ 9, "WRT0", ""},
			{ 11, "WRT1", ""},
			{ 12, "DEBUG", ""},
			{ 14, "CP", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC16627,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTEN", ""},
			{ 3, "/PWRTEN", ""},
			{ 4, "FOSC2", ""},
			{ 5, "MCLRE", ""},
			{ 6, "BOREN", ""},
			{ 7, "LVP", ""},
			{ 8, "CPD", ""},
			{ 10, "CP0", ""},
			{ 11, "CP1", ""},
			{ 12, "CP0", ""},
			{ 13, "CP1", ""}
		},
		{
			// lock mask description
		}
	},

	{
		PIC16628,
		{
			// fuse
		},
		{
			// fuse mask description
		},
		{
			// lock
			{ 0, "FOSC0", ""},
			{ 1, "FOSC1", ""},
			{ 2, "WDTEN", ""},
			{ 3, "/PWRTEN", ""},
			{ 4, "FOSC2", ""},
			{ 5, "MCLRE", ""},
			{ 6, "BOREN", ""},
			{ 7, "LVP", ""},
			{ 8, "CPD", ""},
			{ 10, "CP0", ""},
			{ 11, "CP1", ""},
			{ 12, "CP0", ""},
			{ 13, "CP1", ""}
		},
		{
			// lock mask description
		}
	}
};

