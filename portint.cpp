//=========================================================================//
//-------------------------------------------------------------------------//
// portint.cpp -- Source for PortInterface class                           //
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
#include "portint.h"
#include "errcode.h"

//#include "e2app.h"
#include "dlportio.h"

#ifdef	_LINUX_
//#include <asm/io.h>		//May be your system need this too
#include <sys/io.h>
//May be your system need this instead of <sys/io.h>
//#include <linux/ioport.h>
#include <unistd.h>

	//Request Access to I/O ports
	int PortInterface::IOperm(int a, int b, int c)
	{
		int retval;
		UserDebug3(UserApp3, "PortInterface::IOPerm(%x, %x, %x)\n", a, b, c);
		retval = ioperm(a, b, c);
		UserDebug1(UserApp3, "PortInterface::IOPerm() %d\n", retval);
		return retval;
	}
#endif

#ifdef	_WINDOWS

//Use the DLPortIO instead of direct I/O
#define	outb(val, id)	DlPortWritePortUchar(id, val)
#define	inb(id)			DlPortReadPortUchar(id)

#ifdef	__BORLANDC__
//#   include <conio.h>
//#   define	inb(id)			inp(id)
//#   define	outb(val, id)	outp(id, val)
#   define	__inline__
#else // MICROSOFT VC++

//#   include <conio.h>
//#   define	inb(id)			_inp(id)
//#   define	outb(val, id)	_outp(id, val)
#   define	__inline__ __inline
#   define _export
#endif

/**
HIDDEN int GetWinVersion()
{ 
	OSVERSIONINFO osvi;

	memset(&osvi, 0, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx (&osvi);

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32s)
		return 1;
	else
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		return 2;
	else
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		return 3;
	else
		return 4;
}
**/

int PortInterface::IOperm(int a, int b, int c)
{
	int rv = OK;

	a += b - c;
/**
	rv = GetWinVersion();

	if (c)	//Open
	{
		if (rv == 1 || rv == 2)
		{
			rv = OK;
		}
		else
		{
			hPort = CreateFile(
			//	"\\\\.\\DirectIo0",                 // pointer to name of the file
				THEAPP->GetIODriverName(),
				GENERIC_READ | GENERIC_WRITE,       // access (read-write) mode
				FILE_SHARE_READ | FILE_SHARE_WRITE, // share mode
				NULL,                               // pointer to security attributes
				OPEN_EXISTING,                      // how to create
				0,                                  // file attributes
				NULL);                              // handle to file with attributes to copy

			if (hPort == INVALID_HANDLE_VALUE)    // did we succeed?
			{
				// possibly call GetLastError() to get a hint what failed
				rv = E2ERR_OPENFAILED;
			}
			else
				rv = OK;
		}
	}
	else
	{
		rv = OK;
	}

	//	if (hPort != INVALID_HANDLE_VALUE)
	//		CloseHandle(hPort);
	//	hPort = INVALID_HANDLE_VALUE;
**/
	return rv;
}
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

	ser_ports_base[0] = 0x3F8;
	ser_ports_base[1] = 0x2F8;
	ser_ports_base[2] = 0x3E8;
	ser_ports_base[3] = 0x2E8;

	par_ports_base[0] = 0x378;
	par_ports_base[1] = 0x278;
	par_ports_base[2] = 0x3BC;

#ifdef	_WINDOWS
	LPTCount = 0;      // No printer ports counted
	COMCount = 0;		// No serial ports counted

	// Detect the printer ports available
	DetectPorts();

	hCom = hPort = INVALID_HANDLE_VALUE;
#else
	lcr_copy = ier_copy = -1;
#endif

	cpwreg = read_port = write_port = 0;
	first_port = last_port = no_ports = 0;

	UserDebug(Constructor, "PortInterface::PortInterface() O\n");
}

PortInterface::~PortInterface()
{
#ifdef	_WINDOWS
	if (hPort != INVALID_HANDLE_VALUE)
		CloseHandle(hPort);
	hPort = INVALID_HANDLE_VALUE;
#endif
	UserDebug(Destructor, "PortInterface::~PortInterface() Destructor\n");
}

