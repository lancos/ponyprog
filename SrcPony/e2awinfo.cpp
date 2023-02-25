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


#include "errcode.h"

#include "e2cmdw.h"
#include "e2profil.h"
#include "e2awinfo.h"           // Header file

#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QtCore>

e2AppWinInfo::e2AppWinInfo(e2CmdWindow *p, const QString &name, BusIO **busvptr) :
	hex_per_line(16),
	buffer_size(BUFFER_SIZE),
	load_type(ALL_TYPE),
	save_type(ALL_TYPE),
	load_relocation(0),
	save_relocation(0),
	buf_ok(false),
	buf_changed(false),
	//              fname(0),
	no_block(0),
	splitted(0),
	roll_over(0),
	fuse_bits(0),
	lock_bits(0),
	fuse_ok(false),
	crc(0)
{
	qDebug() << Q_FUNC_INFO;

	fname = "";

	QString resXml = readXmlDir();
	if (resXml != "")
	{
		QMessageBox msgBox(QMessageBox::Critical, "Error", resXml, QMessageBox::Close);
		msgBox.setStyleSheet(cmdWin->getStyleSheet());
		msgBox.setButtonText(QMessageBox::Close, "Close");
		msgBox.exec();
		exit(1);
	}

	//qDebug() << "e2awinfo" << p << this;

	//p->SetAWInfo(this);

	eep24xx = new E24xx(this, busvptr[I2C - 1]);
	eep2401 = new mE2401(this, busvptr[I2C - 1]);

	eep24xx1 = new E24xx1(this, busvptr[I2C - 1]);
	eep24xx2 = new E24xx2(this, busvptr[I2C - 1]);
	eep24xx5 = new E24xx5(this, busvptr[I2C - 1]);
	eepAt90s = new At90sxx(this, busvptr[AT90S - 1]);
	eepAt89s = new At89sxx(this, busvptr[AT89S - 1]);
	eep93xx16 = new At93cxx(this, busvptr[AT93C - 1]);
	eep93xx8 = new At93cxx8(this, busvptr[AT93C - 1]);
	eepPic16 = new Pic16xx(this, busvptr[PICB - 1]);
	eep250xx = new At250xx(this, busvptr[AT250 - 1]);
	eep25xxx = new At25xxx(this, busvptr[AT250BIG - 1]);
	eep2506 = new Sde2506(this, busvptr[SDEB - 1]);
	eepPic168xx = new Pic168xx(this, busvptr[PICNEWB - 1]);
	eep3060 = new Nvm3060(this, busvptr[IMBUS - 1]);
	eepPic125xx = new Pic125xx(this, busvptr[PIC12B - 1]);
	eep17xxx  = new At17xxx(this, busvptr[I2C - 1]);
	eep2444 = new X2444(this, busvptr[X2444B - 1]);
	eep2430 = new X2444(this, busvptr[S2430B - 1]);

	SetFileName(name);

	//Initialize File Formats vector
	fbufvet[E2P] = &e2pfbuf;
	fbufvet[INTEL] = &intfbuf;
	fbufvet[MOTOS] = &motsfbuf;             //** 17/03/99
	fbufvet[BIN] = &binfbuf;
	fbufvet[CSM] = &csmfbuf;

	for (int k = 0; k < NO_OF_FILETYPE; k++)
	{
		fbufvet[k]->SetAWInfo(this);
	}

	eeprom_string = "";						//eeprom string ID
	eeprom_comment = "";					//eeprom comment

	ClearBuffer();                          //Clear the new buffer

	QString nm = E2Profile::GetLastDevType();
	quint32 type = GetTypeFromString(nm);
	SetId(type);

	SetFileBuf(E2Profile::GetDefaultFileType());       //      SetFileBuf(E2P);

	SetLoadAutoClearBuf(E2Profile::GetClearBufBeforeLoad());

	// Test and initialize the hardware
	{
		int err;
		//              QMessageBox::note(win);

		if (!E2Profile::GetSkipStartupDialog())
		{
			if (!cmdWin->scriptMode)
			{
				cmdWin->About();
			}
		}

		err = cmdWin->LoadDriver(1);

		if (err != OK)
		{
			//TODO: translate message
			QMessageBox msgBox(QMessageBox::Critical, "Error", "Load I/O driver failed.", QMessageBox::Ok);
			msgBox.setStyleSheet(cmdWin->getStyleSheet());
			msgBox.setButtonText(QMessageBox::Ok, "Close");
			msgBox.exec();
		}

		//imposta il bus iniziale (relativo al tipo di eeprom)
		cmdWin->SetInitialBus(eep->GetBus());

		//case of command line parameter
		if (GetFileName().length())
		{
			if (Load() <= 0)
			{
				SetFileName("");
			}
		}
		else	//Why we call OpenPort only if name is "" ???
		{
			err = cmdWin->OpenPort();

			if (err == E2ERR_ACCESSDENIED)
			{
#ifdef Q_OS_WIN32
				QString msg = "I/O access denied. Driver not found, try to install the software again";
#else
				QString msg = "I/O access denied. Run as root, or change the interface";
#endif
				//TODO: translate message
				QMessageBox msgBox(QMessageBox::Critical, "Error", msg, QMessageBox::Ok);
				msgBox.setStyleSheet(cmdWin->getStyleSheet());
				msgBox.setButtonText(QMessageBox::Ok, "Close");
				msgBox.exec();
			}

			cmdWin->ClosePort();
		}
	}
}


