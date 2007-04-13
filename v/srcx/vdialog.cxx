//===============================================================
// vdialog.cxx - vDialog class
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vxutil.h>	// Motif/Athena mapping
#include <v/vdialog.h>		// our header
#include <v/vapp.h>
#include <v/vctlclrs.h>

extern "C"
{
#include <X11/Shell.h>

#ifdef Athena

#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>

#else

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/DialogS.h>
#include <Xm/Frame.h>
#include <Xm/Protocols.h>

#endif
}

#ifndef Athena

#define formWidgetC xmFormWidgetClass
#define Ncallback XmNactivateCallback
#define Nheight XmNheight
#define Nlabel XmNlabelString
#define NmappedWhenManaged XmNmappedWhenManaged
#define Nwidth XmNwidth
#define NborderWidth XmNborderWidth

#define Ntitle XmNtitle
#define NallowShellResize XmNallowShellResize
#define XNx XmNx
#define XNy XmNy

#define setLabel(x,y) XmString x = XmStringCreateSimple(y);
#define freeLabel(x) XmStringFree(x);

#else	//------------------------------------------------------

#define formWidgetC formWidgetClass
#define NborderWidth XtNborderWidth
#define Ncallback XtNcallback
#define Nheight XtNheight
#define Nlabel XtNlabel
#define NmappedWhenManaged XtNmappedWhenManaged
#define Nwidth XtNwidth
#define setLabel(x,y) char* x = y;
#define freeLabel(x)

#define Ntitle XtNtitle
#define NallowShellResize XtNallowShellResize
#define XNx XtNx
#define XNy XtNy

#endif	// -----------------------------------------------------

// Define static data of the class

#ifdef Athena
#define GRAY_BACKGROUND
#else
#undef GRAY_BACKGROUND
#endif

#ifdef GRAY_BACKGROUND
#define gray_width 2
#define gray_height 2
static char gray_bits[] = {
   0x01, 0x02};
static Pixmap gray_bitmap = 0;
#endif

//========================>>> ignore_DW_CB  <<<=======================
  static void ignore_d_DW_CB(Widget, XtPointer client_data, XtPointer)
  {
     ;
  }

//===================>>> vDialog::vDialog <<<=======================
  vDialog::vDialog(vBaseWindow* creator, int modal, VCONST char* title) :
    vBaseWindow(title)		// constructor
  {
    _parentHandle = creator->vHandle();
    init(modal,title);
  }

//===================>>> vDialog::vDialog <<<=======================
  vDialog::vDialog(vApp* creator, int modal, VCONST char* title) :
    vBaseWindow(title)		// constructor
  {
    _parentHandle = creator->vHandle();
    init(modal,title);
  }

//=================>>> vDialog::~vDialog <<<=======================
  vDialog::~vDialog()
  {

    SysDebug(Destructor,"vDialog::~vDialog() destructor\n")

    _IsDisplayed = 0;		// The dialog is not up
    closeBaseWindow();		// close this window
  }

//===================>>> vDialog::vDialog <<<=======================
  vDialog::vDialog(const vDialog& d) : vBaseWindow(d), vCmdParent()
  {
    vSysError("vDialog - V semantics do not support copy constructors!");
  }

