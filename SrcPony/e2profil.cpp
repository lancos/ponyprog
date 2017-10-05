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

#include <QDebug>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QSettings>

#include "version.h"
#include "e2profil.h"
#include "types.h"
#include "errcode.h"
#include "globals.h"
#include "portint.h"

//QSettings *E2Profile::s = new QSettings("ponyprog.ini", QSettings::IniFormat);
QSettings *E2Profile::s = new QSettings(APP_NAME);

//=====>>> Costruttore <<<======
#if 0
E2Profile::E2Profile(const QString &nm)  :
	s(0)
{
	if (nm.length())
	{
		filename = nm;
	}
	else
	{
		filename = "e2p.ini";
	}

	s = new QSettings(filename, QSettings::IniFormat);
}


E2Profile::~E2Profile()
{
	s->sync();
}
#endif

int E2Profile::GetBogoMips()
{
	QString sp = s->value("BogoMipsX1000", "0").toString();
	int rval = 0;           //Defaultvalue

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetBogoMips(int value)
{
	//      QString str;
	s->setValue("BogoMipsX1000", QString::number(value));
	//      if ( decnum2str(value, str, MAXNUMDIGIT) == OK )
	//      {
	//              return s->setValue("BogoMipsX1000", str);
	//      }
	//      else
	//      {
	//              return BADPARAM;
	//      }
}


#include "eeptypes.h"


long E2Profile::GetLastDevType()
{
	QString sp = s->value("DeviceType", "24XX Auto").toString();

	if (sp.length())
	{
		return GetEEPTypeFromString(sp);
	}
	else
	{
		return E2400;        //Default device type
	}
}


//void E2Profile::SetConfigFile(const QString &nm)
//{
//	if (nm.length())
//	{
//		delete s;
//		s = new QSettings(nm, QSettings::IniFormat);
//	}
//}


void E2Profile::SetLastDevType(long devtype)
{
	QString sp = GetEEPTypeString(devtype);

	if (sp.length())
	{
		s->setValue("DeviceType", sp.remove(QChar(' ')));
	}
}


HInterfaceType E2Profile::GetParInterfType()
{
	extern HInterfaceType NameToInterfType(const QString & name);

	QString v = s->value("InterfaceType", "SI-ProgAPI").toString();

	return NameToInterfType(v);
}


void E2Profile::SetParInterfType(HInterfaceType type)
{
	QString TypeToInterfName(HInterfaceType type);

	s->setValue("InterfaceType", TypeToInterfName(type));
}


int E2Profile::GetPortNumber()
{
	return s->value("PortNumber", "-1").toInt();
}


void E2Profile::SetPortNumber(int port)
{
	if (port >= 0 /* && port < 4 */)
	{
		s->setValue("PortNumber", QString::number(port));
	}
}


// QString E2Profile::GetLastScript()
// {
// 	return s->value("LastScript", "").toString();
// }


void E2Profile::SetLastScript(const QString &name)
{
	QStringList l = GetLastScripts();
	l.insert(0, name);
	SetLastScripts(l);
// 	s->setValue("LastScript", name);
}

//Check if the entry is already in the list, in this case remove
//then insert the entry in the head of the list
void E2Profile::SetLastFile(const QString &name, int data_type)
{
	if (name.length() > 0)
	{
		QStringList l = GetLastFiles();
		QString item_name = name;
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

//QString E2Profile::GetPrevFile(int &data)
//{
//	return GetLastFile(data, 1);
//}


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

uint8_t E2Profile::GetPolarityControl()
{
	uint8_t res;
	QString rval;

	res = 0;
	rval = s->value("ClockPolarity", "").toString();

	if (rval.length())
		if (rval == "INV")
		{
			res |= CLOCKINV;
		}

	rval = s->value("ResetPolarity", "").toString();

	if (rval.length())
		if (rval == "INV")
		{
			res |= RESETINV;
		}

	rval = s->value("DOutPolarity", "").toString();

	if (rval.length())
		if (rval == "INV")
		{
			res |= DOUTINV;
		}

	rval = s->value("DInPolarity", "").toString();

	if (rval.length())
		if (rval == "INV")
		{
			res |= DININV;
		}

	return res;
}


void E2Profile::SetPolarityControl(uint8_t polarity_control)
{
	s->setValue("ResetPolarity",
				(polarity_control & RESETINV) ? "INV" : "TRUE");

	s->setValue("ClockPolarity",
				(polarity_control & CLOCKINV) ? "INV" : "TRUE");

	s->setValue("DOutPolarity",
				(polarity_control & DOUTINV) ? "INV" : "TRUE");

	s->setValue("DInPolarity",
				(polarity_control & DININV) ? "INV" : "TRUE");
}


int E2Profile::GetI2CPageWrite()
{
	QString sp = s->value("I2CBusPageWrite", "16").toString();
	int rval = 16;          //Default: 16 bytes page write (only for 16-bit I2C Bus eeprom)

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetI2CPageWrite(int page_write)
{
	if (page_write > 0)
	{
		s->setValue("I2CBusPageWrite", QString::number(page_write));
	}
}


int E2Profile::GetSPIPageWrite()
{
	QString sp = s->value("BigSPIPageWrite", "16").toString();
	int rval = 16;          //Default: 16 bytes page write

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetSPIPageWrite(int page_write)
{
	if (page_write > 0)
	{
		s->setValue("BigSPIPageWrite", QString::number(page_write));
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

		if (ok == false)
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


int E2Profile::GetI2CSpeed()
{
	QString sp = s->value("I2CBusSpeed", "NORMAL").toString();
	int rval = NORMAL;              //Default speed

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


void E2Profile::SetI2CSpeed(int speed)
{
	if (speed == TURBO)
	{
		s->setValue("I2CBusSpeed", "TURBO");
	}
	else if (speed == FAST)
	{
		s->setValue("I2CBusSpeed", "FAST");
	}
	else if (speed == NORMAL)
	{
		s->setValue("I2CBusSpeed", "NORMAL");
	}
	else if (speed == SLOW)
	{
		s->setValue("I2CBusSpeed", "SLOW");
	}
	else if (speed == VERYSLOW)
	{
		s->setValue("I2CBusSpeed", "VERYSLOW");
	}
	else if (speed == ULTRASLOW)
	{
		s->setValue("I2CBusSpeed", "ULTRASLOW");
	}
}


int E2Profile::GetSPIResetPulse()
{
	QString sp = s->value("SPIResetPulse", "100").toString();
	int rval = 100;         //Default: 100 msec

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetSPIResetPulse(int delay)
{
	if (delay > 0)
	{
		s->setValue("SPIResetPulse", QString::number(delay));
	}
}


int E2Profile::GetSPIDelayAfterReset()
{
	QString sp = s->value("SPIDelayAfterReset", "50").toString();
	int rval = 50;          //Default: 50 msec

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetSPIDelayAfterReset(int delay)
{
	if (delay > 0)
	{
		s->setValue("SPIDelayAfterReset", QString::number(delay));
	}
}


int E2Profile::GetAT89DelayAfterReset()
{
	QString sp = s->value("AT89DelayAfterReset", "50").toString();
	int rval = 50;          //Default: 50 msec

	if (sp.length())
	{
		rval = sp.length();
	}

	return rval;
}


void E2Profile::SetAT89DelayAfterReset(int delay)
{
	if (delay > 0)
	{
		s->setValue("AT89DelayAfterReset", QString::number(delay));
	}
}


int E2Profile::GetAVRDelayAfterReset()
{
	QString sp = s->value("AVRDelayAfterReset", "50").toString();
	int rval = 50;          //Default: 50 msec

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetAVRDelayAfterReset(int delay)
{
	if (delay > 0)
	{
		s->setValue("AVRDelayAfterReset", QString::number(delay));
	}
}


int E2Profile::GetSPISpeed()
{
	QString sp = s->value("SPIBusSpeed", "NORMAL").toString();
	int rval = NORMAL;              //Default speed

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


void E2Profile::SetSPISpeed(int speed)
{
	if (speed == TURBO)
	{
		s->setValue("SPIBusSpeed", "TURBO");
	}
	else if (speed == FAST)
	{
		s->setValue("SPIBusSpeed", "FAST");
	}
	else if (speed == NORMAL)
	{
		s->setValue("SPIBusSpeed", "NORMAL");
	}
	else if (speed == SLOW)
	{
		s->setValue("SPIBusSpeed", "SLOW");
	}
	else if (speed == VERYSLOW)
	{
		s->setValue("SPIBusSpeed", "VERYSLOW");
	}
	else if (speed == ULTRASLOW)
	{
		s->setValue("SPIBusSpeed", "ULTRASLOW");
	}
}


int E2Profile::GetMegaPageDelay()
{
	QString sp = s->value("ATMegaPageWriteDelay", "50").toString();
	int rval = 50;          //Default: 50 msec

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetMegaPageDelay(int delay)
{
	if (delay > 0)
	{
		s->setValue("ATMegaPageWriteDelay", QString::number(delay));
	}
}


int E2Profile::GetMicroWireSpeed()
{
	QString sp = s->value("MicroWireBusSpeed", "NORMAL").toString();
	int rval = NORMAL;              //Default speed

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


void E2Profile::SetMicroWireSpeed(int speed)
{
	if (speed == TURBO)
	{
		s->setValue("MicroWireBusSpeed", "TURBO");
	}
	else if (speed == FAST)
	{
		s->setValue("MicroWireBusSpeed", "FAST");
	}
	else if (speed == NORMAL)
	{
		s->setValue("MicroWireBusSpeed", "NORMAL");
	}
	else if (speed == SLOW)
	{
		s->setValue("MicroWireBusSpeed", "SLOW");
	}
	else if (speed == VERYSLOW)
	{
		s->setValue("MicroWireBusSpeed", "VERYSLOW");
	}
	else if (speed == ULTRASLOW)
	{
		s->setValue("MicroWireBusSpeed", "ULTRASLOW");
	}
}


int E2Profile::GetPICSpeed()
{
	QString sp = s->value("PICBusSpeed", "NORMAL").toString();
	int rval = NORMAL;              //Default speed

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


void E2Profile::SetPICSpeed(int speed)
{
	if (speed == TURBO)
	{
		s->setValue("PICBusSpeed", "TURBO");
	}
	else if (speed == FAST)
	{
		s->setValue("PICBusSpeed", "FAST");
	}
	else if (speed == NORMAL)
	{
		s->setValue("PICBusSpeed", "NORMAL");
	}
	else if (speed == SLOW)
	{
		s->setValue("PICBusSpeed", "SLOW");
	}
	else if (speed == VERYSLOW)
	{
		s->setValue("PICBusSpeed", "VERYSLOW");
	}
	else if (speed == ULTRASLOW)
	{
		s->setValue("PICBusSpeed", "ULTRASLOW");
	}
}


int E2Profile::GetSDESpeed()
{
	QString sp = s->value("SDEBusSpeed", "NORMAL").toString();
	int rval = NORMAL;              //Default speed

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


void E2Profile::SetSDESpeed(int speed)
{
	if (speed == TURBO)
	{
		s->setValue("SDEBusSpeed", "TURBO");
	}
	else if (speed == FAST)
	{
		s->setValue("SDEBusSpeed", "FAST");
	}
	else if (speed == NORMAL)
	{
		s->setValue("SDEBusSpeed", "NORMAL");
	}
	else if (speed == SLOW)
	{
		s->setValue("SDEBusSpeed", "SLOW");
	}
	else if (speed == VERYSLOW)
	{
		s->setValue("SDEBusSpeed", "VERYSLOW");
	}
	else if (speed == ULTRASLOW)
	{
		s->setValue("SDEBusSpeed", "ULTRASLOW");
	}
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


int E2Profile::GetIMBusSpeed()
{
	QString sp = s->value("IMBusSpeed", "NORMAL").toString();
	int rval = NORMAL;              //Default speed

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


void E2Profile::SetIMBusSpeed(int speed)
{
	if (speed == TURBO)
	{
		s->setValue("IMBusSpeed", "TURBO");
	}
	else if (speed == FAST)
	{
		s->setValue("IMBusSpeed", "FAST");
	}
	else if (speed == NORMAL)
	{
		s->setValue("IMBusSpeed", "NORMAL");
	}
	else if (speed == SLOW)
	{
		s->setValue("IMBusSpeed", "SLOW");
	}
	else if (speed == VERYSLOW)
	{
		s->setValue("IMBusSpeed", "VERYSLOW");
	}
	else if (speed == ULTRASLOW)
	{
		s->setValue("IMBusSpeed", "ULTRASLOW");
	}
}


int E2Profile::GetPowerUpDelay()
{
	QString sp = s->value("PowerUpDelay", "200").toString();
	int rval = 200;         //Default: 200 msec

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetPowerUpDelay(int delay)
{
	if (delay > 0)
	{
		s->setValue("PowerUpDelay", QString::number(delay));
	}
}


int E2Profile::GetAVRProgDelay()
{
	QString sp = s->value("AVRByteWriteDelay", "20").toString();
	int rval = 20;          //Default: 20 msec

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetAVRProgDelay(int delay)
{
	if (delay > 0)
	{
		s->setValue("AVRByteWriteDelay", QString::number(delay));
	}
}


int E2Profile::GetFontSize()
{
	QString sp = s->value("FontSize", "9").toString();
	int rval = 9;          //Default: 20 msec

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetFontSize(int sz)
{
	if (sz > 0)
	{
		s->setValue("FontSize", QString::number(sz));
	}
}


int E2Profile::GetAVREraseDelay()
{
	QString sp = s->value("AVREraseDelay", "50").toString();
	int rval = 50;          //Default: 50 msec

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetAVREraseDelay(int delay)
{
	if (delay > 0)
	{
		s->setValue("AVREraseDelay", QString::number(delay));
	}
}


int E2Profile::GetMDAProgDelay()
{
	QString sp = s->value("MDAWriteDelay", "30").toString();
	int rval = 30;          //Default: 30 msec

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetMDAProgDelay(int delay)
{
	return s->setValue("MDAWriteDelay", QString::number(delay));
}


int E2Profile::GetNVMProgDelay()
{
	QString sp = s->value("NVMWriteDelay", "30").toString();
	int rval = 30;          //Default: 30 msec

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetNVMProgDelay(int delay)
{
	return s->setValue("NVMWriteDelay", QString::number(delay));
}


unsigned long E2Profile::GetSerialNumVal()
{
	QString sp = s->value("SerialNumberVal", "0").toString();
	unsigned long rval = 0;         //Default 0

	if (sp.length())
	{
		rval = sp.toULong();
	}

	return rval;
}


void E2Profile::SetSerialNumVal(unsigned long val)
{
	if (val > 0)
	{
		s->setValue("SerialNumberVal", QString::number(val));
	}
}


int E2Profile::GetSerialNumAddress(long &start, int &size, bool &mtype)
{
	QString sp;
	bool ok;

	start = 0;
	size = 1;
	mtype = false;

	if ((sp = s->value("SerialNumberAddr", "0").toString()).length())
	{
		start = sp.toLong(&ok, 0);

		if (!ok)
		{
			start = 0;
		}
	}

	if ((sp = s->value("SerialNumberSize", "1").toString()).length())
	{
		size = sp.toInt();
	}

	if ((sp = s->value("SerialNumberType", "PROG").toString()).length())
	{
		if (sp == "DATA")
		{
			mtype = true;
		}
	}

	return OK;
}


void E2Profile::SetSerialNumAddress(unsigned long start, int size, bool mtype)
{
//	if (start >= 0)
	{
		s->setValue("SerialNumberAddr", QString::number(start));
	}

	if (size >= 1)
	{
		s->setValue("SerialNumberSize", QString::number(size));
	}

	s->setValue("SerialNumberType", mtype ? "DATA" : "PROG");
}


FmtEndian E2Profile::GetSerialNumFormat()
{
	QString sp = s->value("SerialNumberFormat", "BIGENDIAN").toString();

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
	if (fmt == FMT_BIG_ENDIAN)
	{
		s->setValue("SerialNumberFormat", "BIGENDIAN");
	}
	else
	{
		s->setValue("SerialNumberFormat", "LITTLEENDIAN");
	}
}


bool E2Profile::GetSerialNumAutoInc()
{
	QString sp = s->value("SerialNumAutoIncrement", "YES").toString();

	if (sp.length() && (sp == "NO"))
	{
		return false;
	}
	else
	{
		return true;
	}
}


void E2Profile::SetSerialNumAutoInc(bool val)
{
	if (val)
	{
		s->setValue("SerialNumAutoIncrement", "YES");
	}
	else
	{
		s->setValue("SerialNumAutoIncrement", "NO");
	}
}


long E2Profile::GetProgramOptions()
{
	long res;
	QString rval;

	res = 0;
	rval = s->value("ReloadOption", "").toString();

	if (rval.length())
		if (rval != "NO")
		{
			res |= RELOAD_YES;
		}

	rval = s->value("ReadFlashOption", "").toString();

	if (rval.length())
		if (rval != "NO")
		{
			res |= READFLASH_YES;
		}

	rval = s->value("ReadEEpromOption", "").toString();

	if (rval.length())
		if (rval != "NO")
		{
			res |= READEEP_YES;
		}

	rval = s->value("ByteSwapOption", "").toString();

	if (rval.length())
		if (rval != "NO")
		{
			res |= BYTESWAP_YES;
		}

	rval = s->value("SetIDkeyOption", "").toString();

	if (rval.length())
		if (rval != "NO")
		{
			res |= SETID_YES;
		}

	rval = s->value("ReadOscCalibration", "").toString();

	if (rval.length())
		if (rval != "NO")
		{
			res |= READOSCAL_YES;
		}

	rval = s->value("EraseOption", "").toString();

	if (rval.length())
		if (rval != "NO")
		{
			res |= ERASE_YES;
		}

	rval = s->value("WriteFlashOption", "").toString();

	if (rval.length())
		if (rval != "NO")
		{
			res |= FLASH_YES;
		}

	rval = s->value("WriteEEpromOption", "").toString();

	if (rval.length())
		if (rval != "NO")
		{
			res |= EEPROM_YES;
		}

	rval = s->value("WriteSecurityOption", "").toString();

	if (rval.length())
		if (rval != "NO")
		{
			res |= LOCK_YES;
		}

	return res;
}


void E2Profile::SetProgramOptions(long prog_option)
{
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
	QString sp = s->value("CurrentLang", "english").toString();

	return sp;
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

#if 0
QString E2Profile::GetLockDir()
{
	QString sp = s->value("ttyLockDir",  "/var/lock").toString();

	return sp;
}


void E2Profile::SetLockDir(const QString &name)
{
	if (name.length())
	{
		s->setValue("ttyLockDir", name);
	}
}

QString E2Profile::GetDevDir()
{
	QString sp = s->value("ttyDevDir", "/dev").toString();

	return sp;
}

void E2Profile::SetDevDir(const QString &name)
{
	if (name.length())
	{
		s->setValue("ttyDevDir", name);
	}
}
#endif

#ifdef __linux__

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
		qDebug() << "Can't open file /proc/tty/drivers";
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
		qDebug() << "Can't open file /proc/tty/drivers";
	}

	return lst;
}

#endif

QString E2Profile::GetCOMDevName()
{
	QString sp = s->value("COMDevName", "").toString();

	if (sp.length() == 0)
	{
#ifdef __linux__
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
		s->setValue("COMDevName", name);
	}
}


QStringList E2Profile::GetCOMDevList()
{
	QStringList lst = s->value("COMDevList", QStringList()).toStringList();

	if (lst.count() == 0)
	{
#ifdef __linux__
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
	s->setValue("COMDevList", lst);
}


QString E2Profile::GetLPTDevName()
{
	QString sp = s->value("LPTDevName", "").toString();

	if (sp.length() == 0)
	{
#ifdef __linux__
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
		s->setValue("LPTDevName", name);
	}
}


QStringList E2Profile::GetLPTDevList()
{
	QStringList lst = s->value("LPTDevList", QStringList()).toStringList();

	if (lst.count() == 0)
	{
		QString sname = E2Profile::GetLPTDevName();
#ifdef __linux__
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
	s->setValue("LPTDevList", lst);
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
	QString sp = s->value("LogEnabled", "").toString();

	if (sp.length() && (sp == "YES"))
	{
		return true;
	}
	else
	{
		return false;
	}
}


void E2Profile::SetLogEnabled(bool enabled)
{
	if (enabled)
	{
		s->setValue("LogEnabled", "YES");
	}
	else
	{
		s->setValue("LogEnabled", "NO");
	}
}


bool E2Profile::GetSkipStartupDialog()
{
	QString sp = s->value("SkipStartupDialog", "").toString();

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
	if (skip)
	{
		s->setValue("SkipStartupDialog", "YES");
	}
	else
	{
		s->setValue("SkipStartupDialog", "NO");
	}
}


bool E2Profile::GetClearBufBeforeLoad()
{
	QString sp = s->value("ClearBufferBeforeLoad").toString();

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
	if (enabled)
	{
		s->setValue("ClearBufferBeforeLoad", "YES");
	}
	else
	{
		s->setValue("ClearBufferBeforeLoad", "NO");
	}
}


bool E2Profile::GetClearBufBeforeRead()
{
	QString sp = s->value("ClearBufferBeforeRead", "").toString();

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
	if (enabled)
	{
		s->setValue("ClearBufferBeforeRead", "YES");
	}
	else
	{
		s->setValue("ClearBufferBeforeRead", "NO");
	}
}


bool E2Profile::GetAt89PageOp()
{
	QString sp = s->value("AT89SPageOpEnabled", "").toString();

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
	if (enabled)
	{
		s->setValue("AT89SPageOpEnabled", "YES");
	}
	else
	{
		s->setValue("AT89SPageOpEnabled", "NO");
	}
}


bool E2Profile::Get8253FallEdge()
{
	QString sp = s->value("AT89S8253FallEdgeSampling", "").toString();

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
	if (enabled)
	{
		s->setValue("AT89S8253FallEdgeSampling", "YES");
	}
	else
	{
		s->setValue("AT89S8253FallEdgeSampling", "NO");
	}
}


bool E2Profile::GetSoundEnabled()
{
	QString sp = s->value("SoundEnabled", "").toString();

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
	if (enabled)
	{
		s->setValue("SoundEnabled", "YES");
	}
	else
	{
		s->setValue("SoundEnabled", "NO");
	}
}


int E2Profile::GetCalibrationAddress(bool &enabled, long &start, int &size, bool &mtype)
{
	QString sp;
	bool ok;

	enabled = true;
	start = 0;
	size = 1;
	mtype = false;

	if ((sp = s->value("OscCalibrationEnabled", "NO").toString()).length())
	{
		if (sp == "NO")
		{
			enabled = false;
		}
	}

	if ((sp = s->value("OscCalibrationAddr", "0").toString()).length())
	{
		start = sp.toLong(&ok, 0);

		if (!ok)
		{
			start = 0;
		}
	}

	if ((sp = s->value("OscCalibrationSize", "1").toString()).length())
	{
		size = sp.toInt();
	}

	if ((sp = s->value("OscCalibrationMemType", "PROG").toString()).length())
	{
		if (sp == "DATA")
		{
			mtype = true;
		}
	}

	return OK;
}


void E2Profile::SetCalibrationAddress(bool enabled, unsigned long start, int size, bool mtype)
{
	s->setValue("OscCalibrationEnabled", enabled ? "YES" : "NO");

	//      if (start >= 0)
	{
		s->setValue("OscCalibrationAddr", QString::number(start));
	}

	if (size >= 1)
	{
		s->setValue("OscCalibrationSize", QString::number(size));
	}

	s->setValue("OscCalibrationType", mtype ? "DATA" : "PROG");
}


int E2Profile::GetJDMCmd2CmdDelay()
{
	QString sp = s->value("JDM-CmdToCmdDelay", "4000").toString();
	int rval = 4000;                //Default: 4000 usec

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetJDMCmd2CmdDelay(int delay)
{
	if (delay >= 0)
	{
		s->setValue("JDM-CmdToCmdDelay", QString::number(delay));
	}
}


bool E2Profile::GetVerifyAfterWrite()
{
	QString sp = s->value("VerifyAfterWrite", "YES").toString();

	if (sp.length() && (sp == "NO"))
	{
		return false;
	}
	else
	{
		return true;
	}
}


void E2Profile::SetVerifyAfterWrite(bool enabled)
{
	if (enabled)
	{
		s->setValue("VerifyAfterWrite", "YES");
	}
	else
	{
		s->setValue("VerifyAfterWrite", "NO");
	}
}


bool E2Profile::GetAutoDetectPorts()
{
	QString sp = s->value("AutoDetectPorts", "YES").toString();

	if (sp.length() && (sp == "NO"))
	{
		return false;
	}
	else
	{
		return true;
	}
}


void E2Profile::SetAutoDetectPorts(bool enabled)
{
	if (enabled)
	{
		s->setValue("AutoDetectPorts", "YES");
	}
	else
	{
		s->setValue("AutoDetectPorts", "NO");
	}
}


int E2Profile::GetCOMAddress(int &com1, int &com2, int &com3, int &com4)
{
	QString sp = s->value("COMPorts").toString();

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


void E2Profile::SetCOMAddress(int com1, int com2, int com3, int com4)
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

		s->setValue("COMPorts", str);
	}
}


int E2Profile::GetLPTAddress(int &lpt1, int &lpt2, int &lpt3)
{
	QString sp = s->value("LPTPorts").toString();

	lpt1 = 0x378;
	lpt2 = 0x278;
	lpt3 = 0x3BC;

	if (sp.length())
	{
		sscanf(sp.toLatin1().constData(), "%x,%x,%x", &lpt1, &lpt2, &lpt3);
	}

	return OK;
}


void E2Profile::SetLPTAddress(int lpt1, int lpt2, int lpt3)
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

		s->setValue("LPTPorts", str);
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


//RaspberryPi default pins
#define DEF_GPIO_CTRL                   23  //Rst pin 16
#define DEF_GPIO_DATAIN                 27  //Miso pin 13
#define DEF_GPIO_DATAOUT                17  //Mosi pin 11
#define DEF_GPIO_CLOCK                  24  //Clock pin 18

int E2Profile::GetGpioPinCtrl()
{
	QString sp;
	int rval = DEF_GPIO_CTRL;               //Default pin number

	sp = s->value("GpioPinCtrl", "").toString();

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetGpioPinCtrl(int pin)
{
	s->setValue("GpioPinCtrl", QString::number(pin));
}


int E2Profile::GetGpioPinClock()
{
	QString sp;
	int rval = DEF_GPIO_CLOCK;              //Default pin number

	sp = s->value("GpioPinClock").toString();

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetGpioPinClock(int pin)
{
	s->setValue("GpioPinClock", QString::number(pin));
}


int E2Profile::GetGpioPinDataIn()
{
	QString sp;
	int rval = DEF_GPIO_DATAIN;             //Default pin number

	sp = s->value("GpioPinDataIn").toString();

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetGpioPinDataIn(int pin)
{
	s->setValue("GpioPinDataIn", QString::number(pin));
}


int E2Profile::GetGpioPinDataOut()
{
	QString sp;
	int rval = DEF_GPIO_DATAOUT;    //Default pin number

	sp = s->value("GpioPinDataOut").toString();

	if (sp.length())
	{
		rval = sp.toInt();
	}

	return rval;
}


void E2Profile::SetGpioPinDataOut(int pin)
{
	s->setValue("GpioPinDataOut", QString::number(pin));
}

bool E2Profile::GetEditBufferEnabled()
{
	return !(s->value("Editor/ReadOnlyMode", false).toBool());
}

void E2Profile::SetEditBufferEnabled(bool enable)
{
	s->setValue("Editor/ReadOnlyMode", !enable);
}
