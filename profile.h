//=========================================================================//
//-------------------------------------------------------------------------//
// profile.h -- Header for Profile class                                   //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997, 1998  Claudio Lanconelli                           //
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

#ifndef _PROFILE_H
#define _PROFILE_H

#define	MAXFILENAME	256
#define	MAXLINENUM	1024
#define MAXLINESIZE	256

class Profile
{
  public:		//---------------------------------------- public

	Profile(char const *name = 0);
	virtual ~Profile();

	char const *GetParameter(char const *id);
	int SetParameter(char const *id, char const *value = "");
	void SetFileName(char const *name);

  protected:	//--------------------------------------- protected

	char *StripSpace(char const *sp);

  private:		//--------------------------------------- private

   void FlushVet(int force = 0);
   int ReadVet();
   int WriteVet();

	char filename[MAXFILENAME];		//name of .INI file
	char *profilevet[MAXLINENUM];	//elenco di parametri (in pratica linee del file)
    char linebuffer[MAXLINESIZE+1];	//buffer temporaneo di linea
	char strbuf[MAXLINESIZE];
    int cached;						//indica se il file e` gia` in memoria
};
#endif
