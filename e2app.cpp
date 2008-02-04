//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id$
//-------------------------------------------------------------------------//
//                                                                         //
// This program is free software; you can redistribute it and/or           //
// modify it under the terms of the GNU  General Public License            //
// as published by the Free Software Foundation; either version2 of        //
// the License, or (at your option) any later version.                     //
//                                                                         //
// This program is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
// General Public License for more details.                                //
//                                                                         //
// You should have received a copy of the GNU  General Public License      //
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//

#include "e2app.h"		// Header file
#include "e2awinfo.h"

#include "cmdenum.h"
#include "string_table.h"
#include "loaddrv.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <locale.h>

#ifdef	_LINUX_
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#else
#  ifdef	__BORLANDC__
#    define	strncasecmp strnicmp
#    define strcasecmp stricmp
#  else // _MICROSOFT_ VC++
#    define strncasecmp	_strnicmp
#    define strcasecmp _stricmp
#  endif
#endif

//const int idAskToSave = 100; // Dummy Command

//=========================>>> e2App::e2App <<<==========================
e2App::e2App(char* name, int w, int h)
	: vApp(name, 0, w, h),
		winCounter(0),
		port_number(3),		// uses COM3 by default
		abortFlag(0),
		ignoreFlag(0),
		app_status(AppReady)
{
	// Constructor
	UserDebug(Constructor, "e2App::e2App()\n");

	s2430B.SetOrganization(ORG8);		//Default X2444Bus organization is ORG16, but S24H30 organization is ORG8
//	mega103B.SetPageSize(256);
//	mega103B.SetFlashPagePolling(false);
//	atMegaB.SetPageSize(256);
//	atMegaB.SetFlashPagePolling(true);
//	mega16xB.SetPageSize(128);
//	mega16xB.SetFlashPagePolling(true);
//	mega8xB.SetPageSize(64);
//	mega8xB.SetFlashPagePolling(true);
//	tiny2xB.SetPageSize(32);
//	tiny2xB.SetFlashPagePolling(true);

	//AutoTag
	//Initialize Bus Pointers vector
	busvetp[I2C-1] = &iicB;
	busvetp[AT90S-1] = &at90sB;
//	busvetp[ATMEGAB-1] = &atMegaB;
//	busvetp[AT1200S-1] = &at1200B;
	busvetp[AT89S-1] = &at89sB;
	busvetp[AT93C-1] = &at93cB;
	busvetp[AT250-1] = &at250B;
	busvetp[AT250BIG-1] = &at250BigB;
	busvetp[PICB-1] = &picB;
	busvetp[PIC12B-1] = &pic12B;
	busvetp[SXB-1] = &sxB;
	busvetp[SDEB-1] = &sdeB;
	busvetp[PICNEWB-1] = &picNewB;
	busvetp[IMBUS-1] = &imB;
	busvetp[X2444B-1] = &x2444B;
	busvetp[S2430B-1] = &s2430B;

#ifdef	WIN32
	InitDrvLoader();
#endif
	SetInterfaceType();	//Set default interface
}

//=========================>>> e2App::e2App <<<==========================
e2App::~e2App()
{
	UserDebug(Destructor, "e2App::~e2App()\n");

	// Destructor

//	if (_e2CmdWin)
//		delete _e2CmdWin;
}

//=====================>>> e2App::NewAppWin <<<==========================
vWindow* e2App::NewAppWin(vWindow* win, char* name,
		int w, int h, vAppWinInfo* winInfo)
{
	vWindow* thisWin = win;			//local copy to use
	vAppWinInfo* awinfo = winInfo;
	char *appname = name;

	if (!*name)
	{
		appname = "No Name";		// Default name
	}
	
	UserDebug1(Build,"e2App::NewAppWin(%s)\n",appname);

	// Create the first window using provided CmdWindow
	winCounter++;		//incremente il numero di finestre

	if (!thisWin)		//need to new a window
	{
		thisWin = new e2CmdWindow(appname, w, h);
	}

	if (!awinfo)
	{
		awinfo = new e2AppWinInfo((vCmdWindow*)thisWin, name, busvetp);
	}

	return vApp::NewAppWin(thisWin, appname, w, h, awinfo);
}