e2AppWinInfo::~e2AppWinInfo()
{
	qDebug() << Q_FUNC_INFO;

	// Destructor
	fname = "";
}



/**
 * @brief scan the directory with xml files
 *
 */
QString e2AppWinInfo::readXmlDir()
{
	bool found = false;
	QString xmlDirName;
	QStringList dirsXml;
	QDir dir;
	QStringList xmlList;
	// EK 2020
	// this for linux
	QString path = QDir::currentPath();
	QString bdir = "/build";
	if (path.endsWith(bdir))
	{
		int pos = path.lastIndexOf(bdir);
		path.remove(pos, bdir.length());
	}
	path += "/ics/";

	xmlDirName = E2Profile::GetXmlDir();
	if (xmlDirName.length() == 0)
	{
		xmlDirName = qApp->applicationDirPath() + "/ics/";
	}

	qDebug() << "readXmlDir path:" << path << ", Saved: " << xmlDirName;
	qDebug() <<  Q_FUNC_INFO << " path:" << path << ", Saved: " << xmlDirName;

#ifdef Q_OS_LINUX
	dirsXml << xmlDirName << "/usr/share/ponyprog/ics/" << "/usr/local/share/ponyprog/ics/" << path;
#else
	dirsXml << xmlDirName << path;
#endif

	foreach (const QString entry, dirsXml)
	{
		dir = QDir(entry);

		if (dir.exists() == true)
		{
			xmlList = dir.entryList(QStringList("*.xml"));
			if (xmlList.count() > 0)
			{
				E2Profile::SetXmlDir(entry);
				xmlDirName = entry + "/";
				found = true;
				break;
			}
		}
	}

	if (found == false)
	{
		return ("Xml dir not found!\n\n" + E2Profile::GetXmlDir());
	}

	foreach (const QString iL, xmlList)
	{
		if (ReadConfigFromXml(xmlDirName + iL) == false)
		{
			return ("Parse Error in file:\n\n" + xmlDirName + iL + " !");
		}
	}

	return "";
}


bool e2AppWinInfo::SetFileName(const QString &name)
{
	bool rval = false;

	if (name.length())
	{
		if (name != fname)
		{
			fname = name;
			rval = true;
		}
	}
	else
	{
		fname = "";
	}

	return rval;
}


void e2AppWinInfo::SleepBus()
{
	cmdWin->SleepBus();
}

int e2AppWinInfo::OpenBus()
{
	return cmdWin->OpenBus(eep->GetBus());
}


void e2AppWinInfo::Reset()
{
	cmdWin->OpenBus(eep->GetBus());
	SleepBus();
}

