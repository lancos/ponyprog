#ifndef	_LOADDRV_H
#define	_LOADDRV_H

#define OKAY  0

// globals
extern HINSTANCE hInst;
extern SC_HANDLE hSCMan;

// private prototypes
extern DWORD DriverInstall(LPSTR, LPSTR);
extern DWORD DriverStart(LPSTR, LPSTR);
extern DWORD DriverStop(LPSTR, LPSTR);
extern DWORD DriverRemove(LPSTR, LPSTR);

#endif