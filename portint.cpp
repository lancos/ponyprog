//=========================================================================//
//-------------------------------------------------------------------------//
// portint.cpp -- Source for PortInterface class                           //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2002  Claudio Lanconelli                            //
//                                                                         //
//  e-mail: lancos@libero.it                                               //
//  http://www.LancOS.com                                                  //
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
#include "portint.h"
#include "errcode.h"

#include "globals.h"
#include "dlportio.h"

#include "e2app.h"

#ifdef	_LINUX_
//#include <asm/io.h>
#include <sys/io.h>		//#include <linux/ioport.h>
#include <unistd.h>

	int PortInterface::IOperm(int a, int b, int c)
	{
		int retval;
		UserDebug3(UserApp3, "PortInterface::IOPerm(%x, %x, %x)\n", a, b, c);
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

//Use the DLPortIO instead of direct I/O
#define	outb(val, id)	DlPortWritePortUchar(id, val)
#define	inb(id)			DlPortReadPortUchar(id)

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
		ser_ports_base[k] = 0;
		ser_ports_len[k] = 8;
	}
	for (k = 0; k < MAX_LPTPORTS; k++)
	{
		par_ports_base[k] = 0;
		par_ports_len[k] = 8;
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
		return E2ERR_NOTINSTALLED;;

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

int PortInterface::OpenPort(int from_port, int len)
{
	UserDebug2(UserApp3, "PortInterface::OpenPort(%xh, %d) I\n", from_port, len);

	int ret_val = E2ERR_OPENFAILED;

	// PC ISA ports have only 10 bit addresses
	// check for the access to ports is granted
	if (from_port >= 0x100 && /* from_port + len <= 0x400 && */ len > 0)
	{
		// request the access to I/O ports
		if ( IOperm(from_port, len, 1) == 0  )
		{
			ClosePort();	 // close any opened port
			first_port = from_port;
			last_port = from_port + len - 1;
			no_ports = len;

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
		if (ser_ports_base[no-1] == 0)
			DetectPorts();

		//Test if port exist
		if (ser_ports_base[no-1] > 0)
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

				ret_val = OpenPort(ser_ports_base[no-1], ser_ports_len[no-1]);
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
			ret_val = OpenPort(ser_ports_base[no-1], ser_ports_len[no-1]);

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
		if (par_ports_base[no-1] == 0)
			DetectPorts();

		//Test if port exist
		if (par_ports_base[no-1] > 0)
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
				ret_val = OpenPort(par_ports_base[no-1], par_ports_len[no-1]);
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
			ret_val = OpenPort(par_ports_base[no-1], par_ports_len[no-1]);

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
		if (ser_ports_base[no-1] == 0)
			DetectPorts();

		return ser_ports_base[no-1];
	}
	else
		return 0;
}

int PortInterface::GetParBasePort(int no)
{
	if (no >= 1 && no <= MAX_LPTPORTS)
	{
		if (par_ports_base[no-1] == 0)
			DetectPorts();

		return par_ports_base[no-1];
	}
	else
		return 0;
}

#ifdef	_WINDOWS

#include <windows.h>

#define	TRUE	1
#define	FALSE	0

#endif

//---------------------------------------------------------------------------
// DetectPorts()
//---------------------------------------------------------------------------
void PortInterface::DetectPorts()
{
	THEAPP->GetCOMAddress(ser_ports_base[0], ser_ports_base[1], ser_ports_base[2], ser_ports_base[3]);
	THEAPP->GetLPTAddress(par_ports_base[0], par_ports_base[1], par_ports_base[2]);

#ifdef	_WINDOWS
	if (THEAPP->GetAutoDetectPorts())
	{
		LPTCount = 0;		// No printer ports counted
		COMCount = 0;		// No serial ports counted

		// Detect the LPT and COM ports available
		int RunningWinNT;

		// Are we running Windows NT?
		OSVERSIONINFO os;
		memset(&os, NULL, sizeof(OSVERSIONINFO));
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&os);
		RunningWinNT=(os.dwPlatformId==VER_PLATFORM_WIN32_NT);

		// Detect the printer ports available
		if (RunningWinNT)
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

	THEAPP->SetCOMAddress(ser_ports_base[0], ser_ports_base[1], ser_ports_base[2], ser_ports_base[3]);
	THEAPP->SetLPTAddress(par_ports_base[0], par_ports_base[1], par_ports_base[2]);
}


#ifdef	_WINDOWS

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
	for (index=0; index<=MAX_LPTPORTS; index++)
	{
		par_ports_base[index] = 0;
		par_ports_len[index] = 0;
	}

	// Clear the COM port array
	for (index=0; index<=MAX_COMPORTS; index++)
	{
		ser_ports_base[index] = 0;
		ser_ports_len[index] = 0;
	}

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

         BYTE *Data = new BYTE[DataSize];
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
            CurKey, HARDWARE_KEY, NULL, &DataType, (BYTE *)HardwareSubKey, &DataSize
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
               CurKey, PORTNAME, NULL, &DataType, (BYTE *)PortName, &DataSize
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

						ser_ports_base[PortNumber-1] = Allocation[pos+1];
						if (Allocation[pos+2] > 0 && Allocation[pos+2] >= Allocation[pos+1])
							ser_ports_len[PortNumber-1] = Allocation[pos+2] - Allocation[pos+1] + 1;

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
						par_ports_base[PortNumber-1] = Allocation[pos+1];
						if (Allocation[pos+2] > 0 && Allocation[pos+2] >= Allocation[pos+1])
							par_ports_len[PortNumber-1] = Allocation[pos+2] - Allocation[pos+1] + 1;

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


//---------------------------------------------------------------------------
// DetectPorts() Win2K / NT version
//---------------------------------------------------------------------------
void PortInterface::DetectLPTPortsNT()
{
	const char *BASE_KEY = "SYSTEM\\CurrentControlSet\\Services\\Parport\\Enum";
	const char *ENUM_KEY = "SYSTEM\\CurrentControlSet\\Enum\\";

	const char *DEVICEPARAM = "Device Parameters";
	const char *PORTNAME = "PortName";
	const char *COUNT = "Count";
	const char *CONTROL = "Control";
	const char *ACTIVESERVICE = "ActiveService";
	const char *ALLOCCONFIG = "AllocConfig";

	const REGSAM KEY_PERMISSIONS = KEY_ENUMERATE_SUB_KEYS |	KEY_QUERY_VALUE;

	HKEY CurKey;               // Current key when using the registry
	char KeyName[MAX_PATH];    // A key name when using the registry

	char *LPTPortList[10];

	DWORD index;

	// Clear the port count
	LPTCount = 0;

	// Clear the LPT port array
	for (index = 0; index <= MAX_LPTPORTS; index++)
	{
		par_ports_base[index] = 0;
		par_ports_len[index] = 0;
	}

	for (index = 0; index < 10; index++)
		LPTPortList[index] = 0;

	// Open the registry
	if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, BASE_KEY, 0, KEY_PERMISSIONS, &CurKey) != ERROR_SUCCESS )
		return; // Can't do anything without this BASE_KEY

	// Grab all the value names under HKEY_LOCAL_MACHINE
	//
	// Do this by first counting the number of value names,
	// then creating an array big enough to hold them
	// using the ValueList pointer.

	DWORD ValueType, DataSize;
	DWORD DummyLength = MAX_PATH;
	DWORD ValueCount;          // Count of the number of value names in ValueList
	ValueCount = 0;
	while (RegEnumValue(
			CurKey, ValueCount, KeyName, &DummyLength,
			NULL, &ValueType, NULL, NULL
					   ) != ERROR_NO_MORE_ITEMS)
	{
		DummyLength = MAX_PATH;

		RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, NULL, &DataSize);
		if ( ValueType == REG_SZ && isdigit(KeyName[0]) )
		{
			int n = KeyName[0] - '0';

			LPTPortList[n] = new char[DataSize + strlen(ENUM_KEY) + 1];
			BYTE *Data = new BYTE[DataSize];
			RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, Data, &DataSize);
			strcpy(LPTPortList[n], ENUM_KEY);
			strcat(LPTPortList[n], (const char *)Data);

			delete[] Data;
		}
		else
		if ( ValueType == REG_DWORD && strcmp(KeyName, COUNT) == 0 && DataSize == 4)
		{
			DWORD Data;
			RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, (BYTE *)&Data, &DataSize);

			LPTCount = Data;
		}
		ValueCount++;
	}

	// Close the key
	RegCloseKey(CurKey);

	for (DWORD pIndex = 0; pIndex < 10; pIndex++)
	{
		if (LPTPortList[pIndex] != 0)
		{
			char *str = 0;
			int LPTIndex = -1;

			str = new char[strlen(LPTPortList[pIndex]) + 20];
			strcpy(str, LPTPortList[pIndex]);
			strcat(str, "\\");
			strcat(str, DEVICEPARAM);

			if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, str, 0, KEY_PERMISSIONS, &CurKey) != ERROR_SUCCESS )
			{
				delete str;
				continue;
			}
			else
			{
				delete str;
				str = 0;
			}

			ValueCount = 0;
			DummyLength = MAX_PATH;
			while ( RegEnumValue(CurKey, ValueCount, KeyName, &DummyLength,	NULL,
						&ValueType, NULL, NULL) != ERROR_NO_MORE_ITEMS )
			{
				DummyLength = MAX_PATH;

				RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, NULL, &DataSize);
				if ( ValueType == REG_SZ && strcmp(KeyName, PORTNAME) == 0 )
				{
					BYTE *Data = new BYTE[DataSize];
					RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, Data, &DataSize);

					if (strlen((const char *)Data) == 4 && strncmp("LPT", (const char *)Data, 3) == 0)
					{
						LPTIndex = Data[3] - '0';
					}

					delete[] Data;
				}

				ValueCount++;
			}

			RegCloseKey(CurKey);

			str = new char[strlen(LPTPortList[pIndex]) + 20];
			strcpy(str, LPTPortList[pIndex]);
			strcat(str, "\\");
			strcat(str, CONTROL);

			if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, str, 0, KEY_PERMISSIONS, &CurKey) != ERROR_SUCCESS )
			{
				delete str;
				continue;
			}
			else
			{
				delete str;
				str = 0;
			}

			ValueCount = 0;
			DummyLength = MAX_PATH;
			while ( RegEnumValue(CurKey, ValueCount, KeyName, &DummyLength,	NULL,
						&ValueType, NULL, NULL) != ERROR_NO_MORE_ITEMS )
			{
				DummyLength = MAX_PATH;

				RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, NULL, &DataSize);
				/** Ulteriore controllo: serve? **
				if ( ValueType == REG_SZ && strcmp(KeyName, ACTIVESERVICE) == 0 )
				{
					BYTE *Data = new BYTE[DataSize];
					RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, Data, &DataSize);
					if ( strcmp((char *)Data, "Parport") == 0)
					{
					}
					delete[] Data;
				}
				else
				**/
				if ( ValueType == REG_RESOURCE_LIST && strcmp(KeyName, ALLOCCONFIG) == 0 )
				{
					int found = 0;
					DWORD k;
					WORD *Data = new WORD[DataSize/2+1];
					RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, (BYTE *)Data, &DataSize);
					for (k = 0; k < DataSize/2; k++)
					{
						if (Data[k] == 0x3BC || Data[k] == 0x378 || Data[k] == 0x278)
						{
							found = 1;
							break;
						}
					}
					if (LPTIndex > 0 && LPTIndex <= MAX_LPTPORTS && found)
					{
						par_ports_base[LPTIndex - 1] = Data[k];
						par_ports_len[LPTIndex - 1] = 8;
					}
					delete[] Data;
				}

				ValueCount++;
			}

			RegCloseKey(CurKey);
		}
	}

	//Destroy KeyList
	for (index = 0; index < 10; index++)
	{
		if (LPTPortList[index] != 0)
		{
			delete LPTPortList[index];
			LPTPortList[index] = 0;
		}
	}
}

