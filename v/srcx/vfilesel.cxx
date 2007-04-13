//===============================================================
// vfilesel.cxx - vFileSelect class functions - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vfilesel.h>		// our header
#include <v/vnotice.h>		// we use notice
#include <v/vynreply.h>		// we use notice
#include <v/vreply.h>		// we use notice
#include <v/vicon.h>		// for icons

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

// Define static data of the class

    enum {
	fsMSG = 100,
	fsFrame,
	fsLFN,
	fsFN,
	fsLCD,
	fsCD,
	fsLFLT,
	fsFLT,
	fsLIST,
	fsMkdir,
	fsRm,
	fsSEL,
	fsHOME,
	fsSHOWDOT,
	fsBLNK,
	fsBLNK2,
	fsLBLDIR,
	fsLISTDIR,
	fsLBLITEM
      };

    static char* filterList[] =		// The default filter list
      {
	"*", 0
      };

    static char* emptyList[] =
      {
	"[no files found yet]",
	0
      };

#define home_width 24
#define home_height 16
static unsigned char home_bits[] = {
   0x00, 0x80, 0x1f, 0x00, 0x40, 0x20, 0x00, 0x20, 0x40, 0xff, 0xff, 0xff,
   0x01, 0x00, 0x80, 0x01, 0x00, 0x80, 0x01, 0x00, 0xb0, 0xf1, 0x30, 0x98,
   0xf9, 0x19, 0x8c, 0x99, 0x1f, 0x86, 0x0d, 0x0f, 0x83, 0x01, 0x80, 0x81,
   0x01, 0xc0, 0x80, 0x01, 0x00, 0x80, 0x01, 0x00, 0x80, 0xff, 0xff, 0xff};

    static vIcon home(&home_bits[0], home_height, home_width); // use icon for home

#define FilterIndex 4
#define FileListIndex 10
#define DirListIndex 8
#define LS "ls -1aLF"

    static CommandObject FileSelectDialog[] =
      {
	{C_Frame, fsFrame, 0, "", NoList, CA_None, isSens,NoFrame,0,0},
	{C_Label, fsLFLT, 0, "Filter:", 
		NoList,CA_None,isSens,fsFrame, 0,0},

	{C_Blank, fsBLNK, 0, "   ",NoList,CA_None,isSens,fsFrame,fsLFLT,0},

        {C_Label, fsMSG, 0, "Select File",NoList,
                CA_MainMsg,isSens,fsFrame, fsBLNK,0},

	{C_ComboBox,fsFLT,0,"ComboBox",(void*)filterList,	// index 1
		CA_None,isSens,fsFrame,0,fsLFLT},

	{C_Label, fsLCD, 0, "Directory:", 
		NoList,CA_None,isSens,fsFrame, 0,fsFLT},
        {C_Label, fsCD, 0, ".", NoList,
                CA_None,isSens,fsFrame,fsLCD,fsFLT},

	{C_Label, fsLBLDIR, 0, "Directories",
	    NoList,CA_None,isSens,fsFrame,0,fsLCD},

	{C_List, fsLISTDIR, 150, "List", (void*)emptyList,	// index 6
	    	CA_ListWidth | CA_Size,isSens,fsFrame,0,fsLBLDIR,16},
	
	{C_Label, fsLBLITEM, 0, "Items",
	    NoList,CA_None,isSens,fsFrame,fsLISTDIR,fsLCD},

	{C_List, fsLIST, 150, "List", (void*)emptyList,		// index 8
		 CA_ListWidth | CA_Size,isSens,fsFrame,fsLISTDIR,fsLBLITEM,16},


	{C_Label, fsLFN, 0, "Selection", NoList,CA_None,isSens,fsFrame, 0,fsLIST},

	{C_Blank, fsBLNK2, 0, "       ",NoList,CA_None,isSens,fsFrame,fsLFN,fsLIST},

	{C_CheckBox,fsSHOWDOT,0,"Show \".hidden\"",NoList,CA_None,isSens,
		fsFrame,fsBLNK2,fsLIST},


        {C_TextIn, fsFN, 0, "", NoList,
                CA_Large,isSens,fsFrame,0,fsSHOWDOT},


        {C_Button, M_OK, M_OK, " OK ", NoList,CA_DefaultButton,
                isSens,fsFrame,0,fsFN},
        {C_Button, fsSEL, fsSEL, " Select ", NoList,CA_None,
                isSens,fsFrame,M_OK,fsFN,0,"Select highlighted file"},
        {C_Button, M_Cancel, M_Cancel, " Cancel ", NoList,CA_None,
                isSens,fsFrame,fsSEL,fsFN,0,"Cancel file open"},
	{C_IconButton, fsHOME, fsHOME, "Home",(void*)&home, CA_None,
                isSens,fsFrame,M_Cancel,fsFN,0,"Change to HOME directory"},
        {C_Button, fsMkdir, 0, " mkdir ", NoList,CA_None,
                isSens,fsFrame,fsHOME,fsFN,0,"New directory under current directory"},
        {C_Button, fsRm, 0, " rm ", NoList,CA_None,
                isSens,fsFrame,fsMkdir,fsFN,0,"Delete ('rm') selected file"},


        {C_EndOfList,0,0,0,0,CA_None,0,0,0}
      };


  static int dirsort(const char**, const char**);
  static int wild_match(const char* , const char*);

