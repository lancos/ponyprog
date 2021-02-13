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

#include <QDebug>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QSettings>
#include <QtCore>

#include "version.h"
#include "e2profil.h"
#include "types.h"
#include "errcode.h"
#include "globals.h"
#include "portint.h"

#ifdef Q_OS_WIN32
QSettings *E2Profile::s = new QSettings(QSettings::IniFormat, QSettings::UserScope, "LancOS", APP_NAME);
#else
QSettings *E2Profile::s = new QSettings(APP_NAME);
#endif

int E2Profile::GetBogoMips()
{
	return s->value("BogoMipsX1000", 0).toInt();
}

void E2Profile::SetBogoMips(int value)
{
	s->setValue("BogoMipsX1000", value);
}


QString E2Profile::GetLastDevType()
{
	QString vrs = s->value("PonyProgVers", "pre").toString();

	QString sp = s->value("DeviceType", "24XX Auto").toString();

	if (vrs == "pre") // init for old configurations
	{
		sp = "24XX Auto";
	}

	if (sp.length() == 0)
	{
		sp = "24XX Auto";
		//return GetEEPTypeFromString(sp);
	}
// 	else
// 	{
// 		return E2400;        //Default device type
// 	}
	return sp;
}

void E2Profile::SetLastDevType(const QString &name)
{
	QString sp = name;
	s->setValue("PonyProgVers", APP_VERSION);

	if (sp.length())
	{
		s->setValue("DeviceType", sp.remove(QChar(' ')));
	}
}


HInterfaceType E2Profile::GetParInterfType()
{
	QString sp = s->value("InterfaceType", TypeToInterfName(SIPROG_API)).toString();

	return NameToInterfType(sp);
}

void E2Profile::SetParInterfType(HInterfaceType type)
{
	s->setValue("InterfaceType", TypeToInterfName(type));
}


int E2Profile::GetPortNumber()
{
	return s->value("PortNumber", -1).toInt();
}

void E2Profile::SetPortNumber(int port)
{
	if (port >= 0 /* && port < 4 */)
	{
		s->setValue("PortNumber", port);
	}
}


//Check if the entry is already in the list, in this case remove
//then insert the entry in the head of the list
void E2Profile::SetLastScript(const QString &name)
{
	//Save complete file path
	QString fname = QFileInfo(name).canonicalFilePath();

	if (fname.length() > 0)
	{
		QStringList l = GetLastScripts();

		//Check if already in the list
		int idx = l.indexOf(fname);
		if (idx != 0)
		{
			l.insert(0, fname);
			l.removeDuplicates();
			SetLastScripts(l);
		}
	}
}

//Check if the entry is already in the list, in this case remove
//then insert the entry in the head of the list
void E2Profile::SetLastFile(const QString &name, int data_type)
{
	//Save complete file path
	QString fname = QFileInfo(name).canonicalFilePath();

	if (fname.length() > 0)
	{
		QStringList l = GetLastFiles();
		QString item_name = fname;
		if (data_type == PROG_TYPE)
		{
			item_name += "?PROG";
		}
		else if (data_type == DATA_TYPE)
		{
			item_name += "?DATA";
		}
		else
		{
			item_name += "?ALL";
		}
		//int pos = l.indexOf(item_name);
		l.insert(0, item_name);
		l.removeDuplicates();
		SetLastFiles(l);
	}
}

QString E2Profile::GetLastFile(int &data, int index)
{
	QString sp = "";

	if (index < 0 || index >= GetLastFiles().count())
	{
		return sp;
	}

	sp = GetLastFiles().at(index);

	data = ALL_TYPE;

	if (sp.length())
	{
		int p = sp.indexOf('?');

		if (p >= 0)
		{
			if (sp.mid(p + 1) == "DATA")
			{
				data = DATA_TYPE;
			}
			else if (sp.mid(p + 1) == "PROG")
			{
				data = PROG_TYPE;
			}
			sp = sp.left(p);
		}
	}

	return sp;
}

QStringList E2Profile::GetLastFiles()
{
	QStringList res;

	for (int i = 0; i < 8; i++)
	{
		QString sp = s->value(QString().sprintf("LastFile%d", i), "").toString();
		if (sp.length() == 0)
		{
			break;
		}
		res << sp;
	}

	return res;
}

void E2Profile::SetLastFiles(const QStringList &l)
{
	int i = 0;

	foreach (QString t, l)
	{
		s->setValue(QString().sprintf("LastFile%d", i), t);
		i++;
	}
//	for (; i < 8; i++)
//	{
//		s->setValue(QString().sprintf("LastFile%d", i), "");
//	}
}

QStringList E2Profile::GetLastScripts()
{
	QStringList res;

	for (int i = 0; i < 8; i++)
	{
		QString sp = s->value(QString().sprintf("LastScript%d", i), "").toString();
		res << sp;
	}

	return res;
}

void E2Profile::SetLastScripts(const QStringList &l)
{
	int i = 0;

	foreach (QString t, l)
	{
		s->setValue(QString().sprintf("LastScript%d", i), t);
		i++;
	}
	for (; i < 8; i++)
	{
		s->setValue(QString().sprintf("LastFile%d", i), "");
	}
}