void e2AppWinInfo::SetId(quint32 id)
{
	if (id == 0)
	{
		id = E2400;         //to avoid segV
	}

	eep_id = id;

	//eep_type, eep_subtype are local shadow variables of eep_id
	quint32 eep_type = GetPriType(id);
	quint32 eep_subtype = GetSubType(id);

	qDebug() << "SetEEProm" << (Qt::hex) << eep_type << eep_subtype << (Qt::dec);

	if (eep_subtype == 0)
	{
		eep_subtype = GetFirstFromPritype(eep_type);
	}

	switch (eep_type)
	{
	//AutoTag
	//Setting the device pointer to selected type
	case E24XX:
		eep = eep24xx;
		if (id == E2401_A)
		{
			eep = eep24xx1;
		}

		if (id == E2401_B)
		{
			eep = eep2401;
		}
		break;

	case E24XX2:
		eep = eep24xx2;

		eep->DefaultBankSize();
		break;

	case E24XX5:
		eep = eep24xx5;
		break;

	case AT90SXX:
	case ATtiny:
	case ATmega:
	{
		eep = eepAt90s;

		quint32 xtype = GetId();

		eep->SetProgPageSize(GetTypeWPageSize(xtype), false);
		At90sBus *b = static_cast<At90sBus *>(eep->GetBus());
		b->SetFlashPagePolling((xtype != ATmega603) && (xtype != ATmega103));
		b->SetOld1200Mode((xtype == AT90S1200));
		break;
	}

	case AT89SXX:
	{
		eep = eepAt89s;

		quint32 xtype = GetId();
		qDebug() << (Qt::hex) << xtype << (Qt::dec);
		if (E2Profile::GetAt89PageOp())
		{
			eep->SetProgPageSize(GetTypeWPageSize(xtype), false);        //write prog page size
			eep->SetProgPageSize(GetTypeWPageSize(xtype), true);         //read prog page size
			eep->SetDataPageSize(GetTypeWPageSize(xtype) / 2, false);    //write data page size
			eep->SetDataPageSize(GetTypeWPageSize(xtype) / 2, true);     //read data page size
		}

		At89sBus *b = static_cast<At89sBus *>(eep->GetBus());
		b->SetCompatibilityMode((xtype == AT89S8252 || xtype == AT89S53));
		b->SetFallingPhase((xtype == AT89S8253) && E2Profile::Get8253FallEdge());
		b->SetPagePolling(true, (xtype == AT89S8253 || xtype == AT89S51 || xtype == AT89S52));
		b->SetPagePolling(false, (xtype == AT89S8253));
		break;
	}

	case E93X6:
		eep = eep93xx16;

		break;

	case E93XX_8:
		eep = eep93xx8;

		break;

	case PIC16XX:
		eep = eepPic16;

		break;

	case PIC168XX:
		eep = eepPic168xx;

		break;

	case PIC125XX:
		eep = eepPic125xx;

		break;

	case E250XX:
		eep = eep250xx;

		break;

	case E25XXX:
		eep = eep25xxx;

		break;

	case E2506XX:
		eep = eep2506;

		break;

	case ENVMXXX:
		eep = eep3060;

		break;

	case AT17XXX:
		eep = eep17xxx;

		break;

	case X24C44XX:
		if (GetId() == S24H30)
		{
			eep = eep2430;
		}
		else
		{
			eep = eep2444;
		}

		break;

	default:
		eep = eep24xx;         //20/07/99      -- to prevent crash
		break;
	}

	fuse_ok = false;                //invalidate current fuse settings
	SetSplittedInfo(GetTypeSplit(id));

	//Imposta la nuova dimensione della memoria in
	// base al tipo di eeprom.
	SetNoOfBlock(GetTypeSize(id));

	//Imposta la dimensione del banco che dipende
	// dal tipo di eeprom.
	SetBlockSize(eep->GetBankSize());
}

int e2AppWinInfo::Read(int type, int raise_power, int leave_on)
{
	int probe = !GetSubType(eep_id);
	int rval = OK;

	qDebug() << "e2AppWinInfo::Read(" << type << "," << raise_power << "," << leave_on << ") - IN";

	if (E2Profile::GetClearBufBeforeRead())
	{
		if (load_type == ALL_TYPE)
		{
			ClearBuffer(ALL_TYPE);
		}
		else
		{
			if (load_type & PROG_TYPE)
			{
				ClearBuffer(PROG_TYPE);
			}

			if (load_type & DATA_TYPE)
			{
				ClearBuffer(DATA_TYPE);
			}
		}
	}

	if (raise_power)
	{
		rval = OpenBus();        //Set correct bus and power up
	}

	qDebug() << "e2AppWinInfo::Read() ** OpenBus = " << rval;

	if (rval == OK)
	{
		//              CheckEvents();

		if ((rval = eep->Read(probe, type)) > 0)
		{
			qDebug() << "e2AppWinInfo::Read() ** Read = " << rval;

			SetFileName("");          //avoid to use old name on next save()
			buf_ok = true;
			buf_changed = false;

			//Aggiunto il 18/03/99 con la determinazione dei numeri di banchi nelle E24xx2,
			// affinche` la dimensione rimanga quella impostata bisogna correggere la dimensione
			// del banco.
			SetBlockSize(eep->GetBankSize());
		}

		RecalcCRC();

		if (!(rval > 0 && leave_on))
		{
			SleepBus();
		}
	}

	qDebug() << "e2AppWinInfo::Read() = " << rval << " - OUT";

	return rval;
}

