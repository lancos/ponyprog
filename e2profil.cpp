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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "e2profil.h"
#include "types.h"
#include "e2app.h"

#ifndef	_LINUX_
#  ifdef	__BORLANDC__
#    define	strncasecmp strnicmp
#    define	strcasecmp stricmp
#  else // _MICROSOFT_ VC++
#    define strncasecmp	_strnicmp
#    define	strcasecmp _stricmp
#    define snprintf _snprintf
#  endif
#endif


//=====>>> Costruttore <<<======
E2Profile::E2Profile()	: Profile("e2p.ini")
{
}

int E2Profile::GetBogoMips()
{
	char const *sp = GetParameter("BogoMipsX1000");
	int rval = 0;		//Defaultvalue
	if (sp)
		rval = atoi(sp);
	return rval;
}

int E2Profile::SetBogoMips(int value)
{
	char str[MAXNUMDIGIT];

	if ( decnum2str(value, str, MAXNUMDIGIT) == OK )
		return SetParameter("BogoMipsX1000", str);
	else
		return BADPARAM;
}

#include "eeptypes.h"

long E2Profile::GetLastDevType()
{
	char const *sp = GetParameter("DeviceType");

	if (sp)
		return GetEEPTypeFromString(sp);
	else
		return E2400;		//Default device type
}

int E2Profile::SetLastDevType(long devtype)
{
	int rval = -1;
	char const *sp = GetEEPTypeString(devtype);

	if (sp)
	{
		rval = SetParameter("DeviceType", StripSpace(sp));
	}

	return rval;
}



HInterfaceType E2Profile::GetParInterfType()
{
	extern HInterfaceType NameToInterfType(const char *name);

	return NameToInterfType(GetParameter("InterfaceType"));
}

int E2Profile::SetParInterfType(HInterfaceType type)
{
	const char *TypeToInterfName(HInterfaceType type);

	return SetParameter("InterfaceType", (char *)TypeToInterfName(type));
}