//======================>>> vFileSelect::FileSelect <<<=======================
  int vFileSelect::FileSelect(VCONST char* msg, char* filename, 
	const int maxlen, char** filter, int& filterIndex)
  {
    //	Show the file selection dialog.
    //	returns 0 on cancel, 1 otherwise

    int ans;
    char temp[maxFileLen];

    if (!filename)
	return 0;

    _showHidden = 0;
    origFile[0] = 0;
    fileList[0] = 0;
    dirList[0] = 0;

    // If the user supplies a filter, use it. Otherwise, use the default.

    if (filter != 0 && *filter != 0)
      {
	_curFilter = filterIndex;	// which filter we used
	filterPtr = filter;		// so we can change
	fileFilter = filter[_curFilter];
	FileSelectDialog[FilterIndex].itemList = (void *)filter;
	FileSelectDialog[FilterIndex].retVal = _curFilter;
      }
    else
      {
	_curFilter = 0;
	filterPtr = filterList;		// so we can change
	fileFilter = filterList[0];
      }

    // Fill this in dynamically so we can have dynamic heap array

    FileSelectDialog[FileListIndex].itemList = (void *)fileList;
    FileSelectDialog[DirListIndex].itemList = (void *)dirList;

    GetDirList(".", fileFilter, fileList, maxFiles, dirList, maxDirs);

    dirSel = 		// No file selected
    curSel = -1;		// No file selected

    if (!_cmdsAdded)		// Only once
      {
	AddDialogCmds(FileSelectDialog);	// Set up standard dialog
	_cmdsAdded = 1;
      }

    // Read the current directory

    // Note: this SetString won't work on Windows...

    if (getcwd(temp,maxFileLen-1))	// get the current directory
	SetString(fsCD,temp);

    strcpy(curDir,temp);		// preload input with name

    if (*filename)		// provided a base name
	strcpy(origFile,filename);

    if (*filename != 0)
      {
	SetString(fsFN,filename);	// the filename provided
      }
    else
      {
	strcpy(temp,curDir);		// set to curdir
	if (strcmp(temp,"/") != 0)
	    strcat(temp,"/");
	SetString(fsFN,temp);
      }

    (void) ShowModalDialog(msg, ans);	// show and wait

    if (ans != M_Cancel)
      {
	(void) GetTextIn(fsFN, filename, maxlen);
	filterIndex = _curFilter;	// copy back out
      }

    return ans != M_Cancel;
  }

//======================>>> vFileSelect::FileSelect <<<=======================
  int vFileSelect::FileSelectSave(VCONST char* msg, char* filename, 
	const int maxlen, char** filter, int& filterIndex)
  {
    return FileSelect(msg, filename, maxlen, filter, filterIndex);
  }