int e2AppWinInfo::Write(int type, int raise_power, int leave_on)
{
	int probe = !GetSubType(eep_id);
	int rval = OK;

	qDebug() << "e2AppWinInfo::Write(" << type << "," << raise_power << "," << leave_on << ") - IN";

	if (raise_power)
	{
		rval = OpenBus();
	}

	if (rval == OK)
	{
		//              CheckEvents();

		if ((rval = eep->Write(probe, type)) > 0)
		{
			//Aggiunto il 18/03/99 con la determinazione dei numeri di banchi nelle E24xx2,
			// affinche` la dimensione rimanga quella impostata bisogna correggere la dimensione
			// del banco.
			SetBlockSize(eep->GetBankSize());
		}

		if (!(rval > 0 && leave_on))            //23/01/1999
		{
			SleepBus();
		}
	}

	qDebug() << "e2AppWinInfo::Write() = " << rval << " - OUT";

	return rval;
}

int e2AppWinInfo::Verify(int type, int raise_power, int leave_on)
{
	int rval = OK;

	qDebug() << "e2AppWinInfo::Verify(" << type << "," << raise_power << "," << leave_on << ") - IN";

	if (raise_power)
	{
		rval = OpenBus();
	}

	if (rval == OK)
	{
		rval = eep->Verify(type);

		if (!(rval >= 0 && leave_on))
		{
			SleepBus();
		}
	}

	qDebug() << "e2AppWinInfo::Verify() = " << rval << " - OUT";

	return rval;
}

int e2AppWinInfo::Erase(int type, int raise_power, int leave_on)
{
	int rval = OK;

	qDebug() << "e2AppWinInfo::Erase(" << type << "," << raise_power << "," << leave_on << ") - IN";

	if (raise_power)
	{
		rval = OpenBus();
	}

	if (rval == OK)
	{
		rval = eep->Erase(1, type);

		if (!(rval >= 0 && leave_on))
		{
			SleepBus();
		}
	}

	qDebug() << "e2AppWinInfo::Erase() = " << rval << " - OUT";

	return rval;
}

int e2AppWinInfo::BankRollOverDetect(int force)
{
	if (OpenBus() == OK)
	{
		roll_over = eep->BankRollOverDetect(force);
	}

	SleepBus();

	return roll_over;
}

int e2AppWinInfo::Load()
{
	int rval;

	SetFileBuf(E2P);
	rval = LoadFile();

	if (rval <= 0)
	{
		SetFileBuf(MOTOS);
		rval = LoadFile();

		if (rval <= 0)
		{
			SetFileBuf(INTEL);
			rval = LoadFile();
		}

		if (rval <= 0)
		{
			SetFileBuf(CSM);
			rval = LoadFile();
		}

		if (rval <= 0)
		{
			SetFileBuf(BIN);
			rval = LoadFile();
		}

		if (rval > 0)
		{
			quint32 pritype = GetPriType(GetId());
			if (pritype == PIC16XX ||
					pritype == PIC168XX)
			{
				//It seems a bit tricky...
				//Relocate the DATA and CONFIG memory with PIC devices
				//The assembler store the DATA at 0x2100 (word address) and
				//CONFIG at 0x2000 (word address).
				//We move them after PROGRAM memory
				// PROGRAM MEMORY + DATA MEMORY + CONFIG (8 word)
				if (GetSplittedInfo() > 0 && GetSize() > GetSplittedInfo())
				{
					//Copy Config memory
					if (GetSize() + 16 <= GetBufSize())
					{
						memcpy(GetBufPtr() + GetSize(), GetBufPtr() + (0x2000 * 2), 16);
					}

					//Now copy data memory (copy only low byte every word)
					int k;
					quint8 *dst = GetBufPtr() + GetSplittedInfo();
					quint16 *src = (quint16 *)GetBufPtr() + 0x2100;

					for (k = 0; k < GetSize() - GetSplittedInfo(); k++)
					{
						*dst++ = (quint8)(*src++ & 0xff);
					}

					//memcpy(GetBufPtr()+GetSplittedInfo()+16, GetBufPtr() + (0x2100 * 2), GetSize() - (GetSplittedInfo() + 16) );

					//Set fuse bits so the dialog shows the correct values
					quint8 *ptr = GetBufPtr() + GetSize() + 14;
					quint16 config = ptr[0] + ((quint16)ptr[1] << 8);             //little endian buffer
					config = ~config & 0x3fff;              //      CodeProtectAdjust(config, 1);
					SetLockBits(config);
				}
			}
			else if (pritype == PIC125XX)
			{
				//Copy Config memory
				if (GetSize() + 16 <= GetBufSize())
				{
					memcpy(GetBufPtr() + GetSize() + 14, GetBufPtr() + (0xFFF * 2), 2);
				}

				//Set fuse bits so the dialog shows the correct values
				quint8 *ptr = GetBufPtr() + GetSize() + 14;
				quint16 config = ptr[0] + ((quint16)ptr[1] << 8);             //little endian buffer
				SetLockBits(~config & 0x0fff);
			}
		}
	}

	return rval;
}