//===================>>> vDialog::vDialog <<<=======================
  void vDialog::init(int modal, VCONST char* title)
  {

    // Initialize dialog box

    _addedWDW = 0;

    _modal = modal;		// settable modal

    SysDebug(Constructor,"vDialog::vDialog() constructor\n")

    _wType = DIALOG;		// a dialog

    _IsDisplayed = 0;		// The dialog is not up

#ifdef GRAY_BACKGROUND
    // Create pixmap
    if (gray_bitmap == 0)		// haven't built the pixmap yet
      {
	gray_bitmap = XCreatePixmapFromBitmapData(
	    theApp->display(),				// our display
	    DefaultRootWindow(theApp->display()),	// a Drawable
	    gray_bits,		// the gray bitmap data
	    gray_width,		// from the file
	    gray_height,
	    (unsigned long) theApp->Xfg(),	// Forground color
	    (unsigned long) theApp->Xbg(),	// Background color
	    DefaultDepth(theApp->display(),DefaultScreen(theApp->display())));

      }
#endif

#ifdef Athena
   // create a transient shell for the dialog
    _vHandle = XtVaCreatePopupShell(_name,	// name supplied
	    transientShellWidgetClass,		// a transient shell
	    _parentHandle,
	    XtNtitle, _name,			// no label
	    XtNallowShellResize, TRUE,
	    NULL);

#else		// Motif

   // create a transient shell for the dialog
    _vHandle = XtVaCreateWidget(_name,	// name supplied
	    xmDialogShellWidgetClass,		// a transient shell
	    theApp->vHandle(),
	    XmNtitle, _name,			// no label
	    XmNallowShellResize, TRUE,
	    XmNtransientFor,_parentHandle,
	    XmNdeleteResponse,XmDO_NOTHING,	// added 1.24
	    NULL);
#endif

    // Now the dialog widget

#ifndef Athena			// Motif version
    if (_modal)
      {
	_wDialog = XtVaCreateWidget(
	    "vDialog",		// widget name  
	    xmFormWidgetClass,	// widget class 
	    _vHandle,		// based on our popup window
	    XmNverticalSpacing,3,
	    XmNhorizontalSpacing,3,
	    XmNnoResize,false,
	    XmNresizePolicy, XmRESIZE_ANY,
	    XmNshadowThickness,1,
	    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
	    XmNshadowType, XmSHADOW_ETCHED_IN,
	    NULL);			// terminate varargs list 
	_dialogFrame = _wDialog;
      }
    else
      {
	_dialogFrame = XtVaCreateWidget(
	    "vDialogFrame",		// widget name  
	    xmFrameWidgetClass,		// widget class 
	    _vHandle,		// based on our popup window
	    XmNmarginHeight,3,
	    XmNmarginWidth,3,
	    XmNshadowThickness,4,
	    XmNshadowType, XmSHADOW_OUT,
	    XmNresizePolicy, XmRESIZE_ANY,
	    NULL);			// terminate varargs list 

	_wDialog = XtVaCreateWidget(
	    "vDialog",		// widget name  
	    xmFormWidgetClass,	// widget class 
	    _dialogFrame,		// based on our popup window
	    XmNverticalSpacing,3,
	    XmNhorizontalSpacing,3,
	    XmNnoResize,True,
	    XmNdialogStyle, XmDIALOG_MODELESS,
	    XmNresizePolicy, XmRESIZE_ANY,
	    NULL);			// terminate varargs list 
      }
#else	// Athena

    _wDialog = XtVaCreateWidget(
	"vDialog",		// widget name  
	formWidgetC,		// widget class 
	_vHandle,		// based on our popup window
	XtNresizable,1,
	NULL);			// terminate varargs list 

#ifdef GRAY_BACKGROUND
    // set the background pattern
    if (theApp->Xdepth() <= 1)
	XtVaSetValues(_wDialog,		// the widget to set
	  XtNbackgroundPixmap, gray_bitmap, NULL);
    else
#endif
	XtVaSetValues(_wDialog,		// the widget to set
	  XtNbackground, _vDialogBG, NULL);

#endif	// Motif/Athena

    _DefaultButton = 0;	// No default button described
    _FirstTextIn = 0;	// No text in field

  }

//===================>>> vDialog::AddDialogCmds <<<========================
  void vDialog::AddDialogCmds(CommandObject* cList)	// add commands
  {
    // This is called to add commands from the supplied
    // CommandObject list.

    vCmd* defButton = 0;
    DlgCmdList* curCmd;
    int textIns = 0;		// how many text ins

    // scan the entire list

    for (int ix = 0 ; cList && (cList[ix].cmdType != C_EndOfList) ; ++ix)
      {
	curCmd = new DlgCmdList;		// get a new cell

	curCmd->nextDCL = _cmdList;		// add in at front
	_cmdList = curCmd;	

	curCmd->cmdP = AddCmd(&cList[ix]);

	// Track default button

	if (cList[ix].attrs & CA_DefaultButton)
	  {
	     defButton = curCmd->cmdP;
	  }

	if (cList[ix].cmdType == C_TextIn)
	  {
	    ++textIns;
 	    if (_FirstTextIn == 0)
		_FirstTextIn = curCmd->cmdP;
	  }
      }

    if (defButton)			// we have a default
      {
	_DefaultButton = defButton;	// Track the default button

	// This lets us use Enter to activate the default button.
	// The accelerators are defined elsewhere - this just
	// allows the accelerators for _wDialog to apply to
	// the default button as well.

#ifndef Athena
	// @@@ need to do something
#else
	XtInstallAccelerators(_wDialog, defButton->wCmd); // add accelerators
#endif
      }
#ifndef Athena
	// @@@ do something
#else // Athena
    if (textIns == 1 && _FirstTextIn != 0)	// we had a textin field
      {
	XtSetKeyboardFocus(_wDialog,_FirstTextIn->wCmd); // set focus to first text
      }
#endif
  }

