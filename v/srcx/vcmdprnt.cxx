//===============================================================
// vcmdprnt.cxx - vCmdParent Class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vcmdprnt.h>		// our include

#include <v/vbtncmd.h>		// ButtonCmd
#include <v/vcbtncmd.h>		// ColorButtonCmd
#include <v/vclabelc.h>		// Color label
#include <v/vlabelc.h>		// LabelCmd
#include <v/vframec.h>		// FrameCmd
#include <v/vchkboxc.h>		// CheckBoxCmd
#include <v/vradioc.h>		// RadioButtonCmd
#include <v/vtextinc.h>		// TextInCmd
// V:1.14 #include <v/vtextedc.h>		// TextEdCmd
#include <v/vtextc.h>		// TextCmd
#include <v/vlistc.h>		// ListCmd
#include <v/vcomboc.h>		// ComboBoxCmd
#include <v/vspinc.h>		// SpinnerCmd
#include <v/vsliderc.h>		// SliderCmd
#include <v/vprogrsc.h>		// ProgressCmd
#include <v/vtipwin.h>		// for tip window


// Define the event handlers
static void Leave_event(Widget w, XtPointer client_data, XEvent * xevent, Boolean * continue_to_dispatch );
static void Enter_event(Widget w, XtPointer client_data, XEvent * xevent, Boolean * continue_to_dispatch );
 static void Timer_event(XtPointer client_data, XtIntervalId *id);

//======================>>> vCmdParent::vCmdParent <<<============================
  vCmdParent::vCmdParent()
  {
    _cmdList = 0;
  }

//======================>>> vCmdParent::~vCmdParent <<<============================
  vCmdParent::~vCmdParent()
  {
    SysDebug(Destructor,"vCmdParent::~vCmdParent() destructor\n")

    // need to delete everything we added

    DlgCmdList* tmp;

    for (DlgCmdList* cc = _cmdList ; cc != 0  ; )
      {
	delete cc->cmdP;	// delete the cmd object
	tmp = cc;		// don't forget where we were
	cc = cc->nextDCL;	// for next time
	delete tmp;		// delete the list cell
      }	
  }

//======================>>> vCmdParent::AddCmd <<<============================
  vCmd* vCmdParent::AddCmd(CommandObject* cmd)
  {
    // Private method to add a single command to a dialog object.

    SysDebug2(Build,"vCmdParent::AddCmd() - %s(id:%d)\n",cmd->title,cmd->cmdId)

    vCmd* cmdPtr;

    switch (cmd->cmdType)		// depends on type
      {
	case C_IconButton:		// a command button Icon
	case C_ToggleIconButton:	// a toggle command button Icon
	case C_ToggleButton:		// a toggle command button
	case C_Button:			// Button
	  {
	    cmdPtr = new vButtonCmd(this, cmd);
	    break;
	  }

	case C_ColorButton:		// ColorButton
	  {
	    cmdPtr = new vColorButtonCmd(this, cmd);
	    break;
	  }

	case C_ColorLabel:		// ColorButton
	  {
	    cmdPtr = new vColorLabelCmd(this, cmd);
	    break;
	  }

	case C_CheckBox:		// Checked Item
	  {
	    cmdPtr = new vCheckBoxCmd(this, cmd);
	    break;
	  }

	case C_ComboBox:		// Popup combo list
	  {
	    cmdPtr = new vComboBoxCmd(this, cmd);
	    break;
	  }

	case C_Frame:			// General purpose frame
	case C_ToggleFrame:		// Toggle frame
	  {
	    cmdPtr = new vFrameCmd(this, cmd);
	    break;
	  }

	case C_Icon:			// a display only Icon
	case C_Blank:			// to help RightOfs, Belows work
	case C_BoxedLabel:		// Boxed text label
	case C_Label:			// Regular text label
	  {
	    cmdPtr = new vLabelCmd(this, cmd);
	    break;
	  }

	case C_List:			// List of items (scrollable)
	  {
	    cmdPtr = new vListCmd(this, cmd);
	    break;
	  }

	case C_ProgressBar:		// Progress bar
	  {
	    cmdPtr = new vProgressCmd(this, cmd);
	    break;
	  }

	case C_RadioButton:		// radio button
	  {
	    cmdPtr = new vRadioButtonCmd(this, cmd);
	    break;
	  }

	case C_Slider:			// slider
	  {
	    cmdPtr = new vSliderCmd(this, cmd);
	    break;
	  }

	case C_Text:			// Text output field
	  {
	    cmdPtr = new vTextCmd(this, cmd);
	    break;
	  }

	case C_TextIn:			// Text input field
	  {
	    cmdPtr = new vTextInCmd(this, cmd);
	    break;
	  }

	case C_Spinner:		// Value Box list
	  {
	    cmdPtr = new vSpinnerCmd(this, cmd);
	    break;
	  }

	default:			// unknown!
	  {
 SysDebug2(BadVals,"vCmdParent::AddCmd(id:%d, title:%s) ** Unknown DialogCommand\n",cmd->cmdId,cmd->title);
	    cmd->title = "?? Bad CommandObject ??";
	    cmdPtr = new vLabelCmd(this, cmd);
	    break;
	  }
      }

    // Add popup tip here.

    if (cmd->tip && *cmd->tip)		// a tip has been supplied
      {
	Widget tw = cmdPtr->GetWidget();		// main box for tip
	if (tw != 0)			// a widget has been defined!
	  {
	    XtAddEventHandler(tw, EnterWindowMask, False, 
		Enter_event, (XtPointer) cmdPtr);
	    XtAddEventHandler(tw, LeaveWindowMask|ButtonPressMask, 
		False, Leave_event, (XtPointer) cmdPtr);
	  }
      }

    return cmdPtr;
  }

