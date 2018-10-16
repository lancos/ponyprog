//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
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

#ifndef infoMDLG_H
#define infoMDLG_H

#include <QString>
#include <QObject>
#include <QDialog>

#include "Translator.h"

#include "types.h"

#include "ui_notesdlg.h"
#include "ui_editdlg.h"
// #include "ui_blockdlg.h"


class e24xx_infoModalDialog : public QDialog, public cTranslator, public Ui::EditDialog
{
	Q_OBJECT
  public:
	e24xx_infoModalDialog(QWidget *bw, int rlv, uint16_t crc, long size, const QString title = translate(STR_MSGDEVINFO));
	virtual ~e24xx_infoModalDialog();               // Destructor
	//      virtual int infoAction(char* msg = "Device Info");

  private slots:
	void onOk();

  protected:

  private:

};



class other_infoModalDialog : public QDialog, public cTranslator, public Ui::EditDialog
{
	Q_OBJECT
  public:
	other_infoModalDialog(QWidget *bw, long fsize, long esize, uint16_t crc, const QString title = translate(STR_MSGDEVINFO));
	virtual ~other_infoModalDialog();               // Destructor
	//      virtual int infoAction(char* msg = "Device Info");

  private slots:
	void onOk();

  protected:

  private:

};


class notesModalDialog : public QDialog, public cTranslator, public Ui::NotesDialog
{
	Q_OBJECT
  public:
	notesModalDialog(QWidget *bw, QString &id, QString &cm, const QString title = translate(STR_MSGDEVNOTE));
	virtual ~notesModalDialog();            // Destructor
	//      QString GetId();
	//      QString GetComment();
	//      virtual int notesAction(char* msg = translate(STR_MSGDEVNOTE));

  private slots:
	void onOk();

  protected:

  private:
	QString *id_ptr;
	QString *cm_ptr;

	QString id_txt;
	QString cmt_txt;

};

#if 0
class editModalDialog : public QDialog, public cTranslator, public Ui::EditDialog
{
	Q_OBJECT
  public:
	editModalDialog(QWidget *bw, int curval = 0, const QString title = translate(STR_MSGEDITBUG));
	virtual ~editModalDialog();             // Destructor
	//      virtual int editAction(char* msg, int &retval);
	int GetVal();

  private slots:
	void onOk();
	void onEdit();

  protected:

  private:
	int oldval;
};


class editModalDialog2 : public QDialog, public cTranslator, public Ui::EditDialog
{
	Q_OBJECT
  public:
	editModalDialog2(QWidget *bw, const QString curval = "", const QString title = translate(STR_MSGEDITBUG));
	virtual ~editModalDialog2();            // Destructor
	//      virtual int editAction(char* msg, char *text, int len);
	QString GetVal();

  private slots:
	void onOk();

  protected:

  private:
	QString val;
};
#endif

#endif

