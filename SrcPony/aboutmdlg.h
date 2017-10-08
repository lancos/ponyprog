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

#ifndef aboutMDLG_H
#define aboutMDLG_H

#include "ui_aboutdlg.h"

#include <QString>
#include <QObject>
#include <QWidget>

#include "e2cmdw.h"

class e2CmdWindow;

class AboutModalDialog :  public QDialog, public cTranslator, public Ui::AboutDialog
{
	Q_OBJECT
  public:               //---------------------------------------- public
	AboutModalDialog(QWidget *bw = 0, const QString title = "About");
	virtual ~AboutModalDialog();            // Destructor

  protected:    //--------------------------------------- protected

  private slots:
	void onChkStart(bool);
	void onHelp();

  private:              //--------------------------------------- private

	e2CmdWindow *cmdw;
};

#endif

