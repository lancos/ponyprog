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

// #include <QDialog>
#include <QObject>
#include <QString>
#include <QVector>
#include <QCheckBox>

#include "ui_fusedlg.h"

#include "e2cmdw.h"


typedef struct
{
	int  bit;
	QString ShortDescr;
	QString LongDescr;
} BitInfo;


typedef struct
{
	int mask;
	QString LongDescr;
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

  protected:    //--------------------------------------- protected

  private:
	void setTextWidgets();
	void initWidgets(const QString &msg, bool readonly);
	int eep_FindFuses(long type);

  private:              //--------------------------------------- private
	static QVector<ChipBits> eep_bits;

	e2CmdWindow *cmdw;
	e2AppWinInfo *awip;

	bool write;
	bool read;
};


#endif
