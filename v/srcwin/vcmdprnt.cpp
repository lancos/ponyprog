//===============================================================
// vcmdprnt.cxx - vCmdParent Class - Windows
//                                                
// Copyright (C) 1995,1996, 1997, 1998  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vcmdprnt.h>		// our include

#include <v/vbtncmd.h>		// ButtonCmd
#include <v/vcbtncmd.h>		// ColorButtonCmd
#include <v/vclabelc.h>		// Color label
#include <v/vlabelc.h>		// LabelCmd
#include <v/vframec.h>		// FrameCmd
#include <v/vchkboxc.h>		// CheckBoxCmd
#include <v/vradioc.h>		// RadioButtonCmd
#include <v/vtextinc.h>		// TextInCmd
#include <v/vtextc.h>		// TextCmd
#include <v/vlistc.h>		// ListCmd
#include <v/vcomboc.h>		// ComboBoxCmd
#include <v/vspinc.h>		// SpinnerCmd
#include <v/vsliderc.h>		// SliderCmd
#include <v/vprogrsc.h>		// ProgressCmd
#include <v/vboxlblc.h>		// BoxedLabel

#define NWORD DWORD

// --------------------------------------------------------------------- 
// Dynamic Dialog Building Support
// --------------------------------------------------------------------- 

#include <memory.h>

#define DIALOG_CHAR WORD

    typedef struct
      {
	short int PointSize;	// point size if DS_SETFONT
	// char  szTypeFace[];	// typeface name
      } FONTINFO, FAR *LPFONTINFO;

    const int _CtrlSpacing = 2;

//======================>>> vCmdParent::vCmdParent <<<===========================
  static void CopyStringToDialog (LPSTR *inp, LPCSTR src)
  {
    DIALOG_CHAR FAR *in = (DIALOG_CHAR FAR * )(*inp);

    do
      {
	*in++ = (DIALOG_CHAR) *src;
      } while (*src++);

    *inp = (char *)in;
  }

//======================>>> vCmdParent::vCmdParent <<<============================
  vCmdParent::vCmdParent(PaneType pt)
  {
    _cmdList = 0;
    maxX = 0;		// minimum size for a dialog
    maxY = 0;
    if (pt == P_Status)
      {
	maxX = 4;	// give some room at left for drawing box
        maxY = 4;
      }
    
    _callbackID = -1;	// no call back id

    _pt = pt;
  }

