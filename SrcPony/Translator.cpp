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

#include <QDir>
#include <QDebug>
#include <QTextStream>
#include <QMessageBox>

// #include "MainWindow.h"
#include "Translator.h"


QVector<QString> cTranslator::translateTable;


QString cTranslator::engText[] =
{
	"english",

	//Fill this with your reference, if it's empty = ""it will be invisible
	"Translator Credits:",
	"",

	"Serial Device Programmer",
	"You can find last version of",
	"at the address:",

	"No Name",

	//Button captions
#ifdef  Q_OS_WIN32
	"&OK",
	"&Cancel",
	"&Help",
	"&Probe",
#else
	"OK",
	"Cancel",
	"Help",
	"Probe",
#endif

	"Hardware Check",

	"Note",
	"Edit",


	//Dialog messages
	"Version",
	"Yes",
	"No",
	"Ok",
	"Test",
	"Failed",
	"About",
	"Page",
	"Unknown",

	"Buffer \"%s\" changed. Save it before closing?",
	"Buffer changed. Save it before opening?",
	"Nothing to save",
	"Nothing to load",
	"Nothing to print",
	"Nothing to write",
	"Nothing to verify",
	"Bus timing calibration.\nBe sure there are no application running other than ",
	"\n(the CPU and hard disk have to be idle)\nThe calibration may take a couple of seconds.\nDo you want to run calibration now?",
	"Calibration OK",
	"Calibration failed",
	"You need to run Calibration from the Options menu\nbefore any read/write operations",
	"You need to run Setup from the Options menu\nbefore any read/write operations",
	"Reading...",
	"Read successful\nDevice Size:",
	"Are you sure you want to write the device?\nAll previous content will be lost",
	"Writing...",
	"Verifying...",
	"Write successful",
	"Write failed",
	"Erasing...",
	"Erase successful",
	"Cannot verify, device missing or bad device",
	"Verify Failed",
	"Verify successful",
	"Program succesful",
	"Program Failed",
	"Device reset",
	"Writing security bits...",
	"Security bits write failed",
	"Reading security bits...",
	"Security bits read failed",
	"Reading configuration bits...",
	"Configuration bits read failed",
	"Writing configuration bits...",
	"Configuration bits write failed",

	"Script error at line",
	"Bad command",
	"Bad argument",
	"Argument missing",
	"Do you want to continue?",

	"Buffer empty",
	"Operation not supported",
	"Insert parameters",
	"Bad parameters",
	"Accepting commands...",
	"Open device content file",
	"Open program (FLASH) content file",
	"Open data (EEPROM) content file",
	"File not found",
	"File save failed",
	"Save Device Content File As",
	"Save Program (FLASH) Content File As",
	"Save Data (EEPROM) Content File As",
	"Close last window: do you want to exit?",
	"Buffer changed. Save it before closing?",
	"Open script file",

	//Menu entries

	//Main Menu
	"&File",
	"&Edit",
	"&Device",
	"&Command",
	"Scri&pt",
	"&Utility",
	"&Setup",
	"&Options",
	"&?",

	//Menu File
	"&New Window",
	"&Open Device File...",
	"Open &Program (FLASH) File...",
	"Open &Data (EEPROM) File...",
	"&Save Device File",
	"Save Device File &As...",
	"Save Program (&FLASH) File As...",
	"Save Data (&EEPROM) File As...",
	"&Reload Files",
	"Prin&t...",
	"&Close",
	"E&xit",

	//Menu Edit
	"Edit &Note...",
	"Edit Bu&ffer enabled",

	//Menu Command
	"&Read All",
	"Read Program (FLASH)",
	"Read Data (EEPROM)",
	"Read &Security and Configuration Bits",
	"&Write All",
	"Write Program (&FLASH)",
	"Write Data (EEPROM)",
	"Wr&ite Security and Configuration Bits",
	"&Verify All",
	"Verify Program (FLASH)",
	"Verify Data (EEPROM)",
	"Verif&y Security and Configuration Bits",
	"&Erase",
	"&GetInfo",
	"Rese&t",
	"&Program",
	"Program Options...",

	//Menu Options
	"&Interface Setup...",
	"&Calibration",
	"&Remote Mode",

	//Menu Utility
	"&Clear Buffer",
	"&Fill Buffer...",
	"&Double Bank",
	"&Byte Swap",
	"Set Serial&Number",
	"SerialNumber C&onfig...",

	//Menu Help
	"&Help",
	"&About",

	//Menu Options
	"&Log File...",

	//Menu Script
	"&Edit...",
	"&Load and Run...",
	"&Auto Load",
	"&Run",

	//Tool Bar

	//Dialog Messages
	"Dev.Type",

	//Tool tips
	"Open Device File",
	"Save Device File",
	"Print Buffer",
	"Reload Files",
	"Read Device",
	"Read Program Memory (FLASH)",
	"Read Data Memory (EEPROM)",
	"Read Security and Configuration Bits",
	"Write Device",
	"Write Program Memory (FLASH)",
	"Write Data Memory (EEPROM)",
	//      STR_TTWRITESEC  = "Write Security and Configuration Bits"
	"Verify Program Memory (FLASH)",
	"Verify Data Memory (EEPROM)",
	"Verify Security and Configuration Bits",

	"Select device family",
	"Select device type",

	"Edit device notes",


	//Error Messages
	"Device Not Responding",
	"Bad device, select the correct type",
	"Device missing or unknown device",
	"Unable to probe, bad device or device locked",
	"User abort",
	"Communication port not available",
	"I/O access denied. You must be root to perform I/O",
	"Programmer board not found",
	"Hardware error, is the board properly connected?",
	"Bus busy or hardware error",
	"Missing Acknowledge from the device",
	"Missing device",
	"Timeout on the bus",
	"I2CBus Stop condition error (bad timing?)",
	"Write error",
	"Blank check error, need to erase",
	"Error n.",
	"Script error",


	//----Setup dialog

	//Dialog title
	"I/O port setup",
	"Interface board Setup",

	//Dialog messages
	"COM/LPT interface",
	"Serial",
	"Parallel",
	"Interface select",
	"Interface type",
	"COM Port select",
#ifdef Q_OS_WIN32
	"COM",
	// STR_LBLCOM2             = "COM2",
	// STR_LBLCOM3             = "COM3",
	// STR_LBLCOM4             = "COM4",
#else
	"ttyS",
	// STR_LBLCOM2             = "ttyS1",
	// STR_LBLCOM3             = "ttyS2",
	// STR_LBLCOM4             = "ttyS3",
#endif
	"USB Port select",
#ifdef Q_OS_WIN32
	"USB",
	// STR_LBLUSB2             = "USB2",
	// STR_LBLUSB3             = "USB3",
	// STR_LBLUSB4             = "USB4",
	// STR_LBLUSB5             = "USB5",
	// STR_LBLUSB6             = "USB6",
	// STR_LBLUSB7             = "USB7",
	// STR_LBLUSB8             = "USB8",
#else
	"ttyUSB",
	// STR_LBLUSB2             = "ttyS1",
	// STR_LBLUSB3             = "ttyS2",
	// STR_LBLUSB4             = "ttyS3",
	// STR_LBLUSB5             = "ttyS4",
	// STR_LBLUSB6             = "ttyS5",
	// STR_LBLUSB7             = "ttyS6",
	// STR_LBLUSB8             = "ttyS7",
#endif
	"LPT Port select",
	"LPT",
	// STR_LBLLPT2             = "LPT2",
	// STR_LBLLPT3             = "LPT3",
	// STR_LBLLPT4             = "LPT4",
	"Select Polarity of the Control lines",
	"Invert Reset",
	"Invert SCKL",
	"Invert D-IN",
	"Invert D-OUT",

	//Tool tips

	"Select serial port COM1",
	// STR_TTCOM2              = "Select serial port COM2",
	// STR_TTCOM3              = "Select serial port COM3",
	// STR_TTCOM4              = "Select serial port COM4",
	"Select serial port USB1",
	//  STR_TTUSB2              = "Select serial port USB2",
	//  STR_TTUSB3              = "Select serial port USB3",
	//  STR_TTUSB4              = "Select serial port USB4",
	//  STR_TTUSB5              = "Select serial port USB5",
	//  STR_TTUSB6              = "Select serial port USB6",
	//  STR_TTUSB7              = "Select serial port USB7",
	//  STR_TTUSB8              = "Select serial port USB8",
	"Select parallel port LPT1",
	// STR_TTLPT2              = "Select parallel port LPT2",
	// STR_TTLPT3              = "Select parallel port LPT3",


	//----Fill dialog

	//Dialog title
	"Fill Buffer",

	//Dialog messages: NB should be of the same size (number of char)
	"From",
	"To",
	"Value",



	//----Fuse dialog

	//Button captions
#ifdef  Q_OS_WIN32
	"C&lear All",
	"&Set All",
	"&Write",
	"&Read",
#else
	"Clear All",
	"Set All",
	"Write",
	"Read",
#endif

	//Dialog title
	"Configuration and Security bits",

	//Dialog messages: NB should be of the same size (number of char)
	"First Block to Lock",
	"N.of Blocks to Lock",
	"High endurance block",

	//Tool tips
	"All bits to '0'",
	"All bits to '1'",
	"Write bits to the device",
	"Read bits from the device",


	//----Info dialog

	//Button captions
#ifdef  Q_OS_WIN32
	"&Close",
#else
	"Close",
#endif

	//Dialog title
	"Device Info",

	//Dialog messages
	"Bank roll-over capability:",
	"Size:",
	"CRC:",
	"Security block:",
	"High endurance block:",


	//----Char Edit dialog

	//Dialog title
	"Edit Buffer",

	//Dialog messages: NB should be of the same size (number of char)
	"Hex",
	"Decimal",
	"Char",

	"Insert the new value",
	"Enter text from the address",

	//Tool tips
	"Hexadecimal value",
	"Decimal value",
	"Char value",


	//----Program Option dialog

	//Dialog title
	"Program Options",

	//Dialog messages
	"Reload Files",
	"Read Program memory (FLASH)",
	"Read Data memory (EEPROM)",
	"Read Seriial Number",
	"Byte Swap",
	"Set Serial Number",
	"Erase",
	"Write Program memory (FLASH)",
	"Write Data memory (EEPROM)",
	"Write Security and Config bits",
	"Verify Security and Config bits",



	//----Serial Number dialog

	//Dialog title
	"Serial Number Config",

	//Dialog messages: NB should be of the same size (number of char)
	"Address",
	"Size",
	"Value",
	"Format",

	//next 2 probably can't be translated
	"Little endian",
	"Big endian",

	"Data memory offset",

	//Tool tips

	"Memory address",
	"Size in bytes",
	"Next value to be used",
	"Assume the Address relative to Data memory instead of Program memory",
	"Most significant Byte at high address",
	"Most significant Byte at low address",


	//----Retry dialog

	//Button captions
#ifdef  Q_OS_WIN32
	"&Abort",
	"&Retry",
	"&Ignore",
#else
	"Abort",
	"Retry",
	"Ignore",
#endif

	//Dialog title
	"Alert",


	//----Notes dialog

	//Dialog title
	"Edit Device Notes",


	//----Progress dialog

	//Dialog title
	"Status",


	//----Hardware check dialog
	"Hardware Check",


	//----About dialog
	"Disable sound",

	"Don't show this window on startup",


	//---- Oscillator Calibration Byte
	"Oscillator Calibration Byte",

	"Read Osc.Calibration Byte",
	"Osc. Calibration Options...",
	"Read Osc.Calibration Byte successful",

	"Chip Id",
	"Note",
	"EEPROM Size",
	"FLASH Size",

	"Auto Increment",
	"Increment the serial number after every 'set serial number' operation",

	"Can't know bank roll-over without modify and write, do you want to write?",
	"Buffer changed. Save it before reading?",

	"Open Program Memory (FLASH) File",
	"Open Data Memory (EEPROM) File",
	"Save Program Memory (FLASH) File",
	"Save Data Memory (EEPROM) File",
	"New Window",
	"Launch program cycle",
	"Execute script",
	"Open setup dialog",
	"Erase all the device to FF",

	"Security and Configuration &Bits...",
	"Security and Configuration Bits",

	"Refer to device datasheet, please",
	"Checked items means programmed",
	"UnChecked items means unprogrammed",
	"Save the result in the buffer at given location",
	""
};