static int exit_ok = 0;

//============================>>> e2App::Exit <<<===========================
void e2App::Exit()
{
	// This is called to close all windows.

	UserDebug(Build,"e2App::Exit()\n");

	if ( !IsAppReady() )
	{
		//06/09/99
		SetAbortFlag();
		SendWindowCommandAll(idCloseAllDialog, 0, C_Button);
		CheckEvents();
	//	SetAppReady();
	}

	if ( IsAppReady() )
	{
		if (!scriptMode)
			SendWindowCommandAll(idAskToSave, 0, C_Button);

		exit_ok = 1;

		// Close All registered windows and exit
#if 0
		WindList *curWin;
		vWindow *tmp;

		bool inExit = true;
		for (curWin = _WindowList ; curWin !=0 ; curWin = _WindowList)
		{
			int retv;

			tmp = curWin->window;
			if (IsHelpWin(tmp))
				retv = CloseHelpWin(tmp);
			else
				retv = CloseAppWin(tmp);		// use local or derived close app
			if (!retv)
			{
				inExit = false;
				return;
			}
		}

		if (inExit)
			AppExit(returnValue);
#else
		vApp::Exit();		// Default behavior
#endif
	}
}

void e2App::DropFile(const char *fn)
{
	if ( fn && strlen(fn) )
		NewAppWin(0, (char*)fn, DefaultWidth(), DefaultHeight(), 0);
}

#include <v/vynreply.h>	// for vYN dialog
#include <v/v_defs.h>

//======================>>> e2App::CloseAppWin <<<===========================
int e2App::CloseAppWin(vWindow* win)
{
	if ( !IsAppReady() )
	{
		//06/09/99
		SetAbortFlag();
		SendWindowCommandAll(idCloseAllDialog, 0, C_Button);
		CheckEvents();
		
	//	SetAppReady();
	}

	if ( IsAppReady() )
	{
		// This will be called BEFORE a window has been unregistered or
		// closed.  Default behavior: unregister and close the window.
		int really_close = 0;
		vYNReplyDialog yn(this);

		UserDebug(Build,"e2App::CloseAppWin()\n");

		if ( !exit_ok && ((e2CmdWindow *)win)->IsBufChanged() )
		{
			vYNReplyDialog yn(this);
	  		if ( yn.AskYN(STR_MSGCLOSEWINSAVE) > 0 )
	  		{
	  			((e2CmdWindow *)win)->CmdSave();
	  		}
	  	}

	  	if (winCounter > 1 || exit_ok)
	  		really_close = 1;
	  	else
	  	{
	  		if ( yn.AskYN(STR_MSGCLOSEWINEXIT) > 0 )
	  		{
	  			really_close = 1;
	  		}
	  	}

	  	if (really_close)
	  	{
	  		winCounter--;				//decrementa il numero di finestre
	  		return vApp::CloseAppWin(win);
	  	}
	  	else
	  		return 0;
	}
	else
		return 0;
}

//=====================>>> e2App::SetAppBusy <<<==============================
void e2App::SetAppBusy()
{
	app_status = AppBusy;
	SendWindowCommandAll(idEnableToolBar, 0, C_Button);
}

//=====================>>> e2App::SetAppReady <<<==============================
void e2App::SetAppReady()
{
	app_status = AppReady;
	SendWindowCommandAll(idEnableToolBar, 1, C_Button);
}

//=====================>>> e2App::OpenPort <<<==============================
int e2App::OpenPort(int port)
{
	UserDebug1(UserApp1,"e2App::OpenPort(%d)\n",port);
	if (port)
		SetPort(port);
	ClosePort();
	return iniBus->Open(GetPort());
}

//=====================>>> e2App::ClosePort <<<==============================
void e2App::ClosePort()
{
	UserDebug1(UserApp1,"e2App::ClosePort() iniBus=%xh\n",(unsigned int)iniBus);
	iniBus->Close();
}