int PortInterface::InPort(int nport) const
{
	if (first_port == 0)
		return E2ERR_NOTINSTALLED;

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
		return E2ERR_NOTINSTALLED;

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

int PortInterface::OpenPort(int from_port, int len)
{
	UserDebug2(UserApp3, "PortInterface::OpenPort(%xh, %d) I\n", from_port, len);

	int ret_val = E2ERR_OPENFAILED;

	// PC ISA ports have only 10 bit addresses
	// check for the access to ports is granted
	if (from_port >= 0x100 && from_port + len <= 0x400 && len > 0)
	{
		// request the access to I/O ports
		if ( IOperm(from_port, len, 1) == 0  )
		{
			ClosePort();	// close any opened port
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
		//Test if port exist
		if (ser_ports_base[no-1] > 0)
		{
#ifdef	_WINDOWS
			char str[8];

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
				ret_val = OK;

				GetCommMask(hCom, &old_mask);
				SetCommMask(hCom, 0);
			}

			if (ret_val == OK)
				ret_val = OpenPort(ser_ports_base[no-1], ser_ports_len[no-1]);

			if (ret_val == OK)
			{
				write_port = GetFirstPort() + mcrOfst;
				read_port = GetFirstPort() + msrOfst;
				cpwreg = 0;
			}
			else
				CloseSerial();
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
		//Test if port exist
		if (par_ports_base[no-1] > 0)
		{
#ifdef	_WINDOWS
			char str[8];

			//Test if port is already in use
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


int PortInterface::GetSerBasePort(int no) const
{
	if (no >= 1 && no <= MAX_COMPORTS)
	{
		return ser_ports_base[no-1];
	}
	else
		return 0;
}

int PortInterface::GetParBasePort(int no) const
{
	if (no >= 1 && no <= MAX_LPTPORTS)
	{
		return par_ports_base[no-1];
	}
	else
		return 0;
}

#ifdef	_WINDOWS

#include <windows.h>

#define	TRUE	1
#define	FALSE	0

//---------------------------------------------------------------------------
// DetectPorts()
//---------------------------------------------------------------------------
void PortInterface::DetectPorts()
{
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
		LPTCount = 3;      // No printer ports counted
		COMCount = 4;		// No serial ports counted

		//	DetectPortsNT(); // WinNT version
	}
	else
	{
		DetectPorts9x(); // Win9x version
	}
}


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

			//------- Search for COM ports
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
// DetectPortsNT()
//---------------------------------------------------------------------------
void PortInterface::DetectPortsNT()
{
   const char *BASE_KEY = "HARDWARE\\DEVICEMAP\\PARALLEL PORTS";
   const char *LOADED_KEY = "HARDWARE\\RESOURCEMAP\\LOADED PARALLEL DRIVER RESOURCES\\Parport";
   const char *DOS_DEVICES = "\\DosDevices\\LPT";
   const char *DEVICE_PARALLEL = "\\Device\\Parallel";

   const REGSAM KEY_PERMISSIONS = KEY_ENUMERATE_SUB_KEYS |
                                  KEY_QUERY_VALUE;

   HKEY CurKey;               // Current key when using the registry
   char KeyName[MAX_PATH];    // A key name when using the registry

   char **ValueList;          // List of value names
   DWORD ValueCount;          // Count of the number of value names in ValueList

	DWORD index;

	// Clear the port count
	LPTCount = 0;
	// Clear the LPT port array
	for (index=0; index<=MAX_LPTPORTS; index++)
	{
		par_ports_base[index] = 0;
		par_ports_len[index] = 0;
	}

/** Detect Only LPT port
	// Clear the port count
	COMCount = 0;
	// Clear the COM port array
	for (index=0; index<=MAX_COMPORTS; index++)
	{
		ser_ports_base[index] = 0;
		ser_ports_len[index] = 0;
	}
**/

   // Open the registry
   if (RegOpenKeyEx(
         HKEY_LOCAL_MACHINE, BASE_KEY, 0, KEY_PERMISSIONS, &CurKey
                     ) != ERROR_SUCCESS)
      return; // Can't do anything without this BASE_KEY

   // Grab all the value names under HKEY_LOCAL_MACHINE
   //
   // Do this by first counting the number of value names,
   // then creating an array big enough to hold them
   // using the ValueList pointer.

   DWORD DummyLength = MAX_PATH;
   DWORD ValueType;
   ValueCount = 0;
   while (RegEnumValue(
            CurKey, ValueCount++, KeyName, &DummyLength,
            NULL, &ValueType, NULL, NULL
                       ) != ERROR_NO_MORE_ITEMS)
   {
      DummyLength = MAX_PATH;
   }

   ValueList = new char*[ValueCount];

   ValueCount = 0;
   DummyLength = MAX_PATH;
   while (RegEnumValue(
            CurKey, ValueCount, KeyName, &DummyLength,
            NULL, &ValueType, NULL, NULL
                       ) != ERROR_NO_MORE_ITEMS)
   {
      ValueList[ValueCount] = new char[DummyLength+1];
      strcpy(ValueList[ValueCount], KeyName);
      DummyLength = MAX_PATH;
      ValueCount++;
   }

   // Close the key
   RegCloseKey(CurKey);

   for (index=0; index<ValueCount; index++)
   {
		char DosDev[MAX_PATH];    // Key value for \DosDevices\LPT
		DWORD DataType, DataSize; // Type and size of data read from the registry

      // Is it a \DosDevices\LPT key?
      strcpy(KeyName, BASE_KEY);
      if (RegOpenKeyEx(
            HKEY_LOCAL_MACHINE, KeyName, 0, KEY_PERMISSIONS, &CurKey
                        ) == ERROR_SUCCESS)
      {
         DataSize = MAX_PATH;
         RegQueryValueEx(
            CurKey, ValueList[index], NULL, &DataType, (BYTE *)DosDev, &DataSize
                         );
         RegCloseKey(CurKey);

         // Make sure it was a string
         if (DataType != REG_SZ)
            strcpy(DosDev, "");
      }
      else
         strcpy(DosDev, "");

      if (strstr(DosDev, DOS_DEVICES) != NULL)
      {
         int PortNumber;                  // The nubmer of the port
         char PortNumberStr[MAX_PATH];    // String version of PortNumber
         char PortIDStr[MAX_PATH];        // PortID

         memset(PortNumberStr, '\0', MAX_PATH);
         strncpy(PortNumberStr,
                 strstr(DosDev, DOS_DEVICES) + strlen(DOS_DEVICES),
                 strlen(DosDev) - (strstr(DosDev, DOS_DEVICES)-DosDev)
                                - strlen(DOS_DEVICES) + 1
                 );

         // Get the Port ID
         memset(PortIDStr, '\0', MAX_PATH);
         strncpy(PortIDStr,
                 strstr(ValueList[index], DEVICE_PARALLEL) + strlen(DEVICE_PARALLEL),
                 strlen(ValueList[index])
                     - (strstr(ValueList[index], DEVICE_PARALLEL)-ValueList[index])
                     - strlen(DEVICE_PARALLEL) + 1
                 );

         // Get the port number
         PortNumber = atoi(PortNumberStr);

         // Get the port address
         RegOpenKeyEx(HKEY_LOCAL_MACHINE, LOADED_KEY, 0, KEY_PERMISSIONS, &CurKey);

         strcpy(DosDev, "\\Device\\ParallelPort");
         strcat(DosDev, PortIDStr);
         strcat(DosDev, ".Raw");

         if (RegQueryValueEx(
               CurKey, DosDev, NULL, &DataType, NULL, NULL
                             ) == ERROR_SUCCESS &&
             DataType == REG_RESOURCE_LIST)
         {
            WORD Allocation[64]; // Binary data with port number inside

            // Read in the binary data
            DataSize = sizeof(Allocation);
            RegQueryValueEx(
               CurKey, DosDev, NULL, NULL,
               (unsigned char*)Allocation, &DataSize
                            );

            // Found a port; add it to the list
            if (DataSize>0 && PortNumber<=MAX_LPTPORTS)
            {
			//	par_ports_base[LPTCount] = Allocation[12];
			//	if (Allocation[13] > 0 && Allocation[13] >= Allocation[12])
			//		par_ports_len[LPTCount] = Allocation[13] - Allocation[12] + 1;

				par_ports_base[PortNumber-1] = Allocation[12];
			//	if (Allocation[13] > 0 && Allocation[13] >= Allocation[12])
			//		par_ports_len[PortNumber-1] = Allocation[13] - Allocation[12] + 1;
				par_ports_len[PortNumber-1] = 8;

				LPTCount++;
            }
         }

         RegCloseKey(CurKey);
      }
   }

   // Destroy our key value list
   // Destroy our key list
   for (index=0; index<ValueCount; index++)
      delete[] ValueList[index];
   delete ValueList;
}


#endif
