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

#ifndef FUSEMDLG_H
#define FUSEMDLG_H

#include <QStringList>
#include <QObject>
#include <QString>
#include <QVector>
#include <QCheckBox>

#include "ui_fusedlg.h"

#include "e2cmdw.h"

/**
 * @brief BitInfo is the main structure for displaying in the QTreeWidget
 */
typedef struct
{
	int  bit;
	const QString ShortDescr;
	const QString LongDescr;
} BitInfo;

/**
 * @brief MaskDescr is the structure for QComboBoxes with help information
 */
typedef struct
{
	const QString mask;
	const QString LongDescr;
} MaskDescr;


typedef struct
{
	long type; // chip id
	QVector<BitInfo> fuse;
	QVector<MaskDescr> fuseDescr;
	QVector<BitInfo> lock;
	QVector<MaskDescr> lockDescr;
} ChipBits;


class fuseModalDialog : public QDialog, public cTranslator, public Ui::FuseDialog
{
	Q_OBJECT
  public:               //---------------------------------------- public
	fuseModalDialog(e2CmdWindow *bw, e2AppWinInfo *p, bool readonly = false, const QString &msg = "");
	virtual ~fuseModalDialog();             // Destructor

  private slots:
	void onOk();
	void onRead();
	void onProg();
	void onFuseComboSelected(int i);
	void onFuseBitClicked(QTreeWidgetItem *itm, int col);
	void onLockComboSelected(int i);
	void onLockBitClicked(QTreeWidgetItem *itm, int col);

  protected:    //--------------------------------------- protected

  private:
	void setTextWidgets();
	void scanMasks();
	void initWidgets(const QString &msg, bool readonly);
	int  eepFindFuses(long type);
	void setMaskBits(QTreeWidget *w, const QString &m, unsigned int bits);

  private:              //--------------------------------------- private
	static QVector<ChipBits> eep_bits;

	e2CmdWindow *cmdw;
	e2AppWinInfo *awip;

	QStringList maskListFuse; // fuse masks
	QStringList maskListLock; // lock masks

	QVector<QComboBox *> lstLock;
	QVector<QComboBox *> lstFuse;

	int currentChip;
	ChipBits currentBitField;
	bool write;
	bool read;
};


#endif
