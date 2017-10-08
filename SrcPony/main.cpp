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

#include <QApplication>
#include <QTextCodec>
#include <QIcon>
#include <QString>
#include <QDebug>

#include "e2cmdw.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	Q_INIT_RESOURCE(ponyprog);

	app.setApplicationName("PonyProg");
	app.setOrganizationName("PonyProg");
	app.setWindowIcon(QIcon(":/icons/ponyprog-small.png"));

	// Identify locale and load translation if available
	//     QString locale = QLocale::system().name();

	e2CmdWindow mainWin;

	mainWin.show();

	return app.exec();
};
