//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id$
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

#define	MAXFILENAME	512
#define	MAXLINENUM	1024
#define MAXLINESIZE	512

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
	int decnum2str(int value, char *str, int len);
	int decnum2str(unsigned long value, char *str, int len);
	int hexnum2str(int value, char *str, int len);
	int hexnum2str(unsigned long value, char *str, int len);

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
