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

#ifndef FillDIALOG_H
#define FillDIALOG_H

#include <QString>
#include <QDialog>
#include <QObject>

#include "Translator.h"

#include "ui_editdlg.h"



class FillDialog : public QDialog, public cTranslator, public Ui::EditDialog
{
	Q_OBJECT
  public:               //---------------------------------------- public
	FillDialog(QWidget *bw, long &cfrom, long &cto, int &cval, long max_addr, const QString title = translate(STR_LBLFILLBUF));
	virtual ~FillDialog();          // Destructor


  protected:    //--------------------------------------- protected

  private slots:
	void onOk();


  private:              //--------------------------------------- private

	long *pFrom, *pTo;
	int *pVal;

	long mFrom, mTo;
	int mVal;
	long mMax;
};

#endif
