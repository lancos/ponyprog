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
#include "portint.h"
#include "errcode.h"

#include "globals.h"
#include "inpout32.h"

#include "e2app.h"

#ifdef	_LINUX_
#include <sys/io.h>
#include <unistd.h>

	int PortInterface::IOperm(int a, int b, int c)
	{
		int retval = -1;

		UserDebug3(UserApp3, "PortInterface::IOPerm(%x, %x, %x)\n", a, b, c);
		if (a + b <= 0x400)     //access to other ports needs iopl(3)
			retval = ioperm(a, b, c);
		UserDebug1(UserApp3, "PortInterface::IOPerm() %d\n", retval);

		return retval;
	}
#else
	int PortInterface::IOperm(int a, int b, int c)
	{
		c = a + b;
		return 0;
	}
#endif

#ifdef	_WINDOWS

//Use InpOut32 intead of direct I/O
#define	outb(val, id)	Out32(id, val)
#define	inb(id)			Inp32(id)

#ifdef	__BORLANDC__
# define	__inline__
#else // MICROSOFT VC++
# define	__inline__ __inline
# define	_export
#endif

#endif

enum LptRegs {
	dataOfst,
	statOfst,
	ctrlOfst
};

enum UartRegs {
	thrOfst,
	ierOfst,
	iirOfst,
	lcrOfst,
	mcrOfst,
	lsrOfst,
	msrOfst
};


PortInterface::PortInterface()
{
	UserDebug(Constructor, "PortInterface::PortInterface() Constructor\n");

	int k;
	for (k = 0; k < MAX_COMPORTS; k++)
	{
		ser_ports[k].base = 0;
		ser_ports[k].len = 8;
	}
	for (k = 0; k < MAX_LPTPORTS; k++)
	{
		par_ports[k].base = 0;
		par_ports[k].len = 8;
	}

//	ser_ports_base[0] = 0x3F8;
//	ser_ports_base[1] = 0x2F8;
//	ser_ports_base[2] = 0x3E8;
//	ser_ports_base[3] = 0x2E8;

//	par_ports_base[0] = 0x378;
//	par_ports_base[1] = 0x278;
//	par_ports_base[2] = 0x3BC;

#ifdef _LINUX_
	lcr_copy = ier_copy = -1;
#endif

	cpwreg = read_port = write_port = 0;
	first_port = last_port = no_ports = 0;

	UserDebug(Constructor, "PortInterface::PortInterface() O\n");
}

PortInterface::~PortInterface()
{
	UserDebug(Destructor, "PortInterface::~PortInterface() Destructor\n");
}

int PortInterface::InPort(int nport) const
{
	if (first_port == 0)
		return  E2ERR_NOTINSTALLED;;

	if (nport < 0 || nport >= no_ports)		//use default read port
		nport = read_port;
	else
		nport += first_port;

	return inb(nport);
}

int PortInterface::OutPort(int val, int nport)
{
	UserDebug2(UserApp3, "PortInterface::OutPort() ** %xh, %xh\n", first_port, last_port);

	if (first_port == 0)
		return	E2ERR_NOTINSTALLED;

	if (nport < 0 || nport >= no_ports)
		nport = write_port;
	else
		nport += first_port;

	if (nport == write_port)
		cpwreg = val;

	UserDebug2(UserApp3, "PortInterface::outb(%xh, %xh)\n", val, nport);
	outb(val, nport);

	return OK;
}

int PortInterface::OutPortMask(int mask, int val)
{
	UserDebug2(UserApp3, "PortInterface::OutPortMask(%d, %d)\n", mask, val);

	if (write_port == 0)
		return E2ERR_NOTINSTALLED;

	if (val == 0)	//clear the bit 1's in the mask (other bits unchanged)
	{
		cpwreg &= ~mask;
	}
	else
	if (val == 1)	//set the bit 1's in the mask (other bits unchanged)
	{
		cpwreg |= mask;
	}
	else	//toggle the bit 1's in the mask (other bits unchanged)
	{
		cpwreg ^= mask;
	}

	UserDebug2(UserApp3, "PortInterface::outb(%xh, %xh)\n", cpwreg, write_port);

	outb(cpwreg, write_port);

	return OK;
}