//====================>>> vFileSelect::DialogCommand <<<=======================
  void vFileSelect::DialogCommand(ItemVal id, ItemVal val, CmdType ctype)
  {
    char temp[maxFileLen];

    switch (id)
      {
	case fsFLT:		// new filter
	  {
	    if (*filterPtr[val] == 0)
		break;
	    _curFilter = val;
	    fileFilter = filterPtr[_curFilter];
	    GetDirList(".", fileFilter, fileList, maxFiles, dirList, maxDirs);
	    SetValue(fsLIST,0,ChangeList);
	    SetValue(fsLISTDIR,0,ChangeList);
	    SetValue(fsLIST,-1,Value);	// turn off file highlight
	    SetValue(fsLISTDIR,-1,Value);	// turn off file highlight
	    dirSel =
	    curSel = -1;		// No file selected
	    break;
	  }

	case fsLISTDIR:		// item selected
	  {
	    if (val != dirSel)	// This trick implements double click
	      {
		SetValue(fsLIST,-1,Value);	// turn off file highlight
		dirSel = val;
		break;
	      }
	    // else - change dirs
	    if ( dirSel < 0)	// Make sure a valid selection
		break;
	    strcpy(temp,dirList[dirSel]);
	    int len = strlen(temp);
	    if (temp[len-1] != '/')	// not a directory!
		break;
	    temp[len-1] = 0;	// kill the /
	    GetDirList(temp, fileFilter, fileList, maxFiles, dirList, maxDirs);
	    if (getcwd(curDir,maxFileLen-1))	// get the current directory
		SetString(fsCD,curDir);
	    else
	      {
		SetString(fsCD,".");
		strcpy(curDir,".");
	      }
	    SetValue(fsLIST,0,ChangeList);
	    SetValue(fsLISTDIR,0,ChangeList);
	    SetValue(fsLIST,-1,Value);	// turn off file highlight
	    SetValue(fsLISTDIR,-1,Value);	// turn off file highlight
	    strcpy(temp,curDir);
	    if (strcmp(temp,"/") != 0)
		strcat(temp,"/");
	    if (*origFile)
		strcat(temp,origFile);
	    SetString(fsFN,temp);
	    dirSel =
	    curSel = -1;		// No file selected
	    break;
	  }

	case fsLIST:		// item selected
	  {
	    if (val != curSel)	// This trick implements double click
	      {
		SetValue(fsLISTDIR,-1,Value);	// turn off file highlight
		curSel = val;
		break;
	      }
	    // else fall through to fsSEL!
	  }

	// WARNING - fall through from previous case!

	case fsSEL:		// Do select
	  {
	    if (curSel < 0 && dirSel < 0)	// Make sure a valid selection
		break;
	    if (curSel < 0 && dirSel >= 0)	// just a directory to select
	      {
		if (!fileList[dirSel] || !*fileList[dirSel])
		    break;
		strcpy(temp,fileList[dirSel]);
		int len = strlen(temp);
		if (temp[len-1] != '/')	// not a directory!
		    break;
		temp[len-1] = 0;	// kill the /
		GetDirList(temp, fileFilter, fileList, maxFiles, dirList, maxDirs);
		if (getcwd(curDir,maxFileLen-1))	// get the current directory
		    SetString(fsCD,curDir);
		else
		  {
		    SetString(fsCD,".");
		    strcpy(curDir,".");
		  }
		SetValue(fsLIST,0,ChangeList);
		SetValue(fsLISTDIR,0,ChangeList);
		SetValue(fsLIST,-1,Value);	// turn off file highlight
		SetValue(fsLISTDIR,-1,Value);	// turn off file highlight
		strcpy(temp,curDir);
		if (strcmp(temp,"/") != 0)
		    strcat(temp,"/");
		if (*origFile)
		    strcat(temp,origFile);
		SetString(fsFN,temp);
		dirSel =
		curSel = -1;		// No file selected
		break;
	      }

	    strcpy(temp,fileList[curSel]);
	    int len = strlen(temp);
	    if (temp[len-1] == '/')	// a directory!
	      {
		temp[len-1] = 0;	// kill the /
		GetDirList(temp, fileFilter, fileList, maxFiles, dirList, maxDirs);
		if (getcwd(curDir,maxFileLen-1))	// get the current directory
		    SetString(fsCD,curDir);
		else
		  {
		    SetString(fsCD,".");
		    strcpy(curDir,".");
		  }
		SetValue(fsLIST,0,ChangeList);
		SetValue(fsLISTDIR,0,ChangeList);
		SetValue(fsLIST,-1,Value);	// turn off file highlight
		SetValue(fsLISTDIR,-1,Value);	// turn off file highlight
		strcpy(temp,curDir);
		if (strcmp(temp,"/") != 0)
		    strcat(temp,"/");
		if (*origFile)
		    strcat(temp,origFile);
		SetString(fsFN,temp);
		dirSel =
		curSel = -1;		// No file selected
	      }
	    else
	      {
		// Build up the whole name, including the directory.
		strcpy(temp,curDir);	// the directory
		if (strcmp(temp,"/") != 0)
		    strcat(temp,"/");	// needs trailing /
		strcat(temp,fileList[curSel]); // the file
		strcpy(origFile,fileList[curSel]); // the file
		SetString(fsFN,temp);
	      }
	    break;
	  }

	case fsHOME:		// change to home directory
	  {
	    char *hp = getenv("HOME");

	    if (hp == 0)
		strcpy(temp,".");
	    else
		strcpy(temp,hp);
	    // now change to HOME
	    GetDirList(temp, fileFilter, fileList, maxFiles, dirList, maxDirs);
	    if (getcwd(curDir,maxFileLen-1))	// get the current directory
		SetString(fsCD,curDir);
	    else
	      {
		SetString(fsCD,".");
		strcpy(curDir,".");
	      }
	    SetValue(fsLIST,0,ChangeList);
	    SetValue(fsLISTDIR,0,ChangeList);
	    SetValue(fsLIST,-1,Value);	// turn off file highlight
	    SetValue(fsLISTDIR,-1,Value);	// turn off file highlight
	    strcpy(temp,curDir);
	    if (strcmp(temp,"/") != 0)
		strcat(temp,"/");
	    SetString(fsFN,temp);
	    dirSel =
	    curSel = -1;		// No file selected
	    break;
	  }

	case fsRm:			// rm a file
          {
	    (void) GetTextIn(fsFN, temp, maxFileLen-1);
	    if (strlen(temp) < 1 || temp[strlen(temp)-1] == '/')	// no name picked!
	      {
		vNoticeDialog note(this);
		if (strlen(temp) < maxFileLen - 30)
		    strcat(temp,"\nis not a file name.");
		else
		    strcpy(temp,"You must select or specify a valid file name.");
		note.Notice(temp);
		id = 0;		// don't allow OK to exit
		break;			// break if invalid
	      }
	    // temp has the name of the file to rm

	    vYNReplyDialog yn(this);
	    if (!yn.AskYN("Delete selected file - are you sure?"))
		break;

	    (void) remove(temp);		// this should do it!

	    // now, repaint lists
	    GetDirList(".", fileFilter, fileList, maxFiles, dirList, maxDirs);
	    SetValue(fsLIST,0,ChangeList);
	    SetValue(fsLISTDIR,0,ChangeList);
	    SetValue(fsLIST,-1,Value);	// turn off file highlight
	    SetValue(fsLISTDIR,-1,Value);	// turn off file highlight
	    strcpy(temp,curDir);
	    if (strcmp(temp,"/") != 0)
		strcat(temp,"/");
	    SetString(fsFN,temp);
	    dirSel =
	    curSel = -1;		// No file selected
	    break;
          }

	case fsMkdir:			// mkdir
          {
	    vReplyDialog rp(this);
	    if (!rp.Reply("Enter name of new directory:", temp,maxFileLen))
		break;
	    mkdir(temp,0xFFFF);
	    // now, repaint lists
	    GetDirList(".", fileFilter, fileList, maxFiles, dirList, maxDirs);
	    SetValue(fsLIST,0,ChangeList);
	    SetValue(fsLISTDIR,0,ChangeList);
	    SetValue(fsLIST,-1,Value);	// turn off file highlight
	    SetValue(fsLISTDIR,-1,Value);	// turn off file highlight
	    dirSel =
	    curSel = -1;		// No file selected
	    break;
          }

	case fsSHOWDOT:
 	  {
	    _showHidden = val;		// need to repaint
	    GetDirList(".", fileFilter, fileList, maxFiles, dirList, maxDirs);
	    SetValue(fsLIST,0,ChangeList);
	    SetValue(fsLISTDIR,0,ChangeList);
	    SetValue(fsLIST,-1,Value);	// turn off file highlight
	    SetValue(fsLISTDIR,-1,Value);	// turn off file highlight
	    dirSel =
	    curSel = -1;		// No file selected
	    break;
	  }

	case M_OK:
	  {
	    (void) GetTextIn(fsFN, temp, maxFileLen-1);
	    if (strlen(temp) < 1 || temp[strlen(temp)-1] == '/')	// no name picked!
	      {
		vNoticeDialog note(this);
		if (strlen(temp) < maxFileLen - 30)
		    strcat(temp,"\nis not a file name.");
		else
		    strcpy(temp,"You must select or specify a valid file name.");
		note.Notice(temp);
		id = 0;		// don't allow OK to exit
		break;			// break if invalid
	      }
	    // else fall through to default
	  }

	// WARNING - fall through from previous case!
	default:
	  {
	    vModalDialog::DialogCommand(id,val,ctype);
	    break;
	  }
      }

    if (id == M_OK || id == M_Cancel)
	CloseDialog();
  }

