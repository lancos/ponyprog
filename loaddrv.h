#ifndef	_LOADDRV_H
#define	_LOADDRV_H

#define OKAY  0

// globals
//extern HINSTANCE hInst;
//extern SC_HANDLE hSCMan;

extern void InitDrvLoader();
extern bool ConnectSCM();
extern void DisconnectSCM();
extern bool DriverInstall();
extern bool DriverRemove();
extern bool DriverStart();
extern bool DriverStop();
extern void OpenDriver();
void CloseDriver();

/**
// private prototypes
extern DWORD DriverInstall(LPSTR, LPSTR);
extern DWORD DriverStart(LPSTR, LPSTR);
extern DWORD DriverStop(LPSTR, LPSTR);
extern DWORD DriverRemove(LPSTR, LPSTR);
**/

#endif