//====================>>> vCmdParent::HasId <<<=========================
  int vCmdParent::HasId(ItemVal id) VCONST
  {
    // We need this one for panes to work correctly

    for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)
      {
	if (((cc->cmdP)->dlgCmd)->cmdId == id)
	  {
	    return 1;
	  }
      }
    return 0;
  }

//====================>>> vCmdParent::GetValue <<<=========================
  int vCmdParent::GetValue(ItemVal id) VCONST
  {
    for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)
      {
	if (((cc->cmdP)->dlgCmd)->cmdId == id)
	  {
	    return (cc->cmdP)->GetCmdValue(id);  // return the value
	  }
      }
    return 0;
  }

//====================>>> vCmdParent::GetTextIn <<<=========================
  int vCmdParent::GetTextIn(ItemVal id, char* strout, int maxlen)
  {
    // recover the string from TextIn id, up to maxlen
    for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)
      {
	if (((cc->cmdP)->dlgCmd)->cmdId == id)
	  {
	    // need to cast to a vTextInCmd so we can use getstr
	    vTextInCmd* tip = (vTextInCmd*) cc->cmdP;

	    return tip->GetTextIn(id, strout, maxlen);  // return the value
	  }
      }

    *strout = 0;
    return 0;
  }

//====================>>> vCmdParent::SetFrameChildren <<<======================
  void vCmdParent::SetFrameChildren(ItemVal frameId, int frameVal)
  {
    // Set all children of given frame to hidden or not hidden
    // Do this recursively to be sure to get them all.

    // Scan list, setting all children that use this frame

    for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)  // Find the frame
      {
	CommandObject* cop = (cc->cmdP)->dlgCmd;
	if (cop->cFrame == frameId)			// A child of ours
	  {
            // Hide control if not turning on (frameVal is opposite of Hidden)
	    SetValue(cop->cmdId, !frameVal, Hidden);
	    if (cop->cmdType == C_Frame || cop->cmdType == C_ToggleFrame)
		SetFrameChildren(cop->cmdId, frameVal); // Get other chilren
	  }
      }
  }

//====================>>> vCmdParent::getParent <<<=======================
  Widget vCmdParent::getParent(ItemVal id) VCONST
  {
    //	If the object id has a frame specified, find it and return
    //	its widget.  Otherwist, the parent is the dialog.

    Widget tmp;

    if (id == NoFrame)
      {
	return _wDialog;
      }
    else
      {
	tmp = getFrameWidgetFromId(id);
	if (tmp == 0)
	  {
	    vSysWarning("Cannot find Frame parent.");
	    return _wDialog;
	  }
	else
	    return tmp;
      }
  }

//==================>>> vCmdParent::getWidgetFromId <<<=======================
  Widget vCmdParent::getWidgetFromId(ItemVal find_id) VCONST
  {
    // search the list of cmds for id, return its object

    DlgCmdList* cc;

    if (find_id == 0)			// no op
      {
	return 0;
      }

    // search the list
    for (cc = _cmdList->nextDCL ; cc != 0  ; cc = cc->nextDCL)
      {
	if (((cc->cmdP)->dlgCmd)->cmdId == find_id)
	  {
	    if ( (cc->cmdP)->wBox == 0)  // use the surround box if there
		return (cc->cmdP)->wCmd;
	    else
		return (cc->cmdP)->wBox;
	  }
      }
    
    return 0;				// failed to find it
  }