//======================>>> vCmdParent::~vCmdParent <<<============================
  vCmdParent::~vCmdParent()
  {
    SysDebug(Destructor,"vCmdParent::~vCmdParent() destructor\n")

    // need to delete everything we added

    DlgCmdList* tmp;

    if (_hTemplate != 0)
      {
	::GlobalFree(_hTemplate);
	_hTemplate = 0;
      }

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
        case C_ToggleButton:		// a toggle icon button
        case C_ToggleIconButton:	// a toggle icon button
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
	case C_ToggleFrame:		// A toggle frame
	  {
	    cmdPtr = new vFrameCmd(this, cmd);
	    break;
	  }

	case C_Icon:			// a display only Icon
	case C_Blank:			// to help RightOfs, Belows work
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

	case C_BoxedLabel:
          {
	    cmdPtr = new vBoxedLabelCmd(this, cmd);
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

//====================>>> vCmdParent::getParent <<<=======================
  HWND vCmdParent::getParent(void) VCONST
  {
    //	Return parent's HWND

    return _wDialog;
 }

//==================>>> vCmdParent::getThisFromId <<<=======================
  vCmd* vCmdParent::getThisFromId(ItemVal find_id)
  {
    // search the list of cmds for id, return its this

    DlgCmdList* cc;

    // search the list
    for (cc = _cmdList; cc != NULL ; cc = cc->nextDCL)
      {
	if (cc->cmdP == NULL || (cc->cmdP)->dlgCmd == NULL)
            return 0;
	if (((cc->cmdP)->dlgCmd)->cmdId == find_id)
	  {
	    return cc->cmdP;
	  }
      }

    return 0;				// failed to find it
  }

//====================>>> vCmdParent::SetPosition <<<=========================
  void vCmdParent::SetPosition(int& x, int& y, int w, int h, ItemVal frame,
    ItemVal rightOf, ItemVal below, int isSpinner)
  {
    // This method is responsible for controlling the layout of controls in
    // a dialog. All positions of controls are set relative to other controls,
    // which must have been previously defined (i.e., you start with the upper
    // left control and work to the lower right corner). It must dynamically
    // set the size of frames, and track the maximum x and y of the whole
    // dialog.

    int margin = _CtrlSpacing;	// spacing between controls
    vCmd* frameCmd = 0;

    if (!isSpinner)		// leave spinners alone
	x = y = margin;		// start out with standard spacing


    if (frame != 0)		// in a frame?
      {
	for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)
	  {
	    if ((cc->cmdP) == 0 || (cc->cmdP)->dlgCmd == 0) // not completely added yet!
		continue;
	    if (((cc->cmdP)->dlgCmd)->cmdId == frame)
	      {
		frameCmd = cc->cmdP;		// remember our frame
		if ((frameCmd->dlgCmd)->attrs & CA_NoSpace)
		    margin = 0;
		if (isSpinner)			// just finding frame
		    break;
		if (rightOf == 0)
		  {
		    x = (cc->cmdP)->_x + margin; // add to right of frame
		    if ((frameCmd->dlgCmd)->attrs & CA_NoSpace)
			++x;		// adjust for right side BEW 1.21
		  }
		else
		    x = margin;

		if (below == 0)
		  {
		    y = (cc->cmdP)->_y + margin;
		    if ((frameCmd->dlgCmd)->attrs & CA_NoSpace)
			++y;		// adjust for bottom  BEW 1.21
		  }
		else
		    y = margin;
	      }
	  }
      }

    if (isSpinner)
	x = x;
    else if (_dialogType == aCmdBar && frame == 0)
      {
	x = maxX + margin;
	y = margin;
      }
    else if (_dialogType == aStatBar && frame == 0)
      {
	x = maxX + margin;
	y = margin + 1;
      }
    else			// dialog/frame positioning
      {
	for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)
	  {
	    if ((cc->cmdP) == 0 || (cc->cmdP)->dlgCmd == 0) // not completely added yet!
		continue;

	    if (((cc->cmdP)->dlgCmd)->cmdId == rightOf)
	      {
		x += ((cc->cmdP)->_x) + ((cc->cmdP)->_w); // add to right of the right of
	      }
	    if (((cc->cmdP)->dlgCmd)->cmdId == below)
	      {
		y += ((cc->cmdP)->_y) + ((cc->cmdP)->_h); // add to below of the below of
	      }
	  }
      }

    int cxr = x + w;				// right x of control
    int cyb = y + h;				// bottom y of control

    if (maxX < cxr)				// track size of the dialog
	maxX = cxr;
    if (maxY < cyb)
	maxY = cyb;

    if (frameCmd)				// need to fixup frame
      {
	FixFrame(cxr, cyb, margin, frameCmd);		// fixup frames
      }
  }

