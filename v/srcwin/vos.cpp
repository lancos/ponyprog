//===============================================================
//
// vos.cpp - Interface to OS dependent stuff (For Windows)
//
// Copyright (C) 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================
#include <v/vwin32.h>
#include <v/vapp.h>
#include <v/vos.h>
#include <string.h>
//#include <windows.h>

const int MaxCompNameLen = 256;

//=========================>>> vOS::vOS <<<==========================
  vOS::vOS()
  {
    // Constructor
    // This will be used to determing which OS we are running
    // under: e.g., NT vs 95
  }

//=========================>>> vOS::~vOS <<<==========================
  vOS::~vOS()
  {
    // Destructor
  }

//=========================>>> vOS::vDeleteFile <<<==========================
  int vOS::vDeleteFile(const char* filename)
  {
     return ::DeleteFile(filename) == TRUE;
  }

//=========================>>> vOS::vRenameFile <<<==========================
  int vOS::vRenameFile(const char* oldFilename, const char* newFilename)
  {
     return ::MoveFile((LPCTSTR)oldFilename,(LPCTSTR)newFilename) == TRUE;
  }

//=========================>>> vOS::vChDrive <<<==========================
  int vOS::vChDrive(int drive)
  {
    char temp [3] = "-:";
    temp[0] = drive + 'A';
    return ::SetCurrentDirectory(temp);
  }

//=========================>>> vOS::vGetUserName <<<==========================
  int vOS::vGetEnvVal(char* name, char* val, int maxlen)
  {
     return ::GetEnvironmentVariable(name,val,maxlen);
  }

//=========================>>> vOS::vGetUserName <<<==========================
  int vOS::vGetUserName( char* s, int len)
  {
    // Set s to user name

    char UserName[MaxCompNameLen+1];
    DWORD cch = sizeof UserName;

    if (::GetUserName(UserName, &cch))
      {
	if (strlen(UserName) > (unsigned int)len)
	    UserName[len-1] = 0;
	strcpy(s, UserName);
	return 1;
      }
    s[0] = 0;
    return 0;
  }


//=========================>>> vOS::vGetHostName <<<==========================
  void vOS::vGetHostName(char* s, int len)
  {
    // Get host name

    char HostName[MaxCompNameLen+1];
    DWORD cch = MaxCompNameLen;

    if (::GetComputerName(HostName, &cch))
      {
	strcpy(s,HostName);
      }
    else
	strcpy(s, "PC_Win32");
  }

//=========================>>> vOS::vGetPid <<<==========================
  long vOS::vGetPid()
  {
    // return process ID

    return (long) ::GetCurrentProcessId();
  }


//=========================>>> vOS::vGetCWD <<<==========================
  int vOS::vGetCWD(char* buf, int len)
  {
    // Get name of current directory

    int rv = ::GetCurrentDirectory(len, buf);

    return rv;
  }

//=========================>>> vOS::vChDir <<<==========================
  int vOS::vChDir(const char *path)
  {
    if (path[0] == 0)		/* just checking... */
	return 0;

    if (*path == 0)		/* drive name only */
	return 0;

    return ::SetCurrentDirectory(path);	       /* let the normal chdir() do the rest */
  }

//=========================>>> vOS::vSleep <<<==========================
  void vOS::vSleep(long MSec)
  {
    ::Sleep(MSec);
  }

