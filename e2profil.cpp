//=========================================================================//
//-------------------------------------------------------------------------//
// e2profil.cpp -- source for E2Profile class (derived from Profile class) //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2000  Claudio Lanconelli                            //
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
#    define strncasecmp	strnicmp
#    define	strcasecmp stricmp
#  endif
#endif


//=====>>> Costruttore <<<======
E2Profile::E2Profile()	: Profile("e2p.ini")
{
}

/**
int E2Profile::GetParDelayLoop()
{
	char const *sp = GetParameter("BusDelayLoop");
	int rval = -1;
	if (sp)
		rval = atoi(sp);
	return rval;
}

int E2Profile::SetParDelayLoop(int value)
{
	char str[16];
	sprintf(str, "%d", value);
	return SetParameter("BusDelayLoop", str);
}
**/

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
	char str[16];
	sprintf(str, "%d", value);
	return SetParameter("BogoMipsX1000", str);
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
	int rval = -1;
	if (port >= 1 && port <= 4)
	{
		char str[16];
		sprintf(str, "%d", port);
		rval = SetParameter("PortNumber", str);
	}
	return rval;
}

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
}


UBYTE E2Profile::GetPolarityControl() 
{ 
  UBYTE res; 
  const char * rval; 
   
  res = 0; 
  rval = GetParameter("ClockPolarity"); 
  if (rval) 
    if (!strcasecmp(rval,"INV")) 
      res |=CLOCKINV; 
  rval = GetParameter("ResetPolarity"); 
  if (rval) 
    if (!strcasecmp(rval,"INV")) 
      res |=RESETINV; 
  rval = GetParameter("DOutPolarity"); 
  if (rval) 
    if (!strcasecmp(rval,"INV")) 
      res |=DOUTINV; 
  rval = GetParameter("DInPolarity"); 
  if (rval) 
    if (!strcasecmp(rval,"INV")) 
      res |=DININV; 
  return res; 
} 
 
int E2Profile::SetPolarityControl(UBYTE polarity_control) 
{ 
  int rval; 
 
  rval = (SetParameter("ResetPolarity", 
                     (THEAPP->GetPolarity() & RESETINV)?"INV":"TRUE")); 
  rval &=  (SetParameter("ClockPolarity", 
                        (THEAPP->GetPolarity() & CLOCKINV)?"INV":"TRUE")); 
  rval &=  (SetParameter("DOutPolarity", 
                        (THEAPP->GetPolarity() & DOUTINV)?"INV":"TRUE")); 
  rval &=  (SetParameter("DInPolarity", 
                        (THEAPP->GetPolarity() & DININV)?"INV":"TRUE")); 
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
	int rval = -1;
	if (page_write > 0)
	{
		char str[16];
		sprintf(str, "%d", page_write);
		rval = SetParameter("I2CBusPageWrite", str);
	}
	return rval;
}

int E2Profile::GetSPIPageWrite()
{
	char const *sp = GetParameter("BigSPIPageWrite");
	int rval = 16;		//Default: no page write (1 byte)

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetSPIPageWrite(int page_write)
{
	int rval = -1;
	if (page_write > 0)
	{
		char str[16];
		sprintf(str, "%d", page_write);
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
	int rval = -1;
	if (base_addr >= 0x00 && base_addr < 0x100)
	{
		char str[16];
		sprintf(str, "0x%02X", base_addr);
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
	}
	return rval;
}

int E2Profile::SetI2CSpeed(int speed)
{
	int rval = -1;

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

	return rval;
}


int E2Profile::GetSPIResetDelay()
{
	char const *sp = GetParameter("SPIResetDelay");
	int rval = 100;		//Default: 100 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetSPIResetDelay(int delay)
{
	int rval = -1;
	if (delay > 0)
	{
		char str[32];
		sprintf(str, "%d", delay);
		rval = SetParameter("SPIResetDelay", str);
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
	}
	return rval;
}

int E2Profile::SetSPISpeed(int speed)
{
	int rval = -1;

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

	return rval;
}

int E2Profile::GetMegaPageDelay()
{
	char const *sp = GetParameter("ATMegaPageWriteDelay");
	int rval = 30;		//Default: 30 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetMegaPageDelay(int delay)
{
	int rval = -1;
	if (delay > 0)
	{
		char str[32];
		sprintf(str, "%d", delay);
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
	}
	return rval;
}

int E2Profile::SetMicroWireSpeed(int speed)
{
	int rval = -1;

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
	}
	return rval;
}

int E2Profile::SetPICSpeed(int speed)
{
	int rval = -1;

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
	}
	return rval;
}

int E2Profile::SetSDESpeed(int speed)
{
	int rval = -1;

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
	int rval = -1;
	if (delay > 0)
	{
		char str[32];
		sprintf(str, "%d", delay);
		rval = SetParameter("PowerUpDelay", str);
	}
	return rval;
}

int E2Profile::GetAVRProgDelay()
{
	char const *sp = GetParameter("AVRByteWriteDelay");
	int rval = 10;		//Default: 10 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetAVRProgDelay(int delay)
{
	int rval = -1;
	if (delay > 0)
	{
		char str[32];
		sprintf(str, "%d", delay);
		rval = SetParameter("AVRByteWriteDelay", str);
	}
	return rval;
}

int E2Profile::GetAVREraseDelay()
{
	char const *sp = GetParameter("AVREraseDelay");
	int rval = 30;		//Default: 30 msec

	if (sp)
	{
		rval = atoi(sp);
	}
	return rval;
}

int E2Profile::SetAVREraseDelay(int delay)
{
	int rval = -1;
	if (delay > 0)
	{
		char str[32];
		sprintf(str, "%d", delay);
		rval = SetParameter("AVREraseDelay", str);
	}
	return rval;
}

char const *E2Profile::GetIODriverName()
{
	char const *sp = GetParameter("IODriverName");

	if (sp)
		return sp;
	else
		return "\\\\.\\ponyprog";
}

int E2Profile::SetIODriverName(char const *name)
{
	int rval = -1;
	if (name && strlen(name))
	{
		rval = SetParameter("IODriverName", (char *)name);
	}
	return rval;
}