//=====================>>> e2App::TestPort <<<==============================
int e2App::TestPort(int port, int open_only)
{
	UserDebug2(UserApp1,"e2App::TestPort(port=%d, open_only=%d)\n",port,open_only);

	int rv = (open_only) ?
		busIntp->TestOpen(port) :
		busIntp->TestPort(port);

	UserDebug1(UserApp1,"e2App::TestPort() = %d\n",rv);

	return rv;
}

//=====================>>> e2App::OpenBus <<<==============================
int e2App::OpenBus(BusIO *p)
{
	UserDebug1(UserApp1,"e2App::OpenBus(%xh)\n", (unsigned int)p);

	iniBus->Close();

	UserDebug(UserApp1,"e2App::OpenBus() ** Close\n");

	iniBus = p;
	int rv = iniBus->Open(GetPort());

	UserDebug1(UserApp1,"e2App::OpenBus() ** Open = %d\n", rv);

	if (rv == OK)
	{
		rv = busIntp->SetPower(1);

		UserDebug(UserApp1,"e2App::OpenBus() ** SetPower\n");

		//Power up delay
		iniBus->WaitMsec( GetPowerUpDelay() );

		UserDebug(UserApp1,"e2App::OpenBus() ** Reset\n");

		iniBus->Reset();	//28/10/98
	}
	UserDebug1(UserApp1,"e2App::OpenBus() = %d\n", rv);

	return rv;
}

//=====================>>> e2App::SleepBus <<<==============================
void e2App::SleepBus()
{
	UserDebug1(UserApp2,"e2App::CloseBus() iniBus=%xh\n", (unsigned int)iniBus);

	iniBus->WaitMsec(5);	// 08/04/98 -- hold time dell'alimentazione
	busIntp->SetPower(0);
	iniBus->Close();		// 28/09/98 -- richiude la porta dopo averla usata
}

//=====================>>> e2App::GetAbortFlag <<<==============================
int e2App::GetAbortFlag()
{
	CheckEvents();

	int a = abortFlag;
	abortFlag = 0;
	return a;
}

//Update the progress bar
//=====================>>> e2App::SetProgress <<<==============================
void e2App::SetProgress(int progress)
{
	SendWindowCommandAll(pbrProgress,progress,C_Button);    // The horizontal bar
}

//=====================>>> e2App::Calibration <<<==============================
int e2App::Calibration()
{
	UserDebug(UserApp3,"e2App::Calibration()\n");

	int rv = OK;
//	OpenBus(&iicB);		//aggiunto il 06/03/98
	LookForBogoMips();
//	rv = iicB.Calibration();
//	SleepBus();			//aggiunto il 06/03/98
	return rv;
}

//=====================>>> e2App::AppCommand <<<==============================
void e2App::AppCommand(vWindow* win, ItemVal id, ItemVal val, CmdType cType)
{
	// Commands not processed by the window will be passed here

	UserDebug1(Build,"e2App::AppCmd(ID: %d)\n",id);
	vApp::AppCommand(win, id, val, cType);
}

//=========================>>> e2App::KeyIn <<<==============================
void e2App::KeyIn(vWindow* win, vKey key, unsigned int shift)
{
	// Key strokes not processed by the window will be passed here

	vApp::KeyIn(win, key, shift);
}

