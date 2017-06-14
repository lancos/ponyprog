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

#ifndef BITFIELDWIDGET_H
#define BITFIELDWIDGET_H

#include <QString>
#include <QWidget>
#include <QObject>


#include "ui_bitfield.h"



class BitFieldWidget : public QWidget, public Ui::FormBitField
{
	Q_OBJECT
  public:               //---------------------------------------- public
	BitFieldWidget(QWidget *bw, ...);
	virtual ~BitFieldWidget();          // Destructor


  protected:    //--------------------------------------- protected

  private slots:
// 	void onOk();


  private:              //--------------------------------------- private
	QVector<QComboBox *> lstWidget;

// 	long *pFrom, *pTo;
// 	int *pVal;
//
// 	long mFrom, mTo;
// 	int mVal;
// 	long mMax;
};

#endif
