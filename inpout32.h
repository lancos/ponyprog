#ifdef	WIN32
#pragma once

#ifndef INPOUT32_H_INC
#define INPOUT32_H_INC

#ifdef __cplusplus
extern "C" {
#endif

//#define DLPORT_API _stdcall
#define DLPORT_API

//Functions exported from DLL.
//For easy inclusion is user projects.
//Original InpOut32 function support
void	DLPORT_API Out32(short PortAddress, short data);
short	DLPORT_API Inp32(short PortAddress);

//My extra functions for making life easy
BOOL	DLPORT_API IsInpOutDriverOpen();  //Returns TRUE if the InpOut driver was opened successfully
BOOL	DLPORT_API IsXP64Bit();			//Returns TRUE if the OS is 64bit (x64) Windows.

//DLLPortIO function support
UCHAR   DLPORT_API DlPortReadPortUchar (USHORT port);
void    DLPORT_API DlPortWritePortUchar(USHORT port, UCHAR Value);

USHORT  DLPORT_API DlPortReadPortUshort (USHORT port);
void    DLPORT_API DlPortWritePortUshort(USHORT port, USHORT Value);

ULONG	DLPORT_API DlPortReadPortUlong(ULONG port);
void	DLPORT_API DlPortWritePortUlong(ULONG port, ULONG Value);

//WinIO function support (Untested and probably does NOT work - esp. on x64!)
PBYTE	DLPORT_API MapPhysToLin(PBYTE pbPhysAddr, DWORD dwPhysSize, HANDLE *pPhysicalMemoryHandle);
BOOL	DLPORT_API UnmapPhysicalMemory(HANDLE PhysicalMemoryHandle, PBYTE pbLinAddr);
BOOL	DLPORT_API GetPhysLong(PBYTE pbPhysAddr, PDWORD pdwPhysVal);
BOOL	DLPORT_API SetPhysLong(PBYTE pbPhysAddr, DWORD dwPhysVal);

#ifdef __cplusplus
}
#endif

#endif // INPOUT32_H_INC

#endif
