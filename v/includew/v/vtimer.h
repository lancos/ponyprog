//===============================================================
// vtimer.h - a timer class - Windows
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
#include <windows.h>

    class V_EXPORT vTimer
      {
	friend void CALLBACK vTimerProc(HWND hwnd, UINT msg, UINT idTimer, DWORD dwTime);
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
	HWND _id;

    };

#endif
