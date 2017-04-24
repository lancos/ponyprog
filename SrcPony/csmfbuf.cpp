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
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//

//#include <stdio.h>
#include <QString>
#include <QFile>

#include "csmfbuf.h"            // Header file
#include "crc.h"
#include "errcode.h"


//======================>>> csmFileBuf::csmFileBuf <<<=======================
csmFileBuf::csmFileBuf(e2AppWinInfo *wininfo)
	: FileBuf(wininfo)
{
	file_type = CSM;
}

#define MAXLINE 1023

//======================>>> csmFileBuf::Load <<<=======================
int csmFileBuf::Load(int loadtype, long relocation_offfset)
{
	extern int GetE2PSubType(unsigned long x);
	extern int GetE2PPriType(unsigned long x);

	(void)relocation_offfset;	//unused

	QFile fh(FileBuf::GetFileName());

	char *s;//, *s1;
	int rval = OK;
	char riga[MAXLINE + 1];

	if (!fh.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return FILENOTFOUND;
	}

	int state = 0;

	while (!fh.atEnd() && state < 3)
	{
		fh.readLine(riga, MAXLINE);

		switch (state)
		{
		case 0:

			//look for header start
			if ( (s = strchr(riga, '\"')) != NULL )
			{
				state++;

				if ( (s = strstr(s + 1, "REFERENCE")) != NULL )
				{
					state++;

					if ( (s = strchr(s + 1, '\"')) != NULL )
					{
						state++;
					}
				}
			}

			break;

		case 1:

			//look for "REFERENCE"
			if ( (s = strstr(riga, "REFERENCE")) != NULL )
			{
				state++;

				if ( (s = strchr(s + 1, '\"')) != NULL )
				{
					state++;
				}
			}

			break;

		case 2:

			//look for header terminator
			if ( (s = strchr(riga, '\"')) != NULL )
			{
				state++;
			}

			break;
		}
	}

	if (state < 3)				//Header not found
	{
		rval = BADFILETYPE;
	}
	else
	{
		int okline_counter = 0;
		int addr, value, n;

		addr = 0;

		//read all remaining file (body)
		while (!fh.atEnd())
		{
			fh.readLine(riga, MAXLINE);

			if (strlen(riga) > 0)   // salta righe vuote
			{
				n = sscanf(riga, "%x %x", &addr, &value);

				if (n != 2)
				{
					rval = BADFILETYPE;
					break;
				}
				else
				{
					okline_counter++;
				}

				//carica valore nel buffer
				if (loadtype == ALL_TYPE)
				{
					if ( addr < FileBuf::GetBufSize() )
					{
						FileBuf::GetBufPtr()[addr] = (uint8_t)value;
					}
				}
				else if (loadtype == PROG_TYPE)
				{
					long s = FileBuf::GetSplitted();

					if ( s <= 0 )
					{
						s = FileBuf::GetBufSize();
					}

					if ( addr < s )
					{
						FileBuf::GetBufPtr()[addr] = (uint8_t)value;
					}
				}
				else if (loadtype == DATA_TYPE)
				{
					long s = FileBuf::GetSplitted();

					if ( s <= 0 )
					{
						s = 0;
					}

					if ( addr < FileBuf::GetBufSize() - s )
					{
						FileBuf::GetBufPtr()[addr + s] = (uint8_t)value;
					}
				}
			}
		}

		if (okline_counter == 0)
		{
			rval = BADFILETYPE;
		}

		rval = addr;
	}

	fh.close();
	return rval;


	//                      SetStringID(hdr.e2pStringID);
	//                      SetComment(hdr.e2pComment);
}

static const char header[] =
        "REFERENCE	=	=\n"
        "CHASSIS	=	=\n"
        "MODEL	=	=\n"
        "SERIAL	=	=\n"
        "CUSTOMER	NAME	=	=\n"
        "DATE	=	=\n"
        "STREET	=	=\n"
        "CITY	=	=\n"
        "STATE	=	=\n"
        "ZIP	=	=\n"
        "PHONE	NUMBER	=	=\n"
        "CUSTOMER	COMPLAINT	=	=";

//======================>>> csmFileBuf::Save <<<=======================
int csmFileBuf::Save(int savetype, long relocation_offfset)
{
	int rval = 0;

	(void) relocation_offfset;		//unused

	long size = FileBuf::GetBlockSize() * FileBuf::GetNoOfBlock();
	uint8_t *ptr = FileBuf::GetBufPtr();

	if (savetype == PROG_TYPE)
	{
		if (GetSplitted() > 0 && GetSplitted() <= size)
		{
			size = GetSplitted();
		}
		else
		{
			return 0;
		}
	}
	else if (savetype == DATA_TYPE)
	{
		if (GetSplitted() >= 0 && GetSplitted() < size)
		{
			ptr += GetSplitted();
			size -= GetSplitted();
		}
		else
		{
			return 0;
		}
	}

	if (size > 0)
	{
		QFile fh(FileBuf::GetFileName());

		if (!fh.open(QIODevice::WriteOnly | QIODevice::Text ))
		{
			rval = CREATEERROR;
		}
		else
		{
			QTextStream out(&fh);
			int addr, value;

			//Write Header
			out << "\"\n" << header << "\n\"";

			//Write buffer
			for (addr = 0; addr < size; addr++)
			{
				value = ptr[addr];
				out << (hex) << addr << " " << value << "\n";
			}

			fh.close();

			rval = addr;
		}
	}
	else
	{
		rval = NOTHINGTOSAVE;
	}

	return rval;
}
