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

// #include <stdio.h>

#include <QDebug>
#include <QString>
#include <QFile>
#include <QTextStream>

#include "globals.h"
#include "portint.h"
#include "errcode.h"

#include "e2cmdw.h"

#ifdef  __linux__
# include <sys/io.h>
# include <unistd.h>

int PortInterface::IOperm(int a, int b, int c)
{
	int retval = -1;

	qDebug() << "PortInterface::IOPerm(" << (hex) << a << ", " << b << ", " << c << (dec) << ")";

	if (a + b <= 0x400)     //access to other ports needs iopl(3)
	{
		retval = ioperm(a, b, c);
	}

	qDebug() << "PortInterface::IOPerm() " << retval;

	return retval;
}
#else
int PortInterface::IOperm(int a, int b, int c)
{
	return 0;
}
#endif

enum LptRegs
{
	dataOfst,
	statOfst,
	ctrlOfst
};

enum UartRegs
{
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
	qDebug() << "PortInterface::PortInterface()";

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

	//      ser_ports_base[0] = 0x3F8;
	//      ser_ports_base[1] = 0x2F8;
	//      ser_ports_base[2] = 0x3E8;
	//      ser_ports_base[3] = 0x2E8;

	//      par_ports_base[0] = 0x378;
	//      par_ports_base[1] = 0x278;
	//      par_ports_base[2] = 0x3BC;

#ifdef __linux__
	lcr_copy = ier_copy = -1;
#endif

#ifdef  Q_OS_WIN32
	gfpOut32 = NULL;
	gfpInp32 = NULL;
	gfpIsInpOutDriverOpen = NULL;
	gfpIsXP64Bit = NULL;

	//Dynamically load the DLL at runtime (not linked at compile time)
	hInpOutDll = LoadLibrary(L"inpout32.dll");               //The 32bit DLL. If we are building x64 C++

	//application then use InpOutx64.dll
	if (hInpOutDll != NULL)
	{
		gfpOut32 = (lpOut32)GetProcAddress(hInpOutDll, "Out32");
		gfpInp32 = (lpInp32)GetProcAddress(hInpOutDll, "Inp32");
		gfpIsInpOutDriverOpen = (lpIsInpOutDriverOpen)GetProcAddress(hInpOutDll, "IsInpOutDriverOpen");
		gfpIsXP64Bit = (lpIsXP64Bit)GetProcAddress(hInpOutDll, "IsXP64Bit");

		if (!gfpIsInpOutDriverOpen())
		{
			qDebug() << "Unable to open InpOut32 Driver!";
			//All done
			FreeLibrary(hInpOutDll);
			hInpOutDll = NULL;
		}
	}
	else
	{
		qDebug() << "Unable to load InpOut32 DLL!";
	}

#endif

	cpwreg = read_port = write_port = 0;
	first_port = last_port = no_ports = 0;

	qDebug() << "PortInterface::PortInterface() O";
}

PortInterface::~PortInterface()
{
	qDebug() <<  "PortInterface::~PortInterface()";

#ifdef  Q_OS_WIN32

	if (hInpOutDll != NULL)
	{
		//All done
		FreeLibrary(hInpOutDll);
		hInpOutDll = NULL;
	}

#endif
}

int PortInterface::InPort(int nport) const
{
	qDebug() << "PortInterface::OutPort() ** " << (hex) << first_port << ", " <<  nport << (dec);

#ifdef  Q_OS_WIN32

	if (gfpInp32 == NULL)
	{
		return E2ERR_OPENFAILED;
	}

#endif

	if (first_port == 0)
	{
		return  E2ERR_NOTINSTALLED;
	}

	if (nport < 0 || nport >= no_ports)             //use default read port
	{
		nport = read_port;
	}
	else
	{
		nport += first_port;
	}

#ifdef  Q_OS_WIN32
	return gfpInp32(nport);
#else
	return inb(nport);
#endif
}

