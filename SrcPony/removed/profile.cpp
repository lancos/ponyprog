//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: profile.cpp,v 1.6 2009/11/15 13:33:22 lancos Exp $
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

#include <stdio.h>
#include <QString>
#include <ctype.h>
#include "profile.h"
#include "errcode.h"

// #ifndef _LINUX_
// #  ifdef        __BORLANDC__
// #    define     strncasecmp strnicmp
// #    define     strcasecmp stricmp
// #  else // _MICROSOFT_ VC++
// #    define strncasecmp _strnicmp
// #    define     strcasecmp _stricmp
// #    define snprintf _snprintf
// #  endif
// #endif


//=====>>> Costruttore <<<======
Profile::Profile(const QString &name)
{
	cached = 0;
	SetFileName(name);
	FlushVet(1);
}


//--- Distruttore
Profile::~Profile()
{
	FlushVet();
}


QString Profile::StripSpace(const QString &sp)
{
	//toglie gli spazi iniziali
	Qstring s = sp;
	s.remove(QChar(' '));
#if 0
	sp.re

	while( *sp && isspace(*sp) )
	{
		sp++;
	}

	strncpy(strbuf, sp, MAXLINESIZE - 1);
	strbuf[MAXLINESIZE - 1] = '\0';
	//si posiziona in fondo
	char *p = strbuf;

	while( *p )
	{
		p++;
	}

	//toglie
	while( isspace(*--p) )
	{
		*p = '\0';
	}

#endif
	return s;
}


//---
void Profile::SetFileName(const QString &name)
{
	if (name.length())
	{
		strcpy(filename, name);
	}
}


void Profile::FlushVet(int force)
{
	//      int k;

	profilevet.clear();
	/*
	        if (force)
	                for (k = 0; k < MAXLINENUM; k++)
	                {
	                        profilevet[k] = 0;
	                }
	        else
	                for (k = 0; k < MAXLINENUM && profilevet[k] != 0; k++)
	                {
	                        delete profilevet[k];
	                        profilevet[k] = 0;
	                }*/
}


int Profile::WriteVet()
{
	FILE *fh = fopen(filename, "w");

	if (fh == 0)
	{
		return CREATEERROR;
	}

	//Lettura file con allocazione linea parametro e aggiunta al vettore
	int rval = 0;
	int k;

	for (k = 0; k < profilevet.count(); k++)
	{
		QString sp = profilevet.at(k);

		sp.remove(QChar(' '));
		//              while (isspace(*sp))            //elimina eventuali spazi iniziali
		//              {
		//                      sp++;
		//              }

		if (sp.length())                         //scarta le linee vuote
		{
			linebuffer = sp;
			linebuffer += "\n";                       //appende il new-line

			if ( fputs(linebuffer.toLatin1(), fh) == EOF )
			{
				rval = WRITEERROR;
				break;
			}
		}
	}

	cached = 1;             //ripensare attentamente se e` giusto forzarla qui
	fclose(fh);
	return rval;
}


int Profile::ReadVet()
{
	//Se il file e` gia` stato letto in memoria con successo allora non fare niente
	if (cached)
	{
		return 0;
	}

	//Svuotamento vettore parametri
	FlushVet();

	//Apertura file .INI
	FILE *fh = fopen(filename.toLatin1(), "r");

	if (fh == 0)
	{
		return 0;        //non costituisce errore, infatti il file puo` essere
	}

	//  creato con una WriteVet(). Pero` attenti se la
	//  politica dovesse diventare write-back, la variabile
	//  cached andrebbe aggiornata!

	//Lettura file con allocazione linea parametro e aggiunta al vettore
	int rval = OK;
	int k;

	for (k = 0; k < MAXLINENUM && fgets(linebuffer, MAXLINESIZE, fh) != 0; k++)
	{
		char *sp = strchr(linebuffer, '\n');

		if (sp != 0)
		{
			*sp = '\0';        //elimina il new-line finale
		}

		sp = new char[strlen(linebuffer) + 1];

		if (sp == 0)
		{
			rval = OUTOFMEMORY;     //out of memory, sigh!!!
			break;
		}

		profilevet[k] = sp;

		strcpy(sp, linebuffer);
	}

	fclose(fh);

	if (!rval)
	{
		cached = 1;
	}

	return rval;
}