int PortInterface::OpenPort(const base_len *ports)
{
	UserDebug2(UserApp3, "PortInterface::OpenPort(%xh, %d) I\n", ports->base, ports->len);

	int ret_val = E2ERR_OPENFAILED;

	// PC ISA ports have only 10 bit addresses
	// check for the access to ports is granted
	if (ports->base >= 0x100 && ports->len > 0)
	{
		// request the access to I/O ports
		if (IOperm(ports->base, ports->len, 1) == 0)
		{
			ClosePort();	 // close any opened port
			first_port = ports->base;
			last_port = ports->base + ports->len - 1;
			no_ports = ports->len;

			ret_val = OK;
		}
		else
		{
			first_port = last_port = no_ports = 0;
			ret_val = E2ERR_ACCESSDENIED;
		}
	}

	UserDebug1(UserApp3, "PortInterface::OpenPort() = %d O\n", ret_val);

	return ret_val;
}

void PortInterface::ClosePort()
{
	UserDebug(UserApp3, "PortInterface::ClosePort() I\n");

	if (first_port && last_port && no_ports)
	{
		IOperm(first_port, no_ports, 0);
	}
	first_port = last_port = no_ports = 0;

	UserDebug(UserApp3, "PortInterface::ClosePort() O\n");
}

int PortInterface::OpenSerial(int no)
{
	UserDebug1(UserApp2, "PortInterface::OpenSerial(%d) I\n", no);

	int ret_val = E2ERR_OPENFAILED;

	if (no >= 1 && no <= MAX_COMPORTS)
	{
		if (ser_ports[no-1].base == 0)
			DetectPorts();

		//Test if port exist
		if (ser_ports[no-1].base > 0)
		{
#ifdef	_WINDOWS
			char str[MAXNUMDIGIT];

			//Test if port is already in use
			sprintf(str, "COM%d", no);
			hCom = CreateFile(str,
				GENERIC_READ | GENERIC_WRITE,
				0,		/* comm devices must be opened w/exclusive-access */
				NULL,	/* no security attrs */
				OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
				0,		/* not overlapped I/O */
				NULL	/* hTemplate must be NULL for comm devices */
			);

			if ( hCom != INVALID_HANDLE_VALUE )
			{
				GetCommMask(hCom, &old_mask);
				SetCommMask(hCom, 0);

				ret_val = OpenPort(&ser_ports[no-1]);
				if (ret_val == OK)
				{
					write_port = GetFirstPort() + mcrOfst;
					read_port = GetFirstPort() + msrOfst;
					cpwreg = 0;
				}
				else
					CloseSerial();
			}
#else	//Linux
			ret_val = OpenPort(&ser_ports[no-1]);

			if (ret_val == OK)
			{
				lcr_copy = inb(GetFirstPort() + lcrOfst);
				outb(0, GetFirstPort() + lcrOfst);
				ier_copy = inb(GetFirstPort() + ierOfst);
				outb(0, GetFirstPort() + ierOfst);

				write_port = GetFirstPort() + mcrOfst;
				read_port = GetFirstPort() + msrOfst;
				cpwreg = 0;
			}
#endif
		}
	}

	UserDebug1(UserApp2, "PortInterface::OpenSerial() = %d O\n", ret_val);

	return ret_val;
}

void PortInterface::CloseSerial()
{
#ifdef	_WINDOWS
	if ( hCom != INVALID_HANDLE_VALUE )
	{
		SetCommMask(hCom, old_mask);
		CloseHandle(hCom);
		hCom = INVALID_HANDLE_VALUE;
	}
#else
#define	BREAK_MASK	0x40

	if (lcr_copy != -1 && ier_copy != -1)
	{
		outb(lcr_copy&~BREAK_MASK, GetFirstPort() + lcrOfst);
		outb(ier_copy, GetFirstPort() + ierOfst);
		lcr_copy = ier_copy = -1;
	}
#endif
	read_port = write_port = 0;

	ClosePort();
}