int E2Profile::GetParPortNo()
{
	char const *sp = GetParameter("PortNumber");
	int rval = -1;

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetParPortNo(int port)
{
	int rval = BADPARAM;

	if (port >= 1 && port <= 4)
	{
		char str[MAXNUMDIGIT];

		if ( decnum2str(port, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("PortNumber", str);
	}
	return rval;
}

/**
int E2Profile::SetLastFile(char const *name)
{
	int rval = -1;
	if (name && strlen(name))
	{
		char *sp = (char *)GetLastFile();
		if (sp && strcasecmp(name, sp) != 0)
			SetParameter("PreviousFile", sp);
		rval = SetParameter("LastFile", (char *)name);
	}
	return rval;
}

char const *E2Profile::GetLastFile()
{
	return GetParameter("LastFile");
}

char const *E2Profile::GetPrevFile()
{
	return GetParameter("PreviousFile");

**/

char const *E2Profile::GetLastScript()
{
	return GetParameter("LastScript");
}

int E2Profile::SetLastScript(char const *name)
{
	return SetParameter("LastScript", name);
}

int E2Profile::SetLastFile(char const *name, int data)
{
	int rval = BADPARAM;

	if (name && strlen(name))
	{
		char str[MAXPATH];
		int n;
		const char *sp = GetLastFile(n);
		if (sp && strcasecmp(name, sp) != 0)
		{
			strncpy(str, sp, MAXPATH-6);
			str[MAXPATH-6] = '\0';

			if (n == PROG_TYPE)
				strcat(str, "?PROG");
			else
			if (n == DATA_TYPE)
				strcat(str, "?DATA");
			else
				strcat(str, "?ALL");

			SetParameter("PreviousFile", str);
		}

		strncpy(str, name, MAXPATH-6);
		str[MAXPATH-6] = '\0';

		if (data == PROG_TYPE)
		{
			strcat(str, "?PROG");
		}
		else
		if (data == DATA_TYPE)
		{
			strcat(str, "?DATA");
		}
		else
		{
			strcat(str, "?ALL");
		}

		rval = SetParameter("LastFile", str);
	}
	return rval;
}

static char param_copy[MAXLINESIZE+1];

char const *E2Profile::GetLastFile(int &data)
{
	char const *sp = GetParameter("LastFile");

	data = ALL_TYPE;
	if (sp)
	{
		strncpy(param_copy, sp, MAXLINESIZE+1);
		param_copy[MAXLINESIZE] = '\0';
		sp = param_copy;

		char *p = (char *)strchr(sp, '?');
		if (p)
		{
			*p++ = '\0';
			if (strcasecmp(p, "DATA") == 0)
				data = DATA_TYPE;
			else
			if (strcasecmp(p, "PROG") == 0)
				data = PROG_TYPE;
		}
	}

	return sp;
}

char const *E2Profile::GetPrevFile(int &data)
{
	char const *sp = GetParameter("PreviousFile");

	data = ALL_TYPE;
	if (sp)
	{
		strncpy(param_copy, sp, MAXLINESIZE+1);
		param_copy[MAXLINESIZE] = '\0';
		sp = param_copy;

		char *p = (char *)strchr(sp, '?');
		if (p)
		{
			*p++ = '\0';
			if (strcasecmp(p, "DATA") == 0)
				data = DATA_TYPE;
			else
			if (strcasecmp(p, "PROG") == 0)
				data = PROG_TYPE;
		}
	}

	return sp;
}

UBYTE E2Profile::GetPolarityControl()
{
	UBYTE res;
	const char *rval;

	res = 0;
	rval = GetParameter("ClockPolarity");
	if (rval)
		if (!strcasecmp(rval, "INV"))
			res |= CLOCKINV;
	rval = GetParameter("ResetPolarity");
	if (rval)
		if (!strcasecmp(rval,"INV"))
			res |= RESETINV;
	rval = GetParameter("DOutPolarity");
	if (rval)
		if (!strcasecmp(rval,"INV"))
			res |= DOUTINV;
	rval = GetParameter("DInPolarity");
	if (rval)
		if (!strcasecmp(rval,"INV"))
			res |= DININV;
	return res;
}

int E2Profile::SetPolarityControl(UBYTE polarity_control)
{
	int rval = OK;

	if (rval == OK)
		rval = SetParameter("ResetPolarity",
						(polarity_control & RESETINV) ? "INV" : "TRUE");
	if (rval == OK)
		rval = SetParameter("ClockPolarity",
						(polarity_control & CLOCKINV) ? "INV" : "TRUE");
	if (rval == OK)
		rval = SetParameter("DOutPolarity",
						(polarity_control & DOUTINV) ? "INV" : "TRUE");
	if (rval == OK)
		rval = SetParameter("DInPolarity",
						(polarity_control & DININV) ? "INV" : "TRUE");
	return rval;
}

int E2Profile::GetI2CPageWrite()
{
	char const *sp = GetParameter("I2CBusPageWrite");
	int rval = 16;		//Default: 16 bytes page write (only for 16-bit I2C Bus eeprom)

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetI2CPageWrite(int page_write)
{
	int rval = BADPARAM;

	if (page_write > 0)
	{
		char str[MAXNUMDIGIT];

		if ( decnum2str(page_write, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("I2CBusPageWrite", str);
	}
	return rval;
}

int E2Profile::GetSPIPageWrite()
{
	char const *sp = GetParameter("BigSPIPageWrite");
	int rval = 16;		//Default: 16 bytes page write

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetSPIPageWrite(int page_write)
{
	int rval = BADPARAM;

	if (page_write > 0)
	{
		char str[MAXNUMDIGIT];

		if ( decnum2str(page_write, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("BigSPIPageWrite", str);
	}
	return rval;
}


int E2Profile::GetI2CBaseAddr()
{
	char const *sp = GetParameter("I2CBaseAddress");
	int rval = 0xA0;		//Default base address

	if (sp)
	{
		rval = strtol(sp, NULL, 16);
	}
	return rval;
}

int E2Profile::SetI2CBaseAddr(int base_addr)
{
	int rval = BADPARAM;

	if (base_addr >= 0x00 && base_addr < 0x100)
	{
		char str[MAXNUMDIGIT];

		if ( hexnum2str(base_addr, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("I2CBaseAddress", str);
	}
	return rval;
}

int E2Profile::GetI2CSpeed()
{
	char const *sp = GetParameter("I2CBusSpeed");
	int rval = NORMAL;		//Default speed

	if (sp)
	{
		if ( strcasecmp(sp,"TURBO") == 0 )
			rval = TURBO;
		else
		if ( strcasecmp(sp,"FAST") == 0 )
			rval = FAST;
		else
		if ( strcasecmp(sp,"SLOW") == 0 )
			rval = SLOW;
		else
		if ( strcasecmp(sp,"VERYSLOW") == 0 )
			rval = VERYSLOW;
		else
		if ( strcasecmp(sp,"ULTRASLOW") == 0 )
			rval = ULTRASLOW;
	}
	return rval;
}

int E2Profile::SetI2CSpeed(int speed)
{
	int rval = BADPARAM;

	if (speed == TURBO)
		rval = SetParameter("I2CBusSpeed", "TURBO");
	else
	if (speed == FAST)
		rval = SetParameter("I2CBusSpeed", "FAST");
	else
	if (speed == NORMAL)
		rval = SetParameter("I2CBusSpeed", "NORMAL");
	else
	if (speed == SLOW)
		rval = SetParameter("I2CBusSpeed", "SLOW");
	else
	if (speed == VERYSLOW)
		rval = SetParameter("I2CBusSpeed", "VERYSLOW");
	else
	if (speed == ULTRASLOW)
		rval = SetParameter("I2CBusSpeed", "ULTRASLOW");

	return rval;
}


int E2Profile::GetSPIResetPulse()
{
	char const *sp = GetParameter("SPIResetPulse");
	int rval = 100;		//Default: 100 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetSPIResetPulse(int delay)
{
	int rval = BADPARAM;

	if (delay > 0)
	{
		char str[MAXNUMDIGIT];

		if ( decnum2str(delay, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("SPIResetPulse", str);
	}
	return rval;
}

int E2Profile::GetSPIDelayAfterReset()
{
	char const *sp = GetParameter("SPIDelayAfterReset");
	int rval = 50;		//Default: 50 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetSPIDelayAfterReset(int delay)
{
	int rval = BADPARAM;

	if (delay > 0)
	{
		char str[MAXNUMDIGIT];

		if ( decnum2str(delay, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("SPIDelayAfterReset", str);
	}
	return rval;
}

int E2Profile::GetAT89DelayAfterReset()
{
	char const *sp = GetParameter("AT89DelayAfterReset");
	int rval = 50;		//Default: 50 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetAT89DelayAfterReset(int delay)
{
	int rval = BADPARAM;

	if (delay > 0)
	{
		char str[MAXNUMDIGIT];

		if ( decnum2str(delay, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("AT89DelayAfterReset", str);
	}
	return rval;
}

int E2Profile::GetAVRDelayAfterReset()
{
	char const *sp = GetParameter("AVRDelayAfterReset");
	int rval = 50;		//Default: 50 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetAVRDelayAfterReset(int delay)
{
	int rval = BADPARAM;

	if (delay > 0)
	{
		char str[MAXNUMDIGIT];

		if ( decnum2str(delay, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("AVRDelayAfterReset", str);
	}
	return rval;
}

int E2Profile::GetSPISpeed()
{
	char const *sp = GetParameter("SPIBusSpeed");
	int rval = NORMAL;		//Default speed

	if (sp)
	{
		if ( strcasecmp(sp,"TURBO") == 0 )
			rval = TURBO;
		else
		if ( strcasecmp(sp,"FAST") == 0 )
			rval = FAST;
		else
		if ( strcasecmp(sp,"SLOW") == 0 )
			rval = SLOW;
		else
		if ( strcasecmp(sp,"VERYSLOW") == 0 )
			rval = VERYSLOW;
		else
		if ( strcasecmp(sp,"ULTRASLOW") == 0 )
			rval = ULTRASLOW;
	}
	return rval;
}

int E2Profile::SetSPISpeed(int speed)
{
	int rval = BADPARAM;

	if (speed == TURBO)
		rval = SetParameter("SPIBusSpeed", "TURBO");
	else
	if (speed == FAST)
		rval = SetParameter("SPIBusSpeed", "FAST");
	else
	if (speed == NORMAL)
		rval = SetParameter("SPIBusSpeed", "NORMAL");
	else
	if (speed == SLOW)
		rval = SetParameter("SPIBusSpeed", "SLOW");
	else
	if (speed == VERYSLOW)
		rval = SetParameter("SPIBusSpeed", "VERYSLOW");
	else
	if (speed == ULTRASLOW)
		rval = SetParameter("SPIBusSpeed", "ULTRASLOW");

	return rval;
}

int E2Profile::GetMegaPageDelay()
{
	char const *sp = GetParameter("ATMegaPageWriteDelay");
	int rval = 50;		//Default: 50 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetMegaPageDelay(int delay)
{
	int rval = BADPARAM;

	if (delay > 0)
	{
		char str[MAXNUMDIGIT];

		if ( decnum2str(delay, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("ATMegaPageWriteDelay", str);
	}
	return rval;
}

int E2Profile::GetMicroWireSpeed()
{
	char const *sp = GetParameter("MicroWireBusSpeed");
	int rval = NORMAL;		//Default speed

	if (sp)
	{
		if ( strcasecmp(sp,"TURBO") == 0 )
			rval = TURBO;
		else
		if ( strcasecmp(sp,"FAST") == 0 )
			rval = FAST;
		else
		if ( strcasecmp(sp,"SLOW") == 0 )
			rval = SLOW;
		else
		if ( strcasecmp(sp,"VERYSLOW") == 0 )
			rval = VERYSLOW;
		else
		if ( strcasecmp(sp,"ULTRASLOW") == 0 )
			rval = ULTRASLOW;
	}
	return rval;
}

int E2Profile::SetMicroWireSpeed(int speed)
{
	int rval = BADPARAM;

	if (speed == TURBO)
		rval = SetParameter("MicroWireBusSpeed", "TURBO");
	else
	if (speed == FAST)
		rval = SetParameter("MicroWireBusSpeed", "FAST");
	else
	if (speed == NORMAL)
		rval = SetParameter("MicroWireBusSpeed", "NORMAL");
	else
	if (speed == SLOW)
		rval = SetParameter("MicroWireBusSpeed", "SLOW");
	else
	if (speed == VERYSLOW)
		rval = SetParameter("MicroWireBusSpeed", "VERYSLOW");
	else
	if (speed == ULTRASLOW)
		rval = SetParameter("MicroWireBusSpeed", "ULTRASLOW");

	return rval;
}

int E2Profile::GetPICSpeed()
{
	char const *sp = GetParameter("PICBusSpeed");
	int rval = NORMAL;		//Default speed

	if (sp)
	{
		if ( strcasecmp(sp,"TURBO") == 0 )
			rval = TURBO;
		else
		if ( strcasecmp(sp,"FAST") == 0 )
			rval = FAST;
		else
		if ( strcasecmp(sp,"SLOW") == 0 )
			rval = SLOW;
		else
		if ( strcasecmp(sp,"VERYSLOW") == 0 )
			rval = VERYSLOW;
		else
		if ( strcasecmp(sp,"ULTRASLOW") == 0 )
			rval = ULTRASLOW;
	}
	return rval;
}

int E2Profile::SetPICSpeed(int speed)
{
	int rval = BADPARAM;

	if (speed == TURBO)
		rval = SetParameter("PICBusSpeed", "TURBO");
	else
	if (speed == FAST)
		rval = SetParameter("PICBusSpeed", "FAST");
	else
	if (speed == NORMAL)
		rval = SetParameter("PICBusSpeed", "NORMAL");
	else
	if (speed == SLOW)
		rval = SetParameter("PICBusSpeed", "SLOW");
	else
	if (speed == VERYSLOW)
		rval = SetParameter("PICBusSpeed", "VERYSLOW");
	else
	if (speed == ULTRASLOW)
		rval = SetParameter("PICBusSpeed", "ULTRASLOW");

	return rval;
}

int E2Profile::GetSDESpeed()
{
	char const *sp = GetParameter("SDEBusSpeed");
	int rval = NORMAL;		//Default speed

	if (sp)
	{
		if ( strcasecmp(sp,"TURBO") == 0 )
			rval = TURBO;
		else
		if ( strcasecmp(sp,"FAST") == 0 )
			rval = FAST;
		else
		if ( strcasecmp(sp,"SLOW") == 0 )
			rval = SLOW;
		else
		if ( strcasecmp(sp,"VERYSLOW") == 0 )
			rval = VERYSLOW;
		else
		if ( strcasecmp(sp,"ULTRASLOW") == 0 )
			rval = ULTRASLOW;
	}
	return rval;
}

int E2Profile::SetSDESpeed(int speed)
{
	int rval = BADPARAM;

	if (speed == TURBO)
		rval = SetParameter("SDEBusSpeed", "TURBO");
	else
	if (speed == FAST)
		rval = SetParameter("SDEBusSpeed", "FAST");
	else
	if (speed == NORMAL)
		rval = SetParameter("SDEBusSpeed", "NORMAL");
	else
	if (speed == SLOW)
		rval = SetParameter("SDEBusSpeed", "SLOW");
	else
	if (speed == VERYSLOW)
		rval = SetParameter("SDEBusSpeed", "VERYSLOW");
	else
	if (speed == ULTRASLOW)
		rval = SetParameter("SDEBusSpeed", "ULTRASLOW");

	return rval;
}

int E2Profile::GetIMBusSpeed()
{
	char const *sp = GetParameter("IMBusSpeed");
	int rval = NORMAL;		//Default speed

	if (sp)
	{
		if ( strcasecmp(sp,"TURBO") == 0 )
			rval = TURBO;
		else
		if ( strcasecmp(sp,"FAST") == 0 )
			rval = FAST;
		else
		if ( strcasecmp(sp,"SLOW") == 0 )
			rval = SLOW;
		else
		if ( strcasecmp(sp,"VERYSLOW") == 0 )
			rval = VERYSLOW;
		else
		if ( strcasecmp(sp,"ULTRASLOW") == 0 )
			rval = ULTRASLOW;
	}
	return rval;
}

int E2Profile::SetIMBusSpeed(int speed)
{
	int rval = BADPARAM;

	if (speed == TURBO)
		rval = SetParameter("IMBusSpeed", "TURBO");
	else
	if (speed == FAST)
		rval = SetParameter("IMBusSpeed", "FAST");
	else
	if (speed == NORMAL)
		rval = SetParameter("IMBusSpeed", "NORMAL");
	else
	if (speed == SLOW)
		rval = SetParameter("IMBusSpeed", "SLOW");
	else
	if (speed == VERYSLOW)
		rval = SetParameter("IMBusSpeed", "VERYSLOW");
	else
	if (speed == ULTRASLOW)
		rval = SetParameter("IMBusSpeed", "ULTRASLOW");

	return rval;
}

int E2Profile::GetPowerUpDelay()
{
	char const *sp = GetParameter("PowerUpDelay");
	int rval = 200;		//Default: 200 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetPowerUpDelay(int delay)
{
	int rval = BADPARAM;

	if (delay > 0)
	{
		char str[MAXNUMDIGIT];

		if ( decnum2str(delay, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("PowerUpDelay", str);
	}
	return rval;
}

int E2Profile::GetAVRProgDelay()
{
	char const *sp = GetParameter("AVRByteWriteDelay");
	int rval = 20;		//Default: 20 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetAVRProgDelay(int delay)
{
	int rval = BADPARAM;

	if (delay > 0)
	{
		char str[MAXNUMDIGIT];

		if ( decnum2str(delay, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("AVRByteWriteDelay", str);
	}
	return rval;
}

int E2Profile::GetAVREraseDelay()
{
	char const *sp = GetParameter("AVREraseDelay");
	int rval = 50;		//Default: 50 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetAVREraseDelay(int delay)
{
	int rval = BADPARAM;

	if (delay > 0)
	{
		char str[MAXNUMDIGIT];

		if ( decnum2str(delay, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("AVREraseDelay", str);
	}
	return rval;
}

int E2Profile::GetMDAProgDelay()
{
	char const *sp = GetParameter("MDAWriteDelay");
	int rval = 30;		//Default: 30 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetMDAProgDelay(int delay)
{
	char str[MAXNUMDIGIT];

	if ( decnum2str(delay, str, MAXNUMDIGIT) == OK )
		return SetParameter("MDAWriteDelay", str);
	else
		return BADPARAM;
}

int E2Profile::GetNVMProgDelay()
{
	char const *sp = GetParameter("NVMWriteDelay");
	int rval = 30;		//Default: 30 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetNVMProgDelay(int delay)
{
	char str[MAXNUMDIGIT];

	if ( decnum2str(delay, str, MAXNUMDIGIT) == OK )
		return SetParameter("NVMWriteDelay", str);
	else
		return BADPARAM;
}

ULONG E2Profile::GetSerialNumVal()
{
	char const *sp = GetParameter("SerialNumberVal");
	ULONG rval = 0;		//Default 0

	if (sp)
	{
		rval = strtoul(sp,NULL,0);
	}
	return rval;
}

int E2Profile::SetSerialNumVal(unsigned long val)
{
	int rval = BADPARAM;

	if (val > 0)
	{
		char str[MAXNUMDIGIT];

		if ( hexnum2str(val, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("SerialNumberVal", str);
	}

	return rval;
}

int E2Profile::GetSerialNumAddress(long &start, int &size, bool &mtype)
{
	char const *sp;

	start = 0; size = 1; mtype = false;

	if ( (sp = GetParameter("SerialNumberAddr")) )
	{
		start = strtol(sp,NULL,0);
	}
	if ( (sp = GetParameter("SerialNumberSize")) )
	{
		size = atoi(sp);
	}
	if ( (sp = GetParameter("SerialNumberType")) )
	{
		if (strcmp(sp, "DATA") == 0)
			mtype = true;
	}

	return OK;
}

int E2Profile::SetSerialNumAddress(unsigned long start, int size, bool mtype)
{
	int rval = BADPARAM;
	char str[MAXNUMDIGIT];

	if (start >= 0)
	{
		if ( hexnum2str(start, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("SerialNumberAddr", str);
	}
	if (size >= 1)
	{
		if ( decnum2str(size, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("SerialNumberSize", str);
	}
	rval = SetParameter("SerialNumberType", mtype ? "DATA" : "PROG");

	return rval;
}

FmtEndian E2Profile::GetSerialNumFormat()
{
	char const *sp = GetParameter("SerialNumberFormat");

	if ( sp && strcmp("LITTLEENDIAN", sp) == 0 )
	{
		return FMT_LITTLE_ENDIAN;
	}
	else
		return FMT_BIG_ENDIAN;
}

int E2Profile::SetSerialNumFormat(FmtEndian fmt)
{
	if (fmt == FMT_BIG_ENDIAN)
		SetParameter("SerialNumberFormat", "BIGENDIAN");
	else
		SetParameter("SerialNumberFormat", "LITTLEENDIAN");

	return OK;
}

bool E2Profile::GetSerialNumAutoInc()
{
	char const *sp = GetParameter("SerialNumAutoIncrement");

	if (sp && strcasecmp(sp, "NO") == 0)
		return false;
	else
		return true;
}

int E2Profile::SetSerialNumAutoInc(bool val)
{
	if (val)
		SetParameter("SerialNumAutoIncrement", "YES");
	else
		SetParameter("SerialNumAutoIncrement", "NO");

	return OK;
}

UWORD E2Profile::GetProgramOptions()
{
  UWORD res;
  const char * rval;

  res = 0;
  rval = GetParameter("ReloadOption");
  if (rval)
	if (strcasecmp(rval,"NO") != 0)
	  res |=RELOAD_YES;
  rval = GetParameter("ReadFlashOption");
  if (rval)
	if (strcasecmp(rval,"NO") != 0)
	  res |=READFLASH_YES;
  rval = GetParameter("ReadEEpromOption");
  if (rval)
	if (strcasecmp(rval,"NO") != 0)
	  res |=READEEP_YES;
  rval = GetParameter("ByteSwapOption");
  if (rval)
	if (strcasecmp(rval,"NO") != 0)
	  res |=BYTESWAP_YES;
  rval = GetParameter("SetIDkeyOption");
  if (rval)
	if (strcasecmp(rval,"NO") != 0)
	  res |=SETID_YES;
  rval = GetParameter("ReadOscCalibration");
  if (rval)
	if (strcasecmp(rval,"NO") != 0)
	  res |=READOSCAL_YES;
  rval = GetParameter("EraseOption");
  if (rval)
	if (strcasecmp(rval,"NO") != 0)
	  res |=ERASE_YES;
  rval = GetParameter("WriteFlashOption");
  if (rval)
	if (strcasecmp(rval,"NO") != 0)
	  res |=FLASH_YES;
  rval = GetParameter("WriteEEpromOption");
  if (rval)
	if (strcasecmp(rval,"NO") != 0)
	  res |=EEPROM_YES;
  rval = GetParameter("WriteSecurityOption");
  if (rval)
	if (strcasecmp(rval,"NO") != 0)
	  res |=LOCK_YES;
  return res;
}

int E2Profile::SetProgramOptions(UWORD prog_option)
{
  int rval;

  rval =   (SetParameter("ReloadOption",
						(prog_option & RELOAD_YES)?"YES":"NO"));
  rval &=  (SetParameter("ReadFlashOption",
						(prog_option & READFLASH_YES)?"YES":"NO"));
  rval &=  (SetParameter("ReadEEpromOption",
						(prog_option & READEEP_YES)?"YES":"NO"));
  rval &=  (SetParameter("ByteSwapOption",
						(prog_option & BYTESWAP_YES)?"YES":"NO"));
  rval &=  (SetParameter("SetIDkeyOption",
						(prog_option & SETID_YES)?"YES":"NO"));
  rval &=  (SetParameter("ReadOscCalibration",
						(prog_option & READOSCAL_YES)?"YES":"NO"));
  rval &=  (SetParameter("EraseOption",
						(prog_option & ERASE_YES)?"YES":"NO"));
  rval &=  (SetParameter("WriteFlashOption",
						(prog_option & FLASH_YES)?"YES":"NO"));
  rval &=  (SetParameter("WriteEEpromOption",
						(prog_option & EEPROM_YES)?"YES":"NO"));
  rval &=  (SetParameter("WriteSecurityOption",
						(prog_option & LOCK_YES)?"YES":"NO"));
  return rval;
}


char const *E2Profile::GetLogFileName()
{
	char const *sp = GetParameter("LogFileName");

	if (sp == 0)
		sp = "PonyProg.log";

	return sp;
}

int E2Profile::SetLogFileName(char const *name)
{
	if (name && strlen(name))
	{
		SetParameter("LogFileName", name);
	}

	return OK;
}

char const *E2Profile::GetLockDir()
{
	char const *sp = GetParameter("ttyLockDir");
	if (sp == 0)
		sp = "/var/lock";
	return sp;
}
int E2Profile::SetLockDir(char const *name)
{
	if (name && strlen(name))
		SetParameter("ttyLockDir", name);
	return OK;
}

char const *E2Profile::GetDevDir()
{
	char const *sp = GetParameter("ttyDevDir");
	if (sp == 0)
		sp = "/dev";
	return sp;
}
int E2Profile::SetDevDir(char const *name)
{
	if (name && strlen(name))
		SetParameter("ttyDevDir", name);
	return OK;
}

char const *E2Profile::GetDevName()
{
	char const *sp = GetParameter("ttyDevName");
	if (sp == 0)
#ifdef	_LINUX_
		sp = "ttyS";
#else
		sp = "COM";
#endif
	return sp;
}
int E2Profile::SetDevName(char const *name)
{
	if (name && strlen(name))
		SetParameter("ttyDevName", name);
	return OK;
}

char const *E2Profile::GetHtmlBrowseApp()
{
	char const *sp = GetParameter("HtmlBrowseApp");
	if (sp == 0)
		sp = "konqueror";
	return sp;
}
int E2Profile::SetHtmlBrowseApp(char const *name)
{
	if (name && strlen(name))
		SetParameter("HtmlBrowseApp", name);
	return OK;
}

char const *E2Profile::GetLanguageCode()
{
	char const *sp = GetParameter("LanguageCode");

	if (sp == 0)
		sp = "C";

	return sp;
}

int E2Profile::SetLanguageCode(char const *name)
{
	if (name && strlen(name))
		SetParameter("LanguageCode", name);

	return OK;
}

bool E2Profile::GetLogEnabled()
{
	char const *sp = GetParameter("LogEnabled");

	if (sp && strcasecmp(sp, "YES") == 0)
		return true;
	else
		return false;
}

int E2Profile::SetLogEnabled(bool enabled)
{
	if (enabled)
		SetParameter("LogEnabled", "YES");
	else
		SetParameter("LogEnabled", "NO");

	return OK;
}

bool E2Profile::GetClearBufBeforeLoad()
{
	char const *sp = GetParameter("ClearBufferBeforeLoad");

	if (sp && strcasecmp(sp, "YES") == 0)
		return true;
	else
		return false;
}

int E2Profile::SetClearBufBeforeLoad(bool enabled)
{
	if (enabled)
		SetParameter("ClearBufferBeforeLoad", "YES");
	else
		SetParameter("ClearBufferBeforeLoad", "NO");

	return OK;
}

bool E2Profile::GetClearBufBeforeRead()
{
	char const *sp = GetParameter("ClearBufferBeforeRead");

	if (sp && strcasecmp(sp, "YES") == 0)
		return true;
	else
		return false;
}

int E2Profile::SetClearBufBeforeRead(bool enabled)
{
	if (enabled)
		SetParameter("ClearBufferBeforeRead", "YES");
	else
		SetParameter("ClearBufferBeforeRead", "NO");

	return OK;
}

bool E2Profile::GetAt89PageOp()
{
	char const *sp = GetParameter("AT89SPageOpEnabled");

	if (sp && strcasecmp(sp, "NO") == 0)
		return false;
	else
		return true;
}

int E2Profile::SetAt89PageOp(bool enabled)
{
	if (enabled)
		SetParameter("AT89SPageOpEnabled", "YES");
	else
		SetParameter("AT89SPageOpEnabled", "NO");

	return OK;
}

bool E2Profile::Get8253FallEdge()
{
	char const *sp = GetParameter("AT89S8253FallEdgeSampling");

	if (sp && strcasecmp(sp, "NO") == 0)
		return false;
	else
		return true;
}

int E2Profile::Set8253FallEdge(bool enabled)
{
	if (enabled)
		SetParameter("AT89S8253FallEdgeSampling", "YES");
	else
		SetParameter("AT89S8253FallEdgeSampling", "NO");

	return OK;
}

bool E2Profile::GetSoundEnabled()
{
	char const *sp = GetParameter("SoundEnabled");

	if (sp && strcasecmp(sp, "YES") == 0)
		return true;
	else
		return false;
}

int E2Profile::SetSoundEnabled(bool enabled)
{
	if (enabled)
		SetParameter("SoundEnabled", "YES");
	else
		SetParameter("SoundEnabled", "NO");

	return OK;
}


int E2Profile::GetCalibrationAddress(long &start, int &size, bool &mtype)
{
	char const *sp;

	start = 0; size = 1; mtype = false;

	if ( (sp = GetParameter("OscCalibrationAddr")) )
	{
		start = strtol(sp,NULL,0);
	}
	if ( (sp = GetParameter("OscCalibrationSize")) )
	{
		size = atoi(sp);
	}
	if ( (sp = GetParameter("OscCalibrationMemType")) )
	{
		if (strcmp(sp, "DATA") == 0)
			mtype = true;
	}

	return OK;
}

int E2Profile::SetCalibrationAddress(unsigned long start, int size, bool mtype)
{
	int rval = BADPARAM;
	char str[MAXNUMDIGIT];

	if (start >= 0)
	{
		if ( hexnum2str(start, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("OscCalibrationAddr", str);
	}
	if (size >= 1)
	{
		if ( decnum2str(size, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("OscCalibrationSize", str);
	}
	rval = SetParameter("OscCalibrationType", mtype ? "DATA" : "PROG");

	return rval;
}

int E2Profile::GetJDMCmd2CmdDelay()
{
	char const *sp = GetParameter("JDM-CmdToCmdDelay");
	int rval = 4000;		//Default: 4000 usec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetJDMCmd2CmdDelay(int delay)
{
	int rval = BADPARAM;

	if (delay >= 0)
	{
		char str[MAXNUMDIGIT];

		if ( decnum2str(delay, str, MAXNUMDIGIT) == OK )
			rval = SetParameter("JDM-CmdToCmdDelay", str);
	}
	return rval;
}

bool E2Profile::GetVerifyAfterWrite()
{
	char const *sp = GetParameter("VerifyAfterWrite");

	if (sp && strcasecmp(sp, "NO") == 0)
		return false;
	else
		return true;
}

int E2Profile::SetVerifyAfterWrite(bool enabled)
{
	if (enabled)
		SetParameter("VerifyAfterWrite", "YES");
	else
		SetParameter("VerifyAfterWrite", "NO");

	return OK;
}

bool E2Profile::GetAutoDetectPorts()
{
	char const *sp = GetParameter("AutoDetectPorts");

	if (sp && strcasecmp(sp, "NO") == 0)
		return false;
	else
		return true;
}

int E2Profile::SetAutoDetectPorts(bool enabled)
{
	if (enabled)
		SetParameter("AutoDetectPorts", "YES");
	else
		SetParameter("AutoDetectPorts", "NO");

	return OK;
}

int E2Profile::GetCOMAddress(int &com1, int &com2, int &com3, int &com4)
{
	char const *sp = GetParameter("COMPorts");

	com1 = 0x3F8;
	com2 = 0x2F8;
	com3 = 0x3E8;
	com4 = 0x2E8;

	if (sp)
	{
		sscanf(sp, "%x,%x,%x,%x", &com1, &com2, &com3, &com4);
	}

	return OK;
}

int E2Profile::SetCOMAddress(int com1, int com2, int com3, int com4)
{
	char str[STRBUFSIZE];

	if (com1 > 0)
	{
		if (com2 > 0)
		{
			if (com3 > 0)
			{
				if (com4 > 0)
				{
					snprintf(str, STRBUFSIZE, "%X,%X,%X,%X", com1, com2, com3, com4);
				}
				else
				{
					snprintf(str, STRBUFSIZE, "%X,%X,%X", com1, com2, com3);
				}
			}
			else
			{
				snprintf(str, STRBUFSIZE, "%X,%X", com1, com2);
			}
		}
		else
		{
			snprintf(str, STRBUFSIZE, "%X", com1);
		}

		SetParameter("COMPorts", str);
	}

	return OK;
}

int E2Profile::GetLPTAddress(int &lpt1, int &lpt2, int &lpt3)
{
	char const *sp = GetParameter("LPTPorts");

	lpt1 = 0x378;
	lpt2 = 0x278;
	lpt3 = 0x3BC;

	if (sp)
	{
		sscanf(sp, "%x,%x,%x", &lpt1, &lpt2, &lpt3);
	}

	return OK;
}

int E2Profile::SetLPTAddress(int lpt1, int lpt2, int lpt3)
{
	char str[STRBUFSIZE];

	if (lpt1 > 0)
	{
		if (lpt2 > 0)
		{
			if (lpt3 > 0)
			{
				snprintf(str, STRBUFSIZE, "%X,%X,%X", lpt1, lpt2, lpt3);
			}
			else
			{
				snprintf(str, STRBUFSIZE, "%X,%X", lpt1, lpt2);
			}
		}
		else
		{
			snprintf(str, STRBUFSIZE, "%X", lpt1);
		}

		SetParameter("LPTPorts", str);
	}

	return OK;
}

FileType E2Profile::GetDefaultFileType()
{
	char const *sp = GetParameter("DefaultFileType");
	FileType ft = E2P;

	if (sp)
	{
		if ( strcasecmp(sp, "e2p") == 0 )
			ft = E2P;
		else
		if ( strcasecmp(sp, "bin") == 0 )
			ft = BIN;
		else
		if ( strcasecmp(sp, "csm") == 0 )
			ft = CSM;
		else
		if ( strcasecmp(sp, "intel-hex") == 0 )
			ft = INTEL;
		else
		if ( strcasecmp(sp, "mot-srec") == 0 )
			ft = MOTOS;
	}

	return ft;
}

int E2Profile::SetDefaultFileType(FileType ft)
{
	char str[STRBUFSIZE];

	str[0] = '\0';

	if (ft == E2P)
		strncpy(str, "E2P", STRBUFSIZE);
	else
	if (ft == BIN)
		strncpy(str, "BIN", STRBUFSIZE);
	else
	if (ft == CSM)
		strncpy(str, "CSM", STRBUFSIZE);
	else
	if (ft == INTEL)
		strncpy(str, "INTEL-HEX", STRBUFSIZE);
	else
	if (ft == MOTOS)
		strncpy(str, "MOT-SREC", STRBUFSIZE);

	str[STRBUFSIZE-1] = '\0';
	if (strlen(str))
	{
		SetParameter("DefaultFileType", str);
	}

	return OK;
}