//====================>>> vCmdParent::FixFrame <<<=======================
  void vCmdParent::FixFrame(int cxr, int cyb, int margin, vCmd* frameCmd)
  {

    int fxr = frameCmd->_x + frameCmd->_w;	// right x of frame
    int fyb = frameCmd->_y + frameCmd->_h;	// bottom y of frame

    if (fxr < cxr + margin)				// need to make frame wider
      {
	frameCmd->_w = cxr - frameCmd->_x + margin;
	if (margin == 0)
	    ++(frameCmd->_w);		// make nospace look right BEW 1.21
      }
    if (fyb < cyb + margin)				// need to make frame taller
      {
	frameCmd->_h = cyb - frameCmd->_y + margin;
	if (margin == 0)
	    ++(frameCmd->_h);		// make nospace look right BEW 1.21
      }

    // Now possibly fixup the dialog size
    if (maxX < frameCmd->_x + frameCmd->_w)
	maxX = frameCmd->_x + frameCmd->_w;
    if (maxY < frameCmd->_y + frameCmd->_h)
	maxY = frameCmd->_y + frameCmd->_h;

    // Now fixup the dialog template

    if (frameCmd->_CtrlOffset != 0)
      {
	WORD FAR *wNumBytes;
	LPDLGITEMTEMPLATE lpDlgItemTemplate;

	// wNumBytes points to beginning of memory block.
	wNumBytes = (WORD FAR *) ::GlobalLock(_hTemplate);

	// lpDlgItemTemplate points to start of new DLGITEMTEMPLATE.

	lpDlgItemTemplate = (LPDLGITEMTEMPLATE)
	    (((LPSTR) wNumBytes) + frameCmd->_CtrlOffset);

	// Fix the width
	lpDlgItemTemplate->cx = frameCmd->_w;
	lpDlgItemTemplate->cy = frameCmd->_h;
	::GlobalUnlock(_hTemplate);
      }

    // Now, fixup surrounding frames!
    int frame = (frameCmd->dlgCmd)->cFrame;
    if (frame != 0)	// Have to fix frame's frame
      {
	for (DlgCmdList* cc = _cmdList ; cc != 0  ; cc = cc->nextDCL)
	  {
	    if ((cc->cmdP) == 0 || (cc->cmdP)->dlgCmd == 0)	     	// not completely added yet!
		continue;
	    if (((cc->cmdP)->dlgCmd)->cmdId == frame)
	      {
		vCmd* fCmd = cc->cmdP;		// remember frame's frame
		fxr = frameCmd->_x + frameCmd->_w;	// right x of frame
		fyb = frameCmd->_y + frameCmd->_h;	// bottom y of frame
		FixFrame(fxr,fyb,_CtrlSpacing,fCmd);
		break;
	      }
	  }
      }
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
	    (cc->cmdP)->SetCmdStr(str);  // return the value
	    return;
	  }
      }

  }


//====================>>> vCmdParent::CreateDlgTemplate <<<==================
  int vCmdParent::CreateDlgTemplate(
    LONG dtStyle,		// dialog style
    int dtX, int dtY,		// location, In dialog-box units
    int dtCX, int dtCY,		// size, in dialog-box units
    LPSTR dtMenuName,		// "" if no menu
    LPSTR dtClassName,		// "" if standard dialog box class
    LPSTR dtCaption, 		// caption for dialog
    short int PointSize,	// Only used if DS_SETFONT specified
    LPSTR szTypeFace)		// Only used if DS_SETFONT specified
  {
    NWORD wBlockLen, FAR *wNumBytes;
    NWORD wMenuNameLen, wClassNameLen,
	wCaptionLen, wTypeFaceLen;
    LPSTR szDlgTemplate, szDlgTypeFace;
    LPDLGTEMPLATE lpDlgTemplate;
    LPFONTINFO lpFontInfo;

    // Calculate number of bytes required by following fields:
    wMenuNameLen     = (1 + lstrlen(dtMenuName)) * sizeof(DIALOG_CHAR);
    wClassNameLen    = (1 + lstrlen(dtClassName))* sizeof(DIALOG_CHAR);
    wCaptionLen  = (1 + lstrlen(dtCaption)) * sizeof(DIALOG_CHAR);

    // Block must be large enough to contain the following:
    wBlockLen =
	sizeof(NWORD) +		// Stores # of bytes used in block.
	sizeof(DLGTEMPLATE) +	// # bytes: fixed part DLGTEMPLATE.
	wMenuNameLen +		// # bytes: menu name.
	wClassNameLen +		// # bytes: dialog class name.
	wCaptionLen;		// # bytes: dialog box caption.

   /* These blocks have to be aligned on DWORD boundaries */
   wBlockLen = ((wBlockLen + 3) & ~3);

    if (dtStyle & DS_SETFONT)
      {
	// Dialog box uses font other that System font.

	// Calculate # of bytes required for typeface name.
	wTypeFaceLen = (1 + lstrlen(szTypeFace))* sizeof (DIALOG_CHAR);


	// Block must be large enough to include font information.
	wBlockLen +=
	    sizeof(short int) +	// # bytes for font's point size.
	    wTypeFaceLen;	// # bytes for font typeface name.

      }
    else
      {
	// Dialog box uses the System font.
	wTypeFaceLen = 0;

	// Block length does not change.
      }

    // Allocate global block of memory for Dialog template.
    _hTemplate = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, wBlockLen);
    if (_hTemplate == NULL)
	return 0;

    // wNumBytes points to beginning of memory block.
    wNumBytes = (NWORD FAR *) ::GlobalLock(_hTemplate);

    // Store in first two bytes the # of bytes used in the block.
    *wNumBytes = (NWORD) wBlockLen;

    // lpDlgTemplate points to start of DLGTEMPLATE in block.
    lpDlgTemplate = (LPDLGTEMPLATE) (wNumBytes + 1);

    // Set the members of the DLGTEMPLATE structure.
    lpDlgTemplate->style = dtStyle;
    lpDlgTemplate->dwExtendedStyle = 0;
    lpDlgTemplate->cdit = 0;  // Incremented with calls

    // to AddDlgControl.
    lpDlgTemplate->x = dtX;    lpDlgTemplate->y = dtY;
    lpDlgTemplate->cx = dtCX;    lpDlgTemplate->cy = dtCY;

    // szDlgTemplate points to start of variable, part of DLGTEMPLATE.
    szDlgTemplate = (LPSTR) (lpDlgTemplate + 1);

   CopyStringToDialog(&szDlgTemplate, dtMenuName) ;
   CopyStringToDialog(&szDlgTemplate, dtClassName);
   CopyStringToDialog(&szDlgTemplate, dtCaption);
    
    if (dtStyle & DS_SETFONT)
      {
	// Dialog box uses font other that System font.

	// lpFontInfo points to start of FONTINFO structure.
	lpFontInfo = (LPFONTINFO) szDlgTemplate;

	// Set the members of the FONTINFO structure.
	lpFontInfo->PointSize = PointSize;

	// szTypeFace points to start of variable part of FONTINFO.
	szDlgTypeFace = (LPSTR) (lpFontInfo + 1);

	// Append the typeface name to the block.
       CopyStringToDialog(&szDlgTypeFace, szTypeFace);
      }
