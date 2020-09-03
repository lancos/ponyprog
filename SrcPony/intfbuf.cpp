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

// #include <stdio.h>

//Estensione al formato Intel Extended HEX
#include <QTextStream>
#include <QString>

// #include <stdlib.h>
// #include <ctype.h>

#define MAXLINE 520

#define DATA_RECORD     00      //record contain the data bytes
#define END_RECORD      01      //record mark end of file
#define SEG_ADDR_RECORD 02      //record contain the new segmented address (HEX86)
#define START_RECORD    03      //record contain the program entry point (HEX86)
#define LIN_ADDR_RECORD 04      //record contain the new linear address (HEX386)
#define EXT_START_RECORD        05      //record contain the program entry point (HEX386)

#include "types.h"
#include "intfbuf.h"            // Header file
#include "errcode.h"
#include "crc.h"

#include "e2awinfo.h"

IntelFileBuf::IntelFileBuf(e2AppWinInfo *wininfo)
	: FileBuf(wininfo)
{
	file_type = INTEL;
}

IntelFileBuf::~IntelFileBuf()
{
}


int IntelFileBuf::WriteRecord(QFile &fh, quint8 *bptr, long curaddr, long recsize, int fmt)
{
	int rval = 1;

	int discard;
	int j;

	//check for empty data record (all FF's)
	if (fmt == DATA_RECORD)
	{
		discard = 1;

		for (j = 0; j < recsize; j++)
		{
			if (bptr[curaddr + j] != 0xFF)
			{
				discard = 0;
				break;
			}
		}
	}
	else
	{
		discard = 0;
	}

	if (!discard)
	{
		int checksum = 0;
		int len = recsize;
		QTextStream out(&fh);

		out << ":";

		//byte count
		out << QString().sprintf("%02X", len & 0xFF);
		checksum += len & 0xFF;

		//addr field
		out << QString().sprintf("%04lX", curaddr & 0xFFFF);
		checksum += (curaddr >> 8) & 0xFF;
		checksum += curaddr & 0xFF;

		//record type
		out << QString().sprintf("%02X", fmt & 0xFF);
		checksum += fmt & 0xFF;

		for (j = 0; j < recsize; j++)
		{
			out << QString().sprintf("%02X", bptr[curaddr + j]);
			checksum += bptr[curaddr + j];
		}

		out << QString().sprintf("%02X\n", (~checksum + 1) & 0xFF);
	}

	return rval;
}

int IntelFileBuf::WriteAddressRecord(QFile &fh, long curaddr, bool linear_address)
{
	int rval = 1;
	QTextStream out(&fh);

	out << ":";

	int checksum = 0;
	int len = 2;

	//byte count
	out << QString().sprintf("%02X", len & 0xFF);
	checksum += len & 0xFF;

	//addr field
	out << QString().sprintf("%04X", 0);

	if (linear_address)
	{
		//record type
		out << QString().sprintf("%02X", LIN_ADDR_RECORD & 0xFF);
		checksum += LIN_ADDR_RECORD & 0xFF;

		//adjust extended linear address
		curaddr >>= 16;
	}
	else
	{
		//record type
		out << QString().sprintf("%02X", SEG_ADDR_RECORD & 0xFF);
		checksum += SEG_ADDR_RECORD & 0xFF;

		//adjust extended segmented address
		curaddr >>= 4;
	}

	out << QString().sprintf("%04lX", curaddr & 0xFFFF);
	checksum += (curaddr >> 8) & 0xFF;
	checksum += curaddr & 0xFF;

	out << QString().sprintf("%02X\n", (~checksum + 1) & 0xFF);

	return rval;
}


#define min(a,b)        ( (a < b) ? a : b )