int e2AppWinInfo::LoadFile()
{
	if (clear_buffer_before_load)
	{
		if (load_type == ALL_TYPE)
		{
			ClearBuffer(ALL_TYPE);
		}
		else
		{
			if (load_type & PROG_TYPE)
			{
				ClearBuffer(PROG_TYPE);
			}

			if (load_type & DATA_TYPE)
			{
				ClearBuffer(DATA_TYPE);
			}
		}
	}

	int rval = fbufp->Load(load_type, load_relocation);

	if (rval > 0)
	{
		buf_ok = true;
		buf_changed = false;
	}

	RecalcCRC();

	return rval;
}

int e2AppWinInfo::Save()
{
	int rval;

	quint8 *localbuf = new quint8[GetBufSize()];

	//save buffer
	memcpy(localbuf, GetBufPtr(), GetBufSize());
	memset(localbuf + GetSize(), 0xFF, GetBufSize() - GetSize());

	/**
	 ** NON va qui!!! Va messo in IntelFileBuf::Save(int savetype)
	 **               e MotorolaFileBus::Save
	 **/
	if (save_type == ALL_TYPE &&
			(GetFileBuf() == INTEL || GetFileBuf() == MOTOS))
	{
		quint32 pritype = GetPriType(GetId());
		if (pritype == PIC16XX ||
				pritype == PIC168XX)
		{
			//It seems a bit tricky...
			//Relocate the DATA and CONFIG memory with PIC devices
			//The assembler store the DATA at 0x2100 (word address) and
			//CONFIG at 0x2000 (word address).
			//We move them after PROGRAM memory
			// PROGRAM MEMORY + DATA MEMORY + CONFIG (8 word)
			if (GetSplittedInfo() > 0 && GetSize() > GetSplittedInfo())
			{
				//Set ALL overbound buffer to 0xFF
				memset(GetBufPtr(), 0xFF, GetBufSize());

				//Set fuse bits so the dialog shows the correct values
				if (0x2000 * 2 + 16 <= GetBufSize())
				{
					quint16 config = (quint16)GetLockBits();

					if (GetId() == PIC1683 ||
							GetId() == PIC1684 ||
							GetId() == PIC1684A)
					{
						if (config & (1 << 4))
						{
							config |= 0xfff0;
						}
						else
						{
							config &= 0x000f;
						}
					}

					config = ~config & 0x3fff;              //      CodeProtectAdjust(config, 0);

					quint8 *ptr = GetBufPtr() + (0x2000 * 2) + 14;
					ptr[0] = (quint8)(config & 0xFF);                      //little endian buffer
					ptr[1] = (quint8)(config >> 8);
				}

				//Copy prog memory
				if (GetSplittedInfo() <= GetBufSize())
				{
					memcpy(GetBufPtr(), localbuf, GetSplittedInfo());
				}

				//Copy data memory
				if (0x2100 * 2 + (GetSize() - GetSplittedInfo()) <= GetBufSize())
				{
					//copy only low byte every word
					int k;
					quint8 *src = localbuf + GetSplittedInfo();
					quint16 *dst = (quint16 *)GetBufPtr() + 0x2100;

					for (k = 0; k < GetSize() - GetSplittedInfo(); k++)
					{
						*dst++ = *src++;
					}
				}
			}
		}
		else if (pritype == PIC125XX)
		{
			//Set ALL overbound buffer to 0xFF
			memset(GetBufPtr(), 0xFF, GetBufSize());

			//Copy prog memory
			if (GetSplittedInfo() <= GetBufSize())
			{
				memcpy(GetBufPtr(), localbuf, GetSize());
			}

			//Set fuse bits so the dialog shows the correct values
			if (0xFFF * 2 + 2 <= GetBufSize())
			{
				quint16 config = (quint16)GetLockBits();

				config = ~config & 0x0fff;              //      CodeProtectAdjust(config, 0);

				quint8 *ptr = GetBufPtr() + (0xFFF * 2);
				ptr[0] = (quint8)(config & 0xFF);                      //little endian buffer
				ptr[1] = (quint8)(config >> 8);
			}
		}
	}

	rval = fbufp->Save(save_type, save_relocation);

	if (rval > 0)
	{
		buf_changed = false;
	}

	//restore buffer
	memcpy(GetBufPtr(), localbuf, GetBufSize());

	delete[] localbuf;

	return rval;
}

