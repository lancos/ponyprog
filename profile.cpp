//=========================================================================//
//-------------------------------------------------------------------------//
// profile.cpp -- Source for Profile class                                 //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2000   Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lanconel@cs.unibo.it                                           //
//  http://www.cs.unibo.it/~lanconel                                       //
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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "profile.h"

#ifndef	_LINUX_
#  ifdef	__BORLANDC__
#    define	strncasecmp strnicmp
#  else // _MICROSOFT_ VC++
#    define strncasecmp	_strnicmp
#  endif
#endif

//=====>>> Costruttore <<<======
Profile::Profile(char const *name)
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


char *Profile::StripSpace(char const *sp)
{
	//toglie gli spazi iniziali
	while( *sp && isspace(*sp) )
		sp++;
	strncpy(strbuf, sp, MAXLINESIZE-1);
	strbuf[MAXLINESIZE-1] = '\0';
	//si posiziona in fondo
	char *p = strbuf;
	while( *p )
		p++;
	//toglie 
	while( isspace(*--p) )
		*p = '\0';
	return strbuf;
}


//---
void Profile::SetFileName(char const *name)
{
	if (name != 0)
		strcpy(filename, name);
}

void Profile::FlushVet(int force)
{
	int k;

	if (force)
		for (k = 0; k < MAXLINENUM; k++)
			profilevet[k] = 0;
	else
		for (k = 0; k < MAXLINENUM && profilevet[k] != 0; k++)
		{
			delete profilevet[k];
			profilevet[k] = 0;
		}
}

int Profile::WriteVet()
{
	FILE *fh = fopen(filename, "w");
	if (fh == 0)
		return -1;

	//Lettura file con allocazione linea parametro e aggiunta al vettore
	int rval = 0;
	int k;
	for (k = 0; k < MAXLINENUM && profilevet[k] != 0; k++)
	{
		char *sp = profilevet[k];
		while (isspace(*sp))		//elimina eventuali spazi iniziali
			sp++;
		if (strlen(sp))				//scarta le linee vuote
		{
			strcpy(linebuffer, sp);
			strcat(linebuffer, "\n");			//appende il new-line
			if ( fputs(linebuffer, fh) == EOF )
			{
				rval = -2;
				break;
			}
		}
	}
	
	cached = 1;		//ripensare attentamente se e` giusto forzarla qui
	fclose(fh);
	return rval;
}

int Profile::ReadVet()
{
	//Se il file e` gia` stato letto in memoria con successo allora non fare niente
	if (cached)
		return 0;

	//Svuotamento vettore parametri
	FlushVet();

	//Apertura file .INI
	FILE *fh = fopen(filename, "r");
	if (fh == 0)
		return 0;		//non costituisce errore, infatti il file puo` essere
						//  creato con una WriteVet(). Pero` attenti se la
						//  politica dovesse diventare write-back, la variabile
						//  cached andrebbe aggiornata!

	//Lettura file con allocazione linea parametro e aggiunta al vettore
	int rval = 0;
	int k;
	for (k = 0; k < MAXLINENUM && fgets(linebuffer, MAXLINESIZE, fh) != 0; k++)
	{
		char *sp = strchr(linebuffer, '\n');
		if (sp != 0)
			*sp = '\0';		//elimina il new-line finale
		sp = new char[strlen(linebuffer)+1];
		if (sp == 0)
		{
			rval = -2;	//out of memory, sigh!!!
			break;
		}
		profilevet[k] = sp;

		strcpy(sp, linebuffer);
	}
	fclose(fh);

	if (!rval)
		cached = 1;

	return rval;
}

char const *Profile::GetParameter(char const *id)
{
	//Controllo parametri
	if ( id == 0 || strlen(id) == 0 )
		return 0;

	//Legge i parametri dal file
	if ( ReadVet() )
		return 0;

	//Ricerca (lineare) del parametro
	int n = strlen(id);
	int j;
	for (j = 0; j < MAXLINENUM && profilevet[j] != 0; j++)
	{
		char const *sp = profilevet[j];
		while (isspace(*sp))		//elimina eventuali spazi iniziali
			sp++;
		if (strncasecmp(id, sp, n) == 0)	//trovata!!
		{
			sp += n;		//si posiziona in fondo all'id per cercare '='
			while (isspace(*sp))	//tralascia gli spazi
				sp++;
			if (*sp == '=')
			{
				//Solo ora siamo sicuri di avere trovato esattamente cio` che 
				//  cercavamo (vedi caso ricerca "maia" e trovato "maiale")
				while (isspace(*++sp))
					;
				return sp;		//restituisce il valore trovato
			}
		}
	}

	return 0;
}

int Profile::SetParameter(char const *id, char const *value)
{
	if ( id == 0 || strlen(id) == 0 || value == 0)
		return -1;

	//Leggiamo i parametri dal file
	int err = ReadVet();
	if (err)
		return err;

	//Ricerchiamo il parametro da variare, se non c'e` l'aggiungiamo
	int n = strlen(id);
	int found = 0;
	int j;
	for (j = 0; j < MAXLINENUM && profilevet[j] != 0; j++)
	{
		char const *sp = profilevet[j];
		while (isspace(*sp))		//elimina eventuali spazi iniziali
			sp++;
		if (strncasecmp(id, sp, n) == 0)	//trovata!!
		{
			sp += n;		//si posiziona in fondo all'id per cercare '='
			while (isspace(*sp))	//tralascia gli spazi
				sp++;
			if (*sp == '=')
			{
				//Solo ora siamo sicuri di avere trovato esattamente cio` che 
				//  cercavamo (vedi caso ricerca "maia" e trovato "maiale")
				found = 1;
				break;		//usciamo dal ciclo for()
			}
		}
	}	//for()

	//Nel caso non ci sia j sara` puntato sul primo elemento del vettore a 0
	if (!found)
	{
		if (j >= MAXLINENUM)	//Raggiunto il numero max di parametri, sigh!
			return -3;
	}

	//prepariamo la nuova stringa nel linebuffer
	strcpy(linebuffer, id);
	strcat(linebuffer, "=");
	strcat(linebuffer, value);

	//allochiamo memoria e copiamo la nuova stringa
	char *sp = new char[strlen(linebuffer)+1];
	if (sp == 0)
		return -2;		//Out of memory, Sigh!!
	strcpy(sp, linebuffer);

	//liberiamo la memoria della eventuale stringa corrente e aggiorniamo il parametro
	if (profilevet[j])
		delete profilevet[j];
	profilevet[j] = sp;

	//infine salviamo il tutto su file (politica write through)
	WriteVet();

	return 0;
}

