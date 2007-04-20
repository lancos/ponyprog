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

#include <stdio.h>

#ifdef _LINUX_
#include <unistd.h>
#include <sys/time.h>
#endif

#include "e2app.h"
#include "busio.h"

Wait::Wait()
{
	if (htimer == -1)
		CheckHwTimer();
}

Wait::~Wait()
{
}

int Wait::bogokips = 0;
int Wait::htimer = -1;

#ifdef	WIN32
LARGE_INTEGER Wait::mlpf;
#endif

//Check for a good hardware usec timer
int Wait::CheckHwTimer()
{
#ifdef	WIN32
	LARGE_INTEGER i1,i2;

	htimer = 0;								//Disable by default

	if ( QueryPerformanceFrequency(&mlpf) )		//return ticks per second if hw support high resolution timer
	{
		long usec = (long)(8 * mlpf.QuadPart / 1000000);	//test with 5 usec

		int k;
		for (k = 0; k < 50; k++)
		{
			QueryPerformanceCounter(&i1);
			QueryPerformanceCounter(&i2);
			if ( (i2.QuadPart - i1.QuadPart) < usec )
			{
				htimer = 1;		//Enable for fast computers
				break;
			}
		}
	}
#else
	struct timeval t1, t2;

	htimer = 0;		//Disable by default

	int k;
	for (k = 0; k < 50; k++)
	{
		gettimeofday(&t1, NULL);
		gettimeofday(&t2, NULL);
		timersub(&t2, &t1, &t1);
		if (t1.tv_usec < 8)
		{
			htimer = 1;		//Enable for fast computers
			break;
		}
	}
#endif

	return htimer;
}

void Wait::SetHwTimer(int ok)
{
	if (ok == 1)
		htimer = 1;
	else
	if (ok == 0)
		htimer = 0;
	else
		CheckHwTimer();
}

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
	if (msec > 30)
		Sleep(msec);
	else
		WaitUsec(msec * 1000);
# endif
#endif
}

/* Switch optimization OFF, so the compiler don't remove
 * the wait loop
 */
#ifndef __GNUC__
#pragma optimize( "", off )
#endif

void Wait::WaitUsec(int usec)
{
	if (htimer)
	{
#ifdef	WIN32
		LARGE_INTEGER i1, i2;

		QueryPerformanceCounter(&i1);

		long i_usec = (long)(usec * mlpf.QuadPart / 1000000);

		do {
			QueryPerformanceCounter(&i2);
		} while ( (long)(i2.QuadPart - i1.QuadPart) < i_usec );
#else
		struct timeval t1, t2;

		gettimeofday(&t1, NULL);
		t2.tv_sec = 0; t2.tv_usec = usec;
		timeradd(&t1, &t2, &t1);
		do {
			gettimeofday(&t2, NULL);
		} while (timercmp(&t2, &t1, <));
#endif
	}
	else
	{
		int k = usec * GetBogoKips() / 1000;

		while (k--)
			;
	}
}