unsigned int E2Profile::GetPolarityLines()
{
	return s->value("PolarityLines", 0).toUInt();
}

void E2Profile::SetPolarityLines(unsigned int polarity)
{
	s->setValue("PolarityLines", polarity);
}


int E2Profile::GetI2CPageWrite()
{
	return s->value("I2CBusPageWrite", 16).toInt();
}

void E2Profile::SetI2CPageWrite(int page_write)
{
	if (page_write > 0)
	{
		s->setValue("I2CBusPageWrite", page_write);
	}
}


int E2Profile::GetSPIPageWrite()
{
	return s->value("BigSPIPageWrite", 16).toInt();
}

void E2Profile::SetSPIPageWrite(int page_write)
{
	if (page_write > 0)
	{
		s->setValue("BigSPIPageWrite", page_write);
	}
}


int E2Profile::GetI2CBaseAddr()
{
	QString sp = s->value("I2CBaseAddress", "0xA0").toString();
	int rval = 0xA0;                //Default base address

	if (sp.length())
	{
		bool ok;

		rval = sp.toLong(&ok, 0);

		if (!ok)
		{
			rval = 0xA0;
		}
	}

	return rval;
}

void E2Profile::SetI2CBaseAddr(int base_addr)
{
	if (base_addr >= 0x00 && base_addr < 0x100)
	{
		s->setValue("I2CBaseAddress", QString().sprintf("0x%X", base_addr));
	}
}


int E2Profile::GetSPIResetPulse()
{
	//Default: 100 msec
	return s->value("SPIResetPulse", 100).toInt();
}

void E2Profile::SetSPIResetPulse(int delay)
{
	if (delay > 0)
	{
		s->setValue("SPIResetPulse", delay);
	}
}


int E2Profile::GetSPIDelayAfterReset()
{
	//Default: 50 msec
	return s->value("SPIDelayAfterReset", 50).toInt();
}

void E2Profile::SetSPIDelayAfterReset(int delay)
{
	if (delay > 0)
	{
		s->setValue("SPIDelayAfterReset", delay);
	}
}


int E2Profile::GetAT89DelayAfterReset()
{
	//Default: 50 msec
	return s->value("AT89DelayAfterReset", 50).toInt();
}

void E2Profile::SetAT89DelayAfterReset(int delay)
{
	if (delay > 0)
	{
		s->setValue("AT89DelayAfterReset", delay);
	}
}


int E2Profile::GetAVRDelayAfterReset()
{
	//Default: 50 msec
	return s->value("AVRDelayAfterReset", 50).toInt();
}


void E2Profile::SetAVRDelayAfterReset(int delay)
{
	if (delay > 0)
	{
		s->setValue("AVRDelayAfterReset", delay);
	}
}


QString E2Profile::getSpeedName(int speed)
{
	if (speed == TURBO)
	{
		return QString("TURBO");
	}
	else if (speed == FAST)
	{
		return QString("FAST");
	}
	else if (speed == SLOW)
	{
		return QString("SLOW");
	}
	else if (speed == VERYSLOW)
	{
		return QString("VERYSLOW");
	}
	else if (speed == ULTRASLOW)
	{
		return QString("ULTRASLOW");
	}
	else	//if (speed == NORMAL)
	{
		return QString("NORMAL");
	}
}

int E2Profile::getSpeedIndex(const QString &sp)
{
	int rval = NORMAL;		//Default speed

	if (sp.length())
	{
		if (sp == "TURBO")
		{
			rval = TURBO;
		}
		else if (sp == "FAST")
		{
			rval = FAST;
		}
		else if (sp == "SLOW")
		{
			rval = SLOW;
		}
		else if (sp == "VERYSLOW")
		{
			rval = VERYSLOW;
		}
		else if (sp == "ULTRASLOW")
		{
			rval = ULTRASLOW;
		}
	}

	return rval;
}


int E2Profile::GetI2CSpeed()
{
	QString sp = s->value("BusSpeed/I2CBusSpeed", getSpeedName(NORMAL)).toString();
	return getSpeedIndex(sp);
}

void E2Profile::SetI2CSpeed(int speed)
{
	s->setValue("BusSpeed/I2CBusSpeed", getSpeedName(speed));
}


int E2Profile::GetSPISpeed()
{
	QString sp = s->value("BusSpeed/SPIBusSpeed", getSpeedName(NORMAL)).toString();
	return getSpeedIndex(sp);
}

void E2Profile::SetSPISpeed(int speed)
{
	s->setValue("BusSpeed/SPIBusSpeed", getSpeedName(speed));
}


int E2Profile::GetMicroWireSpeed()
{
	QString sp = s->value("BusSpeed/MicroWireBusSpeed", getSpeedName(NORMAL)).toString();
	return getSpeedIndex(sp);
}

void E2Profile::SetMicroWireSpeed(int speed)
{
	s->setValue("BusSpeed/MicroWireBusSpeed", getSpeedName(speed));
}


int E2Profile::GetPICSpeed()
{
	QString sp = s->value("BusSpeed/PICBusSpeed", getSpeedName(NORMAL)).toString();
	return getSpeedIndex(sp);
}

void E2Profile::SetPICSpeed(int speed)
{
	s->setValue("BusSpeed/PICBusSpeed", getSpeedName(speed));
}