int PortInterface::OutPort(int val, int nport)
{
	qDebug() << "PortInterface::OutPort() ** " << (hex) << first_port << ", " << last_port << (dec);

#ifdef  Q_OS_WIN32

	if (gfpOut32 == NULL)
	{
		return E2ERR_OPENFAILED;
	}

#endif

	if (first_port == 0)
	{
		return        E2ERR_NOTINSTALLED;
	}

	if (nport < 0 || nport >= no_ports)
	{
		nport = write_port;
	}
	else
	{
		nport += first_port;
	}

	if (nport == write_port)
	{
		cpwreg = val;
	}

	qDebug() << "PortInterface::outb(" << (hex) << val << ", " << nport << (dec) << ")";
#ifdef  Q_OS_WIN32
	gfpOut32(nport, val);
#else
	outb(val, nport);
#endif
	return OK;
}

int PortInterface::OutPortMask(int mask, int val)
{
	qDebug() << "PortInterface::OutPortMask(" << mask << ", " << val <<  ")";

#ifdef  Q_OS_WIN32

	if (gfpOut32 == NULL)
	{
		return E2ERR_OPENFAILED;
	}

#endif

	if (write_port == 0)
	{
		return E2ERR_NOTINSTALLED;
	}

	if (val == 0)   //clear the bit 1's in the mask (other bits unchanged)
	{
		cpwreg &= ~mask;
	}
	else if (val == 1)     //set the bit 1's in the mask (other bits unchanged)
	{
		cpwreg |= mask;
	}
	else     //toggle the bit 1's in the mask (other bits unchanged)
	{
		cpwreg ^= mask;
	}

	qDebug() << "PortInterface::outb(" << (hex) << cpwreg << ", " << (dec) << write_port << ")";

#ifdef  Q_OS_WIN32
	gfpOut32(write_port, cpwreg);
#else
	outb(cpwreg, write_port);
#endif
	return OK;
}