//====================>>> vFileSelect::GetDirList <<<=======================
  void vFileSelect::GetDirList(const char *dir, const char* wild, 
	char** filelist, const int max, char** dirlist, const int maxdir)
  {
    DIR *dirp;		// for reading a directory

    struct dirent *e;

    char fn[200];

    char** fl;
    char** dl;

    struct stat	statbuf;		// for check

    // make sure we can do this
    if (stat(dir, &statbuf) < 0 || (statbuf.st_mode & S_IFDIR) == 0)
	return;

    if ((dirp = opendir(dir)) == NULL)
	return;

    if (chdir(dir) != 0)		// change to new directory
      {
	SysDebug1(BadVals,"Unable to chdir to %s\n",dir);
	return;
      }

    // ok - will build a new list, so it is safe to kill the old one.
    for (fl = filelist ; *fl ; ++fl)	// free the space
      {
	delete [] *fl;
	*fl = 0;
      }
    for (dl = dirlist ; *dl ; ++dl)	// free the space
      {
	delete [] *dl;
	*dl = 0;
      }

    fl = filelist;
    dl = dirlist;

    int ix, dx;
    for (ix = 0, dx = 0; ix < max && dx < maxdir; )
      {
	if ((e = readdir(dirp)) == NULL)	// read entry
	    break;

	if (strlen(e->d_name) >= 199)		// just ignore too long names
	    continue;
	else
	    strcpy(fn,e->d_name);		// make a copy

	if (strcmp(fn,".") == 0)		// skip the "."
	  {
	    continue;
	  }
	if (strcmp(fn,"..") == 0)		// special entry
	  {
	    dl[dx] = new char[4];
	    strcpy(dl[dx],"../");		// make it look like ../
	    dx++;				// bump list counter
	    continue;
	  }
	if (!_showHidden && *fn == '.')		// hide hidden?
	  {
	    continue;
	  }
	if (stat(fn, &statbuf) < 0)		// need to stat for type
	  {
	    fl[ix] = new char[strlen(fn)+1];	// default action
	    strcpy(fl[ix], fn);
	    ix++;				// bump list counter
	    continue;
	  }
	// OK, to here, then have a stat check file type
	if ((statbuf.st_mode & S_IFDIR) != 0)	// this is a dir
	  {
	    dl[dx] = new char[strlen(fn)+2];	// space for name + /
	    strcpy(dl[dx], fn);
	    strcat(dl[dx],"/");
	    dx++;				// bump list counter
	    continue;
	  }
	// this would allow files only, but we will show all for now
	// if ((statbuf.st_mode & (S_IFMT & ~S_IFLNK)) == 0)
	else if (wild_match(fn,wild))
	  {
	    fl[ix] = new char[strlen(fn)+1];	// space for name + /
	    strcpy(fl[ix], fn);
	    ix++;				// bump list counter
	    continue;
	  }
      }

    fl[ix] = 0;		// mark end of the list
    dl[dx] = 0;

    closedir(dirp);	// close directory

    // sort the lists
    qsort(fl, ix, sizeof(char *),
	(int (*)(const void *,const void *))dirsort);

    qsort(dl, dx, sizeof(char *),
	(int (*)(const void *,const void *))dirsort);
  }