int IntelFileBuf::Save(int savetype, long relocation_offset)
{
	QFile fh(FileBuf::GetFileName());
	int rval = OK;

	(void)relocation_offset;

	if (!fh.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return CREATEERROR;
	}

	long dsize = FileBuf::GetBlockSize() * FileBuf::GetNoOfBlock();
	long size = FileBuf::GetBufSize();
	quint8 *ptr = FileBuf::GetBufPtr();

	//Remove FF's tail
	while (ptr[size - 1] == 0xFF)
	{
		size--;
	}

	if (savetype == PROG_TYPE)
	{
		if (GetSplitted() > 0 && GetSplitted() <= dsize)
		{
			size = GetSplitted();
		}
		else
		{
			fh.close();
			return 0;
		}
	}
	else if (savetype == DATA_TYPE)
	{
		if (GetSplitted() >= 0 && GetSplitted() < dsize)
		{
			ptr += GetSplitted();
			size = dsize - GetSplitted();
		}
		else
		{
			fh.close();
			return 0;
		}
	}

	if (size > 0)
	{
		long curaddr = 0;

		while (curaddr < size)
		{
			//Write extended address record if needed
			if ((curaddr / 0x10000) > 0 && (curaddr % 0x10000) == 0)
				if (!WriteAddressRecord(fh, curaddr))
				{
					rval = WRITEERROR;
					break;
				}

			int recsize = min((size - curaddr), 16);

			if (!WriteRecord(fh, ptr, curaddr, recsize, DATA_RECORD))
			{
				rval = WRITEERROR;
				break;
			}

			curaddr += recsize;
		}

		WriteRecord(fh, ptr, 0, 0, END_RECORD);         //26/08/99

		rval = curaddr;
	}
	else
	{
		rval = NOTHINGTOSAVE;
	}

	fh.close();

	return rval;
}


