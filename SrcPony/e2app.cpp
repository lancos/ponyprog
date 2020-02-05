//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2020   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
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
// along with this program (see LICENSE);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//=========================================================================//

#include "e2profil.h"
#include "e2awinfo.h"
#include "e2app.h"        // Header file

#include <QCoreApplication>
#include <QString>
#include <QtCore>
#include <QDebug>

#ifdef Q_OS_LINUX
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#endif

#include "microbus.h"
#include "interfconv.h"

//const int idAskToSave = 100; // Dummy Command

e2App::e2App() :
	awip(0),
	port_number(0)
{
	// Constructor
	qDebug() << Q_FUNC_INFO;

	//         awinfo = 0;

	s2430B.SetOrganization(ORG8);           //Default X2444Bus organization is ORG16, but S24H30 organization is ORG8
	//      mega103B.SetPageSize(256);
	//      mega103B.SetFlashPagePolling(false);
	//      atMegaB.SetPageSize(256);
	//      atMegaB.SetFlashPagePolling(true);
	//      mega16xB.SetPageSize(128);
	//      mega16xB.SetFlashPagePolling(true);
	//      mega8xB.SetPageSize(64);
	//      mega8xB.SetFlashPagePolling(true);
	//      tiny2xB.SetPageSize(32);
	//      tiny2xB.SetFlashPagePolling(true);

	//AutoTag
	//Initialize Bus Pointers vector
	busvetp[I2C - 1] = &iicB;
	busvetp[AT90S - 1] = &at90sB;
	//busvetp[ATMEGAB-1] = &atMegaB;
	//busvetp[AT1200S-1] = &at1200B;
	busvetp[AT89S - 1] = &at89sB;
	busvetp[AT93C - 1] = &at93cB;
	busvetp[AT250 - 1] = &at250B;
	busvetp[AT250BIG - 1] = &at250BigB;
	busvetp[PICB - 1] = &picB;
	busvetp[PIC12B - 1] = &pic12B;
	//busvetp[SXB - 1] = &sxB;
	busvetp[SDEB - 1] = &sdeB;
	busvetp[PICNEWB - 1] = &picNewB;
	busvetp[IMBUS - 1] = &imB;
	busvetp[X2444B - 1] = &x2444B;
	busvetp[S2430B - 1] = &s2430B;

	//duplicated call in initSettings()
	//SetInterfaceType();     //Set default interface

	initSettings();
}


e2App::~e2App()
{
	qDebug() << Q_FUNC_INFO;
}


void e2App::initSettings()
{
	QString str;

	qDebug() << "Settings file: " << E2Profile::GetConfigFile();	// << " - Format: " << E2Profile::s->format();

	QString currentAppDir = qApp->applicationDirPath();

	helpfile = currentAppDir + "/ponyprog.html";
	ok_soundfile = currentAppDir + "/oksound.wav";
	err_soundfile = currentAppDir + "/errsound.wav";

	QString sp2 = E2Profile::GetLanguageCode();

	if (sp2.length() > 0)
	{
		str = sp2;

		E2Profile::SetLanguageCode(str.toLatin1());

		if (str == "default")
		{
			sp2 = "";
		}
		else if (str == "locale")
		{
			sp2 = translate(STR_LANGUAGE_CODE);
		}
		else
		{
			sp2 = str;
		}

		setlocale(LC_ALL, sp2.toLatin1());
	}

	// Read parameters from INI file
	SetInterfaceType(E2Profile::GetParInterfType());
	SetPort(E2Profile::GetPortNumber());
	SetPolarity(E2Profile::GetPolarityLines());
#if 0
	// Make sure all parameters (even default values) are written
	//   to the INI file.
	E2Profile::GetPowerUpDelay();
	E2Profile::GetSPIResetPulse();
	E2Profile::GetSPIDelayAfterReset();
	E2Profile::GetSPIPageWrite();
	E2Profile::GetMegaPageDelay();
	E2Profile::GetAVRProgDelay();
	E2Profile::GetAVREraseDelay();
	E2Profile::GetAT89DelayAfterReset();
	E2Profile::GetAVRDelayAfterReset();
	//
	E2Profile::GetI2CSpeed();
	E2Profile::GetSPISpeed();
	E2Profile::GetMicroWireSpeed();
	E2Profile::GetPICSpeed();
	E2Profile::GetSDESpeed();
	//
	E2Profile::GetLogEnabled();
	E2Profile::GetSoundEnabled();
	E2Profile::GetSkipStartupDialog();
	E2Profile::GetVerifyAfterWrite();
	E2Profile::GetClearBufBeforeLoad();
	E2Profile::GetClearBufBeforeRead();
	E2Profile::GetAutoDetectPorts();
	E2Profile::GetAt89PageOp();
	E2Profile::Get8253FallEdge();

#ifdef Q_OS_LINUX
	E2Profile::GetLockDir();
	E2Profile::GetDevDir();
#endif

	E2Profile::GetDevName();
#endif

	scriptMode = false;
	returnValue = 0;
	script_name = "";
}
//=====================>>> e2App::OpenPort <<<==============================
int e2App::OpenPort(int port)
{
	qDebug() << Q_FUNC_INFO << "(" << port << ")";

	if (port >= 0)
	{
		SetPort(port);
	}

	ClosePort();
	return iniBus->Open(GetPort());
}