int E2Profile::GetSDESpeed()
{
	QString sp = s->value("BusSpeed/SDEBusSpeed", getSpeedName(NORMAL)).toString();
	return getSpeedIndex(sp);
}

void E2Profile::SetSDESpeed(int speed)
{
	s->setValue("BusSpeed/SDEBusSpeed", getSpeedName(speed));
}


int E2Profile::GetIMBusSpeed()
{
	QString sp = s->value("BusSpeed/IMBusSpeed", getSpeedName(NORMAL)).toString();
	return getSpeedIndex(sp);
}

void E2Profile::SetIMBusSpeed(int speed)
{
	s->setValue("BusSpeed/IMBusSpeed", getSpeedName(speed));
}


void E2Profile::GetPrinterSettings(QPrinter &p)
{
	s->beginGroup("Printer");

	p.setPrinterName(s->value("PrinterName").toString());
	p.setPageSize(QPrinter::PaperSize(s->value("PageSize", QPrinter::A4).toInt()));
	p.setCollateCopies(s->value("Copies", false).toBool());
	p.setColorMode(QPrinter::ColorMode(s->value("ColorMode").toInt()));
	p.setCopyCount(s->value("CopyCount", 1).toInt());
	p.setCreator(s->value("Creator", "").toString());
	p.setDocName(s->value("DocName",  "ponyprog.pdf").toString());
	p.setDoubleSidedPrinting(s->value("DoubleSide", false).toBool());
	p.setDuplex(QPrinter::DuplexMode(s->value("Duplex").toInt()));
	p.setFontEmbeddingEnabled(s->value("FontEmb", false).toBool());
	p.setFullPage(s->value("FullPage", false).toBool());
	p.setOrientation(QPrinter::Orientation(s->value("Orientation", QPrinter::Portrait).toInt()));
	p.setOutputFileName(s->value("OutputName", "").toString());
	p.setOutputFormat(QPrinter::OutputFormat(s->value("OutputFormat", QPrinter::PdfFormat).toInt()));
	p.setPageOrder(QPrinter::PageOrder(s->value("PageOrder").toInt()));
	p.setPaperSize(QPrinter::PaperSize(s->value("PaperSize").toInt()));
	p.setPaperSource(QPrinter::PaperSource(s->value("PaperSource").toInt()));
	p.setPrintProgram(s->value("PrintProg", "").toString());
	p.setPrintRange(QPrinter::PrintRange(s->value("PrintRange").toInt()));
	p.setResolution(s->value("Resolution", 96).toInt());

	qreal left, top, right, bottom;
	left = (s->value("LeftMargin", 15).toFloat());
	top = (s->value("TopMargin", 15).toFloat());
	right = (s->value("RightMargin", 15).toFloat());
	bottom = (s->value("BottomMargin", 15).toFloat());

	p.setPageMargins(left, top, right, bottom, QPrinter::Millimeter);

	s->endGroup();
}

void E2Profile::SetPrinterSettings(QPrinter &p)
{
	s->beginGroup("Printer");

	s->setValue("PrinterName", p.printerName());
	s->setValue("PageSize", p.pageSize());
	s->setValue("Copies", p.collateCopies());
	s->setValue("ColorMode", p.colorMode());
	s->setValue("CopyCount", p.copyCount());
	s->setValue("Creator", p.creator());
	s->setValue("DocName", p.docName());
	s->setValue("DoubleSide", p.doubleSidedPrinting());
	s->setValue("Duplex", p.duplex());
	s->setValue("FontEmb", p.fontEmbeddingEnabled());
	s->setValue("FullPage", p.fullPage());
	s->setValue("Orientation", p.orientation());
	s->setValue("OutputName", p.outputFileName());
	s->setValue("OutputFormat", p.outputFormat());
	s->setValue("PageOrder", p.pageOrder());
	s->setValue("PaperSize", p.paperSize());
	s->setValue("PaperSource", p.paperSource());
	s->setValue("PrintProg", p.printProgram());
	s->setValue("PrintRange", p.printRange());
	s->setValue("Resolution", p.resolution());

	qreal left, top, right, bottom;
	p.getPageMargins(&left, &top, &right, &bottom, QPrinter::Millimeter);

	s->setValue("LeftMargin", left);
	s->setValue("TopMargin", top);
	s->setValue("BottomMargin", bottom);
	s->setValue("RightMargin", right);

	s->endGroup();
}


int E2Profile::GetMegaPageDelay()
{
	//Default: 50 msec
	return s->value("ATMegaPageWriteDelay", 50).toInt();
}

void E2Profile::SetMegaPageDelay(int delay)
{
	if (delay > 0)
	{
		s->setValue("ATMegaPageWriteDelay", delay);
	}
}


int E2Profile::GetPowerUpDelay()
{
	//Default: 200 msec
	return s->value("PowerUpDelay", 200).toInt();
}

void E2Profile::SetPowerUpDelay(int delay)
{
	if (delay > 0)
	{
		s->setValue("PowerUpDelay", delay);
	}
}


int E2Profile::GetAVRProgDelay()
{
	//Default: 20 msec
	return s->value("AVRByteWriteDelay", 20).toInt();
}