//=======================>>> e2App::SetInterface <<<=========================
void e2App::SetInterfaceType(HInterfaceType type)
{
	switch (type)
	{
	//Interface initializers
	case SIPROG_API:
		iType = SIPROG_API;
		busIntp = &siprog_apiI;
		break;
	case SIPROG_IO:
		iType = SIPROG_IO;
		busIntp = &siprog_ioI;
		break;
	case EASYI2C_API:
		iType = EASYI2C_API;
		busIntp = &easyi2c_apiI;
		easyi2c_apiI.SetIOmode(false);
		easyi2c_apiI.Close();
		break;
	case EASYI2C_IO:
		iType = EASYI2C_IO;
		busIntp = &easyi2c_ioI;
		easyi2c_ioI.SetIOmode(true);
		easyi2c_ioI.Close();
		break;
	case AVRISP:
		iType = AVRISP;
		busIntp = &avrisp_apiI;
		avrisp_apiI.SetIOmode(false);
		avrisp_apiI.Close();
		break;
	case AVRISP_IO:
		iType = AVRISP_IO;
		busIntp = &avrisp_ioI;
		avrisp_ioI.SetIOmode(true);
		avrisp_ioI.Close();
		break;
	case JDM_API:
		iType = JDM_API;
		busIntp = &jdm_apiI;
		jdm_apiI.SetCmd2CmdDelay( GetJDMCmd2CmdDelay() );
		break;
//	case JDM_IO:
//		iType = JDM_IO;
//		busIntp = &jdm_ioI;
//		break;
	case DT006_API:
		iType = DT006_API;
		busIntp = &dt006_apiI;
		dt006_apiI.SetIOmode(false);
		dt006_apiI.Close();
		break;
	case DT006_IO:
		iType = DT006_IO;
		busIntp = &dt006_ioI;
		dt006_ioI.SetIOmode(true);
		dt006_ioI.Close();
		break;
	default:
		iType = SIPROG_API;		//20/07/99 -- to prevent crash
		busIntp = &siprog_apiI;
		break;
	}

	int k;
	for (k = 0; k < NO_OF_BUSTYPE; k++)
		busvetp[k]->SetBusInterface(busIntp);
}

static volatile int alarm_caught;

static void alarm(int i)
{
	alarm_caught = 1;
}

#define N_SAMPLE	4
#define	N_CICLI	8000
#define	UDELAY	40
#define	MSLICE	((float)UDELAY * N_CICLI / 1000.0)

void e2App::LookForBogoMips()
{
#ifdef	__unix__
	struct itimerval it;

	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 0;

	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 500000;

	signal(SIGALRM, alarm);

	long ndel[N_SAMPLE];
	int k;
	for (k = 0; k < N_SAMPLE; k++)
	{
		setitimer(ITIMER_REAL, &it, NULL);
		alarm_caught = 0;
		long j = 0;
		while (!alarm_caught)
			j++;
		ndel[k] = j;

	//	fprintf(stderr, "** %ld\n", j);
	}
	long sum = 0;
	for (k = 0; k < N_SAMPLE; k++)
		sum += ndel[k];

	SetBogoMips( (int)(sum / (500 * N_SAMPLE)) );
#endif

#ifdef	_WINDOWS
	DWORD t0;
	DWORD count;
	DWORD multiplier = 1;

	static char strbuf[MAX_PATH];
	strncpy(strbuf, THEAPP->helpfile, MAX_PATH);
	char *sp = strrchr(strbuf, '\\');
	if (sp == NULL)
		sp = strrchr(strbuf, '/');
	if (sp == NULL)
		strcpy(strbuf, "bogomips.out");
	else
		strcpy(sp+1, "bogomips.out");

	FILE *fh = fopen(strbuf, "w");

	Wait w;
	int k;

	w.SetHwTimer(0);	//Disable Hw timer for bogomips calibration

	// First BogoMIPS evaluation
	do {
		multiplier *= 5;

		SetBogoMips(700 * multiplier);
		w.SetBogoKips();

		t0 = GetTickCount();
		for (k = 0; k < N_CICLI/10; k++)
			w.WaitUsec(UDELAY);
		count = GetTickCount() - t0;

		if (fh)
			fprintf(fh, "bogo = %d, count = %d\n", GetBogoMips(), count);
	} while (count <= MSLICE/11);

	t0 = GetTickCount();
	for (k = 0; k < N_CICLI; k++)
		w.WaitUsec(UDELAY);
	count = GetTickCount() - t0;

	if (fh)
		fprintf(fh, "1) count = %d ** mslice = %f *** bogo = %d\n", count, MSLICE, GetBogoMips());

	int j;

	//Fast correction
	for (j = 0; count > (MSLICE * 1.01) && j < 10; j++)
	{
		int nv;

		nv = (DWORD) ((float) GetBogoMips() * (MSLICE / (float)count) + 0.5);
		SetBogoMips( nv );
		w.SetBogoKips();

		t0 = GetTickCount();
		for (k = 0; k < N_CICLI; k++)
			w.WaitUsec(UDELAY);
		count = GetTickCount() - t0;

		if (fh)
			fprintf(fh, "2) count = %d ** mslice = %f *** bogo = %d\n", count, MSLICE, GetBogoMips());
	}

	//Fine correction
	for (j = 0; count < (MSLICE * 0.99) && j < 10; j++)
	{
		int nv;

		nv = (DWORD) ((float) GetBogoMips() * 1.01 + 0.5);
		SetBogoMips( nv );
		w.SetBogoKips();

		t0 = GetTickCount();
		for (k = 0; k < N_CICLI; k++)
			w.WaitUsec(UDELAY);
		count = GetTickCount() - t0;

		if (fh)
			fprintf(fh, "3) count = %d ** mslice = %f *** bogo = %d\n", count, MSLICE, GetBogoMips());
	}

	w.CheckHwTimer();	//Check to enable again Hw timer

	if (fh)
	{
		if (w.GetHwTimer())
			fprintf(fh, "Hardware timer is OK.\n");
		else
			fprintf(fh, "Hardware timer is too slow, use bogomips (%d)\n", GetBogoMips());

		fclose(fh);
	}
#endif
}