int PortInterface::OpenParallel(int no)
{
	int ret_val = E2ERR_OPENFAILED;

	if (no >= 1 && no <= MAX_LPTPORTS)
	{
		if (!par_ports[no-1].base)
			DetectPorts();

		//Test if port exist
		if (par_ports[no-1].base)
		{
#ifdef	_WINDOWS
			char str[MAXNUMDIGIT];

			sprintf(str, "LPT%d", no);
			hCom = CreateFile(str,
				GENERIC_READ | GENERIC_WRITE,
				0,		/* comm devices must be opened w/exclusive-access */
				NULL,	/* no security attrs */
				OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
				0,		/* not overlapped I/O */
				NULL	/* hTemplate must be NULL for comm devices */
			);

			if ( hCom != INVALID_HANDLE_VALUE )
			{
				ret_val = OpenPort(&par_ports[no-1]);
				if (ret_val == OK)
				{
					write_port = GetFirstPort() + dataOfst;
					read_port = GetFirstPort() + statOfst;
					cpwreg = 0;
				}
				else
					CloseParallel();
			}
#else		//Linux
			ret_val = OpenPort(&par_ports[no-1]);

			if (ret_val == OK)
			{
				write_port = GetFirstPort() + dataOfst;
				read_port = GetFirstPort() + statOfst;
				cpwreg = 0;
			}
#endif
		}
	}

	return ret_val;
}

void PortInterface::CloseParallel()
{
#ifdef	_WINDOWS
	if ( hCom != INVALID_HANDLE_VALUE )
	{
		CloseHandle(hCom);
		hCom = INVALID_HANDLE_VALUE;
	}
#endif

	read_port = write_port = 0;

	ClosePort();
}


int PortInterface::GetSerBasePort(int no)
{
	if (no >= 1 && no <= MAX_COMPORTS)
	{
		if (!ser_ports[no-1].base)
			DetectPorts();

		return ser_ports[no-1].base;
	}
	else
		return 0;
}

int PortInterface::GetParBasePort(int no)
{
	if (no >= 1 && no <= MAX_LPTPORTS)
	{
		if (!par_ports[no-1].base)
			DetectPorts();

		return par_ports[no-1].base;
	}
	else
		return 0;
}

#ifdef WIN32

#include <windows.h>

#define	TRUE	1
#define	FALSE	0

static int winSystemVersion()
{
#if 0
	if ((long)GetVersion() >= 0)
		return 2;
#else
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

		if ( !GetVersionEx((OSVERSIONINFO *)&osvi) )
			return 0;
	}

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:

		return 2;		//WINNT

		break;

	case VER_PLATFORM_WIN32_WINDOWS:

		return 1;		//WIN9X

		break;

	}
#endif
	return 0;
}

#endif

//---------------------------------------------------------------------------
// DetectPorts()
//---------------------------------------------------------------------------
void PortInterface::DetectPorts()
{
	THEAPP->GetCOMAddress(ser_ports[0].base, ser_ports[1].base, ser_ports[2].base, ser_ports[3].base);
	THEAPP->GetLPTAddress(par_ports[0].base, par_ports[1].base, par_ports[2].base);

#ifdef	_WINDOWS
	if (THEAPP->GetAutoDetectPorts())
	{
		LPTCount = 0;		// No printer ports counted
		COMCount = 0;		// No serial ports counted

		// Detect the LPT and COM ports available

		// Are we running Windows NT?
		// Detect the printer ports available
		if (winSystemVersion() == 2)
		{
			DetectLPTPortsNT(); // Win2K / NT version
			DetectCOMPortsNT(); // Win2K / NT version
		}
		else
		{
			DetectPorts9x(); // Win9x version
		}
	}
	else
	{
		LPTCount = 3;
		COMCount = 4;
	}

	hCom = INVALID_HANDLE_VALUE;
#endif

	THEAPP->SetCOMAddress(ser_ports[0].base, ser_ports[1].base, ser_ports[2].base, ser_ports[3].base);
	THEAPP->SetLPTAddress(par_ports[0].base, par_ports[1].base, par_ports[2].base);
}


#ifdef	_WINDOWS

#if 0	// Not needed, PonyProg should run on Win95 and NT4, or not?
/********************************************************
 * TODO: These routines should be avoided completely,	*
 * it's much better resolved inside (my) inpout32.dll	*
 ********************************************************/