bool cTranslator::loadTranslation(const QString fname)
{
	QFile langFile(fname);
	short vecSize = (short)(sizeof(engText) / sizeof(char *));
	translateTable.clear();
	translateTable.resize(vecSize + 1);

	if (!langFile.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QTextStream stream(&langFile);
	stream.setCodec("UTF-8");

	QString llEng = stream.readLine();

	if (llEng.length() > 0)
	{
		llEng.remove("\n");
	}

	if (llEng == "LANGUAGE_NAME")
	{
		translateTable << llEng;
	}
	else
	{
		QMessageBox::warning(NULL, "File error", "File is not a language file!", QMessageBox::Close);

		langFile.close();
		return (false);
	}

	bool begTranslation = false;

	while (!stream.atEnd())
	{
		llEng = stream.readLine();

		if (llEng == "LANGUAGE_CHARSET")
		{
			llEng = stream.readLine();
			stream.setCodec(llEng.toLatin1());
			continue;
		}

		QString tmpText = convertString(llEng);

		// when at end of string
		if (tmpText.lastIndexOf("\\") >= tmpText.length() - 2)
		{
			llEng += tmpText;
			//             qDebug() << tmpText;
			continue;
		}
		else
		{
			llEng = tmpText;
		}

		if (llEng.indexOf(QRegExp("t[0-9]+=")) == 0)
		{
			int num, pos;
			QString str;
			pos = llEng.indexOf("=");

			if (pos > 0)
			{
				str = llEng.mid(pos + 1);
				//
				num = llEng.mid(1, pos - 1).toShort();

				//                     qDebug() << num << str;
				if (num >= vecSize)
				{
					qDebug() << "size too big";
				}
				else
				{
					translateTable[num] = str;
				}
			}
		}
		else
		{
			if (llEng == "Ok")   // translation part beginns with "Ok"
			{
				begTranslation = true;
			}

			if (begTranslation == false)
			{
				continue;
			}

			bool found = false;
			QString llTranslate = stream.readLine();

			if (llTranslate.length() == 0)
			{
				continue;
			}

			llTranslate = convertString(llTranslate);

			for (int i = 0; i < vecSize; i++)  // suche
			{
				QString tmpEng = engText[i];

				if (tmpEng.indexOf("%HOME%") >= 0)
				{
					tmpEng.replace("%HOME%", QDir::homePath());
				}

				if (tmpEng == llEng)
				{
					translateTable[i] = llTranslate;
					found = true;
					break;
				}
			}

			if (found == false)
			{
				qDebug() << "no translation found" << llEng;
			}

		}
	}

	langFile.close();

	return (true);
}


QString cTranslator::convertString(const QString &s)
{
	QString res = s;

	if (res.length() > 0)
	{
		res.replace("\\r\\n", "\n");
		res.replace("\\n", "\n");
		res.replace("\\t", "\t");
		res.replace("\\\\", "/");
		res.replace("&amp;", "&");
		res.replace("&gt;", ">");
		res.replace("&lt;", "<");
	}

	if (res.length() == 0)
	{
		return res;
	}

	if (res.indexOf("C:/") >= 0)
	{
		res.replace("C:/", "%HOME%/");
	}

	if (res.indexOf("c:/") >= 0)
	{
		res.replace("c:/", "%HOME%/");
	}

	if (res.indexOf("%HOME%") >= 0)
	{
		res.replace("%HOME%", QDir::homePath());
	}

	return res;
}


QString cTranslator::translate(int id)
{
	QString m;

	if (id < 0)
	{
		return "";
	}

	if (id >= (int)(sizeof(engText) / sizeof(char *)))
	{
		return "";
	}

	if (translateTable.size() == 0)
	{
		return QString(engText[id]);
	}

	//     if (id < (int)(sizeof(engText) / sizeof(char*))) {
	if (translateTable[id].length() == 0)
	{
		m = QString(engText[id]);
	}
	else
	{
		m = translateTable[id];
	}

	if (m.indexOf("%HOME%") >= 0)
	{
		m.replace("%HOME%", QDir::homePath());
	}

	//     }

	return m;
}


