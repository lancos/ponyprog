//=========================================================================//
//-------------------------------------------------------------------------//
// wait.cpp -- Source for Wait class                                       //
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

//TODO: look for a blocking way to delay usec

#include <stdio.h>


#ifdef _LINUX_
#include <unistd.h>
#endif

#include "e2app.h"
#include "busio.h"

Wait::Wait()
{
}

Wait::~Wait()
{
}

int Wait::bogokips = 0;

void Wait::SetBogoKips()
{
	Wait::bogokips = THEAPP->GetBogoMips();
}

inline int Wait::GetBogoKips()
{
	if (Wait::bogokips == 0)
		SetBogoKips();

	return Wait::bogokips;
}

void Wait::WaitMsec(int msec)
{
#ifdef _LINUX_
	usleep(msec * 1000);
#else
# ifdef _WINDOWS
	Sleep(msec);
# endif
#endif
}

void Wait::WaitUsec(int usec, int test)
{
	int k = usec * GetBogoKips() / 1000;

	while (k--)
		;
}