long e2AppWinInfo::GetLoadRelocation() const
{
	return load_relocation;
}

void e2AppWinInfo::SetLoadRelocation(long val)
{
	if (val >= 0 && val < GetBufSize())
	{
		load_relocation = val;
	}
}

long e2AppWinInfo::GetSaveRelocation() const
{
	return save_relocation;
}

void e2AppWinInfo::SetSaveRelocation(long val)
{
	if (val >= 0 && val < GetBufSize())
	{
		save_relocation = val;
	}
}


int e2AppWinInfo::GetLoadType() const
{
	return load_type;
}

int e2AppWinInfo::SetLoadType(int val)
{
	int rval = 0;

	if (val == ALL_TYPE || val == PROG_TYPE || val == DATA_TYPE || val == CONFIG_TYPE)
	{
		load_type = val;
		rval = 1;
	}

	return rval;
}

int e2AppWinInfo::GetSaveType() const
{
	return save_type;
}

int e2AppWinInfo::SetSaveType(int val)
{
	int rval = 0;

	if (val == ALL_TYPE || val == PROG_TYPE || val == DATA_TYPE || val == CONFIG_TYPE)
	{
		save_type = val;
		rval = 1;
	}

	return rval;
}

void e2AppWinInfo::SetLockBits(quint32 bits)
{
	lock_bits = bits;
}

void e2AppWinInfo::SetFuseBits(quint32 bits)
{
	fuse_bits = bits;
	fuse_ok = true;
}

long e2AppWinInfo::GetSize() const
{
	return (long)GetNoOfBlock() * (long)GetBlockSize();
}

void e2AppWinInfo::SetFileBuf(FileType type)
{
	if (type >= 0 && type < NO_OF_FILETYPE)
	{
		fbufp = fbufvet[type];
	}
	else
	{
		fbufp = fbufvet[E2P];        //Tipo di default
	}
}

FileType e2AppWinInfo::GetFileBuf() const
{
	FileType type;
	int n;

	for (n = 0; n < NO_OF_FILETYPE; n++)
	{
		type = (FileType)n;

		if (fbufvet[type] == fbufp)
		{
			break;
		}
	}

	return type;
}


QString e2AppWinInfo::GetStringID()
{
	return eeprom_string;
}


void e2AppWinInfo::SetStringID(const QString &s)
{
	eeprom_string = s;
}

QString e2AppWinInfo::GetComment()
{
	return eeprom_comment;
}

void e2AppWinInfo::SetComment(const QString &s)
{
	eeprom_comment = s;
}

void e2AppWinInfo::ClearBuffer(int type)
{
	if (type == ALL_TYPE)
	{
		memset(GetBufPtr(), 0xFF, GetBufSize());
		SetStringID("");
		SetComment("");
		buf_ok = true;          //Validate buffer
	}
	else if (type == PROG_TYPE)
	{
		long s = GetSplittedInfo();

		if (s <= 0 || s > GetBufSize())
		{
			s = GetSize();
		}

		memset(GetBufPtr(), 0xFF, s);
	}
	else if (type == DATA_TYPE)
	{
		long s = GetSplittedInfo();

		if (s < 0 || s >= GetSize())
		{
			s = 0;
		}

		memset(GetBufPtr() + s, 0xFF, GetSize() - s);
	}

	RecalcCRC();
}

void e2AppWinInfo::FillBuffer(int init_pos, int ch, long len)
{
	long l;

	if (len > 0)
	{
		l = len;
	}
	else
	{
		l = GetBufSize();
	}

	if (init_pos >= GetBufSize())
	{
		return;        //Bad value
	}

	// Check for buffer overflow
	if (init_pos + l > GetBufSize())
	{
		l = GetBufSize() - init_pos;
	}

	// If the buffer was not yet initialized we first
	//   clear it.
	if (!buf_ok)
	{
		ClearBuffer();
	}

	memset(GetBufPtr() + init_pos, ch, l);

	RecalcCRC();
	BufChanged();
}

