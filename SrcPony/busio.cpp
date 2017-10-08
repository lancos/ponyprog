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

#include "busio.h"

#include "e2cmdw.h"

BusIO::BusIO(BusInterface *p)
	:       err_no(0),
			last_addr(0),
			shot_delay(5),
			busI(p),
			old_progress(0),
			last_programmed_addr(0)
{
}

int BusIO::CheckAbort(int progress)
{
	int abort = cmdWin->GetAbortFlag();

	if (!abort)
	{
		if ((progress == 0 /* && old_progress != 0*/) ||
				progress > old_progress + 4)
		{
			cmdWin->SetProgress(progress);
			old_progress = progress;
		}
	}

	return abort;
}

int BusIO::Error()
{
	int old_val = err_no;
	err_no = 0;
	return old_val;
}

void BusIO::SetDelay()
{
	shot_delay = 5; //basic timing of 5usec
}

void BusIO::SetDelay(int delay)
{
	if (delay >= 0)
	{
		shot_delay = delay;
	}
}