//=========================>>> vOS::vRunProcess <<<==========================
  int vOS::vRunProcess(const char* cmd, const char* StdOut, const char* StdErr,
      const int Wait, const int minimize)
  {
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

//    ::AllocConsole();

    si.cb = sizeof(si);
    si.lpReserved = 0;
    si.lpDesktop = 0;
    si.lpTitle = 0;
    si.dwFlags = 0;
    si.cbReserved2 = 0;
    si.lpReserved2 = 0;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES ;
    if (minimize)
        si.wShowWindow = SW_MINIMIZE;
    else
        si.wShowWindow = SW_SHOWNORMAL;

    HANDLE hNewStderr = INVALID_HANDLE_VALUE;
    HANDLE hNewStdout = INVALID_HANDLE_VALUE;


    if (StdErr && *StdErr)	// they specified a stderr out
      {
        hNewStderr = ::CreateFile(
          StdErr,			// address of name of the file
    	  GENERIC_WRITE	,		// access (read-write) mode
    	  (DWORD) 0,			// share mode
          (LPSECURITY_ATTRIBUTES) 0,	// address of security descriptor
    	  CREATE_ALWAYS	,		// how to create
    	  FILE_ATTRIBUTE_NORMAL,	// file attributes
          (HANDLE) 0 		// handle of file with attributes to copy
          );
      }

    if (StdOut && *StdOut)	// they specified a stderr out
      {
        hNewStdout = ::CreateFile(
          StdOut,			// address of name of the file
    	  GENERIC_WRITE	,		// access (read-write) mode
    	  (DWORD) 0,			// share mode
          (LPSECURITY_ATTRIBUTES) 0,	// address of security descriptor
    	  CREATE_ALWAYS	,		// how to create
    	  FILE_ATTRIBUTE_NORMAL,	// file attributes
          (HANDLE) 0 		// handle of file with attributes to copy
          );
      }

    HANDLE myself = ::GetCurrentProcess();	//my own process handle
    HANDLE dupStdout = INVALID_HANDLE_VALUE;
    HANDLE dupStderr = INVALID_HANDLE_VALUE;


    si.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);

    if (hNewStdout != INVALID_HANDLE_VALUE)	// specified out file
      {
        ::DuplicateHandle(myself, hNewStdout ,
           myself,&dupStdout,0, TRUE, DUPLICATE_SAME_ACCESS);
        si.hStdOutput = dupStdout;
      }
    else
      {
         si.hStdOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
      }

    if (hNewStderr != INVALID_HANDLE_VALUE)	// specified err file
      {
        ::DuplicateHandle(myself, hNewStderr ,
           myself,&dupStderr,0, TRUE, DUPLICATE_SAME_ACCESS);
        si.hStdError = dupStderr;
      }
    else
      {
         si.hStdError = ::GetStdHandle(STD_ERROR_HANDLE);
      }

//	    CREATE_NEW_CONSOLE,		/* Creation flags */
    // Now, run the command
    if (!::CreateProcess (0,	/* Executable name */
	(char *)cmd,		/* Command to execute */
        0,				/* Process security attributes */
	0,				/* Thread security attributes */
	TRUE,			/* Inherit handles */
	0,			// create flags
	0,				/* Environment */
	0,				/* Current directory */
	&si,			/* Startup information */
	&pi))			/* Process information */
	  {
	    if (dupStdout != INVALID_HANDLE_VALUE)
		::CloseHandle(dupStdout);
	    if (dupStderr != INVALID_HANDLE_VALUE)
		::CloseHandle(dupStderr);
	    return 99;
	  }

    if (dupStdout != INVALID_HANDLE_VALUE)
        ::CloseHandle(dupStdout);
    if (dupStderr != INVALID_HANDLE_VALUE)
        ::CloseHandle(dupStderr);

//    ::FreeConsole();
    if (!Wait)
        return 0;


    /* Wait for the command to terminate before continuing */
    ::WaitForSingleObject(pi.hProcess, INFINITE);

    /* Get the command exit code */
    ::GetExitCodeProcess(pi.hProcess, &ret);

    /* Close the handles to the subprocess, so that it goes away */
    ::CloseHandle(pi.hThread);
    ::CloseHandle(pi.hProcess);
    if (hNewStdout != INVALID_HANDLE_VALUE)
        ::CloseHandle(hNewStdout);
    if (hNewStderr != INVALID_HANDLE_VALUE)
        ::CloseHandle(hNewStderr);

    return ret;
  }
