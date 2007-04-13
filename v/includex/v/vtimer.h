//===============================================================
// vtimer.h - a timer class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VTIMER_H
#define VTIMER_H

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
}

    extern "C"
      {
	void CtimerCB(XtPointer client_data, XtIntervalId* notUsed);
      }

    class vTimer
      {

	friend void CtimerCB(XtPointer client_data, XtIntervalId* notUsed);

      public:		//---------------------------------------- public

	vTimer();		// constructor
	virtual ~vTimer();	// destructor

	virtual int TimerSet(long interval);
	virtual void TimerStop(void);
	virtual void TimerTick(void);


      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private

	void tick(void);	// internal tick routine

	long _interval;	// approx. how long in milliseconds between ticks

	XtIntervalId _id;  // id used by X

      };

#endif