#ifdef NEED_TO_HANDLE_NOFONT
    // Since font not specified, don't need to fill this
    // part in. Has a bug - overwrites memory anwway.
    else
      {
	// lpFontInfo points to start of FONTINFO structure.
	lpFontInfo = (LPFONTINFO) szDlgTemplate;

	// Set the members of the FONTINFO structure.
	lpFontInfo->PointSize = 0;

	// szTypeFace points to start of variable part of FONTINFO.
	szDlgTypeFace = (LPSTR) (lpFontInfo + 1);

	// Append the typeface name to the block.
       CopyStringToDialog(&szDlgTypeFace, "");
      }
#endif

    ::GlobalUnlock(_hTemplate);
    return 1;			// success
  }

//====================>>> vCmdParent::AddDlgControl <<<=====================
  int vCmdParent::AddDlgControl (
    int dtilX, int dtilY,	// In dialog-box units.
    int dtilCX, int dtilCY,	// In dialog-box units.
    int dtilID,       
    long dtilStyle,		// WS_CHILD is automatically added.
    LPSTR dtilClass,		// Class name
    LPCSTR dtilText,		// label
    BYTE dtilInfo,		// Number of additional data bytes.
    LPBYTE dtilData)	// Value passed through lpCreateParams of CREATESTRUCT.
  {

#define NEXTRA_SIZE WORD
    HGLOBAL hGlblNew;
	NWORD wBlockLen, wClassLen, wTextLen, FAR *wNumBytes;
	

    LPDLGTEMPLATE lpDlgTemplate;
    LPDLGITEMTEMPLATE lpDlgItemTemplate;
    LPSTR szDlgItemTemplate;
    int CtrlOffset = 0;		// Offset of this control

    // Calculate number of bytes required by following fields:
    wClassLen = (1 + lstrlen(dtilClass)) * sizeof (DIALOG_CHAR);
    wTextLen  = (1 + lstrlen(dtilText)) * sizeof (DIALOG_CHAR);

    // Block must be increased by to contain the following:
    wBlockLen =
	sizeof(DLGITEMTEMPLATE) + // # bytes for fixed part of DLGITEMTEMPLATE.
	wClassLen +		// # bytes for control class.
	wTextLen +		// # bytes for control text.
	sizeof(NEXTRA_SIZE) +	// # bytes of dtilInfo (below).
        dtilInfo;		// # bytes for extra data.

   /* These blocks have to be aligned on DWORD boundaries */
   wBlockLen = ((wBlockLen + 3) & ~3);
    // Guarantee that all controls have WS_CHILD style.
    dtilStyle |= WS_CHILD;

    // Get number of bytes currently in the memory block.
    wBlockLen += * (NWORD FAR *) ::GlobalLock(_hTemplate);

    ::GlobalUnlock(_hTemplate);

    // Increase size of memory block to include new dialog item.
    hGlblNew = ::GlobalReAlloc(_hTemplate, wBlockLen,
	GMEM_MOVEABLE | GMEM_ZEROINIT);

    if (hGlblNew == NULL)	// Fail?
	return 0;

    _hTemplate = hGlblNew;

    // wNumBytes points to beginning of memory block.
    wNumBytes = (NWORD FAR *) ::GlobalLock(_hTemplate);

    // lpDlgTemplate points to start of DLGTEMPLATE in block.
    lpDlgTemplate = (LPDLGTEMPLATE) (wNumBytes + 1);

    // Increment the number of controls in the template.
    lpDlgTemplate->cdit++;

    // lpDlgItemTemplate points to start of new DLGITEMTEMPLATE.
    // This is at the end of the memory block.
    lpDlgItemTemplate = (LPDLGITEMTEMPLATE) (((LPSTR) wNumBytes) + *wNumBytes);
    CtrlOffset = *wNumBytes;

   if (((int)lpDlgItemTemplate) &3)
     vSysError("Can't build dialog template.");

    // Set the members of the DLGITEMTEMPLATE structure.
    lpDlgItemTemplate->x = dtilX;
    lpDlgItemTemplate->y = dtilY;
    lpDlgItemTemplate->cx = dtilCX;
    // We have to handle comboboxes specially because the
    // height needed here is the combobox PLUS its list, while
    // the hieght we want to use internally is the height WITHOUT the list
    if (strcmp(dtilClass, "COMBOBOX") == 0)	// Combo?
	 lpDlgItemTemplate->cy = dtilCY + 8*6;
    else
	 lpDlgItemTemplate->cy = dtilCY;
    lpDlgItemTemplate->id = dtilID;
    lpDlgItemTemplate->style = dtilStyle;

    // szDlgTemplate points to start of variable part of DLGITEMTEMPLATE.
    szDlgItemTemplate = (LPSTR) (lpDlgItemTemplate + 1);

    // Append the control's class name, text to the block.

    CopyStringToDialog (&szDlgItemTemplate, dtilClass);
    CopyStringToDialog (&szDlgItemTemplate, dtilText);


    // Append the control's dtilInfo member.
    *(NEXTRA_SIZE FAR *)szDlgItemTemplate = dtilInfo;
    szDlgItemTemplate += sizeof(NEXTRA_SIZE);

    // Append the control's dtilData member.
    _fmemcpy(szDlgItemTemplate, dtilData, dtilInfo);
    szDlgItemTemplate += dtilInfo;


    // Store in the first two bytes the # of bytes used in block.
    *wNumBytes = wBlockLen;

    ::GlobalUnlock(_hTemplate);
    return CtrlOffset;
  }