//=====================>>> e2App::ClosePort <<<==============================
void e2App::ClosePort()
{
	qDebug() << Q_FUNC_INFO << " iniBus=" << (hex) << iniBus << (dec);
	iniBus->Close();
}

//=====================>>> e2App::TestPort <<<==============================
int e2App::TestPort(int port, bool open_only)
{
	qDebug() << Q_FUNC_INFO << "(port=" << port << ", open_only=" << open_only << ")";

	int rv = (open_only) ?
			 busIntp->TestOpen(port) :
			 busIntp->TestPort(port);

	qDebug() << Q_FUNC_INFO << " = " << rv;

	return rv;
}

//=====================>>> e2App::OpenBus <<<==============================
int e2App::OpenBus(BusIO *p)
{
	qDebug() << Q_FUNC_INFO << "(" << (hex) << p << (dec) << ")";

	iniBus->Close();

	qDebug() << Q_FUNC_INFO << " ** Close";

	iniBus = p;
	int rv = iniBus->Open(GetPort());

	qDebug() << Q_FUNC_INFO << " ** Open = " << rv;

	if (rv == OK)
	{
		rv = busIntp->SetPower(true);

		qDebug() << Q_FUNC_INFO << " ** SetPower";

		//Power up delay
		busIntp->WaitMsec(E2Profile::GetPowerUpDelay());

		qDebug() << Q_FUNC_INFO << " ** Reset";

		iniBus->Reset();        //28/10/98
	}

	qDebug() << Q_FUNC_INFO << " = " << rv;

	return rv;
}

//=====================>>> e2App::SleepBus <<<==============================
void e2App::SleepBus()
{
	qDebug() << Q_FUNC_INFO << " iniBus=" << (hex) << iniBus << (dec);

	busIntp->WaitMsec(5);		// 08/04/98 -- power hold time
	busIntp->SetPower(false);
	iniBus->Close();			// 28/09/98 -- richiude la porta dopo averla usata
}


