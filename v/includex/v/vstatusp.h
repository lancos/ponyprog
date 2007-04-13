//===============================================================
// vstatusp.h - Status Bar class .h file - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VSTATUSP_H
#define VSTATUSP_H

#include <v/v_defs.h>

#include <v/vpane.h>		// we are derived from a pane

    class vWindow;		// we are part-of a vWindow

    typedef struct StatusList
      {
	vStatus* statusPtr;	// pointer to static status definition
	Widget w;		// widget for this status box
	StatusList* nextSL;	// next status
      } StatusList;

    class vStatusPane : public vPane
      {
      public:		//---------------------------------------- public
	vStatusPane(vStatus* s_bar);
	vStatusPane(const vStatusPane& b);	// Copy constructor
 	~vStatusPane();

	virtual void initialize(vWindow* pWin, Widget pWidget,
		Widget drawCanvas = (Widget)0, Widget drawArea = (Widget)0);
	virtual void ShowPane(int OnOrOff);

      protected:	//--------------------------------------- protected

	virtual void SetPaneValue(ItemVal id, int val, ItemSetType setType);
	virtual void SetPaneString(ItemVal id, char* str);


      private:		//--------------------------------------- private

	Widget _statusBar;		// Widget for Bar box
	Widget _Shadow;			// Motif only
	StatusList* _statusList;	// list of status boxes on this bar
      };
#endif
