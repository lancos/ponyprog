#ifndef	_LOADDRV_H
#define	_LOADDRV_H

#define OKAY  0

extern void InitDrvLoader();

extern bool ConnectSCM();
extern bool DisconnectSCM();

extern bool DriverInstall();
extern bool DriverRemove();

extern bool DriverStart();
extern bool DriverStop();

extern void OpenDriver();
extern void CloseDriver();

extern char *GetLastErrorMsg();

#endif