void e2AppWinInfo::SwapBytes()
{
	long size = GetNoOfBlock() * GetBlockSize();
	long k;

	for (k = 0; k < size; k += 2)
	{
		quint8 tmp = buffer[k];
		buffer[k] = buffer[k + 1];
		buffer[k + 1] = tmp;
	}

	RecalcCRC();
	BufChanged();
}

void e2AppWinInfo::DoubleSize()
{
	if (GetSize() == AUTOSIZE_ID)
	{
		return;
	}

	//Attenzione!! Il buffer deve essere capiente il doppio
	//  della dimensione del dispositivo attuale
	//      int n = GetNoOfBlock();
	int k;

	for (k = GetNoOfBlock() - 1; k >= 0; k--)
	{
		//Copia l'ultimo banco nei due banchi in fondo
		if (k)
		{
			//Non copiarlo su se stesso!
			memcpy(buffer + k * 2 * GetBlockSize(),
				   buffer + k * GetBlockSize(),
				   GetBlockSize()
				  );
		}

		memcpy(buffer + (k * 2 + 1) * GetBlockSize(),
			   buffer + k * GetBlockSize(),
			   GetBlockSize()
			  );
	}

	RecalcCRC();
	BufChanged();
}

int e2AppWinInfo::SecurityRead(quint32 &bits)
{
	int rv;

	OpenBus();
	rv = eep->SecurityRead(bits);
	SleepBus();

	RecalcCRC();

	if (rv == OK)
	{
		lock_bits = bits;
	}

	return rv;
}

int e2AppWinInfo::SecurityWrite(quint32 bits, bool no_param)
{
	int rv;

	if (no_param)
	{
		bits = lock_bits;
	}

	OpenBus();
	rv = eep->SecurityWrite(bits);
	SleepBus();

	return rv;
}

//The 24C65 microchip device has high endurance bits
// in place of Fuse bits.
// Use the same variable because there is no room for
// both in the e2p header.
int e2AppWinInfo::HighEnduranceRead(quint32 &block_no)
{
	int rv;

	OpenBus();
	rv = eep->HighEnduranceRead(block_no);
	SleepBus();

	if (rv == OK)
	{
		SetFuseBits(block_no);
	}

	return rv;
}

int e2AppWinInfo::HighEnduranceWrite(quint32 block_no, bool no_param)
{
	int rv;

	if (no_param)
	{
		block_no = GetFuseBits();
	}

	OpenBus();
	rv = eep->HighEnduranceWrite(block_no);
	SleepBus();

	if (rv == OK)
	{
		quint32 block2;

		OpenBus();
		rv = eep->HighEnduranceRead(block2);
		SleepBus();

		if (rv == OK && block_no != block2)
		{
			rv = E2ERR_WRITEFAILED;
		}
	}

	return rv;
}

int e2AppWinInfo::FusesRead(quint32 &bits)
{
	int rv;

	OpenBus();
	rv = eep->FusesRead(bits);
	SleepBus();

	if (rv == OK)
	{
		SetFuseBits(bits);
	}

	return rv;
}

int e2AppWinInfo::FusesWrite(quint32 bits, bool no_param)
{
	int rv;

	if (no_param)
	{
		bits = GetFuseBits();
	}

	OpenBus();
	rv = eep->FusesWrite(bits);
	SleepBus();

	/**
	the read op doesn't work with every device
	        if (rv == OK)
	        {
	                quint32 bits2;

	                OpenBus();
	                rv = eep->FusesRead(bits2);
	                SleepBus();

	                if (rv == OK && bits != bits2)
	                        rv = E2ERR_WRITEFAILED;
	        }
	**/
	return rv;
}

int e2AppWinInfo::ReadOscCalibration(int addr)
{
	int val;

	OpenBus();
	val = eep->ReadCalibration(addr);
	SleepBus();

	return val;
}

#include "crc.h"

quint16 e2AppWinInfo::RecalcCRC()
{
	quint16 crc_val = mcalc_crc(GetBufPtr(), GetSize());
	SetCRC(crc_val);
	return crc_val;
}


// #include <ctype.h>