int e2App::GetIgnoreFlag() const
{
	return ignoreFlag;
}

void e2App::SetIgnoreFlag()
{
	ignoreFlag = 1;
}

void e2App::ClearIgnoreFlag()
{
	ignoreFlag = 0;
}
 
int e2App::LoadDriver(int start)
{
#ifdef  _WINDOWS
	if (start)
	{
		OpenDriver();
	}
	else
	{
		CloseDriver();
	}
#endif

	return OK;
}

//###########################################################################

static e2App e2_App(APPNAME " - " APPNAME_EXT);	// The instance of the app

//============================>>> AppMain <<<==============================
int AppMain(int argc, char** argv)
{
	char str[MAXPATH];

#ifdef	__unix__
	char *sp;

	str[0] = '\0';
	if ( (sp = getenv("HOME")) != NULL )
	{
		strncpy(str, sp, MAXPATH - (strlen(APPNAME) + 5) );
		strcat(str, "/");
	}
	strcat(str, "." APPNAME "rc");
	str[MAXPATH-1] = '\0';
	e2_App.SetFileName(str);
#else
	//The profile file is called "ponyprog.ini"
	//while the help file "ponyprog.html"
	char *sp = strrchr(argv[0], '.');
	strcpy(sp+1, "html");
	strncpy(THEAPP->helpfile, argv[0], MAXPATH);
	THEAPP->helpfile[MAXPATH-1] = '\0';

	strcpy(sp+1, "ini");
	e2_App.SetFileName(argv[0]);

	sp = strrchr(argv[0], '\\');
	if (sp)
	{
		sp[1] = '\0';
		strncpy(str, argv[0], MAXPATH);
		str[MAXPATH-1] = '\0';
	}
	else
	{
		str[0] = '\0';
	}
	strncpy(THEAPP->ok_soundfile, str, MAXPATH - (strlen("oksound.wav") + 1));
	strcat(THEAPP->ok_soundfile, "oksound.wav");
	strncpy(THEAPP->err_soundfile, str, MAXPATH - (strlen("errsound.wav") + 1));
	strcat(THEAPP->err_soundfile, "errsound.wav");
#endif

	char const *sp2 = e2_App.GetLanguageCode();
	if (sp2)
	{
		strncpy(str, sp2, MAXPATH);
		str[MAXPATH-1] = '\0';
		e2_App.SetLanguageCode( str );
		if (strcasecmp(str, "default") == 0)
			sp2 = "";
		else
		if (strcasecmp(str, "locale") == 0)
			sp2 = STR_LANGUAGE_CODE;
		else
			sp2 = str;
		setlocale(LC_ALL, sp2 );
	}

	// Read parameters from INI file
	// Make sure all parameters (even default values) are written
	//   to the INI file.
	e2_App.SetInterfaceType( (HInterfaceType) e2_App.GetParInterfType() );
	e2_App.SetPort( e2_App.GetParPortNo() );
	e2_App.SetPowerUpDelay( e2_App.GetPowerUpDelay() );
	e2_App.SetPolarity( e2_App.GetPolarityControl() );
	e2_App.SetSPIResetPulse( e2_App.GetSPIResetPulse() );
	e2_App.SetSPIDelayAfterReset( e2_App.GetSPIDelayAfterReset() );
	e2_App.SetSPIPageWrite( e2_App.GetSPIPageWrite() );
	e2_App.SetMegaPageDelay( e2_App.GetMegaPageDelay() );
	e2_App.SetAVRProgDelay( e2_App.GetAVRProgDelay() );
	e2_App.SetAVREraseDelay( e2_App.GetAVREraseDelay() );

	e2_App.SetI2CSpeed( e2_App.GetI2CSpeed() );
	e2_App.SetSPISpeed( e2_App.GetSPISpeed() );
	e2_App.SetMicroWireSpeed( e2_App.GetMicroWireSpeed() );
	e2_App.SetPICSpeed( e2_App.GetPICSpeed() );
	e2_App.SetSDESpeed( e2_App.GetSDESpeed() );

	e2_App.SetLogEnabled( e2_App.GetLogEnabled() );
	e2_App.SetSoundEnabled( e2_App.GetSoundEnabled() );
	e2_App.SetVerifyAfterWrite( e2_App.GetVerifyAfterWrite() );
	e2_App.SetClearBufBeforeLoad( e2_App.GetClearBufBeforeLoad() );
	e2_App.SetClearBufBeforeRead( e2_App.GetClearBufBeforeRead() );
	e2_App.SetAutoDetectPorts( e2_App.GetAutoDetectPorts() );
	e2_App.SetAt89PageOp( e2_App.GetAt89PageOp() );
#ifdef	__unix__
	e2_App.SetHtmlBrowseApp( e2_App.GetHtmlBrowseApp() );
	e2_App.SetLockDir( e2_App.GetLockDir() );
	e2_App.SetDevDir( e2_App.GetDevDir() );
#endif
	e2_App.SetDevName( e2_App.GetDevName() );
	e2_App.scriptMode = false;
	e2_App.returnValue = 0;
	e2_App.script_name[0] = '\0';

	// The first parameter of the command line is the file to open (optional)
	char *param;
	if (argc > 1 && strlen(argv[1]) > 0)
	{
		param = argv[1];
		//Elimina eventuali virgolette che possono confondere
		//  il programma
		if (*param == '"')
			param++;
		sp = strrchr(param, '"');
		if (sp)
			*sp = '\0';

		sp = strrchr(param, '.');
		if ( sp && strcasecmp(sp, ".e2s") == 0 )
		{	//Switch to script mode
			e2_App.scriptMode = true;
			strncpy(e2_App.script_name, param, MAXPATH);
			e2_App.script_name[MAXPATH-1] = '\0';
			param = "";
		}
		else
		if ( (strcasecmp(param, "-s") == 0 ||
			 strcasecmp(param, "--script") == 0) )
		{
			if ( argc > 2 && strlen(argv[2]) > 0 )
			{
				param = argv[2];
				//Elimina eventuali virgolette che possono confondere
				//  il programma
				if (*param == '"')
					param++;
				sp = strrchr(param, '"');
				if (sp)
					*sp = '\0';

				e2_App.scriptMode = true;
				strncpy(e2_App.script_name, param, MAXPATH);
				e2_App.script_name[MAXPATH-1] = '\0';
				param = "";
			}
		}
	}
	else
	{
		param = "";
	}

	e2CmdWindow *cwin;

	// Use AppMain to create the main window
	cwin = (e2CmdWindow *)theApp->NewAppWin(0, param,
		theApp->DefaultWidth(),
		theApp->DefaultHeight()
		);

	if (e2_App.scriptMode && cwin)
	{
		e2_App.returnValue = cwin->CmdRunScript();

		if (e2_App.returnValue != 0)
			return e2_App.returnValue;		//Se AppMain ritorna un valore != 0 esce immediatamente
		else
			e2_App.Exit();
	}

	return 0;
}
