// loaddrv.c - Dynamic driver install/start/stop/remove

#ifdef	WIN32

#include <windows.h>
#include <stdio.h>

#include "globals.h"
#include "loaddrv.h"

//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------

// DriverLINX DLL filename
//static const char LIBRARY_FILENAME[] = "DLPortIO.dll";
#define	LIBRARY_FILENAME	"DLPortIO.dll"

// WinNT DriverLINX Information
//static const char DRIVER_NAME[] = "DLPortIO";
//static const char DISPLAY_NAME[] = "DriverLINX Port I/O Driver";
//static const char DRIVER_GROUP[] = "SST miniport drivers";
#define	DRIVER_NAME		"DLPortIO"
#define	DRIVER_EXT		".SYS"
#define	DRIVER_DIR		"\\DRIVERS"
#define	DISPLAY_NAME	"DriverLINX Port I/O Driver"
#define	DRIVER_GROUP	"SST miniport drivers"

// globals
//HINSTANCE hInst = NULL;
static SC_HANDLE hSCMan = NULL;

// Used for the Windows NT version only
static bool FActiveHW;		// Is the DLL loaded?
static bool FHardAccess;	// Not used: for compatibility only
static bool FRunningWinNT;	// True when we're running Windows NT
static bool FDrvPrevInst;   // DriverLINX driver already installed?
static bool FDrvPrevStart;  // DriverLINX driver already running?

static char FDriverPath[MAXPATH]; // Full path of WinNT driver
static char FDLLPath[MAXPATH];    // Full path of DriverLINX DLL
static char FLastError[MAXMSG];  // Last error which occurred in Open/CloseDriver()

void InitDrvLoader()
{
	// Are we running Windows NT?
	OSVERSIONINFO os;
	memset(&os, NULL, sizeof(OSVERSIONINFO));
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	FRunningWinNT=(os.dwPlatformId==VER_PLATFORM_WIN32_NT);

	// Set default WinNT driver path
	char Buffer[MAX_PATH];
	GetSystemDirectory(Buffer, MAX_PATH);
	strncpy(FDriverPath, Buffer, MAXPATH);
	if ( strlen(Buffer) + strlen(DRIVER_DIR) < MAXPATH )
		strcat(FDriverPath, DRIVER_DIR);

	// Set the default DLL path
	strcpy(FDLLPath,"");

	FActiveHW=false;  // DLL/Driver not loaded
	FHardAccess=true; // Not used, default true

	strcpy(FLastError,"");    // No errors yet
}

char *GetLastErrorMsg()
{
	char *sp;

	sp = FLastError;
	strcpy(FLastError,"");

	return sp;
}

//---------------------------------------------------------------------------
// ConnectSCM()
//    Connects to the WinNT Service Control Manager
//---------------------------------------------------------------------------
bool ConnectSCM()
{
   DWORD dwStatus = 0; // Assume success, until we prove otherwise
   DWORD scAccess;     // Access mode when connecting to SCM

   // Try and connect as administrator
   scAccess = SC_MANAGER_CONNECT |
              SC_MANAGER_QUERY_LOCK_STATUS |
              SC_MANAGER_ENUMERATE_SERVICE |
              SC_MANAGER_CREATE_SERVICE;      // Admin only

   // Connect to the SCM
   hSCMan = OpenSCManager(NULL, NULL, scAccess);

   // If we're not in administrator mode, try and reconnect
   if (hSCMan==NULL && GetLastError()==ERROR_ACCESS_DENIED)
   {
      scAccess = SC_MANAGER_CONNECT |
                 SC_MANAGER_QUERY_LOCK_STATUS |
                 SC_MANAGER_ENUMERATE_SERVICE;

      // Connect to the SCM
      hSCMan = OpenSCManager(NULL, NULL, scAccess);
   }

   // Did it succeed?
   if (hSCMan==NULL)
   {
      // Failed, save error information
      dwStatus=GetLastError();
	  sprintf(FLastError,"ConnectSCM: Error #%lu", dwStatus);
   }

   return dwStatus==0; // Success == 0
}


//---------------------------------------------------------------------------
// DisconnectSCM()
//    Disconnects from the WinNT Service Control Manager
//---------------------------------------------------------------------------
bool DisconnectSCM()
{
   if (hSCMan != NULL)
   {
      // Disconnect from our local Service Control Manager
      CloseServiceHandle(hSCMan);
      hSCMan=NULL;
   }

   return true;
}

//---------------------------------------------------------------------------
// DriverInstall()
//    Installs the DriverLINX driver into Windows NT's registry
//---------------------------------------------------------------------------
bool DriverInstall()
{
	SC_HANDLE hService; // Handle to the new service
	DWORD dwStatus = 0; // Assume success, until we prove otherwise

	FDrvPrevInst=false; // Assume the driver wasn't installed previously

	// Path including filename
	static char DriverPath[MAXPATH];

	if ( strlen(FDriverPath) + strlen("\\" DRIVER_NAME DRIVER_EXT) < MAXPATH )
	{
		strncpy(DriverPath, FDriverPath, MAXPATH);
		strcat(DriverPath, "\\" DRIVER_NAME DRIVER_EXT);
	}
	else
		strncpy(DriverPath, DRIVER_NAME ".SYS", MAXPATH);
	DriverPath[MAXPATH-1] = '\0';

	// Is the DriverLINX driver already in the SCM? If so,
	// indicate success and set FDrvPrevInst to true.
	hService=OpenService(hSCMan, DRIVER_NAME, SERVICE_QUERY_STATUS);
	if (hService!=NULL)
	{
		FDrvPrevInst=true;            // Driver previously installed, don't remove
		CloseServiceHandle(hService); // Close the service
		return true;                  // Success
	}

	// Add to our Service Control Manager's database
	hService=CreateService(
				hSCMan,
				DRIVER_NAME,
				DISPLAY_NAME,
				SERVICE_START | SERVICE_STOP | DELETE | SERVICE_QUERY_STATUS,
				SERVICE_KERNEL_DRIVER,
				SERVICE_DEMAND_START,
				SERVICE_ERROR_NORMAL,
				DriverPath,
				DRIVER_GROUP,
				NULL, NULL, NULL, NULL);

	if (hService==NULL)
		dwStatus=GetLastError();
	else
	// Close the service for now...
		CloseServiceHandle(hService);

	if (dwStatus!=0)
		sprintf(FLastError, "DriverInstall: Error #%lu", dwStatus);

   return dwStatus==0; // Success == 0
}

