//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2021   Claudio Lanconelli                           //
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

#include <QtCore>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QFile>
#include <QComboBox>
#include <QFileDialog>

#include <QProgressDialog>

#include <QAbstractTextDocumentLayout>
#include <QIODevice>
#include <QBuffer>
#include <QTextDocument>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopServices>

#include "qhexedit.h"

#include "version.h"

#if USE_QT_VERSION == 6
#include <QSoundEffect>
#else
#include <QSound>
#endif

#include "e2profil.h"
#include "e2dlg.h"
#include "e2cmdw.h"             // our header
#include "infomdlg.h"
#include "aboutmdlg.h"
#include "filldlg.h"
#include "fusemdlg.h"
#include "blockmdlg.h"
#include "progoption.h"
#include "sernumdlg.h"
#include "errcode.h"
#include "prefdialog.h"


#define STATUSBAR_FORM  "    Size     0 Bytes     CRC  0000h      "
#define STATUSBAR_PRINT "    Size %5ld Bytes     CRC  %04Xh     %c"


class e2AppWinInfo;

//Global e2CmdWindow pointer
e2CmdWindow *cmdWin;


void e2CmdWindow::About()
{
	AboutModalDialog about(this, translate(STR_ABOUT) + " " APP_NAME);

	PlaySoundMsg(E2Profile::GetSoundEnabled());

	about.exec();
}

e2CmdWindow::e2CmdWindow(QWidget *parent) :
	QMainWindow(parent),
	e2App(),
	ignoreFlag(false),
	abortFlag(false),
	app_status(AppReady),
	verbose(verboseAll)
{
	cmdWin = this;

	setupUi(this);

	qDebug() << Q_FUNC_INFO << "(" APP_NAME ")";

	cbxEEPType = NULL;
	cbxEEPSubType = NULL;

	lblStringID = NULL;
	lblEEPInfo = NULL;

	e2Prg = NULL;

	currentMenu = NULL;
	currentAct = NULL;

	// EK 2017
	// to check this
	fontSize = E2Profile::GetFontSize();	//sysFont.pointSize();
#if USE_QT_VERSION == 4
	programStyleSheet = QString().sprintf("font-size: %dpt", fontSize);
#else
	programStyleSheet = QString().asprintf("font-size: %dpt", fontSize);
#endif
	if (fontSize == -1)
	{
		fontSize = sysFont.pixelSize();
#if USE_QT_VERSION == 4
		programStyleSheet = QString().sprintf("font-size: %dpx", fontSize);
#else
		programStyleSheet = QString().asprintf("font-size: %dpx", fontSize);
#endif
	}

	if (programStyleSheet.length() > 0)
	{
		setStyleSheet(programStyleSheet);
	}

	if (readLangDir() == false)   // init from langFiles variable in format "filename:language"
	{
		QMessageBox msgBox(QMessageBox::Warning, "Warning", "Directory with other languages not found\nDefault GUI language is english", QMessageBox::Close);
		msgBox.setStyleSheet(programStyleSheet);
		msgBox.setButtonText(QMessageBox::Close, "Close");
		msgBox.exec();
	}

	createFontSizeMenu();

	// reading of arguments
	arguments = QCoreApplication::arguments();

	// The first parameter of the command line is the file to open (optional)
	if (arguments.count() > 1 && arguments.at(1).length() > 0)
	{
		QString param = arguments.at(1);

		//Elimina eventuali virgolette che possono confondere
		//  il programma
		param.remove(QChar('\"'));

		int p = param.lastIndexOf(".");

		if (p > 0 && param.indexOf(".e2s", p) >= 0)
		{
			//Switch to script mode
			scriptMode = true;
			script_name = param;
		}
		else if (param == "-s" || param == "--script")
		{
			if (arguments.count() > 2 && arguments.at(2).length() > 0)
			{
				param = arguments.at(2);

				//Elimina eventuali virgolette che possono confondere
				//  il programma
				param.remove(QChar('\"'));

				scriptMode = true;
				script_name = param;
			}
		}
	}

	// end of init from AppMain
	{
		BusIO **b = GetBusVectorPtr();

		awip = new e2AppWinInfo(this, "", b);
		//              qDebug() << b << a;
	}

	createFileList();
	createScriptList();

	// menu creation for all devices
	createDeviceMenues();

	// status widget
	createStatusWidgets();

	// combo boxes
	CbxMenuInit();

	// The Canvas
	qbuf = new QBuffer(this);
	e2HexEdit = new QHexEdit(this);
	//e2HexEdit->setMinimumSize(100, 100);
	qbufSplit = new QBuffer(this);
	e2HexEditSplit = new QHexEdit(this);
	splitter = new QSplitter(this);
	splitter->setOrientation(Qt::Vertical);
	splitter->setChildrenCollapsible(false);
	splitter->addWidget(e2HexEdit);
	splitter->addWidget(e2HexEditSplit);

	setCentralWidget(splitter);
	e2HexEdit->setFocus();

	// create all signals, from e2HexEdit too
	createSignalSlotConnections();

	if (getLangTable() == false)
	{
		QMessageBox msgBox(QMessageBox::Warning, "Language file error",
						   "Can't open language file!\nDefault GUI language is english", QMessageBox::Close);
		msgBox.setStyleSheet(programStyleSheet);
		msgBox.setButtonText(QMessageBox::Close, "Close");
		msgBox.exec();

		E2Profile::SetCurrentLang("english");
	}

	foreach (QAction *itL, actLangSelect)
	{
		QString lName = (*itL).text();
		lName = lName.toLower();
		if (lName == E2Profile::GetCurrentLang())
		{
			(*itL).setChecked(true);
			break;
		}
	}

	translateGUI();

	//      if (!awip)
	{
#if 0
		BusIO **b = GetBusVectorPtr();

		awip = new e2AppWinInfo(this, "", b);
		//              qDebug() << b << a;
		readXmlDir();
#endif
		PostInit(); // removed from e2AppWinInfo
	}

	// The Status Bar
	//      e2Status = new vStatusPane(StatBar);
	//      AddPane(e2Status);

	//      _timer = new e2Timer(this);             // create timer
	//      _timer->TimerSet(1000);         // 1 second intervals


	// to show th main window
	//      show();

	//      type_index = -1;
	//Initialize Type controls in Tool Bars
	//      UpdateChipType( GetE2PPriType(GetLastDevType()), GetE2PSubType(GetLastDevType()) );
	//         UpdateMenuType( GetE2PPriType(E2Profile::GetLastDevType()), GetE2PSubType(E2Profile::GetLastDevType()) );

	e2HexEdit->setReadOnly(!actionEditBuferEnabled->isChecked());
	e2HexEditSplit->setReadOnly(!actionEditBuferEnabled->isChecked());

	//      e2HexEdit->setHighlightingColor(settings.value("Editor/HighlightingColor").value<QColor>());
	//      e2HexEdit->setSelectionColor(settings.value("Editor/SelectionColor").value<QColor>());
	//      e2HexEdit->setAddressAreaColor(settings.value("Editor/AddressAreaColor").value<QColor>());
	e2HexEdit->setAddressFontColor(Qt::darkRed);
	e2HexEditSplit->setAddressFontColor(Qt::darkRed);
	//      e2HexEdit->setAsciiAreaColor(settings.value("Editor/AsciiAreaColor").value<QColor>());

	e2HexEdit->setAsciiFontColor(Qt::darkMagenta);
	e2HexEditSplit->setAsciiFontColor(Qt::darkMagenta);
	//e2HexEdit->setHexFontColor(Qt::darkGreen);
	e2HexEditSplit->setHexFontColor(Qt::darkBlue);
	e2HexEdit->setHexCaps(true);
	e2HexEditSplit->setHexCaps(true);
	//      e2HexEdit->setFont(settings.value("Editor/Font").value<QFont>());
	//      e2HexEdit->setAddressWidth(settings.value("AddressAreaWidth").toInt());
	//      e2HexEdit->setBytesPerLine(settings.value("BytesPerLine").toInt());

	first_line = 0;
	//curIndex = 0;

	E2Profile::readDialogSettings(this, false);
	E2Profile::restoreSplitter(splitter);

	if (scriptMode)
	{
		returnValue = CmdRunScript();

		exit(returnValue);  //Se AppMain ritorna un valore != 0 esce immediatamente
	}
}

e2CmdWindow::~e2CmdWindow()
{
	qDebug() << Q_FUNC_INFO;

	// Now put a delete for each new in the constructor.

	//delete splitter;
	delete e2HexEdit;
	delete e2HexEditSplit;
	delete qbuf;
	delete qbufSplit;

	// EK 2017
	// TODO remove created QAction* lists and other

	//      delete e2ToolBar;
	//      delete e2ToolBar2;
	//      delete e2Status;
	//      _timer->TimerStop();    // end it
	//      delete _timer;                  // free it
	//      delete e2Dlg;
	if (e2Prg)
	{
		delete e2Prg;
	}
}

/**
 * @brief create the list of loaded data files
 *
 */
void e2CmdWindow::createFileList()
{
	filesMenu = new QMenu("Last files"); // TODO translate this
	actionFileList = menuFile->insertMenu(actionOpen, filesMenu);

	fileListgrp = new QActionGroup(this);
}

/**
 * @brief create the list of used scripts
 *
 */
void e2CmdWindow::createScriptList()
{
	scrptsMenu = new QMenu("Last scripts"); // TODO translate this
	actionScriptList = menuScript->addMenu(scrptsMenu);

	scrListgrp = new QActionGroup(this);
}


/**
 * @brief slot from flAction
 * EK 2017
 * TODO to implement
 */
void e2CmdWindow::onSelectFile(QAction *a)
{
	//Check the Icon to set the load type
	int load_type;

//	qDebug() << "Ico name: " + QString(a->icon().cacheKey()) + " - " + actionOpenFlash->icon().cacheKey() + " - " + actionOpenEep->icon().cacheKey() + " - " + actionOpen->icon().cacheKey();

	if (a->icon().cacheKey() == actionOpenFlash->icon().cacheKey())
	{
		load_type = PROG_TYPE;
	}
	else if (a->icon().cacheKey() == actionOpenEep->icon().cacheKey())
	{
		load_type = DATA_TYPE;
	}
	else
	{
		load_type = ALL_TYPE;
	}

	//We don't use text() here because some systems (KDE) insert '&' automatically to text. See https://bugs.kde.org/show_bug.cgi?id=337491
	//don't use fname.remove(Qchar'&') in case there is a '&' in the filename
	QString fname = a->iconText();
	fname.replace("~", QDir().homePath());

	if (QFile().exists(fname))
	{
		CmdOpen(load_type, fname);
	}
	else
	{
		QMessageBox msgBox(QMessageBox::Critical, "File error", translate(STR_MSGFILENOTFOUND), QMessageBox::Close);
		msgBox.setStyleSheet(programStyleSheet);
		msgBox.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		msgBox.exec();
	}
}


/**
 * @brief scan the directory with translations. language files are with .utf extentions
 *
 */
bool e2CmdWindow::readLangDir()
{
	bool found = false;
	QString lngDirName;
	QStringList dirsLang;
	QDir dir;
	QStringList trList;
	// EK 2017
	// this for linux
	QString path = QDir::currentPath();
	QString bdir = "/build";
	if (path.endsWith(bdir))
	{
		int pos = path.lastIndexOf(bdir);
		path.remove(pos, bdir.length());
	}
	path += "/lang";

	lngDirName = E2Profile::GetLangDir();
	if (lngDirName.length() == 0)
	{
		lngDirName = qApp->applicationDirPath() + "/lang";
	}

	qDebug() <<  Q_FUNC_INFO << "readLangDir path:" << path << ", Saved: " << lngDirName;

#ifdef Q_OS_LINUX
	dirsLang << lngDirName << "/usr/share/ponyprog/lang" << "/usr/local/share/ponyprog/lang" << path;
#else
	dirsLang << lngDirName << path;
#endif

	foreach (const QString entry, dirsLang)
	{
		dir = QDir(entry);

		if (dir.exists() == true)
		{
			trList = dir.entryList(QStringList("*.utf"));
			if (trList.count() > 0)
			{
				E2Profile::SetLangDir(entry);
				lngDirName = entry + "/";
				found = true;
				break;
			}
		}
	}

	if (found == false)
	{
		return false;
	}

	langFiles.clear();

	found = false;

	QMenu *langMenu = new QMenu("Language");
	QAction *actionLanguage = menuSetup->addMenu(langMenu);

	langGroup = new QActionGroup(this);


	foreach (const QString iL, trList)
	{
		QFile fLang(lngDirName + iL);

		if (fLang.exists() == false)
		{
			continue;
		}

		QString iconName;

		if (fLang.open(QIODevice::ReadOnly))        //load
		{
			QTextStream stream(&fLang);
#if USE_QT_VERSION == 6
			stream.setEncoding(QStringConverter::Utf8);
#else
			stream.setCodec("UTF-8");
#endif

			int lines = 0;

			while (!stream.atEnd())
			{
				QString line, nm;
				line = stream.readLine(); // line of text excluding '\n'
				lines++;

				if (line == "LANGUAGE_NAME")
				{
					line = stream.readLine();

					selectedLang = line;
					lines++;

					nm = line;
					line[0] = line[0].toUpper();

					found = true;

					langFiles << QString(iL + ":" + nm);
					QAction *tmpAction = new QAction(line, actionLanguage);
					tmpAction->setCheckable(true);


					langGroup->addAction(tmpAction);
					langMenu->addAction(tmpAction);

					if (E2Profile::GetCurrentLang() == nm)
					{
						tmpAction->setChecked(true);
					}

					actLangSelect.push_back(tmpAction);
					break;
				}

				if (lines > 8)
				{
					break;
				}
			}

			fLang.close();

		}
		else
		{
			continue;
		}
	}

	connect(langGroup, SIGNAL(triggered(QAction *)), this, SLOT(setLang(QAction *)));

	return (found);
}



/**
 * @brief set GUI to selected language and do the translation of all GUI widgets
 *
 */
void e2CmdWindow::setLang(QAction *mnu)
{
	QString lngStr;

	//mnu = langGroup->checkedAction();

	lngStr = mnu->text();
	lngStr = lngStr.remove(QChar('&'));
	lngStr = lngStr.toLower();

	qDebug() << Q_FUNC_INFO << " lang: " << lngStr;

	E2Profile::SetCurrentLang(lngStr);

	if (getLangTable() == false)
	{
		qDebug() << Q_FUNC_INFO << "setLang" << false;
	}

	disconnect(langGroup, SIGNAL(triggered(QAction *)), this, SLOT(setLang(QAction *)));

	mnu->setChecked(true);

	connect(langGroup, SIGNAL(triggered(QAction *)), this, SLOT(setLang(QAction *)));

	translateGUI();
}


/**
 * @brief translate the GUI widgets
 *
 */
void e2CmdWindow::translateGUI()
{
	menuFile->setTitle(translate(STR_MENUFILE));
// 	actionNew->setText(translate(STR_NEWWIN)); // EK 2017 ???
	// TODO translation for last files
	actionOpen->setText(translate(STR_OPEN));
	actionOpenFlash->setText(translate(STR_OPENFLASH));
	actionOpenEep->setText(translate(STR_OPENDATA));
	actionSave->setText(translate(STR_SAVE));
	actionSaveAs->setText(translate(STR_SAVEAS));
	actionSaveFlashAs->setText(translate(STR_SAVEFLASH));
	actionSaveEepAs->setText(translate(STR_SAVEDATA));
	actionReload->setText(translate(STR_RELOAD));
	actionPrint->setText(translate(STR_PRINT));
// 	actionClose->setText(translate(STR_CLOSE));
	actionExit->setText(translate(STR_EXIT));

	menuDevice->setTitle(translate(STR_MENUDEVICE)); // for all device and chip id's

	menuCommand->setTitle(translate(STR_MENUCMD));
	actionReadAll->setText(translate(STR_READALL));
	actionReadFlash->setText(translate(STR_READPROG));
	actionReadEep->setText(translate(STR_READDATA));
	actionWriteAll->setText(translate(STR_WRITEALL));
	actionWriteFlash->setText(translate(STR_WRITEPROG));
	actionWriteEep->setText(translate(STR_WRITEDATA));
	actionVerifyAll->setText(translate(STR_VERIFYALL));
	actionVerifyFlash->setText(translate(STR_VERIFYPROG));
	actionVerifyEep->setText(translate(STR_VERIFYDATA));
	//actionWriteSecurity->setText(translate(STR_WRITESECBITS));
	actionErase->setText(translate(STR_ERASE));
	actionReset->setText(translate(STR_RESET));
	actionProgram->setText(translate(STR_PROGRAM));
	actionProgramOptions->setText(translate(STR_PROGOPTION));
	actionReadOscByte->setText(translate(STR_READOSCCALIB));
	actionOscOptions->setText(translate(STR_OSCCALIBOPTION));

	menuScript->setTitle(translate(STR_MENUSCRIPT));
	// TODO translation for last scripts
	actionLoadScript->setText(translate(STR_LOADSCR));
	actionRunScript->setText(translate(STR_RUNSCR));
	//     STR_EDITSCR
	//      STR_AUTOLOADSCR

	menuUtility->setTitle(translate(STR_MENUUTIL));
	actionClearBuffer->setText(translate(STR_CLEARBUF));
	actionFillBuffer->setText(translate(STR_FILLBUF));
	actionDoubleBank->setText(translate(STR_DOUBLEBANK));
	actionSetSN->setText(translate(STR_DOWNSERNUM));
	actionSNConfig->setText(translate(STR_SERNUMCONF));
	//      STR_BYTESWAP

	menuSetup->setTitle(translate(STR_MENUSETUP));
	// TODO translation for font size
	actionInterfaceSetup->setText(translate(STR_INTERFSETUP));
	actionCalibration->setText(translate(STR_CALIBRATION) + "...");
	actionPreferences->setText(translate(STR_PREFERENCES) + "...");
	//     STR_REMOTEMODE

	menuHelp->setTitle(translate(STR_MENUQMARK));
	actionHelp->setText(translate(STR_MENUHELP));
	actionAbout->setText(translate(STR_MENUABOUT));

	menuEdit->setTitle(translate(STR_MENUEDIT));
	actionEditNote->setText(translate(STR_EDITNOTE));
	actionEditBuferEnabled->setText(translate(STR_EDITBUFENA));
}


/**
 * @brief try to find the translation file
 *
 */