#ifdef useAddDialogCmdObj
// move next line to .h if used
virtual void vDialog::AddDialogCmdObj(CommandObject* Cmd, vCmd* CmdInstance);
//===================>>> vDialog::AddDialogCmdObj <<<========================
  void vDialog::AddDialogCmdObj(CommandObject* Cmd, vCmd* CmdInstance)
  {
    // This is called to add an already instantiated command object to the
    // CommandObject list.

    vCmd* defButton = 0;

    // scan the entire list

    DlgCmdList* curCmd = new DlgCmdList;		// get a new cell

    curCmd->nextDCL = _cmdList;		// add in at front of list

    _cmdList = curCmd;	

    curCmd->cmdP = CmdInstance;		// point to the object

    // Track default button

    if (Cmd->attrs & CA_DefaultButton)
      {
	_DefaultButton = CmdInstance;	// This is the default button
	// This lets us use Enter to activate the default button.
	// The accelerators are defined elsewhere - this just
	// allows the accelerators for _wDialog to apply to
	// the default button as well.

#ifndef Athena
	// @@@ need to do something
#else
	XtInstallAccelerators(_wDialog, defButton->wCmd); // add accelerators
#endif
      }
  }
#endif

//====================>>> vDialog::CancelDialog <<<=======================
  void vDialog::CancelDialog(void)
  {
    // Cancel selected - reset all values to original values


    SysDebug(CmdEvents,"vDialog::CancelDialog()\n")

    for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)
      {
	(cc->cmdP)->ResetItemValue();
      }

    // And close the dialog

    CloseDialog();
  }

//====================>>> vDialog::CloseDialog <<<=======================
  void vDialog::CloseDialog(void)
  {

    SysDebug(Build,"vDialog::CloseDialog()\n")

    // This is UGLY, but so are combo boxes, and we gotta do it!
    for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)
      {
	if ((cc->cmdP)->dlgCmd->cmdType == C_ComboBox)
	    (cc->cmdP)->SetCmdVal(0,Checked);
      }

    XtPopdown(_vHandle);	// pop down
    _IsDisplayed = 0;		// The dialog is not up
  }

//====================>>> vDialog::DialogCommand <<<=======================
  void vDialog::DialogCommand(ItemVal id, ItemVal retval, CmdType ctype)
  {
    // After the user has selected a command from the dialog,
    // this routine is called with the value


    SysDebug2(CmdEvents,"vDialog::DialogCommand(id:%d, val:%d)\n",id, retval)

    switch (id)			// We will do some things depending on value
      {
	case M_Cancel:
	    CancelDialog();
	    break;

	case M_Done:		// default, close dialog
	case M_OK:
	  {
	    CloseDialog();
	    break;
	  }
      }
  }

//====================>>> vDialog::DialogDisplayed <<<=======================
  void vDialog::DialogDisplayed()
  {
  }

//==================>>> vDialog::ProcessCmd <<<=======================
  void vDialog::ProcessCmd(ItemVal id, ItemVal rv, CmdType ct)
  {
    // simple interface between command objects and the
    // inherited vCmdParent ProcessCmd to the CommandObject which does the work

    if (ct == C_TextIn)		// TextIn special
      {
	if (_DefaultButton == 0)	// There is NOTa default button
	    return;
	// change the textin return to the default button
	id = _DefaultButton->_cmdId;	// get the id of the default cmd
	rv = _DefaultButton->_retVal;
        ct = (_DefaultButton->dlgCmd)->cmdType;
      }

    DialogCommand(id, rv, ct);
  }

//====================>>> vDialog::SetDialogTitle <<<=======================
  void vDialog::SetDialogTitle(VCONST char* title)
  {
    XtVaSetValues(_vHandle, XtNtitle, title, NULL);
  }

//================>>> vDialog::GetDialogPosition <<<========================
  void vDialog::GetDialogPosition(int& left, int& top, int& width,
	 int& height) VCONST
  {
    Dimension w,h,b;
    Position l,t;

    XtVaGetValues(_vHandle, Nwidth, &w, Nheight, &h,
		  XNx, &l, XNy, &t, NborderWidth, &b, NULL);
    
    left = l; top = t; width = w+b+b; height = h+b+b;

  }