//---------------------------------------------------------------------------
// DriverStart()
//    Starts the Windows NT DriverLINX driver for use
//---------------------------------------------------------------------------
bool DriverStart()
{
   SC_HANDLE hService; // Handle to the service to start
   DWORD dwStatus = 0; // Assume success, until we prove otherwise

   SERVICE_STATUS sStatus;

   FDrvPrevStart=false; // Assume the driver was not already running

   hService = OpenService(hSCMan, DRIVER_NAME, SERVICE_QUERY_STATUS);
   if (hService!=NULL && QueryServiceStatus(hService, &sStatus))
   {
      // Got the service status, now check it
      if (sStatus.dwCurrentState == SERVICE_RUNNING)
      {
         FDrvPrevStart=true;           // Driver was previously started
         CloseServiceHandle(hService); // Close service
         return true;                  // Success
      }
      else if (sStatus.dwCurrentState==SERVICE_STOPPED)
      {
         // Driver was stopped. Start the driver.
         CloseServiceHandle(hService);
         hService = OpenService(hSCMan, DRIVER_NAME, SERVICE_START);
         if (!StartService(hService, 0, NULL))
            dwStatus=GetLastError();
         CloseServiceHandle(hService); // Close service
      }
      else dwStatus=-1; // Can't run the service
   }
   else
      dwStatus=GetLastError();

   if (dwStatus!=0)
      sprintf(FLastError,"DriverStart: Error #%lu", dwStatus);

   return dwStatus==0; // Success == 0
}

//---------------------------------------------------------------------------
// DriverStop()
//    Stops a Windows NT driver from use
//---------------------------------------------------------------------------
bool DriverStop()
{
   SC_HANDLE hService; // Handle to the service to start
   DWORD dwStatus = 0; // Assume success, until we prove otherwise

   // If we didn't start the driver, then don't stop it.
   // Pretend we stopped it, by indicating success.
	if (FDrvPrevStart)
		return true;

   // Get a handle to the service to stop
   hService = OpenService(
                 hSCMan,
                 DRIVER_NAME,
                 SERVICE_STOP | SERVICE_QUERY_STATUS);

   if (hService!=NULL)
   {
      // Stop the driver, then close the service
      SERVICE_STATUS sStatus;

      if (!ControlService(hService, SERVICE_CONTROL_STOP, &sStatus))
         dwStatus = GetLastError();

      // Close the service
      CloseServiceHandle(hService);
   }
   else
      dwStatus = GetLastError();

   if (dwStatus!=0)
      sprintf(FLastError,"DriverStop: Error #%lu", dwStatus);

   return dwStatus==0; // Success == 0
}

//---------------------------------------------------------------------------
// DriverRemove()
//    Removes a driver from the Windows NT system (service manager)
//---------------------------------------------------------------------------
bool DriverRemove()
{
   SC_HANDLE hService; // Handle to the service to start
   DWORD dwStatus = 0; // Assume success, until we prove otherwise

   // If we didn't install the driver, then don't remove it.
   // Pretend we removed it, by indicating success.
   if (FDrvPrevInst)
	   return true;

   // Get a handle to the service to remove
   hService = OpenService(
                 hSCMan,
                 DRIVER_NAME,
                 DELETE);

   if (hService!=NULL)
   {
      // Remove the driver then close the service again
      if (!DeleteService(hService))
         dwStatus = GetLastError();

      // Close the service
      CloseServiceHandle(hService);
   }
   else
      dwStatus = GetLastError();

   if (dwStatus!=0)
      sprintf(FLastError, "DriverRemove: Error #%lu", dwStatus);

   return dwStatus==0; // Success == 0
}

static bool IsLoaded()
{
	return FActiveHW;
}

//---------------------------------------------------------------------------
// OpenDriver()
//    Opens the DLL dynamically
//---------------------------------------------------------------------------
void OpenDriver()
{
   // If the DLL/driver is already open, then forget it!
   if (IsLoaded())
	   return;

   // If we're running Windows NT, install the driver then start it
   if (FRunningWinNT)
   {
      // Connect to the Service Control Manager
      if (!ConnectSCM())
		  return;

      // Install the driver
      if (!DriverInstall())
      {
         // Driver install failed, so disconnect from the SCM
         DisconnectSCM();
         return;
      }

      // Start the driver
      if (!DriverStart())
      {
         // Driver start failed, so remove it then disconnect from SCM
         DriverRemove();
         DisconnectSCM();
         return;
      }
   }

	FActiveHW=true; // Success
}

//---------------------------------------------------------------------------
// CloseDriver()
//    Closes the dynamically opened DLL
//---------------------------------------------------------------------------
void CloseDriver()
{
   // Don't close anything if it wasn't opened previously
   if (!IsLoaded())
	   return;

   // If we're running Windows NT, stop the driver then remove it
   if (FRunningWinNT)
   {
      if (!DriverStop())
		  return;
      if (!DriverRemove())
		  return;
      DisconnectSCM();
   }

   FActiveHW=false; // Success
}

#endif