// EK 2017
// TODO to remove this to init function
#if 0
//=====================>>> e2App::NewAppWin <<<==========================
vWindow *e2App::NewAppWin(vWindow *win, char *name,
						  int w, int h, vAppWinInfo *winInfo)
{
#ifdef Q_OS_WIN32            // Yes it's a dirty hack here but clean in Windows sense, heha 130406
	HKEY key;       // But I wouldn't change the very incomplete V-lib source.

	if (!RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\h#s\\PonyProg", 0, KEY_QUERY_VALUE, &key))
	{
		WINDOWPLACEMENT wp;
		wp.length = sizeof wp;
		HWND w = theApp->winHwnd();

		if (GetWindowPlacement(w, &wp))
		{
			DWORD size = sizeof wp.rcNormalPosition;

			if (!RegQueryValueEx(key, "WinPos", NULL, NULL, (LPBYTE)&wp.rcNormalPosition, &size)
					&& size == sizeof wp.rcNormalPosition)
			{
				SetWindowPlacement(w, &wp);
			}
		}

		RegCloseKey(key);
	}

#endif
	vWindow *thisWin = win;                 //local copy to use
	vAppWinInfo *awinfo = winInfo;
	char *appname = name;

	if (!*name)
	{
		appname = "No Name";            // Default name
	}

	qDebug() << "e2App::NewAppWin(" << appname << ")";

	// Create the first window using provided CmdWindow
	winCounter++;           //incremente il numero di finestre

	if (!thisWin)           //need to new a window
	{
		thisWin = new e2CmdWindow(appname, w, h);
	}

	if (!awinfo)
	{
		awinfo = new e2AppWinInfo((vCmdWindow *)thisWin, name, busvetp);
	}

	return vApp::NewAppWin(thisWin, appname, w, h, awinfo);
}

#endif

int e2App::Calibration()
{
	qDebug() << Q_FUNC_INFO;

	int rv = OK;
	//      OpenBus(&iicB);         //aggiunto il 06/03/98
	LookForBogoMips();
	//      rv = iicB.Calibration();
	//      SleepBus();                     //aggiunto il 06/03/98
	return rv;
}

void e2App::SetInterfaceType(HInterfaceType type)
{
	switch (type)
	{
	//Interface initializers
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
		jdm_apiI.SetCmd2CmdDelay(E2Profile::GetJDMCmd2CmdDelay());
		break;

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

	case LINUXSYSFS_IO:
		iType = LINUXSYSFS_IO;
		busIntp = &linuxsysfs_ioI;
		break;

	case FTDI_JTAGKEY:
		iType = FTDI_JTAGKEY;
		busIntp = &jtagkeyI;
		break;

	case PONYPROG_FT:
		iType = PONYPROG_FT;
		busIntp = &ponyprog_ftI;
		break;

	case SIPROG_API:
	default:	             //20/07/99 -- to prevent crash
		iType = SIPROG_API;
		busIntp = &siprog_apiI;
		break;
	}

	busIntp->SetUSBVidPid(TypeToInterfVidPid(iType));
	InterfPins pins;
	E2Profile::GetInterfacePins(iType, pins);
	busIntp->ConfigPins(pins);

	for (int k = 0; k < NO_OF_BUSTYPE; k++)
	{
		busvetp[k]->SetBusInterface(busIntp);
	}
}

#ifdef  __unix__
static volatile int alarm_caught;

static void alarm(int i)
{
	(void)i;
	alarm_caught = 1;
}
#endif

#define N_SAMPLE        4
#define N_CICLI 8000
#define UDELAY  40
#define MSLICE  ((float)UDELAY * N_CICLI / 1000.0)

void e2App::LookForBogoMips()
{
#ifdef  __unix__
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
		{
			j++;
		}

		ndel[k] = j;

		//      fprintf(stderr, "** %ld\n", j);
	}

	long sum = 0;

	for (k = 0; k < N_SAMPLE; k++)
	{
		sum += ndel[k];
	}

	E2Profile::SetBogoMips((int)(sum / (500 * N_SAMPLE)));
#else
	DWORD t0;
	DWORD count;
	DWORD multiplier = 1;

	QString strbuf;
	QFileInfo fi(E2Profile::GetConfigFile());
	if (fi.exists())
	{
		strbuf = fi.canonicalPath();
	}
	else
	{
		strbuf = qApp->applicationDirPath();
	}

	strbuf.append("/bogomips.out");

	QFile fh(strbuf);

	if (!fh.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return;
	}

	QTextStream out(&fh);

	Wait w;
	int k;

	w.SetHwTimer(0);        //Disable Hw timer for bogomips calibration

	// First BogoMIPS evaluation
	do
	{
		multiplier *= 5;

		E2Profile::SetBogoMips(700 * multiplier);
		w.SetBogoKips();

		t0 = GetTickCount();

		for (k = 0; k < N_CICLI / 10; k++)
		{
			w.WaitUsec(UDELAY);
		}

		count = GetTickCount() - t0;

		out << "bogo = " << E2Profile::GetBogoMips() << ", count =" << count << "\n";
	}
	while (count <= MSLICE / 11);

	t0 = GetTickCount();

	for (k = 0; k < N_CICLI; k++)
	{
		w.WaitUsec(UDELAY);
	}

	count = GetTickCount() - t0;

	out << "1) count = " << count << " ** mslice = " << MSLICE << " *** bogo = " << E2Profile::GetBogoMips() << "\n";

	int j;

	//Fast correction
	for (j = 0; count > (MSLICE * 1.01) && j < 10; j++)
	{
		int nv;

		nv = (DWORD)((float) E2Profile::GetBogoMips() * (MSLICE / (float)count) + 0.5);
		E2Profile::SetBogoMips(nv);
		w.SetBogoKips();

		t0 = GetTickCount();

		for (k = 0; k < N_CICLI; k++)
		{
			w.WaitUsec(UDELAY);
		}

		count = GetTickCount() - t0;

		out << "2) count = " << count << " ** mslice = " << MSLICE << " *** bogo = " << E2Profile::GetBogoMips() << "\n";
	}

	//Fine correction
	for (j = 0; count < (MSLICE * 0.99) && j < 10; j++)
	{
		int nv;

		nv = (DWORD)((float) E2Profile::GetBogoMips() * 1.01 + 0.5);
		E2Profile::SetBogoMips(nv);
		w.SetBogoKips();

		t0 = GetTickCount();

		for (k = 0; k < N_CICLI; k++)
		{
			w.WaitUsec(UDELAY);
		}

		count = GetTickCount() - t0;

		out << "3) count = " << count << " ** mslice = " << MSLICE << " *** bogo = " << E2Profile::GetBogoMips() << "\n";
	}

	w.CheckHwTimer();       //Check to enable again Hw timer

	if (w.GetHwTimer())
	{
		out << "Hardware timer is OK.\n";
	}
	else
	{
		out << "Hardware timer is too slow, use bogomips (" << E2Profile::GetBogoMips() << ")\n";
	}

	fh.close();
#endif
}


int e2App::LoadDriver(int start)
{
	int rv = OK;

	(void)start;

	return rv;
}
