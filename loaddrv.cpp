// loaddrv.c - Dynamic driver install/start/stop/remove

#ifdef	WIN32

#include <windows.h>

#include "loaddrv.h"

// globals
//HINSTANCE hInst = NULL;
SC_HANDLE hSCMan = NULL;

static void GetServiceName(LPSTR, LPSTR);

/**-----------------------------------------------------**/
DWORD DriverInstall(LPSTR lpPath, LPSTR lpDriver)
{
	BOOL dwStatus = OKAY;
	SC_HANDLE hService = NULL;

	GetServiceName(lpPath, lpDriver);

	// add to service control manager's database
	if ((hService = CreateService(hSCMan, lpDriver, 
			lpDriver, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, lpPath, 
			NULL, NULL, NULL, NULL, NULL)) == NULL)
		dwStatus = GetLastError();
	else
		CloseServiceHandle(hService);

	return dwStatus;
} // DriverInstall

/**-----------------------------------------------------**/
DWORD DriverStart(LPSTR lpPath, LPSTR lpDriver)
{
	BOOL dwStatus = OKAY;
	SC_HANDLE hService = NULL;

	GetServiceName(lpPath, lpDriver);

	// get a handle to the service
	if ((hService = OpenService(hSCMan, lpDriver, SERVICE_ALL_ACCESS)) != NULL) 
	{
		// start the driver
		if (!StartService(hService, 0, NULL))
		dwStatus = GetLastError();
	}
	else
		dwStatus = GetLastError();

	if (hService != NULL)
		CloseServiceHandle(hService);

	return dwStatus;
} // DriverStart

/**-----------------------------------------------------**/
DWORD DriverStop(LPSTR lpPath, LPSTR lpDriver)
{
	BOOL dwStatus = OKAY;
	SC_HANDLE hService = NULL;
	SERVICE_STATUS serviceStatus;

	GetServiceName(lpPath, lpDriver);

	// get a handle to the service
	if ((hService = OpenService(hSCMan, lpDriver, SERVICE_ALL_ACCESS)) != NULL)
	{
		// stop the driver
		if (!ControlService(hService, SERVICE_CONTROL_STOP,	&serviceStatus))
			dwStatus = GetLastError();
	}
	else
		dwStatus = GetLastError();

	if (hService != NULL)
		CloseServiceHandle(hService);

	return dwStatus;
} // DriverStop

/**-----------------------------------------------------**/
DWORD DriverRemove(LPSTR lpPath, LPSTR lpDriver)
{
	BOOL dwStatus = OKAY;
	SC_HANDLE hService = NULL;

	GetServiceName(lpPath, lpDriver);

	// get a handle to the service
	if ((hService = OpenService(hSCMan, lpDriver, SERVICE_ALL_ACCESS)) != NULL)
	{
		// remove the driver
		if (!DeleteService(hService))
			dwStatus = GetLastError();
	}
	else
		dwStatus = GetLastError();

	if (hService != NULL)
		CloseServiceHandle(hService);
	return dwStatus;
} // DriverRemove

/**-----------------------------------------------------**/
static void GetServiceName(LPSTR lpPath, LPSTR lpDriver)
{
//	GetDlgItemText(IDC_DRVNAME, lpPath, MAX_PATH);
	_splitpath(lpPath, NULL, NULL, lpDriver, NULL);
} // GetServiceName

#endif
