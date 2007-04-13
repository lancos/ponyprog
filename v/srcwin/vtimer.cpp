//===============================================================
// vTimer.cxx - vTimer class functions - Windows
//
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vapp.h>
#include <v/vtimer.h>
#include <v/vthislst.h>

// Define static data of the class

  void CALLBACK vTimerProc(HWND /*hwnd*/, UINT msg, UINT idTimer, DWORD /*dwTime*/);

  static vThisList _timers;

//=========================>>> vTimer::vTimerProc <<<=======================
  void CALLBACK vTimerProc(HWND hwnd, UINT msg, UINT idTimer, DWORD dwTime)
  {
    if (msg != WM_TIMER)
	return;			// ignore...

    vTimer* mytime = (vTimer*) _timers.GetThis((ThisId)idTimer);
    if (mytime)			// Make sure we got a this
	mytime->tick();
  }

//======================>>> vTimer::vTimer <<<=======================
  vTimer::vTimer( )		// default constructor
  {
    SysDebug(Constructor,"vTimer::vTimer - constructor\n");

    _id = 0;			// no id
    _interval = 0;		// no interval
  }

//======================>>> vTimer::~vTimer <<<=======================
  vTimer::~vTimer( )
  {
    SysDebug(Destructor,"vTimer::~vTimer - destructor\n");

    if (_id)			// remove timer if in effect
      {
	TimerStop();
      }
  }

//======================>>> vTimer::TimerSet <<<=======================
  int vTimer::TimerSet(long interval)
  {

    if (_id)			// remove timer if in effect
      {
	TimerStop();
      }

    _interval = interval;

    _id = (HWND) ::SetTimer(0,0, interval, (TIMERPROC)vTimerProc);

    if (_id != 0)
      {
	_timers.Add((ThisId)_id, (void*)this);
	return 1;
      }
    else
	return 0;

  }

//======================>>> vTimer::TimerStop <<<=======================
  void vTimer::TimerStop(void)
  {
    if (_id)			// remove timer if in effect
      {
	::KillTimer(_id, 0);
	_timers.Delete((ThisId)_id);	// delete old timer
	_id = 0;
      }
  }

//======================>>> vTimer::TimerTick <<<=======================
  void vTimer::TimerTick(void)
  {
    // default has no action - will be overridden
  }

//=========================>>> vTimer::tick <<<=======================
  void vTimer::tick(void)
  {
    // internal tick routine

    TimerTick();		// call the work routine
  }