int PortInterface::OpenPort(const base_len *ports)
{
	qDebug() << "PortInterface::OpenPort(" << (hex) << ports->base << (dec) << ", " << ports->len << ") I";

	int ret_val = E2ERR_OPENFAILED;

	// PC ISA ports have only 10 bit addresses
	// check for the access to ports is granted
	if (ports->base >= 0x100 && ports->len > 0)
	{
		// request the access to I/O ports
		if (IOperm(ports->base, ports->len, 1) == 0)
		{
			ClosePort();     // close any opened port
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

	qDebug() << "PortInterface::OpenPort() = " << ret_val << " O";

	return ret_val;
}

void PortInterface::ClosePort()
{
	qDebug() << "PortInterface::ClosePort() I";

	if (first_port && last_port && no_ports)
	{
		IOperm(first_port, no_ports, 0);
	}

	first_port = last_port = no_ports = 0;

	qDebug() << "PortInterface::ClosePort() O";
}

int PortInterface::OpenSerial(int no)
{
	qDebug() << "PortInterface::OpenSerial(" << no << ") I";

	int ret_val = E2ERR_OPENFAILED;

	if (no >= 0 && no < MAX_COMPORTS)
	{
		if (ser_ports[no].base == 0)
		{
			DetectPorts();
		}

		//Test if port exist
		if (ser_ports[no].base > 0)
		{
#ifdef Q_OS_WIN32
			//Test if port is already in use
			QString str = E2Profile::GetCOMDevName() + QString::number(no + 1);
			hCom = CreateFile((LPCWSTR)str.utf16(),
							  GENERIC_READ | GENERIC_WRITE,
							  0,                    // comm devices must be opened w/exclusive-access
							  NULL,                 // no security attrs
							  OPEN_EXISTING,// comm devices must use OPEN_EXISTING
							  0,                    // not overlapped I/O
							  NULL                  // hTemplate must be NULL for comm devices
							 );

			if (hCom != INVALID_HANDLE_VALUE)
			{
				GetCommMask(hCom, &old_mask);
				SetCommMask(hCom, 0);

				ret_val = OpenPort(&ser_ports[no]);

				if (ret_val == OK)
				{
					write_port = GetFirstPort() + mcrOfst;
					read_port = GetFirstPort() + msrOfst;
					cpwreg = 0;
				}
				else
				{
					CloseSerial();
				}
			}

#else   //Linux
			ret_val = OpenPort(&ser_ports[no]);

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

	qDebug() << "PortInterface::OpenSerial() = " << ret_val << " O";

	return ret_val;
}

void PortInterface::CloseSerial()
{
#ifdef  Q_OS_WIN32

	if (hCom != INVALID_HANDLE_VALUE)
	{
		SetCommMask(hCom, old_mask);
		CloseHandle(hCom);
		hCom = INVALID_HANDLE_VALUE;
	}

#else
#define BREAK_MASK      0x40

	if (lcr_copy != -1 && ier_copy != -1)
	{
		outb(lcr_copy & ~BREAK_MASK, GetFirstPort() + lcrOfst);
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

	if (no >= 0 && no < MAX_LPTPORTS)
	{
		if (par_ports[no].base == 0)
		{
			DetectPorts();
		}

		//Test if port exist
		if (par_ports[no].base)
		{
#ifdef Q_OS_WIN32
			QString str = E2Profile::GetLPTDevName() + QString::number(no + 1);
			hCom = CreateFile((LPCWSTR)str.utf16(),
							  GENERIC_READ | GENERIC_WRITE,
							  0,                    // comm devices must be opened w/exclusive-access
							  NULL,                 // no security attrs
							  OPEN_EXISTING,// comm devices must use OPEN_EXISTING
							  0,                    // not overlapped I/O
							  NULL                  // hTemplate must be NULL for comm devices
							 );

			if (hCom != INVALID_HANDLE_VALUE)
			{
				ret_val = OpenPort(&par_ports[no]);

				if (ret_val == OK)
				{
					write_port = GetFirstPort() + dataOfst;
					read_port = GetFirstPort() + statOfst;
					cpwreg = 0;
				}
				else
				{
					CloseParallel();
				}
			}

#else           //Linux
			ret_val = OpenPort(&par_ports[no]);

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
#ifdef  Q_OS_WIN32

	if (hCom != INVALID_HANDLE_VALUE)
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
		if (ser_ports[no - 1].base == 0)
		{
			DetectPorts();
		}

		return ser_ports[no - 1].base;
	}
	else
	{
		return 0;
	}
}

int PortInterface::GetParBasePort(int no)
{
	if (no >= 1 && no <= MAX_LPTPORTS)
	{
		if (par_ports[no - 1].base == 0)
		{
			DetectPorts();
		}

		return par_ports[no - 1].base;
	}
	else
	{
		return 0;
	}
}

#ifdef Q_OS_WIN32

#include <windows.h>

// #define TRUE    1
// #define FALSE   0

static int winSystemVersion()
{
#if 0

	if ((long)GetVersion() >= 0)
	{
		return 2;
	}

#else
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		if (!GetVersionEx((OSVERSIONINFO *)&osvi))
		{
			return 0;
		}
	}

	qDebug() << "PlatformId: " << osvi.dwPlatformId << ", Major: " << osvi.dwMajorVersion << ", Minor: " << osvi.dwMinorVersion << ", BuildNum: " << osvi.dwBuildNumber;

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		return 2;               //WINNT
		break;

	case VER_PLATFORM_WIN32_WINDOWS:
		return 1;               //WIN9X
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
	E2Profile::GetCOMAddress(ser_ports[0].base, ser_ports[1].base, ser_ports[2].base, ser_ports[3].base);
	E2Profile::GetLPTAddress(par_ports[0].base, par_ports[1].base, par_ports[2].base);

#ifdef  Q_OS_WIN32

	if (E2Profile::GetAutoDetectPorts())
	{
		LPTCount = 0;           // No printer ports counted
		COMCount = 0;           // No serial ports counted

		// Detect the LPT and COM ports available

		// Are we running Windows NT?
		// Detect the printer ports available
		if (winSystemVersion() == 2)
		{
			// Win2K / NT version
			DetectLPTPortsNT();
			DetectCOMPortsNT();
		}
		else
		{
			// Win9x version
			qCritical() << "Can't auto-detect ports on this Window version";
		}
	}
	else
	{
		LPTCount = 3;
		COMCount = 4;
	}

	hCom = INVALID_HANDLE_VALUE;
#endif

	E2Profile::SetCOMAddress(ser_ports[0].base, ser_ports[1].base, ser_ports[2].base, ser_ports[3].base);
	E2Profile::SetLPTAddress(par_ports[0].base, par_ports[1].base, par_ports[2].base);
}


#ifdef  Q_OS_WIN32

static int DetectPortsNT(const QString &ServiceName, const QString &PortFormat, base_len *ports, int nports);

//---------------------------------------------------------------------------
// DetectPorts() Win2K / NT version
//---------------------------------------------------------------------------
void PortInterface::DetectLPTPortsNT()
{
	LPTCount = DetectPortsNT("Parport", "LPT%d", par_ports, MAX_LPTPORTS);
}

void PortInterface::DetectCOMPortsNT()
{
	COMCount = DetectPortsNT("Serial", "COM%d", ser_ports, MAX_COMPORTS);
}

// Static member function
// See the two possible usage examples above!
static int DetectPortsNT(const QString &ServiceName, const QString &PortFormat, base_len *ports, int nports)
{
	LONG retval;
	// This revised code keeps some registry keys open, rather than collecting intermediate strings.
	// No silly key enumeration at all!
	const REGSAM KEY_PERMISSIONS = KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE;
	//char buf[MAX_PATH];     // handy stack-allocated multi-purpose buffer
	QString buf;
	int Count = 0;          // return value (can be greater than nports)

	QFile fh("detect_ports_NT.log");

	if (!fh.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
	{
		return -1;
	}

	QTextStream out(&fh);

	out << "Enter DetectPortsNT(" << ServiceName << ", " << PortFormat << ", " << (hex) << ports << (dec) << ", " << nports << ")\n";

	memset(ports, 0, nports * sizeof(base_len));    // Clear port array
	HKEY hCCS;              // Open the registry (first stage)
	retval = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet", 0, KEY_PERMISSIONS, &hCCS);

	if (retval == ERROR_SUCCESS)
	{
		HKEY hSvcEnum;
		buf = "Services\\" + ServiceName + "\\Enum";
		//_snprintf(buf, sizeof(buf), L"Services\\%s\\Enum", ServiceName);
		retval = RegOpenKeyEx(hCCS, (LPCWSTR)buf.utf16(), 0, KEY_PERMISSIONS, &hSvcEnum);

		if (retval == ERROR_SUCCESS)
		{
			DWORD Length = sizeof(Count);           // Count gets the number of ports in the system
			retval = RegQueryValueEx(hSvcEnum, L"Count", NULL, NULL, (PBYTE)&Count, &Length);

			if (retval == ERROR_SUCCESS)
			{
				out << "Port count: " << Count << ")\n";

				for (int i = 0; i < Count; i++)
				{
					buf = QString::number(i);
					//_snprintf(buf, sizeof(buf), "%d", i);   // A simple number
					uint16_t PnpPath[MAX_PATH];         // Another buffer (small changes in code would avoid even this)
					DWORD lptyp;
					Length = sizeof(PnpPath);
					retval = RegQueryValueEx(hSvcEnum, (LPCWSTR)buf.utf16(), NULL, &lptyp, (LPBYTE)PnpPath, &Length);

					if (retval == ERROR_SUCCESS)
					{
						HKEY hParams;

						QString strPnpPath = QString::fromUtf16(PnpPath);

						Q_ASSERT(lptyp == REG_SZ);
						out << "Cycle: " << i << ", PNPPath: " << strPnpPath << " ";

						//_snprintf(buf, sizeof(buf), "Enum\\%s\\Device Parameters", PnpPath);
						buf = "Enum\\" + strPnpPath + "\\Device Parameters";
						retval = RegOpenKeyEx(hCCS, (LPCWSTR)buf.utf16(), 0, KEY_PERMISSIONS, &hParams);

						if (retval == ERROR_SUCCESS)
						{
							int Index;
							uint16_t buf2[MAX_PATH];
							Length = sizeof(buf2);
							retval = RegQueryValueEx(hParams, L"PortName", NULL, &lptyp, (LPBYTE)buf2, &Length);

							if (retval == ERROR_SUCCESS)
							{
								Q_ASSERT(lptyp == REG_SZ);
								QString strbuf2 = QString::fromUtf16(buf2);

								if (sscanf(strbuf2.toLatin1().constData(), PortFormat.toLatin1().constData(), &Index) == 1
										&& (unsigned)--Index < (unsigned)nports)        // Use zero-based index from here, avoid negative values
								{
									HKEY hControl;
									//_snprintf(buf, sizeof(buf), "Enum\\%s\\Control", PnpPath);
									buf = "Enum\\" + strPnpPath + "\\Control";
									retval = RegOpenKeyEx(hCCS, (LPCWSTR)buf.utf16(), 0, KEY_PERMISSIONS, &hControl);

									if (retval == ERROR_SUCCESS)
									{
										Length = sizeof(buf2);
										retval = RegQueryValueEx(hControl, L"AllocConfig", NULL, &lptyp, (LPBYTE)buf2, &Length);

										if (retval == ERROR_SUCCESS)
										{
											// This undocumented AllocConfig structure is checked against Win2k and Win8/64.
											// In both cases, the ResType entry was at byte offset 16.
											DWORD *p = (DWORD *)buf2 + 4;

											if ((p[0] == 2 || p[0] == 3 || p[0] == 4 || p[0] == 5)   // have ResType_IO
													&& !HIWORD(p[2]) && p[2] > 0x100        // port address less than 64K and more than 0x100
													&& !p[3]                                                        // no high DWORD part
													&& p[4] >= 8 && p[4] < 16)              // length limited to 16
											{
												out << strbuf2 << (dec) << " [0]=" << p[0] << ", [1]=" << p[1] << ", [2]=" << (hex) << p[2] << "h, [3]=" << p[3] << "h, [4]=" << (dec) << p[4] << "\n";

												ports[Index].base = p[2];       // We got one
												ports[Index].len = p[4];        // (NO check for typical ISA addresses anymore!!)
											}
										}
										else
										{
											out << "RegQueryValueEx(AllocConfig) Failed with code " << retval << "\n";
										}

										RegCloseKey(hControl);
									}
									else
									{
										out << "RegOpenKeyEx(" << buf << ") Failed with code " << retval << "\n";
									}
								}

								RegCloseKey(hParams);
							}
						}
						else
						{
							out << "RegOpenKeyEx(" << buf << ") Failed with code " << retval << "\n";
						}
					}
				} //for
			}
			else
			{
				out << "RegQueryValueEx(Count) Failed with code " << retval << "\n";
			}

			RegCloseKey(hSvcEnum);
		}
		else
		{
			out << "RegOpenKeyEx(" << buf << ") Failed with code " << retval << "\n";
		}

		RegCloseKey(hCCS);
	}
	else
	{
		out << "RegOpenKeyEx(HKEY_LOCAL_MACHINE, SYSTEM\\CurrentControlSet) Failed with code " << retval << "\n";
	}

	out << "Enter DetectPortsNT() *** Count = " << Count << "\n";
	fh.close();

	return (Count < nports) ? Count : nports;
}

#endif