#include <setupapi.h>
extern "C" {
#include <cfgmgr32.h>
}
#include <devguid.h>

void PortInterface::DetectPorts2k() {
 memset(ser_ports,0,sizeof(base_len));
 memset(par_ports,0,sizeof(base_len));
 LPTCount=0;
 COMCount=0;
 HDEVINFO devs=SetupDiGetClassDevs((GUID*)&GUID_DEVCLASS_PORTS,NULL,0,DIGCF_PRESENT);
 if (devs!=INVALID_HANDLE_VALUE) {
  SP_DEVINFO_DATA devInfo;
  devInfo.cbSize=sizeof(devInfo);
  for (int i=0; SetupDiEnumDeviceInfo(devs,i,&devInfo); i++) {
   HKEY hKey;
   if (CM_Open_DevNode_Key(devInfo.DevInst,KEY_QUERY_VALUE,0,
     RegDisposition_OpenExisting,&hKey,CM_REGISTRY_HARDWARE)) continue;
   char s[16];
   DWORD slen=sizeof(s);
   *s=0;
   if (!RegQueryValueEx(hKey,"PortName",NULL,NULL,(LPBYTE)s,&slen)) {
    LOG_CONF Config;
    if (!CM_Get_First_Log_Conf(&Config,devInfo.DevInst,ALLOC_LOG_CONF)) {
     RES_DES resDes=0;
     RESOURCEID resId;
     if (!CM_Get_Next_Res_Des(&resDes,Config,ResType_IO,&resId,0)) {
      IO_RESOURCE ior;
      CM_Get_Res_Des_Data(resDes,&ior,sizeof(ior),0);
      CM_Free_Res_Des_Handle(resDes);
      unsigned n;
      char msg[80];
      _snprintf(msg,sizeof(msg),"PonyProg:DetectPorts2k(): Detected %s @ 0x%X\n",s,(int)ior.IO_Header.IOD_Alloc_Base);
      OutputDebugString(msg);
      if (sscanf(s,"COM%d",&n)==1) {
// sorry, this program cannot handle COM ports above COM4
       if (--n<MAX_COMPORTS) {
        ser_ports[n].base = (int)ior.IO_Header.IOD_Alloc_Base;
        ser_ports[n].len  = (int)ior.IO_Header.IOD_Alloc_End+1-ser_ports[n].base;
        COMCount++;
       }
      }else if (sscanf(s,"LPT%d",&n)==1) {
// sorry, this program cannot handle LPT ports above LPT4
       if (--n<MAX_LPTPORTS) {
        par_ports[n].base = (int)ior.IO_Header.IOD_Alloc_Base;
        par_ports[n].len  = (int)ior.IO_Header.IOD_Alloc_End+1-par_ports[n].base;
        LPTCount++;
       }
      }
     }
    }
    RegCloseKey(hKey);
   }
  }
 }
 SetupDiDestroyDeviceInfoList(devs);
}
#endif