void E2Profile::SetAVRProgDelay(int delay)
{
	if (delay > 0)
	{
		s->setValue("AVRByteWriteDelay", delay);
	}
}


int E2Profile::GetFontSize()
{
	return s->value("FontSize", 9).toInt();
}

void E2Profile::SetFontSize(int sz)
{
	if (sz > 0)
	{
		s->setValue("FontSize", sz);
	}
}


int E2Profile::GetAVREraseDelay()
{
	//Default: 50 msec
	return s->value("AVREraseDelay", 50).toInt();
}

void E2Profile::SetAVREraseDelay(int delay)
{
	if (delay > 0)
	{
		s->setValue("AVREraseDelay", delay);
	}
}


int E2Profile::GetMDAProgDelay()
{
	//Default: 30 msec
	return s->value("MDAWriteDelay", 30).toInt();
}

void E2Profile::SetMDAProgDelay(int delay)
{
	if (delay >= 0)
	{
		s->setValue("MDAWriteDelay", delay);
	}
}


int E2Profile::GetNVMProgDelay()
{
	//Default: 30 msec
	return s->value("NVMWriteDelay", 30).toInt();
}

void E2Profile::SetNVMProgDelay(int delay)
{
	if (delay >= 0)
	{
		s->setValue("NVMWriteDelay", delay);
	}
}


unsigned long E2Profile::GetSerialNumVal()
{
	QString sp = s->value("SerialNumber/Value", "0").toString();
	unsigned long rval = 0;         //Default 0

	if (sp.length())
	{
		bool ok;
		rval = sp.toULong(&ok);
		if (!ok)
		{
			rval = 0;
		}
	}

	return rval;
}

void E2Profile::SetSerialNumVal(unsigned long val)
{
	s->setValue("SerialNumber/Value", QString::number(val));
}


int E2Profile::GetSerialNumAddress(long &start, int &size, bool &mtype)
{
	QString sp;
	bool ok;

	start = 0;

	s->beginGroup("SerialNumber");
	sp = s->value("Address", "0").toString();
	if (sp.length())
	{
		start = sp.toLong(&ok, 0);

		if (!ok)
		{
			start = 0;
		}
	}

	size = s->value("Size", 1).toInt(&ok);
	if (!ok)
	{
		size = 1;
	}

	mtype = false;
	sp = s->value("Type", "PROG").toString();
	if (sp.length() && sp == "DATA")
	{
		mtype = true;
	}
	s->endGroup();

	return OK;
}

void E2Profile::SetSerialNumAddress(unsigned long start, int size, bool mtype)
{
	s->beginGroup("SerialNumber");
	s->setValue("Address", QString().sprintf("0x%04lX", start));

	if (size >= 1)
	{
		s->setValue("Size", size);
	}

	s->setValue("Type", mtype ? "DATA" : "PROG");
	s->endGroup();
}


FmtEndian E2Profile::GetSerialNumFormat()
{
	QString sp = s->value("SerialNumber/Format", "BIGENDIAN").toString();

	if (sp.length() && ("LITTLEENDIAN" == sp))
	{
		return FMT_LITTLE_ENDIAN;
	}
	else
	{
		return FMT_BIG_ENDIAN;
	}
}

void E2Profile::SetSerialNumFormat(FmtEndian fmt)
{
	s->setValue("SerialNumber/Format", (fmt == FMT_BIG_ENDIAN) ? "BIGENDIAN" : "LITTLEENDIAN");
}


bool E2Profile::GetSerialNumAutoInc()
{
	return s->value("SerialNumber/AutoIncrement", true).toBool();
}

void E2Profile::SetSerialNumAutoInc(bool val)
{
	s->setValue("SerialNumber/AutoIncrement", val);
}


long E2Profile::GetProgramOptions()
{
	long res = 0;
	QString rval;

	s->beginGroup("ProgramOptions");

	rval = s->value("ReloadOption", "").toString();
	if (rval.length() && rval != "NO")
	{
		res |= RELOAD_YES;
	}

	rval = s->value("ReadFlashOption", "").toString();
	if (rval.length() && rval != "NO")
	{
		res |= READFLASH_YES;
	}

	rval = s->value("ReadEEpromOption", "").toString();
	if (rval.length() && rval != "NO")
	{
		res |= READEEP_YES;
	}

	rval = s->value("ByteSwapOption", "").toString();
	if (rval.length() && rval != "NO")
	{
		res |= BYTESWAP_YES;
	}

	rval = s->value("SetIDkeyOption", "").toString();
	if (rval.length() && rval != "NO")
	{
		res |= SETID_YES;
	}

	rval = s->value("ReadOscCalibration", "").toString();
	if (rval.length() && rval != "NO")
	{
		res |= READOSCAL_YES;
	}

	rval = s->value("EraseOption", "").toString();
	if (rval.length() && rval != "NO")
	{
		res |= ERASE_YES;
	}

	rval = s->value("WriteFlashOption", "").toString();
	if (rval.length() && rval != "NO")
	{
		res |= FLASH_YES;
	}

	rval = s->value("WriteEEpromOption", "").toString();
	if (rval.length() && rval != "NO")
	{
		res |= EEPROM_YES;
	}

	rval = s->value("WriteSecurityOption", "").toString();
	if (rval.length() && rval != "NO")
	{
		res |= LOCK_YES;
	}

	s->endGroup();

	return res;
}