//==========================>>> wild_match <<<==============================
  static int wild_match(const char* file, const char* wild)
  {
    // compares list of file suffixes in form .x or *.x  e.g.:"*.c *.h"

    char curpat[100] = "";
    int fileLen = strlen(file);

    if (wild == 0 || wild[0] == 0 || strcmp(wild,"*") == 0)
	return 1;

    char* pat = curpat;			// copy to current pattern
    *pat = 0;

    for (const char *cp = wild ; ; )		// scan all suffixes
      {
	// The X version doesn't handle patterns like XXXX* very well
	// It will treat XXX* the same as XXX*.ext the same - oh well...
	if (*cp == '*' && *curpat != 0)			// pattern of form XXX* found
	  {				// we will be stupid for now
	    *pat = 0;			// finish the pattern
	    // First, see if we have a match...
	    if (strstr(file,curpat)== file)
		return 1;
	    for (++cp ; ;++cp)		// skip till pattern break
	      {
		if (*cp == ' ' || *cp == '\t' || *cp == 0 || *cp == ';')
		    break;
	      }
	    while (*cp == ' ' || *cp == '\t' || *cp == ';' )
		++cp;			// skip whitespace
	    // at beginning of next pattern
	    if (*cp == 0)			// all done?
		break;
	    pat = curpat;		// start over
	    *pat = 0;
	    continue;			// try next pattern
	  }
	else if (*cp == '*')
	  {
	    ++cp;
	    continue;			// ignore leading *'s
	  }
	// look for separator - means we've build a suffix to check
	if (*cp == ' ' || *cp == '\t' || *cp == 0 || *cp == ';')
	  {
	    *pat = 0;			// end pattern string
	    int patLen = strlen(curpat);  // length of pattern
	    if (fileLen < patLen)	// gotta be >=
	      {
		if (*cp == 0)		// out of matches, return 0
		    return 0;
	      }
	    // a match is possible
	    else if (strcmp(&file[fileLen-patLen],curpat) == 0)
		return 1;

	    pat = curpat;		// start over
	    *pat = 0;
	    while (*cp == ' ' || *cp == '\t' || *cp == ';' )
		++cp;			// skip whitespace
	    // at beginning of next pattern
	    if (*cp == 0)			// all done?
		break;
	  }
	else
	    *pat++ = *cp++;		// copy pattern
      }
    return 0;
  }

//==========================>>> dirsort <<<==============================
  static int dirsort(const char **a, const char **b)
  {
    // utility function to sort dir entries
    const char* astr = *a;
    const char* bstr = *b;
    const char* aend = astr + strlen(astr) - 1;
    const char* bend = bstr + strlen(bstr) - 1;

    if (strncmp(astr, "../", 3) == 0)	// this goes first
	return -1;
    if (strncmp(bstr, "../", 3) == 0)
	return  1;

    if (*aend == '/' && *bend != '/')	// directories are next
	return -1;
    if (*aend != '/' && *bend == '/')
	return  1;
    return strcmp(astr, bstr);		// just normal order
  }