//====================>>> vDialog::SetDialogPosition <<<======================
  void vDialog::SetDialogPosition(int left, int top)
  {
    Position l = left;
    Position t = top;

    XtVaSetValues(_vHandle,XNx,l, XNy,t,NULL);
  }

//====================>>> vDialog::ShowDialog <<<=======================
  void vDialog::ShowDialog(VCONST char* msg)
  {
    //	Show the dialog with the default message

    //  We will control the position of the dialog box based on
    //	the parent's location. The user can move the dialog later.

    Position x, y;
    Dimension width, height;
    Dimension mywidth, myheight;


    SysDebug1(Build,"vDialog::ShowDialog(%s)\n",msg)

    // First, get the values from our parent

    if (IsDisplayed())
	return;				// make this a no-op

    XtVaGetValues(_parentHandle,	// parent widget 
	Nwidth, &width,		// get these values 
	Nheight, &height,
	NULL);
	
    //	Now, calculate the center of our parent using its width
    //	and height, translate them to root widow world coordinates

    XtTranslateCoords(_parentHandle,	// Widget 
	(Position) width/2,		// x 
	(Position) height/2,		// y 
	&x, &y);			// coords on root window 

    XtVaGetValues(_vHandle,	// parent widget 
	Nwidth, &mywidth,		// get these values 
	Nheight, &myheight,
	NULL);
	
    // x and y are the world coordinates of the center of our parent.
    // Adjust them so the dialog  will be centered over the parent,
    // but not above or left of the parent

    if (myheight == 0 || myheight > height)	// I'm taller than my parent
	y = y - (height/2) + 35;
    else
	y = y - (myheight/2);

    if (mywidth == 0 || mywidth > width)
	x = x - (width/2) + 35;
    else
	x = x - (mywidth/2);
	
    XtVaSetValues(_vHandle, XtNx, x,  XtNy, y, NULL);

    // set the message field 

    if (msg && *msg)
      {
	for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)
	  {
	    if (((cc->cmdP)->dlgCmd)->attrs & CA_MainMsg)
	      {
		SetString( ((cc->cmdP)->dlgCmd)->cmdId, msg); 
	      }
	  }
      }

    // Since the dialog isn't necessarily re-created each time it is
    // popped up, we need to loop through now to set the original values

    for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)
      {
	(cc->cmdP)->SaveItemValue();
      }

    // Finally, make sure form is managed

    if (!XtIsManaged(_wDialog))
	XtManageChild(_wDialog);

    if (!_modal)
      {
#ifndef Athena
	if (!XtIsManaged(_dialogFrame))
	  {
	    XtManageChild(_dialogFrame);
	    theApp->XWaitUntilMapped(_dialogFrame);
	  }
#endif
      }
    else
	theApp->XWaitUntilMapped(_wDialog);

#ifdef Athena
    // Now ready to popup the dialog
    if (_modal)
	XtPopup(_vHandle, XtGrabExclusive);
    else
	XtPopup(_vHandle, XtGrabNone);
    theApp->XWaitUntilMapped(_vHandle);
#endif

    // V 1.24 - actually, we have to handle close button for
    // dialogs because some win managers decorate them anyway

    if (!_addedWDW)
      {
	_addedWDW = 1;
#ifndef Athena
	XmAddWMProtocolCallback(_vHandle, theApp->vx_wm_delete_window, 
		ignore_d_DW_CB, 0);
#else
	Atom WDW = XInternAtom(XtDisplay(_vHandle), "WM_DELETE_WINDOW", False);
	XSetWMProtocols(XtDisplay(_vHandle), XtWindow(_vHandle),
                    &WDW, 1);
	 XtAddEventHandler(_vHandle, (EventMask)0L, 1,
	    (XtEventHandler)ignore_d_DW_CB, 0);
#endif
      }

#ifndef DONT_WARP_MOUSEXXX
    if (_DefaultButton != 0)		// There is a default button
      {
	Widget wDef = _DefaultButton->wCmd;	// get the def button widget

	XWarpPointer(XtDisplay(wDef),None, XtWindow(wDef),
				0,0,0,0, 10, 10);	// Warp to button
	XSync(XtDisplay(wDef),False);
      }
#endif
    _IsDisplayed = 1;			// The dialog is up

    theApp->CheckEvents();
    DialogDisplayed();
  }
// --------------------------------------------------------------------- 
