//===============================================================
// vcomboc.h - ComboBox
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VCOMBOC_H
#define VCOMOBC_H

#include <v/vcmd.h>	// We are derived from vcmd

extern "C"		// wrap in "C"
{
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
}

    const int MAX_COMBO_LIST = 64;	// put 8 items in the list

    class vCmdParent;

    extern "C"
     {
//	void CComboAcceptCB(Widget w, XtPointer This, XtPointer lr);
	void CComboDownCB(Widget w, XtPointer This, XtPointer lr);
	void CComboBoxCmdCB(Widget w, XtPointer This, XtPointer lr);
      }

    class vComboBoxCmd : public vCmd
      {
//	friend void CComboAcceptCB(Widget w, XtPointer This, XtPointer lr);
	friend void CComboDownCB(Widget w, XtPointer This, XtPointer lr);
	friend void CComboBoxCmdCB(Widget w, XtPointer This, XtPointer lr);

      public:		//---------------------------------------- public

	vComboBoxCmd(vCmdParent* dp, CommandObject* dc);

	virtual ~vComboBoxCmd();

	virtual void initialize(void);

	virtual int GetCmdValue(ItemVal id) VCONST;
	virtual void SetCmdVal(ItemVal val, ItemSetType st);

      protected:	//--------------------------------------- protected


      private:		//--------------------------------------- private

	char* _workList[MAX_COMBO_LIST];	// we will allow upto MAX_LIST items

	char** _fullList;		// alias pointer to full list
	int _maxWidth;			// max width of any item
	int _numItems;			// number of items on list
	int _curSelection;		// index of current selected item

	int PoppedUp;
	Widget _mainButton;		// The main button of the combo
	Widget _downButton;		// Widget for the down button
	Widget _popup;
	Widget _popupList;
	Widget _arrowBox;


	void SetupComboList(void);

	void ComboBoxCmdCB(int index);	// callback for user action
//	void ComboAcceptCB(void);	// callback for user action
	void ComboDownCB(void);		// callback for user action

      };
#endif