QString e2AppWinInfo::Dump(int line, int type)
{
	long idx;
	long upperlimit;

	if (!buf_ok)
	{
		return "";
	}

	upperlimit = GetSize() == 0 ? GetBufSize() : GetSize();

	idx = line * hex_per_line;

	if (idx < upperlimit)
	{
		if (type == 0)
		{
			char tmpbuf[16 + 1];

			for (int k = 0; k < hex_per_line; k++)
			{
				tmpbuf[k] = isprint(buffer[idx + k]) ? buffer[idx + k] : '.';
			}

			tmpbuf[hex_per_line] = 0;
#if USE_QT_VERSION == 4
			linebuf.sprintf("  %06lX) %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X    %s\n",
							//                      snprintf(linebuf, LINEBUF_SIZE, "  %06lX) %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X    %s\n",
							idx,
							buffer[idx + 0],
							buffer[idx + 1],
							buffer[idx + 2],
							buffer[idx + 3],
							buffer[idx + 4],
							buffer[idx + 5],
							buffer[idx + 6],
							buffer[idx + 7],

							buffer[idx + 8],
							buffer[idx + 9],
							buffer[idx + 10],
							buffer[idx + 11],
							buffer[idx + 12],
							buffer[idx + 13],
							buffer[idx + 14],
							buffer[idx + 15],

							tmpbuf
						   );
#else
			linebuf.asprintf("  %06lX) %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X    %s\n",
							 //                      snprintf(linebuf, LINEBUF_SIZE, "  %06lX) %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X    %s\n",
							 idx,
							 buffer[idx + 0],
							 buffer[idx + 1],
							 buffer[idx + 2],
							 buffer[idx + 3],
							 buffer[idx + 4],
							 buffer[idx + 5],
							 buffer[idx + 6],
							 buffer[idx + 7],

							 buffer[idx + 8],
							 buffer[idx + 9],
							 buffer[idx + 10],
							 buffer[idx + 11],
							 buffer[idx + 12],
							 buffer[idx + 13],
							 buffer[idx + 14],
							 buffer[idx + 15],

							 tmpbuf
							);
#endif
		}
		else if (type == 1)
		{
#if USE_QT_VERSION == 4
			linebuf.sprintf("  %06lX)",     idx);
#else
			linebuf.asprintf("  %06lX)",     idx);
#endif
			//                      snprintf(linebuf, LINEBUF_SIZE, "  %06lX)",     idx);
		}
		else if (type == 2)
		{
#if USE_QT_VERSION == 4
			linebuf.sprintf(" %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X  ",
							//                      snprintf(linebuf, LINEBUF_SIZE, " %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X  ",
							buffer[idx + 0],
							buffer[idx + 1],
							buffer[idx + 2],
							buffer[idx + 3],
							buffer[idx + 4],
							buffer[idx + 5],
							buffer[idx + 6],
							buffer[idx + 7],

							buffer[idx + 8],
							buffer[idx + 9],
							buffer[idx + 10],
							buffer[idx + 11],
							buffer[idx + 12],
							buffer[idx + 13],
							buffer[idx + 14],
							buffer[idx + 15]
						   );
#else
			linebuf.asprintf(" %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X  ",
							 //                      snprintf(linebuf, LINEBUF_SIZE, " %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X  ",
							 buffer[idx + 0],
							 buffer[idx + 1],
							 buffer[idx + 2],
							 buffer[idx + 3],
							 buffer[idx + 4],
							 buffer[idx + 5],
							 buffer[idx + 6],
							 buffer[idx + 7],

							 buffer[idx + 8],
							 buffer[idx + 9],
							 buffer[idx + 10],
							 buffer[idx + 11],
							 buffer[idx + 12],
							 buffer[idx + 13],
							 buffer[idx + 14],
							 buffer[idx + 15]
							);
#endif
		}
		else
		{
			char tmpbuf[16 + 1];

			for (int k = 0; k < hex_per_line; k++)
			{
				tmpbuf[k] = isprint(buffer[idx + k]) ? buffer[idx + k] : '.';
			}

			tmpbuf[hex_per_line] = 0;
#if USE_QT_VERSION == 4
			linebuf.sprintf("  %s\n", tmpbuf);
#else
			linebuf.asprintf("  %s\n", tmpbuf);
#endif
			//                      snprintf(linebuf, LINEBUF_SIZE, "  %s\n", tmpbuf);
		}

		//              linebuf[LINEBUF_SIZE - 1] = '\0';
	}

	return linebuf;
}