QString Profile::GetParameter(const QString &id)
{
	//Controllo parametri
	if ( id == 0 || id.length() == 0 )
	{
		return 0;
	}

	//Legge i parametri dal file
	if ( ReadVet() )
	{
		return 0;
	}

	//Ricerca (lineare) del parametro
	int n = id.length();
	int j;

	for (j = 0; j < MAXLINENUM && profilevet[j] != 0; j++)
	{
		QString sp = profilevet[j];

		while (isspace(*sp))            //elimina eventuali spazi iniziali
		{
			sp++;
		}

		if (strncasecmp(id, sp, n) == 0)        //trovata!!
		{
			sp += n;                //si posiziona in fondo all'id per cercare '='

			while (isspace(*sp))    //tralascia gli spazi
			{
				sp++;
			}

			if (*sp == '=')
			{
				//Solo ora siamo sicuri di avere trovato esattamente cio` che
				//  cercavamo (vedi caso ricerca "maia" e trovato "maiale")
				while (isspace(*++sp))
					;

				return sp;              //restituisce il valore trovato
			}
		}
	}

	return 0;
}


int Profile::SetParameter(const QString &id, const QString &value)
{
	if ( id == 0 || id.length() == 0 || value == 0)
	{
		return BADPARAM;
	}

	//Leggiamo i parametri dal file
	int err = ReadVet();

	if (err)
	{
		return err;
	}

	//Ricerchiamo il parametro da variare, se non c'e` l'aggiungiamo
	int n = id.length();
	int found = 0;
	int j;

	for (j = 0; j < MAXLINENUM && profilevet[j] != 0; j++)
	{
		QString sp = profilevet[j];

		while (isspace(*sp))            //elimina eventuali spazi iniziali
		{
			sp++;
		}

		if (strncasecmp(id, sp, n) == 0)        //trovata!!
		{
			sp += n;                //si posiziona in fondo all'id per cercare '='

			while (isspace(*sp))    //tralascia gli spazi
			{
				sp++;
			}

			if (*sp == '=')
			{
				//Solo ora siamo sicuri di avere trovato esattamente cio` che
				//  cercavamo (vedi caso ricerca "maia" e trovato "maiale")
				found = 1;
				break;          //usciamo dal ciclo for()
			}
		}
	}       //for()

	//Nel caso non ci sia j sara` puntato sul primo elemento del vettore a 0
	if (!found)
	{
		if (j >= MAXLINENUM)    //Raggiunto il numero max di parametri, sigh!
		{
			return BUFFEROVERFLOW;
		}
	}

	//prepariamo la nuova stringa nel linebuffer
	strcpy(linebuffer, id);
	strcat(linebuffer, "=");
	strcat(linebuffer, value);

	//allochiamo memoria e copiamo la nuova stringa
	char *sp = new char[strlen(linebuffer) + 1];

	if (sp == 0)
	{
		return OUTOFMEMORY;        //Out of memory, Sigh!!
	}

	strcpy(sp, linebuffer);

	//liberiamo la memoria della eventuale stringa corrente e aggiorniamo il parametro
	if (profilevet[j])
	{
		delete profilevet[j];
	}

	profilevet[j] = sp;

	//infine salviamo il tutto su file (politica write through)
	WriteVet();

	return OK;
}

#if 0
int Profile::decnum2str(int value, char *str, int len)
{
	int ret = snprintf(str, len, "%d", value);
	str[len - 1] = '\0';

	if (ret > 0 && ret < len)
	{
		return OK;
	}
	else
	{
		return BADPARAM;
	}
}


int Profile::decnum2str(unsigned long value, char *str, int len)
{
	int ret = snprintf(str, len, "%lu", value);
	str[len - 1] = '\0';

	if (ret > 0 && ret < len)
	{
		return OK;
	}
	else
	{
		return BADPARAM;
	}
}


int Profile::hexnum2str(int value, char *str, int len)
{
	int ret = snprintf(str, len, "0x%02X", value);
	str[len - 1] = '\0';

	if (ret > 0 && ret < len)
	{
		return OK;
	}
	else
	{
		return BADPARAM;
	}
}


int Profile::hexnum2str(unsigned long value, char *str, int len)
{
	int ret = snprintf(str, len, "0x%04lX", value);
	str[len - 1] = '\0';

	if (ret > 0 && ret < len)
	{
		return OK;
	}
	else
	{
		return BADPARAM;
	}
}
#endif