//==================>>> vCmdParent::DoneAddingControls <<<===================
  void vCmdParent::DoneAddingControls(void)
  {
   NWORD FAR *wNumBytes;
    LPDLGTEMPLATE lpDlgTemplate;

    // wNumBytes points to beginning of memory block.
    wNumBytes = (NWORD FAR *) ::GlobalLock(_hTemplate);

    // lpDlgTemplate points to start of DLGTEMPLATE in block.
    lpDlgTemplate = (LPDLGTEMPLATE) (wNumBytes + 1);

    // Fixup size
    maxX = lpDlgTemplate->cx = maxX + _CtrlSpacing;
//    if (_dialogType == aCmdBar)
//	maxY = lpDlgTemplate->cy = maxY + 1;
//    else
	maxY = lpDlgTemplate->cy = maxY + _CtrlSpacing;

    // Move all of the bytes in the block down byte the NWORD.
   char FAR *dst = (char FAR *)wNumBytes;
   char FAR *src = (char FAR *)(wNumBytes + 1);
   int len = *wNumBytes - sizeof(NWORD);
   while (len)
     {
       *dst++ = *src++;
       len --;
     }
    ::GlobalUnlock(_hTemplate);

    // Once this function is executed, no more items can be 
    // added to the dialog box template.
  }

// --------------------------------------------------------------------- 