void E2Profile::SetProgramOptions(long prog_option)
{
	s->beginGroup("ProgramOptions");
	s->setValue("ReloadOption",
				(prog_option & RELOAD_YES) ? "YES" : "NO");
	s->setValue("ReadFlashOption",
				(prog_option & READFLASH_YES) ? "YES" : "NO");
	s->setValue("ReadEEpromOption",
				(prog_option & READEEP_YES) ? "YES" : "NO");
	s->setValue("ByteSwapOption",
				(prog_option & BYTESWAP_YES) ? "YES" : "NO");
	s->setValue("SetIDkeyOption",
				(prog_option & SETID_YES) ? "YES" : "NO");
	s->setValue("ReadOscCalibration",
				(prog_option & READOSCAL_YES) ? "YES" : "NO");
	s->setValue("EraseOption",
				(prog_option & ERASE_YES) ? "YES" : "NO");
	s->setValue("WriteFlashOption",
				(prog_option & FLASH_YES) ? "YES" : "NO");
	s->setValue("WriteEEpromOption",
				(prog_option & EEPROM_YES) ? "YES" : "NO");
	s->setValue("WriteSecurityOption",
				(prog_option & LOCK_YES) ? "YES" : "NO");
	s->endGroup();
}


QString E2Profile::GetXmlDir()
{
	return s->value("XmlDir", "").toString();
}

void E2Profile::SetXmlDir(const QString &name)
{
	if (name.length())
	{
		s->setValue("XmlDir", name);
	}
}


QString E2Profile::GetLangDir()
{
	return s->value("LangDir", "").toString();
}

void E2Profile::SetLangDir(const QString &name)
{
	if (name.length())
	{
		s->setValue("LangDir", name);
	}
}


QString E2Profile::GetCurrentLang()
{
	return s->value("CurrentLang", "english").toString();
}

void E2Profile::SetCurrentLang(const QString &name)
{
	if (name.length())
	{
		s->setValue("CurrentLang", name);
	}
}


QString E2Profile::GetLogFileName()
{
	QString sp = s->value("LogFileName", "PonyProg.log").toString();

	return sp;
}

void E2Profile::SetLogFileName(const QString &name)
{
	if (name.length())
	{
		s->setValue("LogFileName", name);
	}
}

#ifdef Q_OS_LINUX

static QString retrieve_ttyS_name()
{
	QString sp = "COM";
	QFile file("/proc/tty/drivers");

	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file);
		QString line = in.readLine();
		while (!line.isNull())
		{
			line = line.simplified();

			if (line.size() > 0)
			{
				QStringList l = line.split(" ");
				if (l.count() > 1 && l.at(0) == "serial")
				{
					sp = l.at(1);
					break;
				}
			}
			line = in.readLine();
		}
		file.close();
	}
	else
	{
		qWarning() << "Can't open file /proc/tty/drivers";
	}

	return sp;
}

static QStringList retrieve_ttyS_list()
{
	QStringList lst;
	QString name = retrieve_ttyS_name();

	if (name.length() == 0)
	{
		return lst;
	}

	QFile file("/proc/tty/driver/serial");		//Need Root access!!!!

	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file);
		QString line = in.readLine();
		while (!line.isNull())
		{
			if (line.contains("tx:"))
			{
				QStringList l = line.split(":");
				if (l.count() > 0)
				{
					bool ok = false;

					l.at(0).toInt(&ok);
					if (ok)
					{
						lst << name + l.at(0);
						qDebug() << "ttyS dev: " << name << l.at(0);
					}
				}
			}
			line = in.readLine();
		}
		file.close();
	}
	else
	{
		qWarning() << "Can't open file /proc/tty/driver/serial";
	}

	return lst;
}

#endif

QString E2Profile::GetCOMDevName()
{
	QString sp = s->value("LegacyPorts/COMDevName", "").toString();

	if (sp.length() == 0)
	{
#ifdef Q_OS_LINUX
		sp = retrieve_ttyS_name();
#else
		sp = "COM";
#endif
	}

	return sp;
}

void E2Profile::SetCOMDevName(const QString &name)
{
	if (name.length())
	{
		s->setValue("LegacyPorts/COMDevName", name);
	}
}


QStringList E2Profile::GetCOMDevList()
{
	QStringList lst = s->value("LegacyPorts/COMDevList", QStringList()).toStringList();

	if (lst.count() == 0)
	{
#ifdef Q_OS_LINUX
		lst = retrieve_ttyS_list();
		if (lst.count() == 0)
		{
			QString sname = E2Profile::GetCOMDevName();

			for (int i = 0; i < MAX_COMPORTS; i++)
			{
				lst << sname + QString::number(i);
			}
		}
		else
		{
			E2Profile::SetCOMDevList(lst);
		}
#else
		QString sname = E2Profile::GetCOMDevName();

		for (int i = 1; i <= MAX_COMPORTS; i++)
		{
			lst << sname + QString::number(i);
		}
#endif
	}

	return lst;
}

void E2Profile::SetCOMDevList(const QStringList &lst)
{
	s->setValue("LegacyPorts/COMDevList", lst);
}


