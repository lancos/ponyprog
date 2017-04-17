//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: profile.h,v 1.6 2009/11/15 14:45:00 lancos Exp $
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

#ifndef _PROFILE_H
#define _PROFILE_H

#include <QString>
#include <QVector>

// #define MAXFILENAME     512
// #define MAXLINENUM      1024
// #define MAXLINESIZE     512

class Profile
{
public:               //---------------------------------------- public

	Profile(const QString &name = 0);
	virtual ~Profile();

	QString GetParameter(const QString &id);
	int SetParameter(const QString &id, const QString &value = "");
	void SetFileName(const QString &name = "");

protected:    //--------------------------------------- protected

	QString StripSpace(const QString &sp);
	//      int decnum2str(int value, char *str, int len);
	//      int decnum2str(unsigned long value, char *str, int len);
	//      int hexnum2str(int value, char *str, int len);
	//      int hexnum2str(unsigned long value, char *str, int len);

private:              //--------------------------------------- private

	void FlushVet(int force = 0);
	int ReadVet();
	int WriteVet();

	QString filename; //[MAXFILENAME];             //name of .INI file
	QVector<QString> profilevet; //[MAXLINENUM];   //elenco di parametri (in pratica linee del file)
	QString linebuffer; //[MAXLINESIZE + 1]; //buffer temporaneo di linea
	QString strbuf; //[MAXLINESIZE];
	int cached;                                             //indica se il file e` gia` in memoria
};
#endif
