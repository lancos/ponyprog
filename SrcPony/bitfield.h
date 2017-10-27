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

#ifndef BITFIELDWIDGET_H
#define BITFIELDWIDGET_H

#include <QString>
#include <QWidget>
#include <QObject>
#include <QVector>
#include <QComboBox>

#include "ui_bitfield.h"

/**
 * @brief BitInfo is the main structure for displaying in the QTreeWidget
 */
typedef struct
{
	int  bit;                 // bit number
	const QString ShortDescr; // first column
	const QString LongDescr;  // second column
	const QString ExtDescr;   // for additional infos
} BitInfo;

/**
 * @brief MaskDescr is the structure for QComboBoxes with help information
 */
typedef struct
{
	const QString mask;      // in mask are coded name of mask and bits
	const QString LongDescr; //
	const QString ExtDescr;  // for additional infos
} MaskDescr;


class BitFieldWidget : public QWidget, public Ui::FormBitField
{
	Q_OBJECT
  public:               //---------------------------------------- public
	BitFieldWidget(QWidget *parent, QVector<BitInfo> &vInfo, QVector<MaskDescr> &vMask, unsigned int field, const QString &chipNm);
	virtual ~BitFieldWidget();          // Destructor

	unsigned int getBitfield();


  protected:    //--------------------------------------- protected

  signals:
	void displayBitFields(int field);

  private slots:
	void onComboSelected(int i);
	void onBitClicked(QTreeWidgetItem *itm, int col);

  private:
	void initWidget();
	void scanMasks();
	void createComboLists();
	void setMaskBits(const QString &m);//, unsigned int bits);
	bool isExp(unsigned int a);

  private:              //--------------------------------------- private
	QVector<QComboBox *> lstComboBoxes;

	QVector<BitInfo> *vecInfo;
	QVector<MaskDescr> *vecDescr;

	QString chipName;

	unsigned int bField;

	QStringList maskList; // masks
	QVector<int> maskBitSum;
};

#endif
