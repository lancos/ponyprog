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
#include <v/v_defs.h>

    class V_EXPORT vOS
      {
      public:
      	vOS();
        ~vOS();


	int vChDrive(int drive);
	int vGetEnvVal(char* name, char* val, int maxlen=255);
	int vGetUserName( char* s, int len);
	void vGetHostName(char* s, int len);
	long vGetPid();
	int vGetCWD(char* buf, int len);
	int vChDir(const char* path);

	int vDeleteFile(const char* filename);
	int vRenameFile(const char* oldFilename, const char* newFilename);
	void vSleep(long MSec);
	int vRunProcess(const char* cmd, const char* StdOut = 0,
	const char* StdErr = 0, const int Wait = 0, const int minimize = 0);
     };
#endif