QString E2Profile::GetLPTDevName()
{
	QString sp = s->value("LegacyPorts/LPTDevName", "").toString();

	if (sp.length() == 0)
	{
#ifdef Q_OS_LINUX
		sp = QString("/dev/parport");
#else
		sp = QString("LPT");
#endif
	}

	return sp;
}

void E2Profile::SetLPTDevName(const QString &name)
{
	if (name.length())
	{
		s->setValue("LegacyPorts/LPTDevName", name);
	}
}


QStringList E2Profile::GetLPTDevList()
{
	QStringList lst = s->value("LegacyPorts/LPTDevList", QStringList()).toStringList();

	if (lst.count() == 0)
	{
		QString sname = E2Profile::GetLPTDevName();
#ifdef Q_OS_LINUX
		for (int i = 0; i < MAX_LPTPORTS; i++)
#else
		for (int i = 1; i <= MAX_LPTPORTS; i++)
#endif
		{
			lst << sname  + QString::number(i);
		}
	}

	return lst;
}

void E2Profile::SetLPTDevList(const QStringList &lst)
{
	s->setValue("LegacyPorts/LPTDevList", lst);
}


//QString E2Profile::GetHtmlBrowseApp()
//{
//	return s->value("HtmlBrowseApp", "konqueror").toString();
//}

//void E2Profile::SetHtmlBrowseApp(const QString &name)
//{
//	if (name.length())
//	{
//		s->setValue("HtmlBrowseApp", name);
//	}
//}


QString E2Profile::GetLanguageCode()
{
	QString sp = s->value("LanguageCode", "C").toString();

	if (sp.length() == 0)
	{
		sp = "C";
	}

	return sp;
}

void E2Profile::SetLanguageCode(const QString &name)
{
	if (name.length())
	{
		s->setValue("LanguageCode", name);
	}
}


bool E2Profile::GetLogEnabled()
{
	return s->value("LogEnabled", false).toBool();
}

void E2Profile::SetLogEnabled(bool enabled)
{
	s->setValue("LogEnabled", enabled);
}