void PortInterface::DetectCOMPortsNT()
{
	const char *BASE_KEY = "SYSTEM\\CurrentControlSet\\Services\\Serial\\Enum";
	const char *ENUM_KEY = "SYSTEM\\CurrentControlSet\\Enum\\";

	const char *DEVICEPARAM = "Device Parameters";
	const char *PORTNAME = "PortName";
	const char *COUNT = "Count";
	const char *CONTROL = "Control";
	const char *ACTIVESERVICE = "ActiveService";
	const char *ALLOCCONFIG = "AllocConfig";

	const REGSAM KEY_PERMISSIONS = KEY_ENUMERATE_SUB_KEYS |	KEY_QUERY_VALUE;

	HKEY CurKey;               // Current key when using the registry
	char KeyName[MAX_PATH];    // A key name when using the registry

	char *COMPortList[10];

	DWORD index;

	// Clear the port count
	COMCount = 0;

	// Clear the LPT port array
	for (index = 0; index <= MAX_COMPORTS; index++)
	{
		ser_ports_base[index] = 0;
		ser_ports_len[index] = 0;
	}

	for (index = 0; index < 10; index++)
		COMPortList[index] = 0;

	// Open the registry
	if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, BASE_KEY, 0, KEY_PERMISSIONS, &CurKey) != ERROR_SUCCESS )
		return; // Can't do anything without this BASE_KEY

	// Grab all the value names under HKEY_LOCAL_MACHINE
	//
	// Do this by first counting the number of value names,
	// then creating an array big enough to hold them
	// using the ValueList pointer.

	DWORD ValueType, DataSize;
	DWORD DummyLength = MAX_PATH;
	DWORD ValueCount;          // Count of the number of value names in ValueList
	ValueCount = 0;
	while (RegEnumValue(
			CurKey, ValueCount, KeyName, &DummyLength,
			NULL, &ValueType, NULL, NULL
					   ) != ERROR_NO_MORE_ITEMS)
	{
		DummyLength = MAX_PATH;

		RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, NULL, &DataSize);
		if ( ValueType == REG_SZ && isdigit(KeyName[0]) )
		{
			int n = KeyName[0] - '0';

			COMPortList[n] = new char[DataSize + strlen(ENUM_KEY) + 1];
			BYTE *Data = new BYTE[DataSize];
			RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, Data, &DataSize);
			strcpy(COMPortList[n], ENUM_KEY);
			strcat(COMPortList[n], (const char *)Data);

			delete[] Data;
		}
		else
		if ( ValueType == REG_DWORD && strcmp(KeyName, COUNT) == 0 && DataSize == 4)
		{
			DWORD Data;
			RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, (BYTE *)&Data, &DataSize);

			COMCount = Data;
		}
		ValueCount++;
	}

	// Close the key
	RegCloseKey(CurKey);

	for (DWORD pIndex = 0; pIndex < 10; pIndex++)
	{
		if (COMPortList[pIndex] != 0)
		{
			char *str = 0;
			int COMIndex = -1;

			str = new char[strlen(COMPortList[pIndex]) + 20];
			strcpy(str, COMPortList[pIndex]);
			strcat(str, "\\");
			strcat(str, DEVICEPARAM);

			if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, str, 0, KEY_PERMISSIONS, &CurKey) != ERROR_SUCCESS )
			{
				delete str;
				continue;
			}
			else
			{
				delete str;
				str = 0;
			}

			ValueCount = 0;
			DummyLength = MAX_PATH;
			while ( RegEnumValue(CurKey, ValueCount, KeyName, &DummyLength,	NULL,
						&ValueType, NULL, NULL) != ERROR_NO_MORE_ITEMS )
			{
				DummyLength = MAX_PATH;

				RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, NULL, &DataSize);
				if ( ValueType == REG_SZ && strcmp(KeyName, PORTNAME) == 0 )
				{
					BYTE *Data = new BYTE[DataSize];
					RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, Data, &DataSize);

					if (strlen((const char *)Data) == 4 && strncmp("COM", (const char *)Data, 3) == 0)
					{
						COMIndex = Data[3] - '0';
					}

					delete[] Data;
				}

				ValueCount++;
			}

			RegCloseKey(CurKey);

			str = new char[strlen(COMPortList[pIndex]) + 20];
			strcpy(str, COMPortList[pIndex]);
			strcat(str, "\\");
			strcat(str, CONTROL);

			if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, str, 0, KEY_PERMISSIONS, &CurKey) != ERROR_SUCCESS )
			{
				delete str;
				continue;
			}
			else
			{
				delete str;
				str = 0;
			}

			ValueCount = 0;
			DummyLength = MAX_PATH;
			while ( RegEnumValue(CurKey, ValueCount, KeyName, &DummyLength,	NULL,
						&ValueType, NULL, NULL) != ERROR_NO_MORE_ITEMS )
			{
				DummyLength = MAX_PATH;

				RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, NULL, &DataSize);
				/** Ulteriore controllo: serve? **
				if ( ValueType == REG_SZ && strcmp(KeyName, ACTIVESERVICE) == 0 )
				{
					BYTE *Data = new BYTE[DataSize];
					RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, Data, &DataSize);
					if ( strcmp((char *)Data, "Parport") == 0)
					{
					}
					delete[] Data;
				}
				else
				**/
				if ( ValueType == REG_RESOURCE_LIST && strcmp(KeyName, ALLOCCONFIG) == 0 )
				{
					int found = 0;
					DWORD k;
					WORD *Data = new WORD[DataSize/2+1];
					RegQueryValueEx(CurKey, KeyName, NULL, &ValueType, (BYTE *)Data, &DataSize);
					for (k = 0; k < DataSize/2; k++)
					{
						if (Data[k] == 0x3F8 || Data[k] == 0x2F8 || Data[k] == 0x3E8 || Data[k] == 0x2E8)
						{
							found = 1;
							break;
						}
					}
					if (COMIndex > 0 && COMIndex <= MAX_COMPORTS && found)
					{
						ser_ports_base[COMIndex - 1] = Data[k];
						ser_ports_len[COMIndex - 1] = 8;
					}
					delete[] Data;
				}

				ValueCount++;
			}

			RegCloseKey(CurKey);
		}
	}

	//Destroy KeyList
	for (index = 0; index < 10; index++)
	{
		if (COMPortList[index] != 0)
		{
			delete COMPortList[index];
			COMPortList[index] = 0;
		}
	}
}


#endif
