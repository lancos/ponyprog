//===============================================================
// vos. - the OS interface object
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VOS_H
#define VOS_H

    class vOS
      {
      public:
      	vOS();
        ~vOS();


	char* vGetFullPath(char *buf, char *fname, int len);
	int vGetEnvVal(char* name, char *val, int maxlen=255);
	int vChDrive(int drive);
	int vGetUserName( char* s, int len);
	void vGetHostName(char* s, int len);
	long vGetPid();
	int vGetCWD(char* buf, int len);
	int vChDir(const char *path);
	void vSleep(long MSec);

	int vDeleteFile(const char* filename);
	int vRenameFile(const char* filename, const char* newFilename);
	int vRunProcess(const char* cmd, const char* StdOut = 0,
	  const char* StdErr = 0, const int Wait = 0, const int minimize = 0);
     };
#endif