bool E2Profile::GetSkipStartupDialog()
{
	QString sp = s->value("Preferences/SkipStartupDialog", "NO").toString();

	if (sp.length() && (sp == "YES"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void E2Profile::SetSkipStartupDialog(bool skip)
{
	s->setValue("Preferences/SkipStartupDialog", skip ? "YES" : "NO");
}


bool E2Profile::GetClearBufBeforeLoad()
{
	QString sp = s->value("ClearBufferBeforeLoad", "NO").toString();

	if (sp.length() && (sp == "YES"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void E2Profile::SetClearBufBeforeLoad(bool enabled)
{
	s->setValue("ClearBufferBeforeLoad", enabled ? "YES" : "NO");
}


bool E2Profile::GetClearBufBeforeRead()
{
	QString sp = s->value("ClearBufferBeforeRead", "NO").toString();

	if (sp.length() && (sp == "YES"))
	{
		return true;
	}
	else
	{
		return false;
	}
}


void E2Profile::SetClearBufBeforeRead(bool enabled)
{
	s->setValue("ClearBufferBeforeRead", enabled ? "YES" : "NO");
}


bool E2Profile::GetAt89PageOp()
{
	QString sp = s->value("AT89SPageOpEnabled", "YES").toString();

	if (sp.length() && (sp == "NO"))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void E2Profile::SetAt89PageOp(bool enabled)
{
	s->setValue("AT89SPageOpEnabled", enabled ? "YES" : "NO");
}


bool E2Profile::Get8253FallEdge()
{
	QString sp = s->value("AT89S8253FallEdgeSampling", "YES").toString();

	if (sp.length() && (sp == "NO"))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void E2Profile::Set8253FallEdge(bool enabled)
{
	s->setValue("AT89S8253FallEdgeSampling", enabled ? "YES" : "NO");
}


bool E2Profile::GetSoundEnabled()
{
	QString sp = s->value("Preferences/SoundEnabled", "NO").toString();

	if (sp.length() && (sp == "YES"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void E2Profile::SetSoundEnabled(bool enabled)
{
	s->setValue("Preferences/SoundEnabled", enabled ? "YES" : "NO");
}


int E2Profile::GetCalibrationAddress(bool &enabled, long &start, int &size, bool &mtype)
{
	QString sp;
	bool ok;

	s->beginGroup("OscillatorCalibration");
	enabled = s->value("Enabled", true).toBool();

	start = 0;
	sp = s->value("StartAddress", "0").toString();
	if (sp.length())
	{
		start = sp.toLong(&ok, 0);

		if (!ok)
		{
			start = 0;
		}
	}

	size = s->value("Size", 1).toInt(&ok);
	if (!ok)
	{
		size = 1;
	}

	mtype = false;
	sp = s->value("MemoryType", "PROG").toString();
	if (sp.length() && sp == "DATA")
	{
		mtype = true;
	}
	s->endGroup();

	return OK;
}

void E2Profile::SetCalibrationAddress(bool enabled, unsigned long start, int size, bool mtype)
{
	s->beginGroup("OscillatorCalibration");
	s->setValue("Enabled", enabled);
	s->setValue("StartAddress", QString().sprintf("0x%04lX", start));
	if (size >= 1)
	{
		s->setValue("Size", size);
	}
	s->setValue("MemoryType", mtype ? "DATA" : "PROG");
	s->endGroup();
}


unsigned int E2Profile::GetJDMCmd2CmdDelay()
{
	//Default: 4000 usec
	return s->value("JDM-CmdToCmdDelay", 4000).toUInt();
}

void E2Profile::SetJDMCmd2CmdDelay(unsigned int delay)
{
	s->setValue("JDM-CmdToCmdDelay", delay);
}


bool E2Profile::GetVerifyAfterWrite()
{
	return s->value("VerifyAfterWrite", true).toBool();
}

void E2Profile::SetVerifyAfterWrite(bool enabled)
{
	s->setValue("VerifyAfterWrite", enabled);
}


bool E2Profile::GetAutoDetectPorts()
{
	return s->value("LegacyPorts/AutoDetectPorts", true).toBool();
}

void E2Profile::SetAutoDetectPorts(bool enabled)
{
	s->setValue("LegacyPorts/AutoDetectPorts", enabled);
}


int E2Profile::GetCOMAddress(unsigned int &com1, unsigned int &com2, unsigned int &com3, unsigned int &com4)
{
	QString sp = s->value("LegacyPorts/COMPorts").toString();

	com1 = 0x3F8;
	com2 = 0x2F8;
	com3 = 0x3E8;
	com4 = 0x2E8;

	if (sp.length())
	{
		sscanf(sp.toLatin1().constData(), "%x,%x,%x,%x", &com1, &com2, &com3, &com4);
	}

	return OK;
}

void E2Profile::SetCOMAddress(unsigned int com1, unsigned int com2, unsigned int com3, unsigned int com4)
{
	QString str;

	if (com1 > 0)
	{
		if (com2 > 0)
		{
			if (com3 > 0)
			{
				if (com4 > 0)
				{
					str.sprintf("%X,%X,%X,%X", com1, com2, com3, com4);
				}
				else
				{
					str.sprintf("%X,%X,%X", com1, com2, com3);
				}
			}
			else
			{
				str.sprintf("%X,%X", com1, com2);
			}
		}
		else
		{
			str.sprintf("%X", com1);
		}

		s->setValue("LegacyPorts/COMPorts", str);
	}
}


int E2Profile::GetLPTAddress(unsigned int &lpt1, unsigned int &lpt2, unsigned int &lpt3)
{
	QString sp = s->value("LegacyPorts/LPTPorts").toString();

	lpt1 = 0x378;
	lpt2 = 0x278;
	lpt3 = 0x3BC;

	if (sp.length())
	{
		sscanf(sp.toLatin1().constData(), "%x,%x,%x", &lpt1, &lpt2, &lpt3);
	}

	return OK;
}

void E2Profile::SetLPTAddress(unsigned int lpt1, unsigned int lpt2, unsigned int lpt3)
{
	QString str;

	if (lpt1 > 0)
	{
		if (lpt2 > 0)
		{
			if (lpt3 > 0)
			{
				str.sprintf("%X,%X,%X", lpt1, lpt2, lpt3);
			}
			else
			{
				str.sprintf("%X,%X", lpt1, lpt2);
			}
		}
		else
		{
			str.sprintf("%X", lpt1);
		}

		s->setValue("LegacyPorts/LPTPorts", str);
	}
}


FileType E2Profile::GetDefaultFileType()
{
	QString sp = s->value("DefaultFileType", "E2P").toString();
	FileType ft = E2P;

	if (sp.length())
	{
		if (sp == "e2p")
		{
			ft = E2P;
		}
		else if (sp == "bin")
		{
			ft = BIN;
		}
		else if (sp == "csm")
		{
			ft = CSM;
		}
		else if (sp == "intel-hex")
		{
			ft = INTEL;
		}
		else if (sp == "mot-srec")
		{
			ft = MOTOS;
		}
	}

	return ft;
}

void E2Profile::SetDefaultFileType(FileType ft)
{
	QString str;

	if (ft == E2P)
	{
		str = "E2P";
	}
	else if (ft == BIN)
	{
		str = "BIN";
	}
	else if (ft == CSM)
	{
		str = "CSM";
	}
	else if (ft == INTEL)
	{
		str = "INTEL-HEX";
	}
	else if (ft == MOTOS)
	{
		str = "MOT-SREC";
	}

	if (str.length())
	{
		s->setValue("DefaultFileType", str);
	}
}


bool E2Profile::GetEditBufferEnabled()
{
	return !(s->value("Editor/ReadOnlyMode", false).toBool());
}

void E2Profile::SetEditBufferEnabled(bool enable)
{
	s->setValue("Editor/ReadOnlyMode", !enable);
}


void E2Profile::GetInterfacePins(HInterfaceType type, InterfPins &pins)
{
	InterfPins dpins;	//default pins
	if (!TypeToInterfPins(type, dpins))
	{
		qWarning() << "TypeToInterfPins(" << type << ") Failed";
	}

	s->beginGroup("InterfacePins-" + TypeToInterfName(type));
	pins.clock = s->value("clockout", dpins.clock).toInt();
	pins.clockin = s->value("clockin", dpins.clockin).toInt();
	pins.ctrl = s->value("controlout", dpins.ctrl).toInt();
	pins.ctrlin = s->value("controlin", dpins.ctrlin).toInt();
	pins.datain = s->value("datain", dpins.datain).toInt();
	pins.dataout = s->value("dataout", dpins.dataout).toInt();
	pins.enbus = s->value("enbus", dpins.enbus).toInt();
	pins.poweron = s->value("poweron", dpins.poweron).toInt();
	s->endGroup();
}

void E2Profile::SetInterfacePins(HInterfaceType type, const InterfPins &pins)
{
	s->beginGroup("InterfacePins-" + TypeToInterfName(type));
	if (pins.clock > -1)
	{
		s->setValue("clockout", pins.clock);
	}
	if (pins.clockin > -1)
	{
		s->setValue("clockin", pins.clockin);
	}
	if (pins.ctrl > -1)
	{
		s->setValue("controlout", pins.ctrl);
	}
	if (pins.ctrlin > -1)
	{
		s->setValue("controlin", pins.ctrlin);
	}
	if (pins.datain > -1)
	{
		s->setValue("datain", pins.datain);
	}
	if (pins.dataout > -1)
	{
		s->setValue("dataout", pins.dataout);
	}
	if (pins.enbus > -1)
	{
		s->setValue("enbus", pins.enbus);
	}
	if (pins.poweron > -1)
	{
		s->setValue("poweron", pins.poweron);
	}
	s->endGroup();
}


int E2Profile::GetInterfacePort()
{
	QString sp = s->value("InterfacePort", "A").toString();
	int rv = FTDI_PORTA;

	if (sp.length() != 0)
	{
		if (sp.compare("B", Qt::CaseInsensitive) == 0 || sp.compare("1") == 0)
		{
			rv = FTDI_PORTB;
		}
		else if (sp.compare("C", Qt::CaseInsensitive) == 0 || sp.compare("2") == 0)
		{
			rv = FTDI_PORTC;
		}
		else if (sp.compare("D", Qt::CaseInsensitive) == 0 || sp.compare("3") == 0)
		{
			rv = FTDI_PORTD;
		}
	}

	return rv;
}

void E2Profile::SetInterfacePort(int port_index)
{
	QString intname = QString::number(port_index);

	if (intname.length())
	{
		s->setValue("InterfacePort", intname);
	}
}


void E2Profile::readDialogSettings(QWidget *window, bool recurse)
{
	s->beginGroup(window->objectName());
	//window->restoreGeometry(s->value("geometry").toByteArray());
	if (s->value("maximized", false).toBool())
	{
		window->showMaximized();
	}
	else
	{
		QVariant value = s->value("pos");
		if (!value.isNull())
		{
			window->move(s->value("pos").toPoint());
			window->resize(s->value("size").toSize());
		}
	}
	if (recurse)
	{
		recurseRead(window);
	}
	s->endGroup();
}

void E2Profile::writeDialogSettings(QWidget *window, bool recurse)
{
	s->beginGroup(window->objectName());
	//s->setValue("geometry", window->saveGeometry());
	if (window->isMaximized())
	{
		s->setValue("maximized", true);
	}
	else
	{
		s->setValue("maximized", false);
		s->setValue("pos", window->pos());
		s->setValue("size", window->size());
	}
	if (recurse)
	{
		recurseWrite(window);
	}
	s->endGroup();
}

#include <QCheckBox>
#include <QComboBox>

void E2Profile::recurseRead(QObject *object)
{
	Q_CHECK_PTR(object);
	QCheckBox *checkbox = dynamic_cast<QCheckBox *>(object);
	if (0 != checkbox)
	{
		checkbox->setChecked(s->value(checkbox->objectName()).toBool());
	}
	QComboBox *combobox = dynamic_cast<QComboBox *>(object);
	if (0 != combobox)
	{
		combobox->setCurrentIndex(s->value(combobox->objectName()).toInt());
	}
	QSplitter *spl = dynamic_cast<QSplitter *>(object);
	if (0 != spl)
	{
		spl->restoreState(s->value(spl->objectName()).toByteArray());
	}

	foreach (QObject *child, object->children())
	{
		recurseRead(child);
	}
}

void E2Profile::recurseWrite(QObject *object)
{
	Q_CHECK_PTR(object);
	QCheckBox *checkbox = dynamic_cast<QCheckBox *>(object);
	if (0 != checkbox)
	{
		s->setValue(checkbox->objectName(), checkbox->isChecked());
	}
	QComboBox *combobox = dynamic_cast<QComboBox *>(object);
	if (0 != combobox)
	{
		s->setValue(combobox->objectName(), combobox->currentIndex());
	}
	QSplitter *spl = dynamic_cast<QSplitter *>(object);
	if (0 != spl)
	{
		s->setValue(spl->objectName(), spl->saveState());
	}

	foreach (QObject *child, object->children())
	{
		recurseWrite(child);
	}
}

void E2Profile::restoreSplitter(QSplitter *spl)
{
	Q_CHECK_PTR(spl);
	spl->restoreState(s->value("MainWindow/splitterSizes").toByteArray());
}

void E2Profile::saveSplitter(QSplitter *spl)
{
	Q_CHECK_PTR(spl);
	s->setValue("MainWindow/splitterSizes", spl->saveState());
}