//==================>>> vCmdParent::getFrameWidgetFromId <<<=======================
  Widget vCmdParent::getFrameWidgetFromId(ItemVal find_id) VCONST
  {
    // search the list of cmds for id, return its object

    DlgCmdList* cc;

    if (find_id == 0)			// no op
	return 0;

    // search the list
    for (cc = _cmdList->nextDCL ; cc != 0  ; cc = cc->nextDCL)
      {
	if (((cc->cmdP)->dlgCmd)->cmdId == find_id)
	  {
#ifndef Athena
	    // This bit of code comes from the need to use the xmForm when
	    // placing children, but to use the xmFrame when placing the
	    // frame itself -- all because xmForm doesn't have a frame.
	    if (((cc->cmdP)->dlgCmd)->cmdType == C_Frame ||
		((cc->cmdP)->dlgCmd)->cmdType == C_ToggleFrame)	// need wCmd!
	      {
		if ( (cc->cmdP)->wCmd != 0)	// this should not happen, but...
		    return (cc->cmdP)->wCmd;
	      }
#endif
	    if ( (cc->cmdP)->wBox == 0)  // use the surround box if there
		return (cc->cmdP)->wCmd;
	    else
		return (cc->cmdP)->wBox;
	  }
      }
    
    return 0;
  }

//====================>>> vCmdParent::SetValue <<<=======================
  void vCmdParent::SetValue(ItemVal id, ItemVal val, ItemSetType setType)
  {

    SysDebug2(Misc,"vCmdParent::SetValue(id:%d, val:%d)\n",id,val)

    for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)
      {
	if (((cc->cmdP)->dlgCmd)->cmdId == id)
	  {
	    (cc->cmdP)->SetCmdVal(val,setType);  // return the value
	    return;
	  }
      }
  }

//====================>>> vCmdParent::SetString <<<=======================
  void vCmdParent::SetString(ItemVal id, VCONST char* str)
  {

    SysDebug2(Misc,"vCmdParent::SetString(id:%d, str:%s)\n",id,str)

    for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)
      {
	if (((cc->cmdP)->dlgCmd)->cmdId == id)
	  {
    SysDebug1(Misc,"vCmdParent::SetCmdStr( str:%s)\n",str)
	    (cc->cmdP)->SetCmdStr(str);  // return the value
	    return;
	  }
      }

  }

//################################ TIP CODE ###############################

    static vTipWindow tipWin;	// just need one tip window
    static long tipTime = 600;	// 600 ms time for tips

//====================>>> Timer_event <<<=======================
  static void Timer_event( XtPointer client_data, XtIntervalId *id)
  {
    vCmd* cmd = (vCmd*) client_data;

    Position abs_x, abs_y;
    Position w_height;	

    if (cmd->GetIntId() == 0)
	return;	// timeout was removed but callback happened anyway

    cmd->SetIntId(0);		// force to 0

    // now figure out where to put the popup

    Widget tw = cmd->GetWidget();		// main box for tip
    if (tw == 0)
	return;			// nowhere to go

    XtVaGetValues(tw, XtNheight, &w_height, NULL );

    /* position just below the widget */

    XtTranslateCoords(tw, 0, w_height, &abs_x, &abs_y);
    tipWin.ShowTipWindow(abs_x + 12, abs_y, cmd->GetDlgCmd()->tip);
}

//====================>>> Enter_event <<<=======================
  static void Enter_event(Widget w, XtPointer client_data, XEvent * xevent,
	 Boolean * continue_to_dispatch )
  {
    vCmd* cmd = (vCmd *) client_data;
    XEnterWindowEvent* event = & xevent->xcrossing;

    // Check for two enters in a row. Can happen when widget is
    // exposed under a pop-up

    if (tipTime <= 0)
	return;
    if (cmd->GetIntId() != 0) 
	return;
    if(event->mode != NotifyNormal)
	return;
    if (!cmd->GetDlgCmd()->tip)
	return;

    tipWin.CloseTipWindow();		// pop down previous!

    // Add a timer so the help pops up after a delay
    XtIntervalId id = XtAppAddTimeOut(
	XtWidgetToApplicationContext(w),
	tipTime, Timer_event, client_data);
    cmd->SetIntId(id);			// remember id
  }

/*
Remove timer, if its pending. Then popdown help.
*/
//====================>>> Leave_event <<<=======================
  static void Leave_event(Widget w, XtPointer client_data, XEvent * xevent, Boolean * continue_to_dispatch )
  {
    vCmd* cmd = (vCmd *) client_data;
    XEnterWindowEvent* event = & xevent->xcrossing;

    if (cmd->GetIntId() != 0)
      {
	XtRemoveTimeOut(cmd->GetIntId());
	cmd->SetIntId(0);		// force to 0
      }
    tipWin.CloseTipWindow();		// pop down previous!

  }
