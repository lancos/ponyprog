//===============================================================
// vTimer.cxx - vTimer class functions - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vapp.h>
#include <v/vtimer.h>

// Define static data of the class


//======================>>> vTimer::vTimer <<<=======================
  vTimer::vTimer( )		// default constructor
  {
    SysDebug(Constructor,"vTimer::vTimer - constructor\n")

    _id = 0;			// no id
    _interval = 0;		// no interval
  }

//======================>>> vTimer::~vTimer <<<=======================
  vTimer::~vTimer( )
  {
    SysDebug(Destructor,"vTimer::~vTimer - destructor\n")

    if (_id)			// remove timer if in effect
      {
	XtRemoveTimeOut(_id);
	_id = 0;
      }
  }

//======================>>> vTimer::TimerSet <<<=======================
  int vTimer::TimerSet(long interval)
  {

    if (_id)			// remove timer if in effect
      {
	XtRemoveTimeOut(_id);
	_id = 0;
      }

    _interval = interval;

    _id = XtAppAddTimeOut(
	theApp->appContext(),	// needs app context
	_interval,		// interval in ms
	CtimerCB,		// the callback
	(XtPointer)this);	// us

    return (_id == 0) ? 0 : 1;
  }

//======================>>> vTimer::TimerStop <<<=======================
  void vTimer::TimerStop(void)
  {
    if (_id)			// remove timer if in effect
      {
	XtRemoveTimeOut(_id);
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

    // X requires you to reinstall timer to get another tick
    if (_id != 0)		// TimerTick might call TimerStop
      {
	_id = XtAppAddTimeOut(
            theApp->appContext(),	// needs app context
	    _interval,		// interval in ms
	    CtimerCB,		// the callback
	    (XtPointer)this);	// us
      }

  }

extern "C"
{
//============================>>> CtimerCB <<<==========================
  void CtimerCB(XtPointer client_data, XtIntervalId* notUsed)
  { 
    //	timer tick callback
    //
    // client_data will have the this pointer of our object

    ((vTimer*)client_data)->tick();	// call our tick routine
  }
}