bool e2CmdWindow::getLangTable()
{
	QString lang = E2Profile::GetCurrentLang();
	QString fileLang = "";

	foreach (const QString iLang, langFiles)
	{
		int pos = iLang.lastIndexOf(":" + lang);

		if (pos > 0)
		{
			fileLang = iLang.left(pos);
			break;
		}
	}

	if (fileLang == "")
	{
		return false;
	}

	qDebug() << Q_FUNC_INFO << " " << fileLang;

	if (QFile::exists(E2Profile::GetLangDir() + "/" + fileLang) == false)
	{
		QMessageBox msgBox(QMessageBox::Warning, "Warning", "Language file does not exist!\n\n"
						   + E2Profile::GetLangDir() + "\n\n" + fileLang, QMessageBox::Close);
		msgBox.setStyleSheet(programStyleSheet);
		msgBox.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		msgBox.exec();
		// not found
		return false;
	}

	return loadTranslation(E2Profile::GetLangDir() + "/" + fileLang);
}


int e2CmdWindow::OnError(int err_no, const QString &msgerr)
{
	int rv = 0;
	QString msg;

	// TODO translate buttons
	switch (err_no)
	{
	case 0:
	{
		QMessageBox note(QMessageBox::Critical, "Error", translate(STR_DEVNOTRESP), QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}

	case BADPARAM:
	{
		QMessageBox msgBox(QMessageBox::Question,  "Error", translate(STR_MSGBADPARAM), QMessageBox::Ignore | QMessageBox::Abort | QMessageBox::Retry);
		msgBox.setStyleSheet(programStyleSheet);
		msgBox.setButtonText(QMessageBox::Ignore, translate(STR_IGNORE));
		msgBox.setButtonText(QMessageBox::Abort, translate(STR_ABORT));
		msgBox.setButtonText(QMessageBox::Retry, translate(STR_RETRY));
		rv = msgBox.exec();
		break;
	}

	case DEVICE_BADTYPE:
	{
		QString str = awip ? awip->GetDetectedTypeStr() : "";

		if (str.length() > 0)
		{
			msg = QString("%1 (%2)\nDetected type: %3").arg(translate(STR_DEVBADTYPE)).arg(err_no).arg(str);
		}
		else
		{
			msg = QString("%1 (%2)").arg(translate(STR_DEVBADTYPE)).arg(err_no);
		}

		QMessageBox msgBox(QMessageBox::Question,  "Error", msg, QMessageBox::Ignore | QMessageBox::Abort | QMessageBox::Retry);
		msgBox.setStyleSheet(programStyleSheet);
		msgBox.setButtonText(QMessageBox::Ignore, translate(STR_IGNORE));
		msgBox.setButtonText(QMessageBox::Abort, translate(STR_ABORT));
		msgBox.setButtonText(QMessageBox::Retry, translate(STR_RETRY));
		rv = msgBox.exec();
		break;
	}

	case DEVICE_UNKNOWN:
	{
		QString str = awip ? awip->GetDetectedSignatureStr() : "";

		if (str.length() > 0)
		{
			msg = QString("%1 (%2)\nDetected signature: %3").arg(translate(STR_DEVUNKNOWN)).arg(err_no).arg(str);
		}
		else
		{
			msg = QString("%1 (%2)").arg(translate(STR_DEVUNKNOWN)).arg(err_no);
		}

		QMessageBox msgBox(QMessageBox::Question,  "Error", msg, QMessageBox::Ignore | QMessageBox::Abort | QMessageBox::Retry);
		msgBox.setStyleSheet(programStyleSheet);
		msgBox.setButtonText(QMessageBox::Ignore, translate(STR_IGNORE));
		msgBox.setButtonText(QMessageBox::Abort, translate(STR_ABORT));
		msgBox.setButtonText(QMessageBox::Retry, translate(STR_RETRY));
		rv = msgBox.exec();
		break;
	}

	case DEVICE_LOCKED:
	{
		msg = QString("%1 (%2)").arg(translate(STR_DEVLOCKED)).arg(err_no);

		QMessageBox msgBox(QMessageBox::Question,  "Error", msg, QMessageBox::Ignore | QMessageBox::Abort | QMessageBox::Retry);
		msgBox.setStyleSheet(programStyleSheet);
		msgBox.setButtonText(QMessageBox::Ignore, translate(STR_IGNORE));
		msgBox.setButtonText(QMessageBox::Abort, translate(STR_ABORT));
		msgBox.setButtonText(QMessageBox::Retry, translate(STR_RETRY));
		rv = msgBox.exec();
		break;
	}

	case OP_ABORTED:
	{
		QMessageBox note(QMessageBox::Information,  "Information", translate(STR_OPABORTED), QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}

	case E2ERR_OPENFAILED:
	{
		msg = QString("%1 (%2)").arg(translate(STR_OPENFAILED)).arg(err_no);

		QMessageBox note(QMessageBox::Critical,  "Error", msg, QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}

	case E2ERR_ACCESSDENIED:
	{
		msg = QString("%1 (%2)").arg(translate(STR_ACCDENIED)).arg(err_no);

		QMessageBox note(QMessageBox::Critical,  "Error", msg, QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}

	case E2ERR_NOTINSTALLED:
	{
		msg = QString("%1 (%2)").arg(translate(STR_NOTINST)).arg(err_no);

		QMessageBox note(QMessageBox::Critical,  "Error", msg, QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}

	case IICERR_SDACONFLICT:
	case IICERR_SCLCONFLICT:
	{
		msg = QString("%1 (%2)").arg(translate(STR_HWERROR)).arg(err_no);

		QMessageBox note(QMessageBox::Critical,  "Error", msg, QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}

	case IICERR_BUSBUSY:
	{
		msg = QString("%1 (%2)").arg(translate(STR_BUSBUSY)).arg(err_no);

		QMessageBox note(QMessageBox::Critical,  "Error", msg, QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}

	case IICERR_NOTACK:
	{
		msg = QString("%1 (%2)").arg(translate(STR_I2CNOACK)).arg(err_no);
		QMessageBox note(QMessageBox::Critical,  "Error", msg, QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}

	case IICERR_NOADDRACK:
	{
		msg = QString("%1 (%2)").arg(translate(STR_I2CNODEV)).arg(err_no);

		QMessageBox msgBox(QMessageBox::Question,  "Error", msg, QMessageBox::Ignore | QMessageBox::Abort | QMessageBox::Retry);
		msgBox.setStyleSheet(programStyleSheet);
		msgBox.setButtonText(QMessageBox::Ignore, translate(STR_IGNORE));
		msgBox.setButtonText(QMessageBox::Abort, translate(STR_ABORT));
		msgBox.setButtonText(QMessageBox::Retry, translate(STR_RETRY));
		rv = msgBox.exec();
		break;
	}

	case IICERR_TIMEOUT:
	{
		msg = QString("%1 (%2)").arg(translate(STR_I2CTIMEOUT)).arg(err_no);
		QMessageBox note(QMessageBox::Critical,  "Error", msg, QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}

	case IICERR_STOP:
	{
		msg = QString("%1 (%2)").arg(translate(STR_I2CSTOPERR)).arg(err_no);
		QMessageBox note(QMessageBox::Critical,  "Error", msg, QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}

	case E2ERR_WRITEFAILED:
	{
		msg = QString("%1 (%2)").arg(translate(STR_WRITEERR)).arg(err_no);
		QMessageBox note(QMessageBox::Critical,  "Error", msg, QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}

	case E2ERR_BLANKCHECKFAILED:
	{
		msg = QString("%1 (%2)").arg(translate(STR_BLANKCHECKERR)).arg(err_no);
		QMessageBox note(QMessageBox::Critical,  "Error", msg, QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}

	case NOTSUPPORTED:
	{
		msg = QString("%1 (%2)").arg(translate(STR_OPNOTSUP)).arg(err_no);
		QMessageBox note(QMessageBox::Critical,  "Error", msg, QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}

	default:
	{
		if (msgerr.length())
		{
			msg = msgerr;
		}
		else
		{
			msg = QString("%1 (%2)").arg(translate(STR_ERRNO)).arg(err_no);
		}
		QMessageBox note(QMessageBox::Critical,  "Error", msg, QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
		break;
	}
	}

	return rv;
}


/**
 * @brief
 *
 */
void e2CmdWindow::createDeviceMenues()
{
	// the loop about all structures
	deviceMenu = awip->CreateDeviceMenues();

	qDebug() << Q_FUNC_INFO << deviceMenu.count() << "entries";

	for (int i = 0; i < deviceMenu.count(); i++)
	{
		addMenuVector(&deviceMenu[i]);
	}
}


void e2CmdWindow::addMenuVector(menuToGroup *vecMnu, const QString &filter)
{
	vecMnu->mnu = new QMenu(vecMnu->title);
	vecMnu->grp = new QActionGroup(this);

	QAction *actionRecent = menuDevice->addMenu(vecMnu->mnu);

	// EK TODO to implement filter?
	for (int i = 0; i < vecMnu->vChip.count(); i++)
	{
		QString entry = vecMnu->vChip[i].name;
		QAction *tmpAction = new QAction(entry, actionRecent);
		tmpAction->setCheckable(true);

		vecMnu->mnu->addAction(tmpAction);
		vecMnu->grp->addAction(tmpAction);
	}

	connect(vecMnu->grp, SIGNAL(triggered(QAction *)), this, SLOT(onSelectChip(QAction *)));

// 	deviceMenu << *vecMnu;
}


/**
 * @brief creating of QProgressDialog and start
 *
 */
void e2CmdWindow::doProgress(const QString &text)
{
	if (e2Prg)
	{
		qDebug() << Q_FUNC_INFO << "e2Prg alread exist";
		e2Prg->setLabelText(text);
		e2Prg->reset();
		//e2Prg->setValue(0);
	}
	else
	{
		e2Prg = new QProgressDialog(text, translate(STR_ABORT), 0, 100, this);
		e2Prg->setWindowModality(Qt::WindowModal);
		e2Prg->setMinimumDuration(100);
		e2Prg->setAutoReset(true);
		e2Prg->setAutoClose(true);
		//connect(e2Prg, SIGNAL(canceled()), this,  SLOT(onEndProgress()));
	}
}

/**
 * @brief SLOT from "cancel" of ProgressDialog
 *
 */
//void e2CmdWindow::onEndProgress()
//{
//	if (e2Prg)
//	{
//		e2Prg->close();
//		delete e2Prg;
//		e2Prg = NULL;
//	}

//	qDebug() << "progress dialog finished";
//}


/**
 * @brief slot for signal from action group
 *
 */
void e2CmdWindow::onSelectChip(QAction *a)
{
	if (!IsAppReady())
	{
		return;
	}

	QString t = ((QAction *)a->parent())->text(); // current type
	QString st = a->text(); // current subtype

	long new_id = selectTypeSubtype(t, st);

	if (currentAct != NULL)
	{
		currentAct->setChecked(false);
	}

	currentAct = a;
	currentAct->setChecked(true);

	qDebug() << Q_FUNC_INFO << "Id: " << (Qt::hex) << awip->GetId() << " NewId: " << new_id;

	if (awip->GetId() != new_id)
	{
		CmdSelectDevice(new_id);
	}
}


quint32 e2CmdWindow::selectTypeSubtype(const QString &tp, const QString &subtp)
{
	QString t_tmp = tp;
	t_tmp.remove(QChar('&'));

	QString st_tmp = subtp;
	st_tmp.remove(QChar('&'));

	qDebug() << Q_FUNC_INFO << " " << t_tmp << st_tmp;
	//bool rebuildSubmenu = true;

	if (currentMenu == NULL || currentMenu->title != t_tmp || cbxEEPSubType->count() == 0) // update the type combobox
	{
		int nt = cbxEEPType->findText(t_tmp);

		if (nt == -1) // not found, it's criminal
		{
			nt = 0;
		}

		disconnect(cbxEEPType, SIGNAL(currentIndexChanged(int)), this, SLOT(onDevType(int)));
		cbxEEPType->setCurrentIndex(nt);
		connect(cbxEEPType, SIGNAL(currentIndexChanged(int)), this, SLOT(onDevType(int)));

		for (int i = 0; i < deviceMenu.count(); i++)
		{
			if (deviceMenu.at(i).title == t_tmp)
			{
				currentMenu = (menuToGroup *)&deviceMenu.at(i);

				break;
			}
		}

		disconnect(cbxEEPSubType, SIGNAL(currentIndexChanged(int)), this, SLOT(onDevSubType(int)));

		cbxEEPSubType->clear();

		QStringList l;

		foreach (chipMenuInfo cInf, currentMenu->vChip)
		{
			l << cInf.name;
		}

		if (l.count()) // refresh combobox list for subtype
		{
			cbxEEPSubType->addItems(l);
		}
		else
		{
			qDebug() << Q_FUNC_INFO << "selectTypeSubtype, something is wrong with search" << currentMenu->title;
			return EID_INVALID;
		}

		connect(cbxEEPSubType, SIGNAL(currentIndexChanged(int)), this, SLOT(onDevSubType(int)));
	}

	int nst = cbxEEPSubType->findText(st_tmp);

	if (nst == -1)
	{
		nst = 0;
	}

	disconnect(cbxEEPSubType, SIGNAL(currentIndexChanged(int)), this, SLOT(onDevSubType(int)));
	cbxEEPSubType->setCurrentIndex(nst);
	connect(cbxEEPSubType, SIGNAL(currentIndexChanged(int)), this, SLOT(onDevSubType(int)));

	if (currentMenu == NULL)
	{
		qDebug() << Q_FUNC_INFO << "selectTypeSubtype, something is wrong with data pointer";
		return EID_INVALID;
	}

	// search id
	long new_id = EID_INVALID;

	for (int i = 0; i < currentMenu->vChip.count(); i++)
	{
		if (currentMenu->vChip.at(i).name == st_tmp)
		{
			new_id = currentMenu->vChip.at(i).id;
		}
	}

	Q_ASSERT(new_id != EID_INVALID);

	return new_id;
}


/**
 * @brief slot from selecto font size menu
 *
 */
void e2CmdWindow::selectFontSize(QAction *mnu)
{
	QString lngStr;

	lngStr = mnu->text();
	lngStr.remove(QChar('&'));

	fontSize = lngStr.toInt();

	mnu->setChecked(true);

	int sz = sysFont.pointSize();

	// for lang menu and for fontsize menu
	if (sz == -1)
	{
#if USE_QT_VERSION == 4
		programStyleSheet = QString().sprintf("font-size: %dpx", fontSize);
#else
		programStyleSheet = QString().asprintf("font-size: %dpx", fontSize);
#endif
	}
	else
	{
#if USE_QT_VERSION == 4
		programStyleSheet = QString().sprintf("font-size: %dpt", fontSize);
#else
		programStyleSheet = QString().asprintf("font-size: %dpt", fontSize);
#endif
	}

	E2Profile::SetFontSize(fontSize);

	QString sSheet2 = QString("QMenu { %1; }").arg(programStyleSheet);

	setStyleSheet(programStyleSheet);

	//     langMenu->setStyleSheet(sSheet2);

	setFontForWidgets();
}


void e2CmdWindow::setFontForWidgets()
{
	//TODO: use font selection dialog to select the font
#ifdef Q_OS_WIN32
	e2HexEdit->setFont(QFont("Courier", E2Profile::GetFontSize()));
	e2HexEditSplit->setFont(QFont("Courier", E2Profile::GetFontSize()));
#else
	e2HexEdit->setFont(QFont("Monospace", E2Profile::GetFontSize()));
	e2HexEditSplit->setFont(QFont("Monospace", E2Profile::GetFontSize()));
#endif

	//     buttonsWidget->setStyleSheet(programStyleSheet);
	//
	//     for (int i = 0; i < 6; i++) {
	//         widgets[i]->setStyleSheet(programStyleSheet);
	//     }
}

/**
 * @brief create menu with action group for font size selection
 *
 */
void e2CmdWindow::createFontSizeMenu()
{
	QMenu *m = new QMenu("Font size");
	/*QAction *fontSizeMenu = */ menuSetup->addMenu(m);
	//     QMenu *fontSizeMenu = new QMenu();
	fsizeGroup = new QActionGroup(this);
	QStringList szList;
	szList << "9" << "10" << "12" << "14" << "16" << "18" << "20";

	foreach (QString entry, szList)
	{
		QAction *tmpAction = new QAction(entry, m);
		tmpAction->setCheckable(true);

		m->addAction(tmpAction);
		fsizeGroup->addAction(tmpAction);

		if (fontSize == entry.toInt())
		{
			tmpAction->setChecked(true);
		}

		actFSizeSelect.push_back(tmpAction);
	}

	connect(fsizeGroup, SIGNAL(triggered(QAction *)), this, SLOT(selectFontSize(QAction *)));
}

void e2CmdWindow::onDtaChanged()
{
	if (awip)
	{
		awip->BufChanged(e2HexEdit->isModified() || e2HexEditSplit->isModified());
		UpdateStatusBar();
	}
}

/**
 * @brief create all SIGNAL -> SLOT connections
 * EK 2017
 * @TODO to check all connections from "action" widgets
 */
void e2CmdWindow::createSignalSlotConnections()
{
	// actions from Ui class

	// hex editor connections
	//      connect(e2HexEdit, SIGNAL(overwriteModeChanged(bool)), this, SLOT(onOverwriteMode(bool)));
	connect(e2HexEdit, SIGNAL(dataChanged()), this, SLOT(onDtaChanged()));
	connect(e2HexEditSplit, SIGNAL(dataChanged()), this, SLOT(onDtaChanged()));

	// EK 2017
	// TODO
	// not implemented are: cut/copy/paste buttons

	// font size
	//connect(fsizeGroup, SIGNAL(triggered()), this, SLOT(selectFontSize()));

	// new
// 	connect(actionNew, SIGNAL(triggered()), this, SLOT(onNew()));

	// open all
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));

	// open flash
	connect(actionOpenFlash, SIGNAL(triggered()), this, SLOT(onOpen()));

	// open eeprom
	connect(actionOpenEep, SIGNAL(triggered()), this, SLOT(onOpen()));

	// reload
	connect(actionReload, SIGNAL(triggered()), this, SLOT(onReload()));

	// save all
	connect(actionSave, SIGNAL(triggered()), this, SLOT(onSave()));
#if 0
	// save all
	connect(actionSaveFlash, SIGNAL(triggered()), this, SLOT(onSave(PROG_TYPE)));

	// save all
	connect(actionSaveEep, SIGNAL(triggered()), this, SLOT(onSave(DATA_TYPE)));
#endif
	// save all
	connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(onSaveAs()));

	// save flash
	connect(actionSaveFlashAs, SIGNAL(triggered()), this, SLOT(onSaveAs()));

	// save eeprom
	connect(actionSaveEepAs, SIGNAL(triggered()), this, SLOT(onSaveAs()));

	// print
	connect(actionPrint, SIGNAL(triggered()), this, SLOT(onPrint()));

	// close
// 	connect(actionClose, SIGNAL(triggered()), this, SLOT(onClose()));

	// exit
	connect(actionExit, SIGNAL(triggered()), this, SLOT(onExit()));

	// about
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));

	// about qt
	connect(actionAboutQt, SIGNAL(triggered()), this, SLOT(onAboutQt()));

	// help
	connect(actionHelp, SIGNAL(triggered()), this, SLOT(onHelp()));

	// write all
	connect(actionWriteAll, SIGNAL(triggered()), this, SLOT(onWrite()));

	// write flash
	connect(actionWriteFlash, SIGNAL(triggered()), this, SLOT(onWrite()));

	// write data
	connect(actionWriteEep, SIGNAL(triggered()), this, SLOT(onWrite()));

	// read all
	connect(actionReadAll, SIGNAL(triggered()), this, SLOT(onRead()));

	// read flash
	connect(actionReadFlash, SIGNAL(triggered()), this, SLOT(onRead()));

	// read data
	connect(actionReadEep, SIGNAL(triggered()), this, SLOT(onRead()));

	// verify all
	connect(actionVerifyAll, SIGNAL(triggered()), this, SLOT(onVerify()));

	// erase all
	// not implemented: erase flash, erase eeprom
	connect(actionErase, SIGNAL(triggered()), this, SLOT(onErase()));

	// verify flash
	connect(actionVerifyFlash, SIGNAL(triggered()), this, SLOT(onVerify()));

	// verify data
	connect(actionVerifyEep, SIGNAL(triggered()), this, SLOT(onVerify()));

	// prog options
	connect(actionProgramOptions, SIGNAL(triggered()), this, SLOT(onProgramOptions()));

	// load script
	connect(actionLoadScript, SIGNAL(triggered()), this, SLOT(onLoadScript()));
	connect(actionScript, SIGNAL(triggered()), this, SLOT(onLoadScript()));

	// run script
	//connect(actionRunScript, SIGNAL(triggered()), this, SLOT(onRunScript()));

	// reset
	connect(actionReset, SIGNAL(triggered()), this, SLOT(onReset()));

#ifdef  vDEBUG
	// load script
	connect(actionDebugger, SIGNAL(triggered()), this, SLOT(onDebugger()));
#endif

	// edit buffer
	connect(actionEditBuferEnabled, SIGNAL(triggered()), this, SLOT(onEditBufToggle()));

	// calibration
	connect(actionCalibration, SIGNAL(triggered()), this, SLOT(onCalibration()));

	// program
	connect(actionProgram, SIGNAL(triggered()), this, SLOT(onProgram()));

	// byte swap
	connect(actionByteSwap, SIGNAL(triggered()), this, SLOT(onByteSwap()));

	// double buffer
	connect(actionDoubleBank, SIGNAL(triggered()), this, SLOT(onDoubleSize()));

	// set serial number
	connect(actionSetSN, SIGNAL(triggered()), this, SLOT(onSetSerialNumber()));
	connect(actionSerial_Number, SIGNAL(triggered()), this, SLOT(onSetSerialNumber()));

	//      connect(actionEraseFlash, SIGNAL(triggered()), this, SLOT( onClearBuf(PROG_TYPE)));
	//      connect(actionEraseEep, SIGNAL(triggered()), this, SLOT( onClearBuf(DATA_TYPE)));

	// edit notes
	connect(actionEditNote, SIGNAL(triggered()), this, SLOT(onEditNote()));

	// write security bits
	//connect(actionWriteSecurity, SIGNAL(triggered()), this, SLOT(onWriteSecurity()));

	// clear buffer
	connect(actionClearBuffer, SIGNAL(triggered()), this, SLOT(onClearBuf()));

	// fill buffer
	connect(actionFillBuffer, SIGNAL(triggered()), this, SLOT(onFillBuf()));

	connect(actionLock, SIGNAL(triggered()), this, SLOT(onSpecialBits()));
	connect(actionSpecialBits, SIGNAL(triggered()), this, SLOT(onSpecialBits()));

	connect(actionOscOptions, SIGNAL(triggered()), this, SLOT(onOscCalibOption()));

	// get device info
	//      connect(actionGetInfo, SIGNAL(triggered()), this, SLOT(onGetInfo()));


	// ???
	//      connect(actionWriteHEndu, SIGNAL(triggered()), this, SLOT( onWriteHEndurance()));


	connect(actionSNConfig, SIGNAL(triggered()), this, SLOT(onSerialNumberOptions()));

#if 0
	connect(actiontEnableToolBar, SIGNAL(triggered()), this, SLOT(onEnableToolBar(int val)));
	//      connect(actiontLastFile1, SIGNAL(triggered()), this, SLOT( onLastFile1()));
	//      connect(actiontLastFile2, SIGNAL(triggered()), this, SLOT( onLastFile2()));

	connect(actiontAskToSave, SIGNAL(triggered()), this, SLOT(onAskToSave()));
	connect(actiontProgress, SIGNAL(triggered()), this, SLOT(onProgress(int val)));
	connect(actiontCloseAll, SIGNAL(triggered()), this, SLOT(onCloseAllDialog()));
	connect(actiontSelectDevice, SIGNAL(triggered()), this, SLOT(onSelectDevice(long new_type)));
	connect(actiontEEPType, SIGNAL(triggered()), this, SLOT(onSelectEEPType(int val)));
	connect(actiontEEPSubtype, SIGNAL(triggered()), this, SLOT(onEEPSubType(int val)));
#endif
	connect(actionReadOscByte, SIGNAL(triggered()), this, SLOT(onReadCalibration()));
	// interface setup
	connect(actionInterfaceSetup, SIGNAL(triggered()), this, SLOT(onInterfSetup()));
	connect(actionSetup, SIGNAL(triggered()), this, SLOT(onInterfSetup()));
	//      connect(actiontReadSecurity, SIGNAL(triggered()), this, SLOT( onReadSecurity(bool display_dialog)));

	//     connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
}


void e2CmdWindow::onInterfSetup()
{
	if (IsAppReady())
	{
		e2Dialog e2dlg(this, translate(STR_DLGIOSETUP));

		if (e2dlg.exec() == QDialog::Accepted)
		{
			// EK 2017
			// TODO set the global settings?
		}
	}
}

#if 0
void e2CmdWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls())
	{
		event->accept();
	}
}


void e2CmdWindow::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasUrls())
	{
		QList<QUrl> urls = event->mimeData()->urls();
		QString filePath = urls.at(0).toLocalFile();
		loadFile(filePath);
		event->accept();
	}
}
#endif


void e2CmdWindow::onGetInfo()
{
	if (IsAppReady())
	{
		SetAppBusy();
		CmdGetInfo();
		SetAppReady();
	}
}


void e2CmdWindow::onExit()
{
	Exit();
	qApp->quit();
}

#include <QCloseEvent>
void e2CmdWindow::closeEvent(QCloseEvent *event)
{
	Exit();
	//event->ignore();
	event->accept();
}

#if 0
void e2CmdWindow::onClose()
{
	//      CloseAppWin();
	Exit();
}
#endif

void e2CmdWindow::onOpen()
{
	if (IsAppReady())
	{
		QAction *a = static_cast<QAction *>(sender());

		if (a == actionOpen)
		{
			CmdOpen(ALL_TYPE);
		}
		else if (a == actionOpenFlash)
		{
			CmdOpen(PROG_TYPE);
		}
		else if (a == actionOpenEep)
		{
			CmdOpen(DATA_TYPE);
		}
	}
}


void e2CmdWindow::onSave()
{
	if (IsAppReady())
	{
		QAction *a = static_cast<QAction *>(sender());

		if (a == actionSave)
		{
			CmdSave(ALL_TYPE);
		}
	}
}


void e2CmdWindow::onSaveAs()
{
	if (IsAppReady())
	{
		QAction *a = static_cast<QAction *>(sender());

		if (a == actionSaveAs)
		{
			CmdSaveAs(ALL_TYPE);
		}
		else if (a == actionSaveFlashAs)
		{
			CmdSaveAs(PROG_TYPE);
		}
		else if (a == actionSaveEepAs)
		{
			CmdSaveAs(DATA_TYPE);
		}
	}
}


void e2CmdWindow::onReload()
{
	if (IsAppReady())
	{
		CmdReload();
	}
}


void e2CmdWindow::onPrint()
{
	if (IsAppReady())
	{
		CmdPrint();
	}
}


void e2CmdWindow::onWrite()
{
	if (IsAppReady())
	{
		bool verify = E2Profile::GetVerifyAfterWrite();
		QAction *a = static_cast<QAction *>(sender());

		SetAppBusy();

		if (a == actionWriteAll)
		{
			CmdWrite(ALL_TYPE, verify);
		}
		else if (a == actionWriteFlash)
		{
			CmdWrite(PROG_TYPE, verify);
		}
		else if (a == actionWriteEep)
		{
			CmdWrite(DATA_TYPE, verify);
		}

		SetAppReady();
	}
}


void e2CmdWindow::onRead()
{
	QAction *a = static_cast<QAction *>(sender());

	SetAppBusy();

	if (a == actionReadAll)
	{
		CmdRead(ALL_TYPE);
	}
	else if (a == actionReadFlash)
	{
		CmdRead(PROG_TYPE);
	}
	else if (a == actionReadEep)
	{
		CmdRead(DATA_TYPE);
	}

	SetAppReady();
}


// EK 2017
// only for "all"?
void e2CmdWindow::onErase()
{
	if (IsAppReady())
	{
		SetAppBusy();
		CmdErase(ALL_TYPE);
		SetAppReady();
	}
}


void e2CmdWindow::onVerify()
{
	if (IsAppReady())
	{
		QAction *a = static_cast<QAction *>(sender());

		SetAppBusy();

		if (a == actionVerifyAll)
		{
			CmdVerify(ALL_TYPE);
		}
		else if (a == actionVerifyFlash)
		{
			CmdVerify(PROG_TYPE);
		}
		else if (a == actionVerifyEep)
		{
			CmdVerify(DATA_TYPE);
		}

		SetAppReady();
	}
	else
	{
		qDebug() << Q_FUNC_INFO << "onVerify() App not ready!!!";
	}
}

/**
 * @brief slot from scrAction
 *
 */
void e2CmdWindow::onSelectScript(QAction *a)
{
	if (IsAppReady())
	{
		QString fname = a->iconText();
		fname.replace("~", QDir().homePath());

		if (QFile().exists(fname))
		{
			script_name = fname;

			SetAppBusy();
			CmdRunScript();
			SetAppReady();
		}
	}
}


void e2CmdWindow::onLoadScript()
{
	if (IsAppReady())
	{
		SetAppBusy();
		int rv = OpenScript("");

		if (rv == OK)
		{
			rv = CmdRunScript();
			if (rv != BADPARAM && rv != FILENOTFOUND)	//rv != CMD_SCRIPTERROR
			{
				E2Profile::SetLastScript(script_name);
				UpdateScriptMenu();
			}
		}

		SetAppReady();
	}
}


void e2CmdWindow::onReset()
{
	if (IsAppReady())
	{
		CmdReset();
	}
}


void e2CmdWindow::onHelp()
{
	if (IsAppReady())
	{
		CmdHelp();
	}
}


void e2CmdWindow::onReadCalibration()
{
	if (IsAppReady())
	{
		CmdReadCalibration(0);
	}
}


void e2CmdWindow::onAbout()
{
	if (IsAppReady())
	{
		About();
	}
}


/**
 * @brief slot for popup window
 *
 */
void e2CmdWindow::onAboutQt()
{
	QMessageBox::aboutQt(this, tr("About Qt"));
}


void e2CmdWindow::onReadSecurity(bool display_dialog)
{
	if (IsAppReady())
	{
		CmdReadSecurity(display_dialog);
	}
}

void e2CmdWindow::onCalibration()
{
	if (IsAppReady())
	{
		SetAppBusy();
		CmdCalibration();
		SetAppReady();
	}
}

//void e2CmdWindow::onWriteSecurity()
//{
//	if (IsAppReady())
//	{
//		CmdWriteSecurity();
//	}
//}

void e2CmdWindow::onWriteHEndurance()
{
	if (IsAppReady())
	{
		CmdWriteSpecial();
	}
}

void e2CmdWindow::onEditBufToggle()
{
	bool b = actionEditBuferEnabled->isChecked();

	e2HexEdit->setReadOnly(!b);
	e2HexEditSplit->setReadOnly(!b);
	E2Profile::SetEditBufferEnabled(b);

	// EK 2017
	// TODO
	Draw();
}

void e2CmdWindow::onEditNote()
{
	if (IsAppReady())
	{
		CmdEditNote();
	}
}


void e2CmdWindow::onProgramOptions()
{
	if (IsAppReady())
	{
		ProgramOptions();
	}
}


void e2CmdWindow::onSerialNumberOptions()
{
	if (IsAppReady())
	{
		SerialNumberOptions();
	}
}


void e2CmdWindow::onSetSerialNumber()
{
	if (IsAppReady())
	{
		CmdSetSerialNumber();
	}
}


void e2CmdWindow::onAskToSave()
{
	if (IsBufChanged())
	{
		QString str;

		str = translate(STR_BUFCHANGED);
		str.replace("%s", GetFileName());

		QMessageBox msgBox(QMessageBox::Warning, "PonyProg", str, QMessageBox::Yes | QMessageBox::No);
		msgBox.setStyleSheet(programStyleSheet);
		msgBox.setButtonText(QMessageBox::Yes, translate(STR_YES));
		msgBox.setButtonText(QMessageBox::No, translate(STR_NO));
		int ret = msgBox.exec();

		if (ret == QMessageBox::Yes)
		{
			SaveFile();
		}
	}
}


// void e2CmdWindow::onProgress(int val)
// {
//      e2Prg->setValue(val);
// }


//void e2CmdWindow::onCloseAllDialog()
//{
//	e2Prg->close();
//	e2Dlg->CloseDialog();
//}


void e2CmdWindow::onOscCalibOption()
{
	if (IsAppReady())
	{
		OscCalibOption();
	}
}


void e2CmdWindow::onProgram()
{
	if (IsAppReady())
	{
		SetAppBusy();
		CmdProgram();
		SetAppReady();
	}
}


void e2CmdWindow::onSpecialBits()
{
	if (IsAppReady())
	{
		SpecialBits();
	}
}


void e2CmdWindow::onDoubleSize()
{
	if (IsAppReady())
	{
		//CmdDoubleSize(val, cType);
		CmdDoubleSize();
	}
}


void e2CmdWindow::onClearBuf()
{
	if (IsAppReady())
	{
		CmdClearBuf();
	}
}

void e2CmdWindow::onFillBuf()
{
	if (IsAppReady())
	{
		CmdFillBuf();
	}
}


void e2CmdWindow::onByteSwap()
{
	if (IsAppReady())
	{
		CmdByteSwap();
	}
}


static bool CmpExtension(const QString &name, const QString &ext);

int e2CmdWindow::CmdSave(int type, const QString &fname, long relocation)
{
	int result = OK;
	QString oldfname = "";

	if (awip->IsBufferValid())
	{
		UpdateBuffer();
		oldfname = awip->GetFileName();
		awip->SetSaveRelocation(relocation);
		awip->SetSaveType(type);

		if (fname.length())
		{
			awip->SetFileName(fname);
		}

		result = SaveFile(0);
	}
	else
	{
		result = CMD_NOTHINGTOSAVE;

		if (verbose != verboseNo)
		{
			QMessageBox note(QMessageBox::Warning, "Warning", translate(STR_NOTHINGSAVE), QMessageBox::Close);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
			note.exec();
		}
	}

	if (result == OK && oldfname != awip->GetFileName())
	{
		E2Profile::SetLastFile(awip->GetFileName(), awip->GetSaveType());
		UpdateFileMenu();
	}

	return result;
}

int e2CmdWindow::CmdSaveAs(int type, long relocation)
{
	int result = OK;
	QString oldfname = "";

	if (awip->IsBufferValid())
	{
		UpdateBuffer();
		oldfname = awip->GetFileName();
		awip->SetSaveRelocation(relocation);
		awip->SetSaveType(type);

		result = SaveFile(1);
	}
	else
	{
		result = CMD_NOTHINGTOSAVE;

		if (verbose != verboseNo)
		{
			QMessageBox note(QMessageBox::Warning, "Warning", translate(STR_NOTHINGSAVE), QMessageBox::Close);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
			note.exec();
		}
	}

	if (result == OK && oldfname != awip->GetFileName())
	{
		E2Profile::SetLastFile(awip->GetFileName(), awip->GetSaveType());
		UpdateFileMenu();
	}

	return result;
}

int e2CmdWindow::CmdLastFile(int index)
{
	int result = OK;

	if (verbose == verboseAll)
	{
		if (IsBufChanged() && awip->IsBufferValid())
		{
			QMessageBox msgBox(QMessageBox::Warning, QString(APP_NAME), "Buffer changed. Save it before to close?",		//TODO: translate message
							   QMessageBox::Yes | QMessageBox::No);
			msgBox.setStyleSheet(programStyleSheet);
			msgBox.setButtonText(QMessageBox::Yes, translate(STR_YES));
			msgBox.setButtonText(QMessageBox::No, translate(STR_NO));
			int ret = msgBox.exec();

			if (ret == QMessageBox::Yes)
			{
				awip->SetSaveType(ALL_TYPE);    //??
				SaveFile();
			}
		}
	}

	int data_type;
	QString s = E2Profile::GetLastFile(data_type, index);

	if (s.length())
	{
		awip->SetLoadType(data_type);
		awip->SetLoadAutoClearBuf(E2Profile::GetClearBufBeforeLoad());

		result = OpenFile(s);
	}

	return result;
}


int e2CmdWindow::CmdReload()
{
	int result = OK;

	if (verbose == verboseAll)
	{
		if (IsBufChanged() && awip->IsBufferValid())
		{
			QMessageBox msgBox(QMessageBox::Warning, QString(APP_NAME), "Buffer changed. Save it before to close?",		//TODO: translate message
							   QMessageBox::Yes | QMessageBox::No);
			msgBox.setStyleSheet(programStyleSheet);
			msgBox.setButtonText(QMessageBox::Yes, translate(STR_YES));
			msgBox.setButtonText(QMessageBox::No, translate(STR_NO));
			int ret = msgBox.exec();

			if (ret == QMessageBox::Yes)
			{
				awip->SetSaveType(ALL_TYPE);    //??
				SaveFile();
			}
		}
	}

	/**
		//solo nel caso in cui il penultimo file era un .hex e l'ultimo
		//  un .eep (caso dell'AVR con caricamento Flash+eeprom), ricarica
		//  entrambi i file in sequenza, in caso contrario ricarica solo
		//  l'ultimo.
		int data_type;
		if ( GetPrevFile(data_type) && FileExist(GetPrevFile(data_type))
				 && CmpExtension(GetLastFile(data_type), ".eep") == 0
				 && CmpExtension(GetPrevFile(data_type), ".hex") == 0 )
	**/
	int dt1, dt2;

	QString sp1 = E2Profile::GetLastFile(dt1, 0);

	if (sp1.length() && QFile::exists(sp1))
	{
		QString sp2 = E2Profile::GetLastFile(dt2, 1);

		// We reload both files only if PrevFile is not E2P and last file is
		// not ALL_TYPE
		// because we don't want PrevFile change the selected device. In
		// case of LastFile is ALL_TYPE there's no need to reload even
		// PrevFile
		if (sp2.length() && QFile::exists(sp2) && dt1 != ALL_TYPE &&
				CmpExtension(sp2.toLower(), ".e2p") != 0)
		{
			awip->SetLoadType(dt2);
			awip->SetLoadAutoClearBuf(E2Profile::GetClearBufBeforeLoad());

			result = OpenFile(sp2);

			if (result == OK)
			{
				awip->SetLoadType(dt1);
				result = OpenFile(sp1);
			}
		}
		else
		{
			awip->SetLoadType(dt1);
			awip->SetLoadAutoClearBuf(E2Profile::GetClearBufBeforeLoad());

			result = OpenFile(sp1);
		}
	}
	else
	{
		result = CMD_NOTHINGTOLOAD;

		if (verbose != verboseNo)
		{
			QMessageBox note(QMessageBox::Warning, "Warning", translate(STR_NOTHINGLOAD), QMessageBox::Close);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
			note.exec();
		}
	}

	return result;
}

int e2CmdWindow::CmdPrint()
{
	if (awip->IsBufferValid())
	{
		Print();
	}
	else
	{
		QMessageBox note(QMessageBox::Warning, "Warning", translate(STR_NOTHINGPRINT), QMessageBox::Close);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
		note.exec();
	}

	return OK;
}

int e2CmdWindow::PlaySoundMsg(bool val)
{
	if (val)
	{
#if USE_QT_VERSION == 6
		QSoundEffect snd;
		snd.setSource(GetOkSound());
		snd.play();
#else
		QSound::play(GetOkSound());
#endif
	}

	return OK;
}

int e2CmdWindow::CmdHelp()
{
	QString str;

	QFile file(GetHelpFile());

	if (file.exists())
	{
		str = GetHelpFile();
		QDesktopServices::openUrl(QUrl::fromLocalFile(str));
	}
	else
	{
// 		str = "http://www.lancos.com/e2p/ponyprog2000.html";
		QDesktopServices::openUrl(QUrl("http://www.lancos.com/e2p/ponyprog2000.html"));
	}
#if 0
#ifdef Q_OS_LINUX
	//system(str.toLatin1().constData());
	QProcess::execute("xdg-open", (QStringList() << str));
#else
	ShellExecute(NULL, L"open", (LPCWSTR)str.utf16(), NULL, NULL, SW_SHOWNORMAL);
#endif
#endif
	return OK;
}

int e2CmdWindow::CmdCalibration()
{
	QMessageBox msgBox(QMessageBox::Warning, QString(APP_NAME), translate(STR_BUSCALIBRA1) + QString(APP_NAME) + translate(STR_BUSCALIBRA2),
					   QMessageBox::Yes | QMessageBox::No);
	msgBox.setStyleSheet(programStyleSheet);
	msgBox.setButtonText(QMessageBox::Yes, translate(STR_YES));
	msgBox.setButtonText(QMessageBox::No, translate(STR_NO));
	int ret = msgBox.exec();

	if (ret == QMessageBox::Yes)
	{
		int err = Calibration();

		if (err == OK)
		{
			QMessageBox note(QMessageBox::Information, "Calibration", translate(STR_BUSCALIBRAOK), QMessageBox::Ok);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
			note.exec();
			UpdateStatusBar();
		}
		else
		{
			QString str;
			str = QString("%1 (%2)").arg(translate(STR_BUSCALIBRAFAIL)).arg(err);

			QMessageBox note(QMessageBox::Critical, "Calibration", str, QMessageBox::Ok);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
			note.exec();
		}
	}

	return OK;
}

// int e2CmdWindow::CmdWrite(int type)
// {
// 	int res;
// 	SetAppBusy();
// 	res = CmdWrite(type, E2Profile::GetVerifyAfterWrite());
// 	SetAppReady();
// 	return res;
// }


int e2CmdWindow::CmdRead(int type)
{
	if (IsBufChanged() && awip->IsBufferValid() && verbose == verboseAll)
	{
		QMessageBox msgBox(QMessageBox::Warning, QString(APP_NAME), translate(STR_BUFCHANGED3),
						   QMessageBox::Yes | QMessageBox::No);
		msgBox.setStyleSheet(programStyleSheet);
		msgBox.setButtonText(QMessageBox::Yes, translate(STR_YES));
		msgBox.setButtonText(QMessageBox::No, translate(STR_NO));
		int ret = msgBox.exec();

		if (ret == QMessageBox::Yes)
		{
			awip->SetSaveType(ALL_TYPE);    //???
			SaveFile();
		}
	}

	ClearIgnoreFlag();

	int result;
	int rval;
	int retry_flag = 1;

	while (retry_flag)
	{
		result = OK;

		retry_flag = 0;

		doProgress(translate(STR_MSGREADING));

		awip->SetFileName("");
		SetTitle();

		rval = awip->Read(type);
		e2Prg->reset();

		first_line = 0;
		//curIndex = 0;
		Draw();

		if (rval > 0)
		{
			QString sp;
			//sp = GetEEPTypeString(awip->GetEEPPriType(), awip->GetEEPSubType());
			sp = awip->GetTypeString(awip->GetId());
			//qDebug() << "CmdRead" << awip->GetEEPPriType() << awip->GetEEPSubType() << sp;
			UpdateStrFromStr(sp, "");
			awip->RecalcCRC();
			UpdateStatusBar();

			QString str;
			str = QString("%1 %2 Byte").arg(translate(STR_MSGREADOK)).arg(GetDevSize());

			if (verbose == verboseAll)
			{
				QMessageBox note(QMessageBox::Information, "Read", str, QMessageBox::Ok);
				note.setStyleSheet(programStyleSheet);
				note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
				note.exec();
			}
		}
		else
		{
			qDebug() << "CmdWindow->Read -- Error";

			result = rval;

			if (verbose != verboseNo)
			{
				rval = OnError(rval);

				if (rval == QMessageBox::Abort)   //Abort
				{
					retry_flag = 0;
					ClearIgnoreFlag();
				}

				if (rval == QMessageBox::Retry)   //Retry
				{
					retry_flag = 1;
					ClearIgnoreFlag();
				}
				else if (rval == QMessageBox::Ignore)     //Ignore
				{
					retry_flag = 1;
					SetIgnoreFlag();
				}
			}
		}
	} //while (retry_flag)

	ClearIgnoreFlag();

	return result;
}


int e2CmdWindow::CmdWrite(int type, bool verify)
{
	int result = OK;
	int old_type = type;

	if (verify)
	{
		type &= ~CONFIG_TYPE;
	}

	if (!awip->IsBufferValid())
	{
		if (verbose != verboseNo)
		{
			QMessageBox note(QMessageBox::Warning, "Write", translate(STR_NOTHINGWRITE), QMessageBox::Ok);

			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
			note.exec();
		}

		result = CMD_NOTHINGTOWRITE;
	}
	else
	{
		if ((verbose != verboseAll) || (QMessageBox::warning(this, QString(APP_NAME),
										translate(STR_ASKWRITE),
										QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes))
		{
			int rval;
			int retry_flag = 1;

			UpdateBuffer();
			ClearIgnoreFlag();

			while (retry_flag)
			{
				result = OK;

				retry_flag = 0;

				doProgress(translate(STR_MSGWRITING));

				rval = awip->Write(type, true, verify ? true : false);

				if (rval > 0)
				{
					if (verify)
					{
						doProgress(translate(STR_MSGVERIFING));

						if ((old_type & CONFIG_TYPE) &&
								!(awip->GetFuseBits() == 0 && awip->GetLockBits() == 0))
						{
							rval = awip->Verify(type, false, true);

							if (rval > 0)
							{
								//e2Prg->UpdateDialog(0, translate(STR_MSGWRITINGFUSE));
								rval = awip->Write(CONFIG_TYPE, false, false);

								if (rval > 0)
								{
									rval = awip->Verify(CONFIG_TYPE, true, false);
								}
							}
						}
						else
						{
							rval = awip->Verify(type, false, false);
						}
					}

					e2Prg->reset();

					if (rval > 0)     //23/10/1999
					{
						first_line = 0;
						Draw();

						if (GetDevSize() == 0)
						{
							QString sp;
							//sp = GetEEPTypeString(awip->GetEEPPriType(), awip->GetEEPSubType());
							sp = awip->GetTypeString(awip->GetId());
							//qDebug() << "CmdWrite" << awip->GetEEPPriType() << awip->GetEEPSubType() << sp;
							UpdateStrFromStr(sp);
						}

						awip->RecalcCRC();
						UpdateStatusBar();

						if (verbose == verboseAll)
						{
							QMessageBox note(QMessageBox::Information, "Write", translate(STR_MSGWRITEOK), QMessageBox::Ok);
							note.setStyleSheet(programStyleSheet);
							note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
							note.exec();
						}
					}
					else if (rval == 0)
					{
						if (verbose != verboseNo)
						{
							QMessageBox note(QMessageBox::Warning, "Write", translate(STR_MSGWRITEFAIL), QMessageBox::Ok);
							note.setStyleSheet(programStyleSheet);
							note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
							note.exec();
						}

						result = CMD_WRITEFAILED;
					}
					else
					{
						if (verbose != verboseNo)
						{
							OnError(rval, translate(STR_MSGWRITEFAIL));
						}

						result = rval;
					}
				} // if ( (rval = ...
				else
				{
					e2Prg->reset();

					result = rval;

					if (verbose != verboseNo)
					{
						rval = OnError(rval, translate(STR_MSGWRITEFAIL));

						if (rval == QMessageBox::Abort)   //Abort
						{
							retry_flag = 0;
							ClearIgnoreFlag();
						}

						if (rval == QMessageBox::Retry)   //Retry
						{
							retry_flag = 1;
							ClearIgnoreFlag();
						}
						else if (rval == QMessageBox::Ignore)     //Ignore
						{
							retry_flag = 1;
							SetIgnoreFlag();
						}
					} // if (verbose != verboseNo)
				}
			} // while (retry_flag)

			ClearIgnoreFlag();
		} // if ( yn.AskYN(...
	} //else

	return result;
}

int e2CmdWindow::CmdReadCalibration(int idx)
{
	ClearIgnoreFlag();

	int result;
	int rval = 0xFF;
	int retry_flag = 1;

	while (retry_flag)
	{
		result = OK;

		retry_flag = 0;

		long loc;
		int size;
		bool mtype;
		bool enabled;

		loc = 0;
		size = 1;
		mtype = false;
		enabled = false;

		E2Profile::GetCalibrationAddress(enabled, loc, size, mtype);

		if (mtype)
		{
			loc += awip->GetSplittedInfo();
		}

		rval = awip->ReadOscCalibration(idx);

		if (rval >= 0)
		{
			if (enabled && (size > 0 && size <= 4) &&
					(loc + size <= awip->GetBufSize())
			   )
			{
				quint8 *bp = awip->GetBufPtr() + loc;

				*bp = (quint8)rval;

				awip->RecalcCRC();
				//awip->BufChanged();
				// EK 2017
				// TODO
				Draw();
				UpdateStatusBar();

				if (verbose == verboseAll)
				{
					QString str;
#if USE_QT_VERSION == 4
					str = translate(STR_MSGREADCALIBOK) + QString().sprintf(": 0x%02X (%d)", rval, rval);
#else
					str = translate(STR_MSGREADCALIBOK) + QString().asprintf(": 0x%02X (%d)", rval, rval);
#endif

					QMessageBox note(QMessageBox::Information, "Calibration", str, QMessageBox::Ok);
					note.setStyleSheet(programStyleSheet);
					note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
					note.exec();
				}
			}
			else
			{
				result = BADPARAM;

				if (verbose != verboseNo)
				{
					rval = OnError(result);

					if (rval == QMessageBox::Abort)   //Abort
					{
						retry_flag = 0;
						ClearIgnoreFlag();
					}

					if (rval == QMessageBox::Retry)   //Retry
					{
						retry_flag = 1;
						ClearIgnoreFlag();
					}
					else if (rval == QMessageBox::Ignore)     //Ignore
					{
						retry_flag = 1;
						SetIgnoreFlag();
					}
				}
			}
		}
		else
		{
			result = rval;

			if (verbose != verboseNo)
			{
				rval = OnError(rval);

				if (rval == QMessageBox::Abort)   //Abort
				{
					retry_flag = 0;
					ClearIgnoreFlag();
				}

				if (rval == QMessageBox::Retry)   //Retry
				{
					retry_flag = 1;
					ClearIgnoreFlag();
				}
				else if (rval == QMessageBox::Ignore)     //Ignore
				{
					retry_flag = 1;
					SetIgnoreFlag();
				}
			}
		}
	} //while (retry_flag)

	ClearIgnoreFlag();
	return result;
}

int e2CmdWindow::CmdErase(int type)
{
	ClearIgnoreFlag();

	int result;
	int rval;
	int retry_flag = 1;

	while (retry_flag)
	{
		result = OK;

		retry_flag = 0;

		doProgress(translate(STR_MSGERASING));

		rval = awip->Erase(type);
		e2Prg->reset();

		if (rval > 0)
		{
			//CmdClearBuf();

			if (verbose == verboseAll)
			{
				QMessageBox note(QMessageBox::Information, "Warning", translate(STR_MSGERASEOK), QMessageBox::Ok);

				note.setStyleSheet(programStyleSheet);
				note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
				note.exec();
			}
		}
		else
		{
			//first_line = 0;
			//curIndex = 0;
			//Draw();

			qDebug() << Q_FUNC_INFO << "CmdWindow->Erase -- Error";

			result = rval;

			if (verbose != verboseNo)
			{
				rval = OnError(rval);

				if (rval == QMessageBox::Abort)   //Abort
				{
					retry_flag = 0;
					ClearIgnoreFlag();
				}

				if (rval == QMessageBox::Retry)   //Retry
				{
					retry_flag = 1;
					ClearIgnoreFlag();
				}
				else if (rval == QMessageBox::Ignore)     //Ignore
				{
					retry_flag = 1;
					SetIgnoreFlag();
				}
			}
		}
	} //while (retry_flag)

	ClearIgnoreFlag();

	return result;
}

int e2CmdWindow::CmdVerify(int type)
{
	int result = OK;

	if (!awip->IsBufferValid())
	{
		if (verbose != verboseNo)
		{
			QMessageBox note(QMessageBox::Information, "Warning", translate(STR_NOTHINGVERIFY), QMessageBox::Ok);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
			note.exec();
		}

		result = CMD_NOTHINGTOVERIFY;
	}
	else
	{
		doProgress(translate(STR_MSGVERIFING));

		int rval = awip->Verify(type);
		e2Prg->reset();

		if (rval < 0)
		{
			result = rval;

			if (verbose != verboseNo)
			{
				OnError(rval, translate(STR_MSGVERIFYFAIL1));
			}
		}
		else if (rval == 0)
		{
			result = CMD_VERIFYFAILED;

			if (verbose != verboseNo)
			{
				QMessageBox note(QMessageBox::Critical, "Verify", translate(STR_MSGVERIFYFAIL2), QMessageBox::Close);
				note.setStyleSheet(programStyleSheet);
				note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
				note.exec();
			}
		}
		else
		{
			result = OK;

			if (verbose == verboseAll)
			{
				QMessageBox note(QMessageBox::Information, "Verify", translate(STR_MSGVERIFYOK), QMessageBox::Ok);
				note.setStyleSheet(programStyleSheet);
				note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
				note.exec();
			}
		}
	}

	return result;
}

int e2CmdWindow::CmdProgram()
{
	int result = OK;
	int prog_opt = E2Profile::GetProgramOptions();

	VerboseType old_verbose = verbose;

	if (verbose == verboseAll)
	{
		verbose = verboseErr;        //Disable all ok messages
	}

	if ((prog_opt & RELOAD_YES))
	{
		result = CmdReload();
	}

	if (result == OK && (prog_opt & READFLASH_YES))
	{
		result = CmdRead(PROG_TYPE);
	}

	if (result == OK && (prog_opt & READEEP_YES))
	{
		result = CmdRead(DATA_TYPE);
	}

	if (result == OK && (prog_opt & BYTESWAP_YES))
	{
		result = CmdByteSwap();
	}

	if (result == OK && (prog_opt & SETID_YES))
	{
		result = CmdSetSerialNumber();
	}

	if (result == OK && (prog_opt & READOSCAL_YES))
	{
		result = CmdReadCalibration(0);
	}

	if (result == OK && (prog_opt & ERASE_YES))
	{
		result = CmdErase(ALL_TYPE);
	}

	if (result == OK && (prog_opt & FLASH_YES))
	{
		result = CmdWrite(PROG_TYPE, true);
	}

	if (result == OK && (prog_opt & EEPROM_YES))
	{
		result = CmdWrite(DATA_TYPE, true);
	}

	if (result == OK && (prog_opt & LOCK_YES))
	{
		result = CmdWriteSecurity();
	}

	verbose = old_verbose;

	if (result == OK)
	{
		if (verbose == verboseAll)
		{
			QMessageBox note(QMessageBox::Information, "Program", translate(STR_MSGPROGRAMOK), QMessageBox::Ok);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
			note.exec();
		}
	}
	else
	{
		if (verbose != verboseNo)
		{
			QString str;
#if USE_QT_VERSION == 4
			str = translate(STR_MSGPROGRAMFAIL) + QString().sprintf(" (%d)", result);
#else
			str = translate(STR_MSGPROGRAMFAIL) + QString().asprintf(" (%d)", result);
#endif

			QMessageBox note(QMessageBox::Critical, "Program", str, QMessageBox::Close);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
			note.exec();
		}
	}

	return result;
}


static QStringList myscantokenizer(char *buf)//, char *arg[], int arglen)
{
	QString ln = buf;
	ln = ln.simplified();
	QStringList l;

	if (ln.size() > 0)
	{
		l = ln.split(" ");
	}

	return l;
}


int e2CmdWindow::ScriptError(int line_number, int arg_index, const QString &s, const QString msg)
{
	QString str;

	//if (s.length() == 0)
	//{
	//	arg = "";
	//}

	if (arg_index == 0)
	{
		str = QString("%1 %2: %3 '%4'").arg(translate(STR_MSGSCRIPTERROR)).arg(line_number).arg(msg.length() ? msg : translate(STR_MSGSCRIPTBADCOMMAND)).arg(s);
	}
	else if (s.length() == 0)
	{
		str = QString("%1 %2: %3").arg(translate(STR_MSGSCRIPTERROR)).arg(line_number).arg(msg.length() ? msg : translate(STR_MSGSCRIPTARGMISSING));
	}
	else
	{
		str = QString("%1 %2: %3 '%4'").arg(translate(STR_MSGSCRIPTERROR)).arg(line_number).arg(msg.length() ? msg : translate(STR_MSGSCRIPTBADARGUMENT)).arg(s);
	}

	QMessageBox note(QMessageBox::Critical, "Script Error", str, QMessageBox::Close);
	note.setStyleSheet(programStyleSheet);
	note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
	note.exec();

	return CMD_SCRIPTERROR;
}

int e2CmdWindow::CmdRunScript(bool test_mode)
{
	int result = OK;
	char buf[512];
	int linecounter;

	VerboseType old_verbose = verbose;

	if (verbose == verboseAll)
	{
		verbose = verboseErr;        //Disable all ok messages
	}

	if (script_name.length() == 0)
	{
		return BADPARAM;
	}

	QString fpath = QFileInfo(script_name).absolutePath();
	qDebug() << Q_FUNC_INFO << "Set current path: " << fpath;
	QDir::setCurrent(fpath);

	QFile fh(script_name);

	if (!fh.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return FILENOTFOUND;
	}

	linecounter = 0;

	while (result == OK && !fh.atEnd())
	{
		fh.readLine(buf, sizeof(buf) - 1);
		linecounter++;

		if (buf[0] == '#')              //Skip comments
		{
			continue;
		}

		QStringList lst = myscantokenizer(buf);
		int n = lst.count();

		if (n == 0)
		{
			//Line empty: do nothing
			continue;
		}

		QString cmdbuf = lst.at(0).toUpper();

		if (cmdbuf == "SELECTDEVICE")
		{
			if (n == 2)
			{
				quint32 new_type;
				QString nm = lst.at(1);
				new_type = awip->GetTypeFromString(nm);

				if (new_type <= 0)
				{
					result = ScriptError(linecounter, 1, lst.at(1));
				}
				else
				{
					if (!test_mode)
					{
						result = CmdSelectDevice(new_type);
					}
				}
			}
			else     //Argument missing
			{
				result = ScriptError(linecounter, 1, lst.at(1));
			}
		}
		else if (cmdbuf == "LOAD-ALL")
		{
			if (n >= 2)
			{
				long reloc_off = 0;
				bool ok = true;

				if (n >= 3)
				{
					reloc_off = lst.at(2).toLong(&ok, 0);

					if (!ok)
					{
						result = ScriptError(linecounter, 2, lst.at(2));
					}
				}

				if (ok && !QFile::exists(lst.at(1)))
				{
					ok = false;
					result = ScriptError(linecounter, 1, lst.at(1), translate(STR_MSGFILENOTFOUND));
				}

				if (ok && !test_mode)
				{
					result = CmdOpen(ALL_TYPE, lst.at(1), reloc_off, 0);        //Don't clear buffer before load on script
				}
			}
			else if (n == 1)
			{
				if (!test_mode)
				{
					result = CmdOpen(ALL_TYPE);
				}
			}
		}
		else if (cmdbuf == "LOAD-PROG")
		{
			if (n >= 2)
			{
				long reloc_off = 0;
				bool ok = true;

				if (n >= 3)
				{
					reloc_off = lst.at(2).toLong(&ok, 0);

					if (!ok)
					{
						result = ScriptError(linecounter, 2, lst.at(2));
					}
				}

				if (ok && !QFile::exists(lst.at(1)))
				{
					ok = false;
					result = ScriptError(linecounter, 1, lst.at(1), translate(STR_MSGFILENOTFOUND));
				}

				if (ok && !test_mode)
				{
					result = CmdOpen(PROG_TYPE, lst.at(1), reloc_off, 0);        //Don't clear buffer before load on script
				}
			}
			else if (n == 1)
			{
				if (!test_mode)
				{
					result = CmdOpen(PROG_TYPE);
				}
			}
		}
		else if (cmdbuf == "LOAD-DATA")
		{
			if (n >= 2)
			{
				long reloc_off = 0;
				bool ok = true;

				if (n >= 3)
				{
					reloc_off = lst.at(2).toLong(&ok, 0);

					if (!ok)
					{
						result = ScriptError(linecounter, 2, lst.at(2));
					}
				}

				if (ok && !QFile::exists(lst.at(1)))
				{
					ok = false;
					result = ScriptError(linecounter, 1, lst.at(1), translate(STR_MSGFILENOTFOUND));
				}

				if (ok && !test_mode)
				{
					result = CmdOpen(DATA_TYPE, lst.at(1), reloc_off, 0);        //Don't clear buffer before load on script
				}
			}
			else if (n == 1)
			{
				if (!test_mode)
				{
					result = CmdOpen(DATA_TYPE);
				}
			}
		}
		else if (cmdbuf == "SAVE-ALL")
		{
			if (n >= 2)
			{
				if (n >= 3)
				{
					QString tp = lst.at(2).toLower();

					if (tp == "e2p")
					{
						awip->SetFileBuf(E2P);
					}
					else if (tp == "bin")
					{
						awip->SetFileBuf(BIN);
					}
					else if (tp == "csm")
					{
						awip->SetFileBuf(CSM);
					}
					else if (tp == "intel-hex")
					{
						awip->SetFileBuf(INTEL);
					}
					else if (tp == "mot-srec")
					{
						awip->SetFileBuf(MOTOS);
					}
					else
					{
						result = ScriptError(linecounter, 2, lst.at(2));
					}
				}

				if (result == OK && !test_mode)
				{
					if (lst.at(1) == "--")
					{
						result = CmdSaveAs(ALL_TYPE);
					}
					else
					{
						result = CmdSave(ALL_TYPE, lst.at(1).toLatin1());
					}
				}
			}
			else if (n == 1)
			{
				if (!test_mode)
				{
					result = CmdSave(ALL_TYPE);
				}
			}
		}
		else if (cmdbuf == "SAVE-PROG")
		{
			if (n >= 2)
			{
				if (n >= 3)
				{
					QString tp = lst.at(2).toLower();

					if (tp == "e2p")
					{
						awip->SetFileBuf(E2P);
					}
					else if (tp == "bin")
					{
						awip->SetFileBuf(BIN);
					}
					else if (tp == "csm")
					{
						awip->SetFileBuf(CSM);
					}
					else if (tp == "intel-hex")
					{
						awip->SetFileBuf(INTEL);
					}
					else if (tp == "mot-srec")
					{
						awip->SetFileBuf(MOTOS);
					}
					else
					{
						result = ScriptError(linecounter, 2, lst.at(2));
					}
				}

				if (result == OK && !test_mode)
				{
					if (lst.at(1) == "--")
					{
						result = CmdSaveAs(PROG_TYPE);
					}
					else
					{
						result = CmdSave(PROG_TYPE, lst.at(1).toLatin1().constData());
					}
				}
			}
			else if (n == 1)
			{
				if (!test_mode)
				{
					result = CmdSave(PROG_TYPE);
				}
			}
		}
		else if (cmdbuf == "SAVE-DATA")
		{
			if (n >= 2)
			{
				if (n >= 3)
				{
					QString tp = lst.at(2).toLower();

					if (tp == "e2p")
					{
						awip->SetFileBuf(E2P);
					}
					else if (tp == "bin")
					{
						awip->SetFileBuf(BIN);
					}
					else if (tp == "csm")
					{
						awip->SetFileBuf(CSM);
					}
					else if (tp == "intel-hex")
					{
						awip->SetFileBuf(INTEL);
					}
					else if (tp == "mot-srec")
					{
						awip->SetFileBuf(MOTOS);
					}
					else
					{
						result = ScriptError(linecounter, 2, lst.at(2));
					}
				}

				if (result == OK && !test_mode)
				{
					if (lst.at(1) == "--")
					{
						result = CmdSaveAs(DATA_TYPE);
					}
					else
					{
						result = CmdSave(DATA_TYPE, lst.at(1).toLatin1());
					}
				}
			}
			else if (n == 1)
			{
				if (!test_mode)
				{
					result = CmdSave(DATA_TYPE);
				}
			}
		}
		else if (cmdbuf == "READ-ALL")
		{
			if (!test_mode)
			{
				result = CmdRead(ALL_TYPE);
			}
		}
		else if (cmdbuf == "READ-PROG")
		{
			if (!test_mode)
			{
				result = CmdRead(PROG_TYPE);
			}
		}
		else if (cmdbuf == "READ-DATA")
		{
			if (!test_mode)
			{
				result = CmdRead(DATA_TYPE);
			}
		}
		else if (cmdbuf == "WRITE&VERIFY-ALL")
		{
			if (!test_mode)
			{
				result = CmdWrite(ALL_TYPE, true);
			}
		}
		else if (cmdbuf == "WRITE&VERIFY-PROG")
		{
			if (!test_mode)
			{
				result = CmdWrite(PROG_TYPE, true);
			}
		}
		else if (cmdbuf == "WRITE&VERIFY-DATA")
		{
			if (!test_mode)
			{
				result = CmdWrite(DATA_TYPE, true);
			}
		}
		else if (cmdbuf == "WRITE-ALL")
		{
			if (!test_mode)
			{
				result = CmdWrite(ALL_TYPE, false);
			}
		}
		else if (cmdbuf == "WRITE-PROG")
		{
			if (!test_mode)
			{
				result = CmdWrite(PROG_TYPE, false);
			}
		}
		else if (cmdbuf == "WRITE-DATA")
		{
			if (!test_mode)
			{
				result = CmdWrite(DATA_TYPE, false);
			}
		}
		else if (cmdbuf == "ERASE-ALL")
		{
			if (!test_mode)
			{
				result = CmdErase(ALL_TYPE);
			}
		}
		else if (cmdbuf == "VERIFY-ALL")
		{
			if (!test_mode)
			{
				result = CmdVerify(ALL_TYPE);
			}
		}
		else if (cmdbuf == "VERIFY-PROG")
		{
			if (!test_mode)
			{
				result = CmdVerify(PROG_TYPE);
			}
		}
		else if (cmdbuf == "VERIFY-DATA")
		{
			if (!test_mode)
			{
				result = CmdVerify(DATA_TYPE);
			}
		}
		else if (cmdbuf == "WRITE-FUSE")
		{
			if (n >= 2)
			{
				bool ok;
				long fuse = lst.at(1).toLong(&ok, 0);

				if (!ok)
				{
					result = ScriptError(linecounter, 1, lst.at(1));
				}
				else if (!test_mode)
				{
					awip->SetFuseBits(fuse);
					result = CmdWriteSpecial();
				}
			}
			else if (n == 1)
			{
				if (!test_mode)
				{
					result = CmdWriteSpecial();
				}
			}
		}
		else if (cmdbuf == "SET-FUSE")
		{
			if (n >= 2)
			{
				bool ok;
				long fuse = lst.at(1).toLong(&ok, 0);

				if (!ok)
				{
					result = ScriptError(linecounter, 1, lst.at(1));
				}
				else if (!test_mode)
				{
					awip->SetFuseBits(fuse);
				}
			}
			else     //Argument missing
			{
				result = ScriptError(linecounter, 1, lst.at(1));
			}
		}
		else if (cmdbuf == "WRITE-LOCK")
		{
			if (n >= 2)
			{
				bool ok;
				long lock = lst.at(1).toLong(&ok, 0);

				if (!ok)
				{
					result = ScriptError(linecounter, 1, lst.at(1));
				}
				else if (!test_mode)
				{
					awip->SetLockBits(lock);
					result = CmdWriteLock();
				}
			}
			else if (n == 1)
			{
				if (!test_mode)
				{
					result = CmdWriteLock();
				}
			}
		}
		else if (cmdbuf == "SET-LOCK")
		{
			if (n >= 2)
			{
				bool ok;
				long lock = lst.at(1).toLong(&ok, 0);

				if (!ok)
				{
					result = ScriptError(linecounter, 1, lst.at(1));
				}
				else if (!test_mode)
				{
					awip->SetLockBits(lock);
				}
			}
			else     //Argument missing
			{
				result = ScriptError(linecounter, 1, lst.at(1));
			}
		}
		else if (cmdbuf == "EDIT-SECURITY")
		{
			if (!test_mode)
			{
				result = SpecialBits();
			}
		}
		else if (cmdbuf == "READ-FUSE")
		{
			if (!test_mode)
			{
				result = CmdReadSpecial();
			}
		}
		else if (cmdbuf == "READ-LOCK")
		{
			if (!test_mode)
			{
				result = CmdReadLock();
			}
		}
		else if (cmdbuf == "SERIALNUMBER")
		{
			if (n >= 2)
			{
				bool ok = false;
				long sernum = lst.at(1).toLong(&ok, 0);

				if (!ok)
				{
					result = ScriptError(linecounter, 1, lst.at(1));
				}
				else if (!test_mode)
				{
					E2Profile::SetSerialNumVal(sernum);
				}
			}

			if (n >= 3)
			{
				long start = 0;
				int size = 1;
				bool mtype = false;
				E2Profile::GetSerialNumAddress(start, size, mtype);

				bool ok = false;
				start = lst.at(2).toLong(&ok, 0);       //address location

				if (!ok)
				{
					result = ScriptError(linecounter, 2, lst.at(2));
				}

				if (ok && n >= 4)
				{
					size = lst.at(3).toInt(&ok);    //address size

					if (!ok)
					{
						result = ScriptError(linecounter, 3, lst.at(3));
					}
				}

				if (ok && n >= 5)
				{
					if (lst.at(4) == "DATA")               //offset
					{
						mtype = true;
					}
				}

				if (ok && !test_mode)
				{
					E2Profile::SetSerialNumAddress(start, size, mtype);
				}

				if (ok && n >= 6)
				{
					FmtEndian fmt;
					fmt = E2Profile::GetSerialNumFormat();

					if ("LITTLEENDIAN" == lst.at(5))
					{
						fmt = FMT_LITTLE_ENDIAN;
					}
					else
					{
						fmt = FMT_BIG_ENDIAN;
					}

					if (!test_mode)
					{
						E2Profile::SetSerialNumFormat(fmt);
					}
				}

				if (ok && n >= 7)
				{
					bool autoinc = E2Profile::GetSerialNumAutoInc();

					if (n >= 7) // <--- ???
					{
						autoinc = ("NO" == lst.at(6)) ? false : true;
					}

					if (!test_mode)
					{
						E2Profile::SetSerialNumAutoInc(autoinc);
					}
				}

				if (ok && !test_mode)
				{
					result = CmdSetSerialNumber();
				}
			}
		}
		else
		{
			//READ-CALIBRATION <address>[mem][osc_index]
			if (cmdbuf == "READ-CALIBRATION")
			{
				long start = 0;
				int size = 1;
				bool mtype = false;
				bool enabled = false;
				E2Profile::GetCalibrationAddress(enabled, start, size, mtype);

				int osc_index = 0;
				bool ok = true;

				if (n >= 2)
				{
					start = lst.at(1).toLong(&ok, 0);      //address location
					if (!ok)
					{
						result = ScriptError(linecounter, 1, lst.at(1));
					}

					if (n >= 3)
					{
						if (lst.at(2) == "DATA")        //offset
						{
							mtype = true;
						}
					}

					if (ok && !test_mode)
					{
						E2Profile::SetCalibrationAddress(enabled, start, size, mtype);
					}

					if (ok && n >= 4)
					{
						osc_index = lst.at(3).toInt(&ok);            //address size

						if (!ok)
						{
							result = ScriptError(linecounter, 3, lst.at(3));
						}
					}
				}

				if (ok && !test_mode)
				{
					result = CmdReadCalibration(osc_index);
				}
			}
			else if (cmdbuf == "BYTESWAP")
			{
				if (!test_mode)
				{
					result = CmdByteSwap();
				}
			}
			else if (cmdbuf == "RESET")
			{
				if (!test_mode)
				{
					result = CmdReset();
				}
			}
			else if (cmdbuf == "CLEARBUFFER" || cmdbuf == "CLEARBUF-ALL")
			{
				if (!test_mode)
				{
					result = CmdClearBuf(ALL_TYPE);
				}
			}
			else if (cmdbuf == "CLEARBUF-PROG")
			{
				if (!test_mode)
				{
					result = CmdClearBuf(PROG_TYPE);
				}
			}
			else if (cmdbuf == "CLEARBUF-DATA")
			{
				if (!test_mode)
				{
					result = CmdClearBuf(DATA_TYPE);
				}
			}
			else if (cmdbuf == "FILLBUFFER")
			{
				if (n >= 4)
				{
					long val, from = 0, to = 0;
					bool ok = true;

					val = lst.at(1).toLong(&ok, 0);
					if (!ok)
					{
						result = ScriptError(linecounter, 1, lst.at(1));
					}
					else
					{
						from = lst.at(2).toLong(&ok, 0);
						if (!ok)
						{
							result = ScriptError(linecounter, 2, lst.at(2));
						}
						else
						{
							to = lst.at(3).toLong(&ok, 0);
							if (!ok)
							{
								result = ScriptError(linecounter, 3, lst.at(3));
							}
						}
					}

					if (ok)
					{
						if (from < to && from >= 0)
						{
							if (!test_mode)
							{
								awip->FillBuffer(from, val, to - from + 1);
								// EK 2017
								// TODO
								Draw();
								UpdateStatusBar();
							}
						}
						else
						{
							result = ScriptError(linecounter, 2, lst.at(2));
						}
					}
				}
				else if (n == 1)
				{
					if (!test_mode)
					{
						result = CmdFillBuf();
					}
				}
				else
				{
					result = ScriptError(linecounter, 3, lst.at(3));
				}
			}
			else if (cmdbuf == "PAUSE")
			{
				QString s;

				if (n >= 2)
				{
					s = lst.at(1);
				}
				else
				{
					s = translate(STR_MSGDEFAULTPAUSE);
				}

				if (!test_mode)
				{
					QMessageBox msgBox(QMessageBox::Warning, QString(APP_NAME), s,
									   QMessageBox::Yes | QMessageBox::No);
					msgBox.setStyleSheet(programStyleSheet);
					msgBox.setButtonText(QMessageBox::Yes, translate(STR_YES));
					msgBox.setButtonText(QMessageBox::No, translate(STR_NO));
					int ret = msgBox.exec();

					if (ret == QMessageBox::Yes)
					{
						result = OK;
					}
					else
					{
						result = OP_ABORTED;
					}
				}
			}
			else if (cmdbuf == "CALL")
			{
				if (n >= 2)
				{
					if (!test_mode)
					{
						//Warning! security risk!
#if 0
						vOS os;
						result = os.vRunProcess(lst.at(1), 0, 0, /*Wait*/ 1, /*minimize*/ 0);

						if (result == 99)
						{
							result = ScriptError(linecounter, 1, lst.at(1), translate(STR_MSGFILENOTFOUND));
						}
#else
						QStringList alst = lst;
						alst.removeFirst();
						alst.removeFirst();

						QProcess process;
						process.start(lst.at(1), alst);
						if (!process.waitForStarted(500))
						{
							result = ScriptError(linecounter, 1, lst.at(1), translate(STR_MSGFILENOTFOUND));
						}
						else
						{
							process.waitForFinished(1000);
						}
#endif
					}
				}
				else     //Argument missing
				{
					result = ScriptError(linecounter, 1, lst.at(1));
				}
			}
			else if (cmdbuf == "DELAY")
			{
				if (n >= 2)
				{
					bool ok = false;
					long msec = lst.at(1).toInt(&ok);

					if (!ok)
					{
						result = ScriptError(linecounter, 1, lst.at(1));
					}
					else if (!test_mode)
					{
						Wait w;
						w.WaitMsec(msec);
					}
				}
				else
				{
					//Bad argument
					result = ScriptError(linecounter, 1, lst.at(1));
				}
			}
			else
			{
				result = ScriptError(linecounter, 0, lst.at(0));   //Bad command
			}
		}
	} //while

	//If in scriptMode don't restore the normal verbose yet
	if (!scriptMode && !test_mode)
	{
		verbose = old_verbose;
	}

	if (result == OK)
	{
		if (verbose == verboseAll)
		{
			QMessageBox note(QMessageBox::Information, "Script information", translate(STR_MSGPROGRAMOK), QMessageBox::Ok);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
			note.exec();
		}
	}
	else if (result == OP_ABORTED)
	{
		if (verbose == verboseAll)
		{
			QMessageBox note(QMessageBox::Warning, "Script information", translate(STR_OPABORTED), QMessageBox::Ok);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
			note.exec();
		}
	}
	else if (result == CMD_SCRIPTERROR)
	{
		if (verbose == verboseAll)
		{
			QMessageBox note(QMessageBox::Critical, "Script information", translate(STR_SCRIPTERROR), QMessageBox::Close);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
			note.exec();
		}
	}
	else
	{
		if (verbose != verboseNo)
		{
			QString str;
#if USE_QT_VERSION == 4
			str = translate(STR_MSGPROGRAMFAIL) + QString().sprintf(" (%d)", result);
#else
			str = translate(STR_MSGPROGRAMFAIL) + QString().asprintf(" (%d)", result);
#endif
			QMessageBox note(QMessageBox::Critical, "Script information", str, QMessageBox::Close);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
			note.exec();
		}
	}

	verbose = old_verbose;

	return result;
}

int e2CmdWindow::CmdGetInfo()
{
	long fsize, esize, tsize;

	tsize = GetDevSize();
	fsize = awip->GetSplittedInfo();

	if (fsize == tsize)
	{
		fsize = 0;
	}

	esize = tsize - fsize;

	int pritype = awip->GetPriType(awip->GetId());

	if (pritype == E24XX || pritype == E24XX2 || pritype == E24XX5)
	{
		//      quint32 hblock, secbits;
		int rlv;

		if (pritype != E24XX)
		{
			rlv = 3;
		}
		else
		{
			rlv = awip->GetBankRollOver();

			if (rlv == 0)
			{
				//                              vYNReplyDialog yn(this);
				QMessageBox msgBox(QMessageBox::Warning, QString(APP_NAME), QString(translate(STR_MSGBANKROLLOVER)),
								   QMessageBox::Yes | QMessageBox::No);
				msgBox.setStyleSheet(programStyleSheet);
				msgBox.setButtonText(QMessageBox::Yes, translate(STR_YES));
				msgBox.setButtonText(QMessageBox::No, translate(STR_NO));
				int ret = msgBox.exec();

				if (ret == QMessageBox::Yes)
				{
					rlv = awip->BankRollOverDetect(1);
				}
			}
		}

		//      awip->SecurityRead(secbits);
		//      awip->HighEnduranceRead(hblock);

		e24xx_infoModalDialog info(this, rlv, awip->GetCRC(), tsize);
		info.exec();
	}
	else
	{
		other_infoModalDialog info(this, fsize, esize, awip->GetCRC());
		info.exec();
	}

	return OK;
}

int e2CmdWindow::CmdReset()
{
	awip->Reset();

	if (verbose == verboseAll)
	{
		QMessageBox note(QMessageBox::Warning, "Reset", translate(STR_MSGDEVRESET), QMessageBox::Ok);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
		note.exec();
	}

	return OK;
}

int e2CmdWindow::CmdDoubleSize()
{
	if (!awip->IsBufferValid())
	{
		QMessageBox note(QMessageBox::Warning, "Double size", translate(STR_BUFEMPTY), QMessageBox::Ok);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
		note.exec();
	}
	else
	{
		// Double the size
		//              long new_type = GetEEPTypeFromSize(awip->GetEEPPriType(), awip->GetNoOfBlock() * 2);
		long new_type = awip->GetTypeFromSize(awip->GetId(), awip->GetNoOfBlock() * 2);

		if (new_type > 0)
		{
			awip->DoubleSize();
			CmdSelectDevice(new_type);
		}
		else
		{
			QMessageBox note(QMessageBox::Warning, "Not supported", translate(STR_OPNOTSUP), QMessageBox::Close);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
			note.exec();
		}
	}

	return OK;
}


int e2CmdWindow::CmdOpen(int type, const QString &fname, long relocation, int clear_buffer)
{
	int result = OK;
	QString oldfname = awip->GetFileName();

	if (IsBufChanged() && awip->IsBufferValid() && verbose == verboseAll)
	{
		QMessageBox msgBox(QMessageBox::Warning, QString(APP_NAME), translate(STR_BUFCHANGED2),
						   QMessageBox::Yes | QMessageBox::No);
		msgBox.setStyleSheet(programStyleSheet);
		msgBox.setButtonText(QMessageBox::Yes, translate(STR_YES));
		msgBox.setButtonText(QMessageBox::No, translate(STR_NO));
		int ret = msgBox.exec();

		if (ret == QMessageBox::Yes)
		{
			awip->SetSaveType(ALL_TYPE);
			SaveFile();
		}
	}

	awip->SetLoadType(type);
	awip->SetLoadRelocation(relocation);

	if (clear_buffer != -1)
	{
		awip->SetLoadAutoClearBuf((clear_buffer == 0) ? false : true);
	}

	result = OpenFile(fname);

	if (result == OK && oldfname != awip->GetFileName())
	{
		E2Profile::SetLastFile(awip->GetFileName(), awip->GetLoadType());
		UpdateFileMenu();
	}

	return result;
}

int e2CmdWindow::CmdClearBuf(int type)
{
	//awip->FillBuffer();
	awip->ClearBuffer(type);
	Draw();
	UpdateStrFromBuf();
	UpdateStatusBar();

	return OK;
}


// EK 2017
// TODO to check this
int e2CmdWindow::CmdFillBuf()
{
	static long fromAddr = 0, toAddr = 0xFFFF;
	static int val = 0xFF;

	FillDialog e2Fill(this, fromAddr, toAddr, val, awip->GetBufSize()/*, translate(STR_MSGINSPARAM)*/);

	if (e2Fill.exec() == QDialog::Accepted)
	{
		if (fromAddr <= toAddr)
		{
			awip->FillBuffer(fromAddr, val, toAddr - fromAddr + 1);
			//      awip->BufChanged();             moved into awip->FillBuffer()
			// EK 2017
			// TODO
			Draw();
			UpdateStatusBar();
		}
		else
		{
			QMessageBox msgBox(QMessageBox::Warning, "Fill buffer",
							   QString(translate(STR_MSGBADPARAM)),
							   QMessageBox::Ok);
			msgBox.setStyleSheet(programStyleSheet);
			msgBox.setButtonText(QMessageBox::Ok, translate(STR_OK));

			msgBox.exec();
		}
	}

	return OK;
}

int e2CmdWindow::SpecialBits(bool readonly)
{
	//int rval;
	//quint32 lock, fuse;

	//If the current fuse settings is invalid try to read it from the device
	if (!awip->IsFuseValid())
	{
		CmdReadSecurity(false);        //call with 'false' to avoid recursion!!!
	}

	bool repeat;

	do
	{
		repeat = false;

		//lock = awip->GetLockBits();
		//fuse = awip->GetFuseBits();

		quint32 type = awip->GetId(); // BuildE2PType(awip->GetEEPPriType(), awip->GetEEPSubType());

		if (type != E2464)
		{
			fuseModalDialog e2Fuse(this, awip, /*" ", type, lock, fuse, */readonly);
			int res = e2Fuse.exec();

			if (res != QDialog::Rejected)
			{
				qDebug() << Q_FUNC_INFO << "fuse " << res;

				if (res == 2)   // read //e2Fuse.isRead() == true)
				{
					CmdReadSecurity(false);
					repeat = true;
				}
				else
				{
					// ok or write
					// EK 2017
					// implemented in fuse dialog
					//awip->SetLockBits(lock);
					//awip->SetFuseBits(fuse);

					if (res == 3)   //Program
					{
						CmdWriteSecurity();
					}
				}
			}
		}
		else
		{
			blockDialog e2Block(this, awip, /*" ", type, lock, fuse, */readonly);

			if (e2Block.exec() == QDialog::Accepted)
			{
				// EK 2017
				// implemented in block dialog
				//awip->SetLockBits(lock);
				//awip->SetFuseBits(fuse);
			}
		}
	}
	while (repeat);

	return OK;
}

// EK 2017
// TODO ???
int e2CmdWindow::ProgramOptions()
{
	//      int rval;
	int reload, reep, erase, flash, eeprom, lock;

	//      lock = awip->GetLockBits();
	//      fuse = awip->GetFuseBits();

	progOptionDialog prog(this, awip->GetId(), // BuildE2PType(awip->GetEEPPriType(), awip->GetEEPSubType()),
						  reload, reep, erase, flash, eeprom, lock);

	if (prog.exec() == QDialog::Accepted)
	{
		//
	}

#if 0
	rval = prog.progAction(" ", BuildE2PType(awip->GetEEPPriType(), awip->GetEEPSubType()),
						   reload, reep, erase, flash, eeprom, lock);

	if (rval != 0)   //OK
	{
		//              awip->SetLockBits(lock);
		//              awip->SetFuseBits(fuse);

		//              if (rval == 2)  //Program
		//              {
		//                      CmdWriteSecurity();
		//              }
	}

#endif
	return OK;
}

int e2CmdWindow::SerialNumberOptions()
{
	SerNumDialog dlg(this) ; //, loc, memtype, autoinc, fmt, size, val);
	dlg.exec();

	return OK;
}

int e2CmdWindow::OscCalibOption()
{
	OscCalibDialog dlg(this, awip);
	dlg.exec();

	return OK;
}


int e2CmdWindow::CmdReadSecurity(bool display_dialog)
{
	int result;

	result = CmdReadLock();

	if (result == OK)
	{
		result = CmdReadSpecial();

		if (result == OK && display_dialog)
		{
			SpecialBits();
		}
	}

	return result;
}

int e2CmdWindow::CmdWriteSecurity()
{
	int result;

	if (awip->GetId() == AT90S4433 || awip->GetId() == AT90S2333)
	{
		result = CmdWriteLock();

		if (result == OK)
		{
			Wait w;
			w.WaitMsec(100);
			result = CmdWriteSpecial();
		}
	}
	else
	{
		result = CmdWriteSpecial();

		if (result == OK)
		{
			result = CmdWriteLock();
		}
	}

	return result;
}

int e2CmdWindow::CmdWriteLock()
{
	int result = OK;

	QMessageBox note;
	/**
	        quint32 type = BuildE2PType( awip->GetEEPPriType(), awip->GetEEPSubType() );
	        if (type == E2464)              //implementazione per le Microchip 24C65
	        {
	                vReplyDialog rp(this);      // instantiate
	                char r[100];                // a buffer for reply

	                rp.Reply("Enter the first block      ",r,99);
	                if (*r)
	                {
	                        int sb, nb;

	                        sb = atoi(r);

	                        rp.Reply("Enter the number of blocks ",r,99);
	                        if (*r)
	                        {
	                                if ((nb = atoi(r)) != 0)
	                                {
	                                        char str[MAXMSG];
											sprintf(str, "Security bits write successful (%d,%d)", sb, nb);

	                                        int err = awip->SecurityWrite( ((sb << 4) & 0xF0) | (nb & 0x0F) );
	                                        if (err)
	                                        {
	                                                if (err == IICERR_NOADDRACK)
	                                                        note.setText("Device Not Responding");
	                                                else
	                                                        note.setText("Security bits write error");
	                                        }
	                                        else
	                                        {
	                                                int val;
	                                                awip->SecurityRead(val);
	                                                if ( ((val >> 4) & 0x0F) != sb || (val & 0xF) != nb)
	                                                        note.setText("Security bits write failed");
	                                                else
	                                                        note.setText(str);
	                                        }
	                                }
	                                else
	                                        note.setText("Error");
	                        }
	                }
	        }
	        else
	**/
	{
		int rval;
		int retry_flag = 1;

		ClearIgnoreFlag();

		while (retry_flag)
		{
			result = OK;

			retry_flag = 0;

			doProgress(translate(STR_MSGWRITINGSEC));

			rval = awip->SecurityWrite(0, true);
			e2Prg->reset();

			if (rval == OK)
			{
				//if (verbose == verboseAll)
				//	note.setText("Security bits write successful");

			} // if ( (rval = ...
			else if (rval == NOTSUPPORTED)
			{
			}
			else
			{
				result = rval;

				if (verbose != verboseNo)
				{
					rval = OnError(rval, translate(STR_MSGWRITESECFAIL));

					if (rval == QMessageBox::Abort)   //Abort
					{
						retry_flag = 0;
						ClearIgnoreFlag();
					}

					if (rval == QMessageBox::Retry)   //Retry
					{
						retry_flag = 1;
						ClearIgnoreFlag();
					}
					else if (rval == QMessageBox::Ignore)     //Ignore
					{
						retry_flag = 1;
						SetIgnoreFlag();
					}
				} // if (verbose != verboseNo)
			} // else
		} // while (retry_flag)

		ClearIgnoreFlag();
	}

	return result;
}

int e2CmdWindow::CmdReadLock()
{
	int result = OK;
	quint32 bits;

	ClearIgnoreFlag();

	int rval;
	int retry_flag = 1;

	//      quint32 type = BuildE2PType( awip->GetEEPPriType(), awip->GetEEPSubType() );

	while (retry_flag)
	{
		result = OK;

		retry_flag = 0;

		doProgress(translate(STR_MSGREADINGSEC));

		rval = awip->SecurityRead(bits);
		e2Prg->reset();

		if (rval == OK)
		{
			// EK 2017
			// TODO
			Draw();
			UpdateStatusBar();

			// orig deactivated     SpecialBits();
		} // if ( (rval = ...
		else if (rval == NOTSUPPORTED)
		{
		}
		else
		{
			result = rval;

			if (verbose != verboseNo)
			{
				rval = OnError(rval, translate(STR_MSGREADSECFAIL));

				if (rval == QMessageBox::Abort)   //Abort
				{
					retry_flag = 0;
					ClearIgnoreFlag();
				}

				if (rval == QMessageBox::Retry)   //Retry
				{
					retry_flag = 1;
					ClearIgnoreFlag();
				}
				else if (rval == QMessageBox::Ignore)     //Ignore
				{
					retry_flag = 1;
					SetIgnoreFlag();
				}
			}
		} // else
	} // while (retry_flag)

	ClearIgnoreFlag();

	return result;
}

int e2CmdWindow::CmdReadSpecial()
{
	int result = OK;
	quint32 bits;

	ClearIgnoreFlag();

	int rval;
	int retry_flag = 1;

	quint32 type = awip->GetId();   // BuildE2PType( awip->GetEEPPriType(), awip->GetEEPSubType() );

	while (retry_flag)
	{
		result = OK;
		retry_flag = 0;

		doProgress(translate(STR_MSGREADINGFUSE));

		if (type == E2464)
		{
			rval = awip->HighEnduranceRead(bits);
		}
		else
		{
			rval = awip->FusesRead(bits);
		}
		e2Prg->reset();

		if (rval == OK)
		{
			// orig deactivated     SpecialBits(1);
		} // if ( (rval = ...
		else if (rval == NOTSUPPORTED)
		{
		}
		else
		{
			result = rval;

			if (verbose != verboseNo)
			{
				rval = OnError(rval, translate(STR_MSGREADFUSEFAIL));

				if (rval == QMessageBox::Abort)   //Abort
				{
					retry_flag = 0;
					ClearIgnoreFlag();
				}

				if (rval == QMessageBox::Retry)   //Retry
				{
					retry_flag = 1;
					ClearIgnoreFlag();
				}
				else if (rval == QMessageBox::Ignore)     //Ignore
				{
					retry_flag = 1;
					SetIgnoreFlag();
				}
			}
		} // else
	} // while (retry_flag)

	ClearIgnoreFlag();

	return result;
}

// TODO E2464
int e2CmdWindow::CmdWriteSpecial()
{
	int result = OK;

	QMessageBox note;

	quint32 type = awip->GetId();   //BuildE2PType( awip->GetEEPPriType(), awip->GetEEPSubType() );
	/**
	if (type == E2464)              //Microchip 24C65 high endurance block
	{
	        vReplyDialog rp(this);      // instantiate
	        char r[100];                // a buffer for reply

	        rp.Reply("Enter the high endurance block ",r,99);
	        if (*r)
	        {
	                int a, err;

	                a = atoi(r);
	                if ( (err = awip->HighEnduranceWrite(a)) )
	                {
	                        if (err == IICERR_NOADDRACK)
	                                note.setText("Device Not Responding");
	                        else
	                                note.setText("High endurance block write error");
	                }
	                else
	                {
	                        int hend = -1;
	                        awip->HighEnduranceRead(hend);
	                        if (hend != a)
	                                note.setText("High enduranche block write failed");
	                        else
	                        {
	                                result = OK;
									note.setText("High endurance block write successful");
	                        }
	                }
	        }
	}
	else
	**/
	{
		int rval;
		int retry_flag = 1;
		ClearIgnoreFlag();

		while (retry_flag)
		{
			result = OK;
			retry_flag = 0;

			doProgress(translate(STR_MSGWRITINGFUSE));

			if (type == E2464)
			{
				rval = awip->HighEnduranceWrite(0, true);
			}
			else
			{
				rval = awip->FusesWrite(0, true);
			}
			e2Prg->reset();

			if (rval == OK)
			{
				//if (verbose == verboseAll)
				//	note.setText("Special write successful");
			} // if ( (rval = ...
			else if (rval == NOTSUPPORTED)
			{
			}
			else
			{
				result = rval;

				if (verbose != verboseNo)
				{
					rval = OnError(rval, translate(STR_MSGWRITEFUSEFAIL));

					if (rval == QMessageBox::Abort)   //Abort
					{
						retry_flag = 0;
						ClearIgnoreFlag();
					}

					if (rval == QMessageBox::Retry)   //Retry
					{
						retry_flag = 1;
						ClearIgnoreFlag();
					}
					else if (rval == QMessageBox::Ignore)     //Ignore
					{
						retry_flag = 1;
						SetIgnoreFlag();
					}
				}
			} // else
		} // while (retry_flag)

		ClearIgnoreFlag();
	}

	return result;
}

int e2CmdWindow::CmdByteSwap()
{
	int result = OK;

	if (!awip->IsBufferValid())
	{
		result = CMD_BUFFEREMPTY;

		if (verbose != verboseNo)
		{
			QMessageBox note(QMessageBox::Information, "Byte swap", translate(STR_BUFEMPTY), QMessageBox::Ok);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
			note.exec();
		}
	}
	else
	{
		awip->SwapBytes();
		// EK 2017
		// TODO
		Draw();
		UpdateStatusBar();
	}

	return result;
}

// EK 2017
// TODO check this
int e2CmdWindow::CmdEditNote()
{
	QString str1 = awip->GetStringID();
	QString str2 = awip->GetComment();

	notesModalDialog notes(this, str1, str2);

	if (notes.exec() == QDialog::Accepted)
	{
		UpdateStrFromStr(str1, str2);

		UpdateStatusBar();
	}

	return OK;
}

// EK 2017
// new_type is the chip id
int e2CmdWindow::CmdSelectDevice(quint32 new_type, bool init)
{
	qDebug() << Q_FUNC_INFO << "CmdSelectDevice" << Qt::hex << new_type << Qt::dec;

	awip->SetId(new_type);
	UpdateMenuType(new_type);

	first_line = 0;
	//curIndex = 0;
	Draw();

	awip->RecalcCRC();
	UpdateStatusBar();

	if (!init)
	{
		E2Profile::SetLastDevType(awip->GetTypeString(new_type));
	}

	return OK;
}

/**
void e2CmdWindow::CmdRemoteMode()
{
        VerboseType old_verbose = verbose;
        verbose = verboseNo;    //No messages when running in remote mode

						e2Prg->setLabelText(translate(STR_MSGACCEPTCMD));
                        e2Prg->setValue(0);

        SocketServer srv(0, this);

        if ( srv.UDPMainLoop() != OK)
                //e2Prg->close();
            emit onEndProgress();

        verbose = old_verbose;
}
**/

int e2CmdWindow::CmdSetSerialNumber()
{
	long loc = 0;
	int size = 1;
	quint32 val = 0;
	bool memtype = false;

	val = E2Profile::GetSerialNumVal();
	E2Profile::GetSerialNumAddress(loc, size, memtype);

	if (memtype)
	{
		loc += awip->GetSplittedInfo();
	}

	if ((size > 0 && size <= 4) && (loc + size <= awip->GetBufSize()))
	{
		quint8 *bp = awip->GetBufPtr() + loc;

		if (E2Profile::GetSerialNumFormat() == FMT_BIG_ENDIAN)
		{
			if (size > 3)
			{
				*bp++ = (quint8)((val >> 24) & 0xff);
			}

			if (size > 2)
			{
				*bp++ = (quint8)((val >> 16) & 0xff);
			}

			if (size > 1)
			{
				*bp++ = (quint8)((val >> 8) & 0xff);
			}

			if (size > 0)
			{
				*bp++ = (quint8)(val & 0xff);
			}
		}
		else
		{
			if (size > 0)
			{
				*bp++ = (quint8)(val & 0xff);
			}

			if (size > 1)
			{
				*bp++ = (quint8)((val >> 8) & 0xff);
			}

			if (size > 2)
			{
				*bp++ = (quint8)((val >> 16) & 0xff);
			}

			if (size > 3)
			{
				*bp++ = (quint8)((val >> 24) & 0xff);
			}
		}

		awip->RecalcCRC();
		//awip->BufChanged();

		if (E2Profile::GetSerialNumAutoInc())
		{
			E2Profile::SetSerialNumVal(++val);
		}

		Draw();
		UpdateStatusBar();
	}

	return OK;
}

long e2CmdWindow::GetDevSize() const
{
	return awip->GetSize() == AUTOSIZE_ID ? 0 : awip->GetSize();
}

int e2CmdWindow::IsBufChanged() const
{
	return awip->IsBufChanged();
}

void e2CmdWindow::UpdateStatusBar()
{
	//int is_splitted = awip->GetSplittedInfo() > 0 && awip->GetSplittedInfo() != awip->GetSize();

	if (lblStringID == NULL || lblEEPInfo == NULL)
	{
		return;
	}

	lblStringID->setText(" ");

	if (awip)
	{
		QString buf;
#if USE_QT_VERSION == 4
		buf.sprintf(STATUSBAR_PRINT, GetDevSize(), awip->GetCRC(), awip->IsBufChanged() ? '*' : ' ');
#else
		buf.asprintf(STATUSBAR_PRINT, GetDevSize(), awip->GetCRC(), awip->IsBufChanged() ? '*' : ' ');
#endif
		lblEEPInfo->setText(buf);
		lblStringID->setText(awip->GetStringID());
	}

}


/**
 * @brief add in the status bar of three widgets
 *
 */
void e2CmdWindow::createStatusWidgets()
{
	//
	lblEEPInfo = new QLabel();
	lblEEPInfo->setFixedWidth(300);
	//      lblEEPInfo->setFixedHeight(17);
	statusbar->addWidget(lblEEPInfo);

	QFrame *verticalLine    =  new QFrame();
	verticalLine->setFrameStyle(QFrame::VLine);
	statusbar->addWidget(verticalLine);

	//         verticalLine->setSizePolicy(QSizePolicy.Minimum,QSizePolicy.Expanding)
	// EK 2017
	// if we need the progress bar in state...
	//      statusProgress = new QProgressBar();
	//      statusProgress->setFixedWidth(100);
	//      statusProgress->setFixedHeight(17);
	//      statusbar->addWidget(statusProgress);

	lblStringID = new QLabel();
	lblStringID->setFixedWidth(300);
	//      lblStringID->setFixedHeight(17);
	statusbar->addWidget(lblStringID);
}


void e2CmdWindow::createToolBarNotice()
{
	txtID = new QLabel();
	txtID->setFixedWidth(90);
	txtID->setFixedHeight(25);
	toolBarNote->addWidget(txtID);

	txtComment = new QLabel();
	txtComment->setFixedHeight(25);
	txtComment->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	txtComment->setStyleSheet("border: 1px solid grey");
	toolBarNote->addWidget(txtComment);
}


void e2CmdWindow::createToolBarCbx()
{
	cbxEEPType = new QComboBox();
	cbxEEPType->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	toolBarDevices->addWidget(cbxEEPType);

	cbxEEPSubType = new QComboBox();
	cbxEEPSubType->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	toolBarDevices->addWidget(cbxEEPSubType);
}


/**
 * @brief slot from comboobox pritype
 *
 */
void e2CmdWindow::onDevType(int t)
{
	// search id
	long new_id = 0;

	if (deviceMenu[t].vChip.count())
	{
		new_id = deviceMenu[t].vChip.at(0).id;
		if (currentAct != NULL)
		{
			currentAct->setChecked(false);
		}
	}

	CmdSelectDevice(new_id);
}


/**
 * @brief slot from comboobox subtype
 *
 */
void e2CmdWindow::onDevSubType(int st)
{
	// search id
	int t = cbxEEPType->currentIndex();
	QString nm = cbxEEPSubType->currentText();
	nm = nm.remove("&");
	long new_id = 0;

	for (int i = 0; i < deviceMenu[t].vChip.count(); i++)
	{
		if (deviceMenu[t].vChip.at(i).name == nm)
		{
			new_id = deviceMenu[t].vChip.at(i).id;
		}
	}

	CmdSelectDevice(new_id);
}


void e2CmdWindow::UpdateMenues(menuToGroup &mnu, QAction &act)
{
	qDebug() << Q_FUNC_INFO << "UpdateMenues" << mnu.title << act.text();
	if (currentMenu != &mnu || currentAct != &act) // main menu was changed
	{
		Q_CHECK_PTR(cbxEEPType);
		Q_CHECK_PTR(cbxEEPSubType);

		if (currentAct != NULL)
		{
			currentAct->setChecked(false);
		}

		selectTypeSubtype(mnu.title, act.text());

		act.setChecked(true);

		currentAct = &act;
	}
}


void e2CmdWindow::UpdateMenuType(quint32 new_type)
{
	if (new_type == 0)
	{
		new_type = awip->GetId();
	}

	quint32 new_pritype = awip->GetPriType(new_type);

	qDebug() << Q_FUNC_INFO << " (Qt::hex) type:" << (Qt::hex) << new_type << " prim:" << new_pritype << (Qt::dec);

	menuToGroup *newMenu = NULL;
	QAction *newAct = NULL;

	foreach (menuToGroup m, deviceMenu)
	{
		if (m.vId.indexOf(new_pritype) == -1)
		{
			continue;
		}

		for (int j = 0; j < m.vChip.count(); j++)
		{
			if (m.vChip.at(j).id == new_type)
			{
				newMenu = &m;

				newAct = m.grp->actions().at(j);

				break;
			}
		}

		if (newAct != NULL)
		{
			break;
		}
	}

	UpdateMenues(*newMenu, *newAct);

	// EK 2017
	// it's not possible to hide the actions, or?
	if (new_pritype == PIC125XX)        //Not Erasable
	{
		actionErase->setDisabled(true);
	}
	else
	{
		actionErase->setEnabled(true);
	}

	if (awip && (awip->GetSplittedInfo() > 0) && (awip->GetSize() > awip->GetSplittedInfo()))
	{
		//Enable menus
		actionReadFlash->setEnabled(true);
		actionReadEep->setEnabled(true);
		actionWriteFlash->setEnabled(true);
		actionWriteEep->setEnabled(true);
		actionVerifyFlash->setEnabled(true);
		actionVerifyEep->setEnabled(true);

		actionOpenFlash->setEnabled(true);
		actionOpenEep->setEnabled(true);
		actionSaveFlashAs->setEnabled(true);
		actionSaveEepAs->setEnabled(true);
	}
	else
	{
		//Disable menus
		actionReadFlash->setEnabled(false);
		actionReadEep->setEnabled(false);
		actionWriteFlash->setEnabled(false);
		actionWriteEep->setEnabled(false);
		actionVerifyFlash->setEnabled(false);
		actionVerifyEep->setEnabled(false);

		actionOpenFlash->setEnabled(false);
		actionOpenEep->setEnabled(false);
		actionSaveFlashAs->setEnabled(false);
		actionSaveEepAs->setEnabled(false);
	}
}


/**
 * @brief search the name in vectors
 *
 */
quint32 e2CmdWindow::GetEEPTypeFromMenu(const QString &name)
{
	QString n = name;
	n = n.remove("&");
	foreach (menuToGroup m, deviceMenu)
	{
		foreach (chipMenuInfo c, m.vChip)
		{
			if (QString::compare(c.name, n, Qt::CaseInsensitive) == 0)
			{
				return c.id;
			}
		}
	}

	return EID_INVALID;
}


// EK 2017
// TODO for toolBar3: combo boxes
void e2CmdWindow::CbxMenuInit()
{
	// create widgets in tool bar 3
	createToolBarCbx();

	createToolBarNotice();

	QStringList lst;

	//generic
	foreach (menuToGroup m, deviceMenu)
	{
		lst << m.title;
	}
// 	for (int i = 0; i < deviceMenu.count(); i++)
// 	{
// 		lst << deviceMenu.at(i).mnu->title();
// 	}

	if (lst.count())
	{
		cbxEEPType->addItems(lst);
		cbxEEPType->setCurrentIndex(0);

		currentMenu = (menuToGroup *)&deviceMenu.at(0);
	}

	//UpdateMenuType();

	//Clear the check in menu Edit buffer
	actionEditBuferEnabled->setChecked(E2Profile::GetEditBufferEnabled());
	//      EditMenu[2].checked = 0;
}

void e2CmdWindow::UpdateStrFromBuf()
{
	txtID->setText(awip->GetStringID());
	txtComment->setText(awip->GetComment());
}

void e2CmdWindow::UpdateStrFromStr(const QString &s1)
{
	txtID->setText(s1);
	awip->SetStringID(s1);
}

void e2CmdWindow::UpdateStrFromStr(const QString &s1, const QString &s2)
{
	txtID->setText(s1);
	awip->SetStringID(s1);

	txtComment->setText(s2);
	awip->SetComment(s2);
}


static QStringList script_filter = QStringList({ "Script Files (*.e2s)", "All Files (*)" });
//static int script_filterIndex = 0;

int e2CmdWindow::OpenScript(const QString &file)
{
	QString fileName;
	int rval = 1;

	if (file.length() == 0)
	{
		if (verbose == verboseNo)
		{
			rval = BADPARAM;
		}
		else
		{
			QString fltr = convertFilterListToString(script_filter);
			QString open_path("");
			QStringList l = E2Profile::GetLastScripts();
			if (l.count() > 0)
			{
				open_path = QFileInfo(l.at(0)).canonicalPath();
			}
			if (open_path.length() == 0)
			{
				open_path = QDir::homePath();
			}

			fileName = QFileDialog::getOpenFileName(this, translate(STR_MSGOPENSCRIPT), open_path, fltr);
		}
	}

	if (fileName.length() > 0)
	{
		if (QFile::exists(fileName))
		{
			QString oldname = script_name;

			script_name = fileName;

			rval = CmdRunScript(true);

			if (rval != OK)
			{
				script_name = oldname;
			}
		}
		else
		{
			if (verbose != verboseNo)
			{
				QMessageBox note(QMessageBox::Warning, "Open script", translate(STR_MSGFILENOTFOUND), QMessageBox::Yes);
				note.setStyleSheet(programStyleSheet);
				note.setButtonText(QMessageBox::Yes, translate(STR_YES));
				note.exec();
			}

			rval = FILENOTFOUND;
		}
	}
	else
	{
		rval = OP_ABORTED;
	}

	return rval;
}


static QStringList filter = QStringList({ "*.e2p", "*.hex", "*.mot", "*.bin", "*.csm", "*" });
//                                         ^^^^^    ^^^^^    ^^^^^    ^^^^^    ^^^^^
//                                       form E2P  form Intel  form S-rec, form Binary (Raw), CSM,    gli altri non aggiungono ulteriori formati
static QStringList filterInfo = QStringList({ "E2P files (*.e2p)", "Intel hex files (*.hex)", "S-rec mot files (*.mot)", "Binary files (*.bin)", "CSM (*.csm)",  "No filter (*)" });

static int filterIndex = 0;

static void AddExtension(QString &name)
{
	int p = name.lastIndexOf('.');  //look for extension

	if (p < 0 || (name.mid(p) != filter.at(filterIndex).mid(1)))
	{
		//if not found append extension
		name += filter.at(filterIndex).mid(1);
	}
}

static bool CmpExtension(const QString &name, const QString &ext)
{
	if ((name.length() > 0) && (ext.length() > 0))
	{
		int pos = name.lastIndexOf('.');    //look for extension

		if (pos > 0)
		{
			if (ext == name.mid(pos))
			{
				return true;
			}
		}
	}

	return false;
}

int e2CmdWindow::OpenFile(const QString &file)
{
	QString fileName;
	int rval = 1;

	if (file.length() == 0)
	{
		if (verbose == verboseNo)
		{
			rval = BADPARAM;
		}
		else
		{
			filterIndex = (int)awip->GetFileBuf(); // ???
			QString open_path = QDir::homePath();			//TODO: load from settings
			QStringList l = E2Profile::GetLastFiles();
			if (l.count() > 0)
			{
				QString fname = l.at(0);
				int pos_and = l.at(0).lastIndexOf("?");
				if (pos_and > 0)
				{
					fname = l.at(0).left(pos_and);
				}

				open_path = QFileInfo(fname).canonicalPath();	//Use open_path from last opened file
			}

			QString fltr = convertFilterListToString(filterInfo);

			if (awip->GetLoadType() == PROG_TYPE)
			{
				fileName = QFileDialog::getOpenFileName(this, translate(STR_MSGOPENPROGFILE), open_path, fltr);
			}
			else if (awip->GetLoadType() == DATA_TYPE)
			{
				fileName = QFileDialog::getOpenFileName(this, translate(STR_MSGOPENDATAFILE), open_path, fltr);
			}
			else
			{
				fileName = QFileDialog::getOpenFileName(this, translate(STR_MSGOPENFILE), open_path, fltr);
			}

			E2Profile::SetDefaultFileType((FileType)filterIndex);
		}
	}
	else
	{
		fileName = file;
	}

	if (fileName.length() > 0)
	{
		if (QFile::exists(fileName))
		{
			QString oldfname = awip->GetFileName();
			awip->SetFileName(fileName);
			long old_type = awip->GetId();			//EEP type can be changed by E2P file load

			rval = awip->Load();

			if (rval <= 0)
			{
				awip->SetFileName(oldfname);

				//If we go here there is a serious problem!! If file exist awip->Load() should succeded
				if (verbose != verboseNo)
				{
					QMessageBox note(QMessageBox::Critical, "Open file", "Unable to load the file", QMessageBox::Close);
					note.setStyleSheet(programStyleSheet);
					note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
					note.exec();
				}
			}
			else
			{
				SetTitle();
				UpdateStrFromBuf();

				//UpdateChipType();
				if (awip->GetId() != old_type)
				{
					UpdateMenuType(awip->GetId());
				}

				first_line = 0;
				//curIndex = 0;
				Draw();
				UpdateStatusBar();

				rval = OK;
			}
		}
		else
		{
			if (verbose != verboseNo)
			{
				QMessageBox note(QMessageBox::Critical, "Open file", translate(STR_MSGFILENOTFOUND), QMessageBox::Close);
				note.setStyleSheet(programStyleSheet);
				note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
				note.exec();
			}

			rval = FILENOTFOUND;
		}
	}

	return rval;
}

int e2CmdWindow::filterNameToIndex(const QString &s, const QStringList &lst)
{
	for (int i = 0; i < lst.count(); i++)
	{
		if (lst.at(i) == s)
		{
			return i;
		}
	}

	return -1;
}

QString e2CmdWindow::convertFilterListToString(const QStringList &lst)
{
	QString fltr;

	for (int i = 0; i < lst.count(); i++)
	{
		if (i < lst.count() - 1)
		{
			fltr += QString(lst.at(i) + ";;");
		}
		else // the last element
		{
			fltr += lst.at(i);
		}
	}

	return fltr;
}


int e2CmdWindow::SaveFile(int force_select)
{
	int err = 0;

	if (!force_select && awip->GetFileName().length() > 0)
	{
		// Name validated
		//awip->SetNoOfBlock(awip->GetNoOfBank());
		if ((err = awip->Save()) <= 0 && verbose != verboseNo)
		{
			QString str;
			str = translate(STR_MSGFILESAVEFAIL) + QString(" (%d)\n").arg(err);
			QMessageBox note(QMessageBox::Critical, "Save", str, QMessageBox::Close);
			note.setStyleSheet(programStyleSheet);
			note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
			note.exec();
		}
	}
	else if (verbose == verboseNo)
	{
		err = BADPARAM;
	}
	else
	{
		// Invalid name: select and save the file
		QString s;

		if (awip->GetSaveType() == PROG_TYPE)
		{
			s = translate(STR_MSGFILESAVEPROG);
		}
		else if (awip->GetSaveType() == DATA_TYPE)
		{
			s = translate(STR_MSGFILESAVEDATA);
		}
		else
		{
			s = translate(STR_MSGFILESAVEAS);
		}

		filterIndex = (int)awip->GetFileBuf(); // ???

		QString fltr = convertFilterListToString(filterInfo);
		QString sFilter = filterInfo.at(filterIndex);
		//fn = awip->GetFileName();	//should be the filename proposed by the dialog by default
		QString fn = QFileDialog::getSaveFileName(this, s, QDir::homePath(), fltr, &sFilter);

		if (fn.length())
		{
			//Save the old name in case some error occurs, so it can restore it
			QString oldname = awip->GetFileName();
			int fidx = filterNameToIndex(sFilter, filterInfo);
			if (fidx > -1)
			{
				filterIndex = fidx;
				awip->SetFileBuf((enum FileType)filterIndex);   //????? Ci vorrebbe un controllo separato dall'estensione sul tipo di file (combobox)
			}
			E2Profile::SetDefaultFileType(awip->GetFileBuf());
			AddExtension(fn);
			awip->SetFileName(fn);

			if ((err = awip->Save()) <= 0)
			{
				QString str;

				str = QString("%1 %2 (%3)\n").arg(translate(STR_MSGFILESAVEFAIL)).arg(fn).arg(err);

				QMessageBox note(QMessageBox::Information, "Save", str, QMessageBox::Close);
				note.setStyleSheet(programStyleSheet);
				note.setButtonText(QMessageBox::Close, translate(STR_CLOSE));
				note.exec();

				//restore previous name
				awip->SetFileName(oldname);
			}
			else
			{
				//Tutto OK, imposta il nuovo titolo
				setWindowTitle(fn);
			}
		}
	}

	//If the Save is ok
	if (err > 0)
	{
		awip->BufChanged(false);
		UpdateStatusBar();

		err = OK;
	}

	return err;
}


void e2CmdWindow::SetTitle()
{
	QString str = QString(APP_NAME " - ") + GetFileName();
	setWindowTitle(str);
}


QString e2CmdWindow::GetFileName()
{
	if (awip->GetFileName().length() > 0)
	{
		return awip->GetFileName();
	}
	else
	{
		return translate(STR_NONAME);
	}
}

void e2CmdWindow::UpdateScriptMenu()
{
	if (!scriptMode)
	{
		disconnect(scrListgrp, SIGNAL(triggered(QAction *)), this, SLOT(onSelectScript(QAction *)));

		scrptsMenu->clear();
		// removeAllActions()
		while (scrListgrp->actions().count())
		{
			scrListgrp->removeAction(scrListgrp->actions().first());
		}

		QStringList sl = E2Profile::GetLastScripts();

		foreach (QString entry, sl)
		{
			if (entry.length() > 0 && QFile().exists(entry))
			{
				QString fname = entry;
				fname.replace(QDir().homePath(), "~");
				QAction *tmpAction = new QAction(fname, actionScriptList);

				scrptsMenu->addAction(tmpAction);
				scrListgrp->addAction(tmpAction);

				tmpAction->setIconText(fname);
			}
		}

		connect(scrListgrp, SIGNAL(triggered(QAction *)), this, SLOT(onSelectScript(QAction *)));

#if 0
		if (sp.length() > 0)
		{
			script_name = sp;

			QString str;
			str = translate(STR_RUNSCR) + " ";

			int flen = script_name.length();

			str += script_name;

			actionRunScript->setText(str);
			actionRunScript->setEnabled(true);
		}
#endif
	}
}

void e2CmdWindow::UpdateFileMenu()
{
	//Remove any previous menu and create it again with updated list
	disconnect(fileListgrp, SIGNAL(triggered(QAction *)), this, SLOT(onSelectFile(QAction *)));

	filesMenu->clear();
	// removeAllActions()
	while (fileListgrp->actions().count())
	{
		fileListgrp->removeAction(fileListgrp->actions().first());
	}

	QStringList lflist = E2Profile::GetLastFiles();

	foreach (QString entry, lflist)
	{
		if (entry.length() > 0)
		{
			int pos_and = entry.lastIndexOf("?");
			QString fname;
			QString stype;
			if (pos_and > 0)
			{
				fname = entry.left(pos_and);
				stype = entry.mid(pos_and + 1);
			}
			else
			{
				fname = entry;
				stype = "";
			}

			if (QFile().exists(fname))
			{
				QString e = fname.replace(QDir().homePath(), "~");
				//e += "[" + stype + "]";
				QAction *tmpAction = new QAction(e, actionFileList);

				filesMenu->addAction(tmpAction);
				fileListgrp->addAction(tmpAction);

				tmpAction->setIconText(e);	//make sure icontext is equal to text

				if (stype == "PROG")
				{
					tmpAction->setIcon(actionOpenFlash->icon());
				}
				else if (stype == "DATA")
				{
					tmpAction->setIcon(actionOpenEep->icon());
				}
				else
				{
					tmpAction->setIcon(actionOpen->icon());
				}
			}
		}
	}

	connect(fileListgrp, SIGNAL(triggered(QAction *)), this, SLOT(onSelectFile(QAction *)));

#if 0
	int data_type;
	sp = E2Profile::GetLastFile(data_type);

	if (sp.length() > 0)
	{
		QString str;
		int p = sp.lastIndexOf('\\');

		if (p >= 0)
		{
			str = sp.mid(p + 1, 6);
		}
		else
		{
			str = sp; // TODO to check this
		}

		if (data_type == DATA_TYPE)
		{
			str += " DATA";
		}
		else if (data_type == PROG_TYPE)
		{
			str += " PROG";
		}

// 		actionFile1->setEnabled(true);
// 		actionFile1->setText(str);
		lastFiles.insert(0, str);
	}
// 	else
// 	{
// 		actionFile1->setText("");
// 		actionFile1->setEnabled(false);
// 	}

	sp = E2Profile::GetPrevFile(data_type);

	if (sp.length())
	{
		QString str;
		int p = sp.lastIndexOf('\\');

		if (p >= 0)
		{
			str = sp.mid(p + 1, 6);
		}
		else
		{
			str = sp; // TODO to check this
		}

		if (data_type == DATA_TYPE)
		{
			str += " DATA";
		}
		else if (data_type == PROG_TYPE)
		{
			str += " PROG";
		}

		actionFile2->setText(str);
		actionFile2->setEnabled(true);
	}
	else
	{
		actionFile2->setText("");
		actionFile2->setEnabled(false);
	}
#endif
}

//Update the buffer with edit changes
void e2CmdWindow::UpdateBuffer()
{
	bool flag1 = false, flag2 = false;

	if (e2HexEdit->isModified())
	{
		const char *ptr = e2HexEdit->data().constData();
		memcpy(awip->GetBufPtr(), ptr, e2HexEdit->data().length());
		//memcpy(awip->GetBufPtr(), ptr, awip->GetSize());
		flag1 = true;
	}
	if (e2HexEditSplit->isModified())
	{
		Q_ASSERT(awip->GetSplittedInfo() > 0);

		const char *ptr = e2HexEditSplit->data().constData();
		long ofst = awip->GetSplittedInfo();
		memcpy(awip->GetBufPtr() + ofst, ptr, e2HexEditSplit->data().length());
		flag1 = true;
	}

	if (flag1 || flag2)
	{
		awip->BufChanged();
		Draw();
	}
}

void e2CmdWindow::Draw()
{
	if (awip == 0 || !awip->IsBufferValid())
	{
		return;        // ** Vlib 1.22 call Draw before the AppWinInfo is created ** 28/08/99
	}

	if (awip->GetSplittedInfo() > 0)
	{
		e2HexEdit->setHexFontColor(Qt::darkGreen);
		qbuf->setData(reinterpret_cast<char *>(awip->GetBufPtr()), awip->GetSplittedInfo());
		e2HexEdit->setData(*qbuf);

		qDebug() << Q_FUNC_INFO << "Datalen: " << e2HexEdit->data().length() << " Splitted: " << awip->GetSplittedInfo() << " Size: " << awip->GetSize();

		char *ptr = reinterpret_cast<char *>(awip->GetBufPtr());
		long ofst = awip->GetSplittedInfo();
		long len = awip->GetSize() - ofst;
		ptr += ofst;

		qbufSplit->setData(ptr, len);
		e2HexEditSplit->setData(*qbufSplit);
		e2HexEditSplit->show();

		qDebug() << Q_FUNC_INFO << "Datalen: " << e2HexEditSplit->data().length() << " Offset: " << ofst << " Len: " << len;
	}
	else
	{
		e2HexEdit->setHexFontColor(Qt::darkBlue);
		qbuf->setData(reinterpret_cast<char *>(awip->GetBufPtr()), awip->GetSize());
		e2HexEdit->setData(*qbuf);
		e2HexEditSplit->hide();
	}
#if 0
	int no_line;
	int new_top, new_shown;
	int k;

	long size = awip->GetSize();// ? awip->GetSize() : awip->GetBufSize();
	long splitted_size = awip->GetSplittedInfo() / awip->GetHexPerLine();

	no_line = (size + awip->GetHexPerLine() - 1) / awip->GetHexPerLine();


	if (!rows)
	{
		rows = e2Canvas->GetRows();
	}

	rows--;                         // to avoid the scrolling

	//Caso in cui non c'e` file o tutto il file sta nella finestra
	if (no_line == 0 || rows >= no_line)
	{
		first_line = 0;
		e2Canvas->ShowVScroll(0);       // Hide Vert Scroll
	}
	else
	{
		//Caso in cui si ingrandisce una finestra che contiene un file
		// visualizzato in fondo
		if (rows > no_line - first_line)
		{
			first_line = no_line - rows;

			if (first_line < 0)
			{
				first_line = 0;
			}
		}

		// rows : no_line = shown : 100
		new_shown = rows * 100 / no_line;

		// first_line : no_line-rows = top : 100
		new_top = first_line * 100 / (no_line - rows);

		e2Canvas->SetVScroll(new_shown, new_top);
		e2Canvas->ShowVScroll(1);       // Show Vert Scroll
	}

	//      e2Canvas->Clear();
	e2Canvas->GotoRC(0, 0);

	for (k = first_line; k < first_line + rows && k < no_line; k++)
	{
#ifdef  _NO_COLOR
		e2Canvas->DrawText(awip->Dump(k));
#else
		e2Canvas->DrawAttrText(awip->Dump(k, 1), ChRed + ChDimColor);

		if (k >= splitted_size)
		{
			e2Canvas->DrawAttrText(awip->Dump(k, 2), ChBlue + ChDimColor);
		}
		else
		{
			e2Canvas->DrawAttrText(awip->Dump(k, 2), ChGreen + ChDimColor);
		}

		e2Canvas->DrawAttrText(awip->Dump(k, 3), ChMagenta + ChDimColor);
#endif
	}

	e2Canvas->ClearToEnd(k - first_line, 0);

	int r = -1, c = -1;

	if (editbuf_enabled)
	{
		IndexToCoord(curIndex, r, c);
	}

	if (r > -1 && c > -1)
	{
		e2Canvas->GotoRC(r, c);
		e2Canvas->ShowTextCursor();
	}
	else
	{
		e2Canvas->HideTextCursor();
	}

#endif
}


void e2CmdWindow::PostInit()
{
	// TODO use QFont
	//      QFont font90(vfDefaultFixed);
	//      e2HexEdit->SetFont(font90);

	UpdateStrFromBuf();
	SetTitle();
	UpdateFileMenu();
	UpdateScriptMenu();

	quint32 tp = GetEEPTypeFromMenu(E2Profile::GetLastDevType());

	qDebug() << Q_FUNC_INFO << "PostInit" << (Qt::hex) << tp << (Qt::dec);

	CmdSelectDevice(tp, true);

	if (E2Profile::GetBogoMips() == 0)
	{
		QMessageBox note(QMessageBox::Information, "Calibration", translate(STR_MSGNEEDCALIB), QMessageBox::Ok);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
		note.exec();
	}

	if (E2Profile::GetPortNumber() < 0)
	{
		QMessageBox note(QMessageBox::Information, "Setup", translate(STR_MSGNEEDSETUP), QMessageBox::Ok);
		note.setStyleSheet(programStyleSheet);
		note.setButtonText(QMessageBox::Ok, translate(STR_CLOSE));
		note.exec();
	}
}

// EK 2017
// TODO to check this
// TODO QPainter convert to QTextDocument ??
void e2CmdWindow::Print()
{
	int no_line = (awip->GetSize() + awip->GetHexPerLine() - 1) / awip->GetHexPerLine();
// 	int k;
	//      int a, b, cHeight;
	int curRow = 0, curPage = 0;

	QPrinter printer(QPrinter::PrinterResolution);

	QPrintDialog printDialog(&printer, this);

	E2Profile::GetPrinterSettings(printer);

	if (printDialog.exec() == QDialog::Accepted)
	{
		E2Profile::SetPrinterSettings(printer);

		QString str;

		QString title = QString(APP_NAME " by " APP_AUTHOR " ") + translate(STR_MSGPAGE) + " " + QString::number(++curPage);

		int k = 0;

		while (k < no_line)
		{
			curRow = 0;
			QStringList t;
			t << QString("File: " + GetFileName());
			t << QString("Device: " + awip->GetStringID());
			t << QString("Note: " + awip->GetComment());
#if USE_QT_VERSION == 4
			t << QString().sprintf("Size  : %ld Bytes    CRC: %04X", GetDevSize(), awip->GetCRC());
#else
			t << QString().asprintf("Size  : %ld Bytes    CRC: %04X", GetDevSize(), awip->GetCRC());
#endif

			for (; k < no_line && curRow < 66; k++)
			{
				t << awip->Dump(k);
			}
			curRow++;
			curPage++;

			QString body = t.join("<br>");

			str = "<table width=\"100%\" border=1 cellspacing=0><br>"
				  "<tr><td bgcolor=\"lightgray\"><font size=\"+1\">"
				  "<b><i>" + title + "</i></b></font><br><tr><td>"
				  + body + "<br></table></font><br><br><br>";

			QTextDocument doc;

			doc.documentLayout()->setPaintDevice(&printer);

			// TODO ??
// 			doc.setPageSize(printer.pageRect().size()); // This is necessary if you want to hide the page number
			doc.setHtml(str);
			doc.print(&printer);
		}
	}
}

bool e2CmdWindow::GetAbortFlag()
{
	qApp->processEvents();

	if (e2Prg && e2Prg->wasCanceled())
	{
		abortFlag = true;
	}

	bool a = abortFlag;
	abortFlag = false;
	return a;
}

bool e2CmdWindow::GetIgnoreFlag()
{
	return ignoreFlag;
}

void e2CmdWindow::SetIgnoreFlag()
{
	ignoreFlag = true;
}

void e2CmdWindow::ClearIgnoreFlag()
{
	ignoreFlag = false;
}

//Update the progress bar
void e2CmdWindow::SetProgress(int progress)
{
	// EK 2017
	// TODO
	//SendWindowCommandAll(pbrProgress, progress, C_Button);  // The horizontal bar
	if (e2Prg)
	{
		e2Prg->setValue(progress);
	}
}

//void e2CmdWindow::DropFile(const char *fn)
//{
//      if ( fn && strlen(fn) )
//      {
//              NewAppWin(0, (char*)fn, DefaultWidth(), DefaultHeight(), 0);
//      }
//}

void e2CmdWindow::SetAppBusy()
{
	qDebug() << Q_FUNC_INFO;
	app_status = AppBusy;
	// EK 2017
	// TODO
	//SendWindowCommandAll(idEnableToolBar, 0, C_Button);
}

void e2CmdWindow::SetAppReady()
{
	qDebug() << Q_FUNC_INFO;
	app_status = AppReady;
	// EK 2017
	// TODO
	//SendWindowCommandAll(idEnableToolBar, 1, C_Button);
}

void e2CmdWindow::Exit()
{
	qDebug() << Q_FUNC_INFO;

	if (!IsAppReady())
	{
		SetAbortFlag();
		//SendWindowCommandAll(idCloseAllDialog, 0, C_Button);
		//CheckEvents();
		SetAppReady();
	}

	if (IsAppReady())
	{
		if (!scriptMode)
		{
			if (IsBufChanged())
			{
				QMessageBox msgBox(QMessageBox::Warning, "PonyProg", translate(STR_MSGCLOSEWINSAVE),
								   QMessageBox::Yes | QMessageBox::No);
				msgBox.setStyleSheet(programStyleSheet);

				msgBox.setButtonText(QMessageBox::Yes, translate(STR_YES));
				msgBox.setButtonText(QMessageBox::No, translate(STR_NO));
				int ret = msgBox.exec();

				if (ret == QMessageBox::Yes)
				{
					CmdSave();
				}
			}

			E2Profile::saveSplitter(splitter);
			E2Profile::writeDialogSettings(this, false);
			E2Profile::sync();
		}
	}
}

void e2CmdWindow::on_actionPreferences_triggered()
{
	if (IsAppReady())
	{
		PrefDialog prefDlg(this, translate(STR_PREFERENCES));

		prefDlg.exec();
	}
}