//---------------------------------------------------------------------------
// DetectPorts9x()
//---------------------------------------------------------------------------
void PortInterface::DetectPorts9x()
{
	const char *BASE_KEY = "Config Manager\\Enum";
	const char *PROBLEM = "Problem";
	const char *ALLOCATION = "Allocation";
	const char *PORTNAME = "PortName";
	const char *HARDWARE_KEY = "HardwareKey";

	const REGSAM KEY_PERMISSIONS = KEY_ENUMERATE_SUB_KEYS |
								  KEY_QUERY_VALUE;

	HKEY CurKey;               // Current key when using the registry
	char KeyName[MAX_PATH];    // A key name when using the registry

	char **KeyList;            // List of keys
	DWORD KeyCount;            // Count of the number of keys in KeyList

	DWORD index;

	// Clear the port count
	LPTCount = 0;
	COMCount = 0;

	// Clear the LPT port array
	memset(par_ports, 0, sizeof(base_len));
	// Clear the COM port array
	memset(ser_ports, 0, sizeof(base_len));

   // Open the registry
   RegOpenKeyEx(HKEY_DYN_DATA, BASE_KEY, 0, KEY_PERMISSIONS, &CurKey);

   // Grab all the key names under HKEY_DYN_DATA
   //
   // Do this by first counting the number of keys,
   // then creating an array big enough to hold them
   // using the KeyList pointer.

   FILETIME DummyFileTime;
   DWORD DummyLength = MAX_PATH;
   KeyCount = 0;
   while (RegEnumKeyEx(
            CurKey, KeyCount++, KeyName, &DummyLength,
            NULL, NULL, NULL, &DummyFileTime
                       ) != ERROR_NO_MORE_ITEMS)
   {
      DummyLength = MAX_PATH;
   }

   KeyList = new char*[KeyCount];

   KeyCount = 0;
   DummyLength = MAX_PATH;
   while (RegEnumKeyEx(
            CurKey, KeyCount, KeyName, &DummyLength,
            NULL, NULL, NULL, &DummyFileTime
                       ) != ERROR_NO_MORE_ITEMS)
   {
      KeyList[KeyCount] = new char[DummyLength+1];
      strcpy(KeyList[KeyCount], KeyName);
      DummyLength = MAX_PATH;
      KeyCount++;
   }

   // Close the key
   RegCloseKey(CurKey);

   // Cycle through all keys; looking for a string valued subkey called
   // 'HardWareKey' which is not NULL, and another subkey called 'Problem'
   // whose fields are all valued 0.
   for (DWORD KeyIndex=0; KeyIndex<KeyCount; KeyIndex++)
   {
      int HasProblem = FALSE; // Is 'Problem' non-zero? Assume it is Ok

      // Open the key
      strcpy(KeyName, BASE_KEY);
      strcat(KeyName, "\\");
      strcat(KeyName, KeyList[KeyIndex]);
      if (RegOpenKeyEx(
            HKEY_DYN_DATA, KeyName, 0, KEY_PERMISSIONS, &CurKey
                        ) != ERROR_SUCCESS)
         continue;

      // Test for a 0 valued Problem sub-key,
      // which must only consist of raw data
      DWORD DataType, DataSize;
      RegQueryValueEx(CurKey, PROBLEM, NULL, &DataType, NULL, &DataSize);
      if (DataType == REG_BINARY)
      {
         // We have a valid, binary "Problem" sub-key
         // Test to see if the fields are zero

         char HardwareSubKey[MAX_PATH];
               // Data from the "Hardware" sub-key

         uint8_t *Data = new uint8_t[DataSize];
               // Data from "Problem" sub-key

         // Read the data from the "Problem" sub-key
         if (RegQueryValueEx(
                  CurKey, PROBLEM, NULL,
                  NULL, Data, &DataSize
                             ) == ERROR_SUCCESS)
         {
            // See if it has any problems
            for (DWORD index=0; index<DataSize; index++)
               HasProblem |= Data[index];
         }
         else
            HasProblem = TRUE; // No good

         delete[] Data;

         // Now try and read the Hardware sub-key
         DataSize = MAX_PATH;
         RegQueryValueEx(
            CurKey, HARDWARE_KEY, NULL, &DataType, (uint8_t *)HardwareSubKey, &DataSize
                         );
         if (DataType != REG_SZ)
            HasProblem = TRUE; // No good

         // Do we have no problem, and a non-null Hardware sub-key?
         if (!HasProblem && strlen(HardwareSubKey) > 0)
         {
            // Now open the key which is "pointed at" by HardwareSubKey
            RegCloseKey(CurKey);

            strcpy(KeyName, "Enum\\");
            strcat(KeyName, HardwareSubKey);
            if (RegOpenKeyEx(
                  HKEY_LOCAL_MACHINE, KeyName, 0, KEY_PERMISSIONS, &CurKey
                              ) != ERROR_SUCCESS)
               continue;

            // Now read in the PortName and obtain the LPT number from it
            char PortName[MAX_PATH];
            DataSize = MAX_PATH;
            RegQueryValueEx(
               CurKey, PORTNAME, NULL, &DataType, (uint8_t *)PortName, &DataSize
                            );
            if (DataType != REG_SZ)
               strcpy(PortName, ""); // No good

			//------- Search for COM ports
			if (strstr(PortName, "COM") != NULL)
			{
               int PortNumber;
                     // The nubmer of the port
               char PortNumberStr[MAX_PATH];
                     // Holds the number of the port as a string

               WORD Allocation[64];
                     // Holds the registry data for the port address allocation

               memset(PortNumberStr, '\0', MAX_PATH);
               strncpy(PortNumberStr,
                       strstr(PortName, "COM")+3,
                       strlen(PortName)-(strstr(PortName, "COM")-PortName)-2);

               // Find the port number
               PortNumber = atoi(PortNumberStr);

               // Find the address
               RegCloseKey(CurKey);

               strcpy(KeyName, BASE_KEY);
               strcat(KeyName, "\\");
               strcat(KeyName, KeyList[KeyIndex]);
               RegOpenKeyEx(HKEY_DYN_DATA, KeyName, 0, KEY_PERMISSIONS, &CurKey);

               DataSize = sizeof(Allocation);
               RegQueryValueEx(
                  CurKey, ALLOCATION, NULL, &DataType,
                  (unsigned char*)Allocation, &DataSize
                               );
               if (DataType == REG_BINARY)
               {
                  // Decode the Allocation data: the port address is present
                  // directly after a 0x000C entry (which doesn't have 0x0000
                  // after it).
                  for (int pos=0; pos<63; pos++)
				  {
					if (Allocation[pos] == 0x000C &&
						Allocation[pos+1] != 0x0000 &&
						PortNumber<=MAX_COMPORTS)
                     {
                        // Found a port; add it to the list
					//	ser_ports_base[COMCount] = Allocation[pos+1];
					//	if (Allocation[pos+2] > 0 && Allocation[pos+2] >= Allocation[pos+1])
					//		ser_ports_len[COMCount] = Allocation[pos+2] - Allocation[pos+1] + 1;

						ser_ports[PortNumber-1].base = Allocation[pos+1];
						if (Allocation[pos+2] > 0 && Allocation[pos+2] >= Allocation[pos+1])
							ser_ports[PortNumber-1].len = Allocation[pos+2] - Allocation[pos+1] + 1;

						COMCount++;
                        break;
                     }
				  }
               }
			}

			//------- Search for LPT ports
            // Make sure it has LPT in it
            if (strstr(PortName, "LPT") != NULL)
            {
               int PortNumber;
                     // The nubmer of the port
               char PortNumberStr[MAX_PATH];
                     // Holds the number of the port as a string

               WORD Allocation[64];
                     // Holds the registry data for the port address allocation

               memset(PortNumberStr, '\0', MAX_PATH);
               strncpy(PortNumberStr,
                       strstr(PortName, "LPT")+3,
                       strlen(PortName)-(strstr(PortName, "LPT")-PortName)-2);

               // Find the port number
               PortNumber = atoi(PortNumberStr);

               // Find the address
               RegCloseKey(CurKey);

               strcpy(KeyName, BASE_KEY);
               strcat(KeyName, "\\");
               strcat(KeyName, KeyList[KeyIndex]);
               RegOpenKeyEx(HKEY_DYN_DATA, KeyName, 0, KEY_PERMISSIONS, &CurKey);

               DataSize = sizeof(Allocation);
               RegQueryValueEx(
                  CurKey, ALLOCATION, NULL, &DataType,
                  (unsigned char*)Allocation, &DataSize
                               );
               if (DataType == REG_BINARY)
               {
                  // Decode the Allocation data: the port address is present
                  // directly after a 0x000C entry (which doesn't have 0x0000
                  // after it).
                  for (int pos=0; pos<63; pos++)
                     if (Allocation[pos] == 0x000C &&
                         Allocation[pos+1] != 0x0000 &&
                         PortNumber<=MAX_LPTPORTS)
                     {
                        // Found a port; add it to the list
						par_ports[PortNumber-1].base = Allocation[pos+1];
						if (Allocation[pos+2] > 0 && Allocation[pos+2] >= Allocation[pos+1])
							par_ports[PortNumber-1].len = Allocation[pos+2] - Allocation[pos+1] + 1;

					//	par_ports_base[LPTCount] = Allocation[pos+1];
					//	if (Allocation[pos+2] > 0 && Allocation[pos+2] >= Allocation[pos+1])
					//		par_ports_len[LPTCount] = Allocation[pos+2] - Allocation[pos+1] + 1;

						LPTCount++;
                        break;
                     }

               }
            }
         }
      }

      RegCloseKey(CurKey);
   }

   // Destroy our key list
   for (index=0; index<KeyCount; index++)
      delete[] KeyList[index];
   delete KeyList;
}

