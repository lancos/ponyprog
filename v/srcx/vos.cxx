//===============================================================
// vos.cpp - Interface to OS dependent stuff (For Unix)
//
// Copyright (C) 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vos.h>
#include <pwd.h>		// for getusername
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <stdlib.h>
#include <stdio.h>

#ifdef sun
extern "C" int gethostname(char* name, int namlen);
#endif
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

//=========================>>> vOS::vGetUserName <<<==========================
  int vOS::vGetEnvVal(char* name, char* val, int maxlen)
  {
     char *ep = getenv(name);
     if (ep != 0 && strlen(ep) < maxlen)
       {
	 strcpy(val, ep);
	 return strlen(val);
       }
     return 0;
   }

//=========================>>> vOS::vDeleteFile <<<==========================
  int vOS::vDeleteFile(const char* filename)
  {
     return remove(filename) == 0;
  }

//=========================>>> vOS::vRenameFile <<<==========================
  int vOS::vRenameFile(const char* oldFilename, const char* newFilename)
  {
    int pid;
    static char *mvarg[4];
    static int status;

    if (rename((char*)oldFilename,(char*)newFilename) != 0)
      {
	mvarg[0] = "/bin/mv";
	mvarg[1] = (char*)oldFilename;
	mvarg[2] = (char*)newFilename;
	mvarg[3]=0;
	pid = fork();
	if (pid == 0)
	  {
	    extern char **environ;
	    execve("/bin/mv",mvarg,environ);
	    return 0;
	  }
//	wait(&status);
//	if (status > 255)		/* error return */
//	  {
//	    return 0;
//	  }
      }
  }

//=========================>>> vOS::vChDrive <<<==========================
  int vOS::vChDrive(int drive)
  {
    return 1;	// no op on unix
  }

//=========================>>> vOS::vGetUserName <<<==========================
  int vOS::vGetUserName( char* s, int len)
  {
    // Set s to user name
    struct passwd   *pw;
    uid_t	    uid;

    uid = getuid();
    if ((pw = getpwuid(uid)) != 0 &&
				 pw->pw_name != 0 && *(pw->pw_name) != 0)
    {
	strncpy(s, pw->pw_name, len);
	return 1;
    }

    strcpy(s,"Unknown");
    return 0;			    /* a number is not a name */

  }


//====================>>> vOS::vGetHostName <<<==========================
  void vOS::vGetHostName(char* s, int len)
  {
    // Get host name
    gethostname((char *)s, len);		// tested on linux only
  }

//=========================>>> vOS::vGetPid <<<==========================
  long vOS::vGetPid()
  {
    // return process ID

    return (long) getpid();
  }


//=========================>>> vOS::vGetCWD <<<==========================
  int vOS::vGetCWD(char* buf, int len)
  {
    // Get name of current directory

    if (getcwd((char *)buf, len) == NULL)
      {
	strcpy(buf, ".");
	return 0;
      }
    return 1;
  }

//=========================>>> vOS::vChDir <<<==========================
  int vOS::vChDir(const char *path)
  {
    if (path[0] == 0)		/* just checking... */
	return 0;

    if (*path == 0)		/* drive name only */
	return 0;

    return chdir(path) == 0;
  }

//=========================>>> vOS::vSleep <<<==========================
  void vOS::vSleep(long MSec)
  {
    usleep(MSec * 1000);
  }

//=========================>>> vOS::vRunProcess <<<==========================
  int vOS::vRunProcess(const char* cmd, const char* StdOut, const char* StdErr,
      const int Wait, const int minimize)
  {
    int ret = 1;

// We will support two modes - a wait mode that lets us redirect
// stdout and stderr, and a don't wait mode that doesn't allow
// redirection. This might get fixed somday if someone wants
// to fix it....

// Execute command via the shell, but redirects stdin/stdout/stderr first.
// stdout/stderr are appended, not overwritten.

    if (Wait)
      {
	if (StdOut && *StdOut)	// change stdout
	  {
	    if (!freopen(StdOut, "w", stdout)) 
		return 0;
	  }
	if (StdErr && *StdErr)	// change stdout
	  {
	    if (!freopen(StdErr, "w", stderr)) 
		return 0;
	  }
	ret = system(cmd);
      }
    else		// no wait
      {
	if (StdOut && *StdOut)	// change stdout
	  {
	    if (!freopen(StdOut, "w", stdout)) 
		return 0;
	  }
	if (StdErr && *StdErr)	// change stdout
	  {
	    if (!freopen(StdErr, "w", stderr)) 
		return 0;
	  }
	char args[256];
	strcpy(args,cmd);		// make a copy to work with
	strcat(args," &");
	ret = system(args);

#ifdef OLDWAY
#define EXEC_FAILED 122	    /* Exit code when shell didn't execute.  Don't use
			       127, some shell use that already */

	char* argv[10];	// up to 10 args
	int argcnt;
	char args[256];
	int	    inquote;

	pid_t    pid;

	int	    status = -1;


	if (strlen(cmd) >= 255)	// don't allow too much!
	    return EXEC_FAILED;

	strcpy(args,cmd);		// make a copy to work with

	char* p = args;

	for (argcnt = 0; *p && argcnt < 10 ; ++argcnt)
	  {
	    inquote = 0;
	    argv[argcnt] = p;		// current arg
	    while (*p && (inquote || (*p != ' ' && *p != '\t')))
	      {
		if (*p == '"')
		    inquote = !inquote;
		++p;
	      }
	    if (*p == 0)
		continue;
	    *p++ = 0;		// terminate the arg
	    while (*p && (*p == ' ' || *p == '\t'))
		++p;		// skip white space
	  }
	argv[argcnt] = 0;

	if ((pid = fork()) == -1)	/* maybe we should use vfork() */
	  {
	    return EXEC_FAILED;
	  }
	else if (pid == 0)		// CHILD - do the execvp
	  {
	    execvp(argv[0], argv);	// should not return!
	    exit(EXEC_FAILED);	// exec failed, return failure code
	  }
	else			// PARENT FORK
	  {
	    //  Wait until child has exited.
#ifdef WAIT_EXECVP
	    wait(&status);
#endif
	    ret = 0;
	  }
#endif
      }
    return ret;
  }