int IntelFileBuf::Load(int loadtype, long relocation_offset)
{
	int rval = OK;
	int okline_counter = 0;

	quint8 *endp = GetBufPtr() + GetBufSize();
	quint8 *dp = GetBufPtr();

	if (loadtype == DATA_TYPE)
	{
		if (GetSplitted() >= 0 && GetSplitted() < GetBufSize())
		{
			dp += GetSplitted();
		}
		else
		{
			return 0;
		}
	}

	//Relocation check
	if (dp + relocation_offset > endp)
	{
		return BADPARAM;
	}
	else
	{
		dp += relocation_offset;
	}

	quint32 laddr = 0;

	QFile fh(GetFileName());

	if (!fh.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return FILENOTFOUND;
	}

	int img_size = 0;
	QString riga; //[MAXLINE + 1];
// 	riga[MAXLINE] = '\0';

	QTextStream stream(&fh);

	while (!stream.atEnd())
	{
		riga = stream.readLine();

// 		char *s;
		int pos;

		if ((pos = riga.indexOf(":")) < 0)
		{
			continue;
		}
		else
		{
			pos++;
		}

		//Byte Count
		quint16 bcount;

		if (!ScanHex(riga.mid(pos, 2), bcount))
		{
			rval = BADFILETYPE;
			break;
		}
		else
		{
			pos += 2;
		}

		quint8 checksum = (quint8)bcount;

		//Address
		quint16 addr;

		if (!ScanHex(riga.mid(pos, 4), addr))
		{
			rval = BADFILETYPE;
			break;
		}
		else
		{
			pos += 4;
		}

		checksum += (quint8)(addr >> 8);
		checksum += (quint8)addr;

		//affect only low 16 bits of address
		laddr &= 0xFFFF0000;
		laddr |= addr;

		//Record Type
		quint16 rectype;

		if (!ScanHex(riga.mid(pos, 2), rectype))
		{
			rval = BADFILETYPE;
			break;
		}
		else
		{
			pos += 2;
		}

		checksum += (quint8)rectype;

		//Data Byte
		quint16 data;

		if (rectype == DATA_RECORD)
		{
			//controllo overflow
			if (dp + laddr + bcount > endp)
			{
				rval = BUFFEROVERFLOW;
				break;
			}

			bool ok = true;
			quint8 *p;
			unsigned int k;

			for (k = 0, p = dp + laddr; k < bcount && ok; k++)
			{
				if (!ScanHex(riga.mid(pos, 2), data))
				{
					ok = false;
				}
				else
				{
					pos += 2;
				}

				checksum += (quint8)data;
				*p++ = (quint8)data;
			}

			if (!ok)        //salta alla riga successiva
			{
				rval = BADFILETYPE;
				break;
			}

			img_size = laddr + bcount;
		}
		else if (rectype == SEG_ADDR_RECORD)
		{
			if (bcount != 2)
			{
				rval = BADFILETYPE;
				break;
			}
			else
			{
				//Address
				quint16 addr;

				if (!ScanHex(riga.mid(pos, 4), addr))
				{
					rval = BADFILETYPE;
					break;
				}
				else
				{
					pos += 4;
				}

				checksum += (quint8)(addr >> 8);
				checksum += (quint8)addr;

				laddr = (quint32)addr << 4;
			}
		}
		else if (rectype == LIN_ADDR_RECORD)
		{
			if (bcount != 2)
			{
				rval = BADFILETYPE;
				break;
			}
			else
			{
				//Address
				quint16 addr;

				if (!ScanHex(riga.mid(pos, 4), addr))
				{
					rval = BADFILETYPE;
					break;
				}
				else
				{
					pos += 4;
				}

				checksum += (quint8)(addr >> 8);
				checksum += (quint8)addr;

				laddr = (quint32)addr << 16;
			}
		}
		/**     just ignored
		        else if (rectype == START_RECORD)
		        {
		                if (bcount != 4)
		                {
		                        rval = BADFILETYPE;
		                        break;
		                }

		                // Lo possiamo ignorare, dovrebbe
		                // essere il linker a mettere tutto
		                // a posto. Noi non fungiamo da loader!
		                //jmpaddr = ScanHex(&s, 8);
		        //      ScanHex(&s, 8);
		                while (bcount--)
		                {
		                        data = (quint8)ScanHex(&s, 2);
		                        checksum += data;
		                }
		        }
		**/
		else    // Unknown record type: discard data bytes (but check for validity)
		{
			bool ok = true;

			while (bcount-- && ok)
			{
				if (!ScanHex(riga.mid(pos, 2), data))
				{
					ok = false;
				}
				else
				{
					pos += 2;
				}

				checksum += (quint8)data;
			}

			if (!ok)
			{
				rval = BADFILETYPE;
				break;
			}
		}

		if (!ScanHex(riga.mid(pos, 2), data))
		{
			rval = BADFILETYPE;
			break;
		}
		else
		{
			pos += 2;
		}

		if ((quint8)data != (quint8)(~checksum + 1))
		{
			rval = BADFILETYPE;
			break;
		}
		else
		{
			okline_counter++;
		}

		if (rectype == END_RECORD)
		{
			break;
		}
	}

	fh.close();

	if (okline_counter == 0)
	{
		rval = BADFILETYPE;
	}
	else
	{
		if (img_size == 0)      //nessun dato ma il formato e` corretto
		{
			img_size++;
		}
	}

	//In questi formati di file "stupidi" la dimensione
	//deve rimanere quella della eeprom attualmente selezionata

	if (rval == OK)
	{
		//SetStringID("");        //????
		//SetComment("");
		SetRollOver(0);         //2 (che significa NO) ??
		//      SetCRC( mcalc_crc(GetBufPtr(), img_size) );

		rval = img_size;
	}

	return rval;
}

/*Passandogli un indirizzo di stringa, converte le prime <len> cifre di tale
* stringa in Hesadecimali, incrementa il puntatore, e restituisce il numero.
* Attenzione! Poiche` il numero restituito e` quint32 (4Byte), il numero max
* di <len> e` 8 (8 cifre esadecimali 0xABCDEF12).
*/
bool IntelFileBuf::ScanHex(const QString &sp, quint32 &result)
{
	bool ok;
	result = sp.toInt(&ok, 16);

	return ok;
}

bool IntelFileBuf::ScanHex(const QString &sp, quint16 &result)
{
	bool ok;
	result = sp.toInt(&ok, 16);

	return ok;
}