#include <ctype.h>

//---------------------------------------------------------------------------
// DetectPorts() Win2K / NT version
//---------------------------------------------------------------------------
void PortInterface::DetectLPTPortsNT()
{
	LPTCount = DetectPortsNT("Parport","LPT%d",par_ports,MAX_LPTPORTS);
}

void PortInterface::DetectCOMPortsNT()
{
	COMCount = DetectPortsNT("Serial","COM%d",ser_ports,MAX_COMPORTS);
}

// Static member function
// See the two possible usage examples above!
int PortInterface::DetectPortsNT(const char *ServiceName, const char *PortFormat, base_len *ports, int nports) {
// This revised code keeps some registry keys open, rather than collecting intermediate strings.
// No silly key enumeration at all!
// This compact code shows USEFULNESS of indent == 1.
 const REGSAM KEY_PERMISSIONS = KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE;
 char buf[MAX_PATH];	// handy stack-allocated multi-purpose buffer
 int Count=0;		// return value (can be greater than nports)

 memset(ports,0,nports*sizeof(base_len));	// Clear port array
 HKEY hCCS;		// Open the registry (first stage)
 if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet",0,KEY_PERMISSIONS,&hCCS)) {
  HKEY hSvcEnum;
  _snprintf(buf,sizeof(buf),"Services\\%s\\Enum",ServiceName);
  if (!RegOpenKeyEx(hCCS,buf,0,KEY_PERMISSIONS,&hSvcEnum)) {
   DWORD Length = sizeof Count;		// Count gets the number of ports in the system
   RegQueryValueEx(hSvcEnum,"Count",NULL,NULL,(PBYTE)&Count,&Length);
   for (int i=0; i<Count; i++) {
    _snprintf(buf,sizeof(buf),"%d",i);	// A simple number
    char PnpPath[MAX_PATH];		// Another buffer (small changes in code would avoid even this)
    Length = sizeof PnpPath;
    if (!RegQueryValueEx(hSvcEnum,buf,NULL,NULL,(PBYTE)PnpPath,&Length)) {
     HKEY hParams;
     _snprintf(buf,sizeof(buf),"Enum\\%s\\Device Parameters",PnpPath);
     if (!RegOpenKeyEx(hCCS,buf,0,KEY_PERMISSIONS,&hParams)) {
      int Index;
      Length=sizeof buf;
      if (!RegQueryValueEx(hParams,"PortName",NULL,NULL,(PBYTE)buf,&Length)
      && sscanf(buf,PortFormat,&Index)==1
      && (unsigned)--Index<(unsigned)nports) {	// Use zero-based index from here, avoid negative values
       HKEY hControl;
       _snprintf(buf,sizeof(buf),"Enum\\%s\\Control",PnpPath);
       if (!RegOpenKeyEx(hCCS,buf,0,KEY_PERMISSIONS,&hControl)) {
        Length=sizeof buf;
        if (!RegQueryValueEx(hControl,"AllocConfig",NULL,NULL,(PBYTE)buf,&Length)) {
// This undocumented AllocConfig structure is checked against Win2k and Win8/64.
// In both cases, the ResType entry was at byte offset 16.
         DWORD *p=(DWORD*)buf;
         for (int k=0; k<(int)Length/4-5; k++,p++) {
          if (p[0]==2			// have ResType_IO
          && !HIWORD(p[2])		// port address less than 64K
          && !p[3]			// no high DWORD part
          && p[4]<16) {			// length limited to 16
           ports[Index].base=p[2];	// We got one
           ports[Index].len =p[4];	// (NO check for typical ISA addresses anymore!!)
           break;
          }
         }
        }
        RegCloseKey(hControl);
       }
      }
      RegCloseKey(hParams);
     }
    }
   }/*for*/
   RegCloseKey(hSvcEnum);
  }
  RegCloseKey(hCCS);
 }
 return Count;
}

#endif
