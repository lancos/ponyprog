//===============================================================
// vsliderc.cxx	- SliderCmd
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vxutil.h>	// Motif/Athena mapping
#include <v/vsliderc.h>	// our definitions
#include <v/vcmdprnt.h>	// a command parent
#include <v/vapp.h>
#include <v/vctlclrs.h>

extern "C"
{
#include <X11/Xlib.h>
#include <X11/StringDefs.h>

#ifndef Athena
#include <Xm/Xm.h>
#include <Xm/Scale.h>
#else
#include <X11/Xaw/Form.h>
#ifndef DONTUSEx3D
#include <v/awsld3d.h>
#else
#include <X11/Xaw/Scrollbar.h>
#endif
#endif
}

// This bitmap makes the slider a solid bar instead of a half-tone gray
#define slider_width 2
#define slider_height 2
static char slider_bits[] = {
   0x03, 0x03};

static Pixmap slider_bitmap = 0;

//=================>>> vSliderCmd::vSliderCmd <<<=======================
  vSliderCmd::vSliderCmd(vCmdParent* dp, CommandObject* dc) :
	    vCmd(dp, dc)
  {
    initialize();			// and initialize
  }

//=======================>>> vSliderCmd::~vSliderCmd <<<=======================
  vSliderCmd::~vSliderCmd()
  {

    SysDebug(Constructor,"vSliderCmd::~vSliderCmd() Destructor\n")

  }

//=====================>>> vSliderCmd::initialize <<<=======================
  void vSliderCmd::initialize()
  {

    CopyToLocal();		// make local copies

#ifdef Athena
    static char scrollTranslations[] =	// make a slider, not a scroll bar
	"<BtnDown>:StartScroll(Continuous) MoveThumb() NotifyThumb()\n\
        <BtnMotion>:MoveThumb() NotifyThumb()\n\
	<BtnUp>:NotifyScroll(Proportional) EndScroll()";

    XtCallbackRec scrollProcSlider[] =
      {
	{ CSliderScrollProcCB, this },
	{ (XtCallbackProc) 0, (XtPointer) 0}
      };

    XtCallbackRec jumpProcSlider[] =
      {
	{ CSliderJumpProcCB, this },
	{ (XtCallbackProc) 0, (XtPointer) 0}
      };
#endif

    SysDebug(Constructor,"vSliderCmd::vSliderCmd() constructor\n")

    // Create bitmaps if need to

    if (slider_bitmap == 0)
      {
	slider_bitmap = XCreateBitmapFromData(
	    theApp->display(),	// our display
	    DefaultRootWindow(theApp->display()),  // a Drawable
	    slider_bits,			// the bitmap data
	    slider_width,		// from the file
	    slider_height);
      }

    Widget WfHoriz = _parentWin->getWidgetFromId(dlgCmd->cRightOf);
    Widget WfVert = _parentWin->getWidgetFromId(dlgCmd->cBelow);

#ifndef Athena
    unsigned char orient = (dlgCmd->attrs & CA_Vertical) ?
			    XmVERTICAL : XmHORIZONTAL;
#else
    XtOrientation orient = (dlgCmd->attrs & CA_Vertical)
				? XtorientVertical : XtorientHorizontal;
#endif

    Dimension SliderHeight = (dlgCmd->attrs & CA_Large) ? 200 : 110;
    if (dlgCmd->attrs & CA_Small)
        SliderHeight = 60;
    if (dlgCmd->size > 0 && dlgCmd->size < 2048)
	SliderHeight = dlgCmd->size;

    int* minMax = (int *)_itemList;	// giving range?
    _minVal = 0; _maxVal = 100;			// default min/max
    if (minMax != 0)				// They gave a range list
      {
	_minVal = minMax[0];
	_maxVal = minMax[1];
      }
    
    if (_minVal > _maxVal)
      {
	SysDebug2(BadVals,"vSliderCmd:vSliderCmd - bad range %d to %d\n",_minVal,_maxVal)
	_minVal = 0; _maxVal = 100;	// make some sense!
      }

    _ScrlShown = 5;

    if (_retVal < _minVal)	// set a legal value for the top
	_curVal = _minVal;
    else if (_retVal > _maxVal)
	_curVal = _maxVal;
    else
	_curVal = _retVal;

    int map = !(dlgCmd->attrs & CA_Hidden);

#ifndef Athena
    int sh = 22;
    int sw = SliderHeight;
    if (dlgCmd->attrs & CA_Vertical)		// could have changed
      {
	sw = 22;
	sh = SliderHeight;
      }

    wCmd = XtVaCreateManagedWidget(
	"vSlider",		// name
	xmScaleWidgetClass,	// class
	wParent,			// parent
	XmNorientation, orient,
	XmNmaximum,_maxVal,
	XmNminimum,_minVal,
	XmNscaleHeight, sh,	// how wide
	XmNscaleWidth, sw,	// how wide
	XmNvalue,_curVal,
	XmNshowValue,0,
	XmNmappedWhenManaged, map,
	NULL);

	if (WfVert != 0)
	    XtVaSetValues(wCmd, Nbelow(WfVert) NULL);
	if (WfHoriz != 0)
	    XtVaSetValues(wCmd, NrightOf(WfHoriz) NULL);

	XtAddCallback(wCmd, XmNvalueChangedCallback, CSliderJumpProcCB, (XtPointer)this);
	XtAddCallback(wCmd, XmNdragCallback, CSliderJumpProcCB, (XtPointer)this);

#else		// Athena

    wCmd = XtVaCreateManagedWidget(
	"vSlider",		// name
#ifndef DONTUSEx3D
	slider3dWidgetClass,	// class
	wParent,			// parent
	XtNtranslations, XtParseTranslationTable(scrollTranslations), // new translations
#else
	scrollbarWidgetClass,	// class
	wParent,			// parent
#endif
	XtNorientation, orient,
	XtNscrollProc, scrollProcSlider,	// callback for scrolling
	XtNjumpProc, jumpProcSlider,	// callback for scrolling
	XtNminimumThumb,10,
	XtNlength,SliderHeight,	// high as Slider
	XtNresizable, TRUE,
	XtNmappedWhenManaged, map,
#ifndef DONTUSEx3D
	XtNbackground, _vControlFace,
	XtNthickness, 20,	// how wide
	XtNslider3dBackground, _vControlBG,
#else
	XtNbackground, _vControlBG,
	XtNthumb,slider_bitmap,
	XtNthickness, 15,	// how wide
#endif
	NULL);
   if (WfVert != 0)
       XtVaSetValues(wCmd, Nbelow(WfVert) NULL);
   if (WfHoriz != 0)
       XtVaSetValues(wCmd, NrightOf(WfHoriz) NULL);

#endif


    // Need to set to insensitive if it is set insensitive
    if (!_Sensitive)
      {
	XtVaSetValues(wCmd,		// the widget to set
#ifndef Athena
	    XmNsensitive, 0,		// change to insensitive
#else
	    XtNsensitive, 0,		// change to insensitive
#endif
	    NULL);
      }

    SetCmdVal(_curVal,Value);

    wBox = 0;				// Not using box
  }

//==================>>> vSliderCmd::GetCmdValue <<<=========================
  int vSliderCmd::GetCmdValue(ItemVal id) VCONST
  {
    if (id != _cmdId)
	return -1;
    return _curVal;
  }

//=====================>>> vSliderCmd::mapVal <<<=========================
  int vSliderCmd::mapVal(int val)
  {
#ifndef Athena
    return val;
#else
    float range = (float)(_maxVal - _minVal);
#ifndef DONTUSEx3D
    float fval = (float)(val - _minVal) * (95. / range);
#else
    float fval = (float)(val - _minVal) * (100. / range);
#endif
    return (int) fval;
#endif
  }

//=====================>>> vSliderCmd::SetCmdVal <<<=========================
  void vSliderCmd::SetCmdVal(ItemVal val, ItemSetType st)
  {

    SysDebug2(Misc,"vSliderCmd::SetCmdVal(val:%d, type:%d)\n",val,st)

    if (st == Value)
      {
	if (val < _minVal || val > _maxVal )
	    return;
	_curVal = val;
	// Now set appropriate _curVal, scroll, and highlight
	_ScrlTop = mapVal(_curVal);
	SetScroll(_ScrlShown, _ScrlTop);
      }
    else if (st == Sensitive)
      {
	_Sensitive = val;
	XtVaSetValues(wCmd,		// the widget to set
#ifndef Athena
	    XmNsensitive, val,		// change to val
#else
	    XtNsensitive, val,		// change to val
#endif
	    NULL);
      }
    else if (st == Hidden)		// hide or unhide
      {
	if (val)
	  {
	    XtUnmapWidget(wCmd);	// unmap this widget
	  }
	else
	  {
	    XtMapWidget(wCmd);	// unmap this widget
	  }
      }
  }

//====================>>> vSliderCmd::JumpProcCB <<<=======================
  void vSliderCmd::JumpProcCB(float percent, int mVal)
  {
    int intPercent = (int) (percent * 100.);
    int retVal;
    float newVal;

#ifndef Athena
    retVal = mVal;
#else

    int shown = _ScrlShown;
    int min_top;

    min_top = 100 - shown;

    if (intPercent > min_top)	// have to fix it
      {
	SetScroll(shown, min_top);
      }
    
    // Adjust for shown part

    newVal = percent;

    if (newVal < 0.)			// don't allow illegal values
	newVal = 0.;
    else if (newVal > 1.00)
	newVal = 1.00;

    // scale to provided range

    float range = (float)(_maxVal - _minVal);

    // The Xaw 3d version will range from 0 to min_top, so we
    // have to map that.

#ifndef DONTUSEx3D
    newVal = ((int)((newVal* 100.) * (range / (float)min_top))) + _minVal;
#else
    newVal = ((int)(newVal * range)) + _minVal;
#endif

    retVal = (int)newVal;

#endif

    if (retVal != _curVal)		// only notify if changed
      {
	_curVal = retVal;
	_parentWin->ProcessCmd(_cmdId, _curVal, dlgCmd->cmdType);
      }
  }

//====================>>> vSliderCmd::SetScroll <<<=======================
  void vSliderCmd::SetScroll(int Shown, int Top)
  {
#ifndef Athena
    XtVaSetValues(wCmd, XmNvalue, Top, NULL);		// change it!
#else
    Arg args[3];		// Used to pass float value

    int shown = Shown;
    int top = Top;		// work with local copies

    if (top > (100 - shown))
	top = 100 - shown;

    union
      {
	XtArgVal arg_value;
	float float_value;
      } shown_value, top_value;

    shown_value.float_value = float (shown / 100.0);
    top_value.float_value = float (top / 100.0);

    if (sizeof (float) > sizeof (XtArgVal))
      {
	XtSetArg(args[0], XtNtopOfThumb, &top_value);
	XtSetArg(args[1], XtNshown, &shown_value);
      }
    else
      {
	XtSetArg(args[0], XtNtopOfThumb, top_value.arg_value);
	XtSetArg(args[1], XtNshown, shown_value.arg_value);
      }

    XtSetValues(wCmd, args, 2);		// change it!
#endif
  }

//====================>>> vSliderCmd::ScrollProcCB <<<=======================
  void vSliderCmd::ScrollProcCB(int position)
  {
    // No op
  }

extern "C"
{

//============================>>> CJumpProcCB <<<=============================
  void CSliderJumpProcCB(Widget w, XtPointer This, XtPointer pc_ptr)
  {
#ifndef Athena
    XmScaleCallbackStruct* scale = (XmScaleCallbackStruct*)pc_ptr;

    ((vSliderCmd*)This)->JumpProcCB(0.0,scale->value);
#else
    float percent = *(float*)pc_ptr;	// get the percent back
    ((vSliderCmd*)This)->JumpProcCB(percent,0);
#endif
  }

//============================>>> CScollProcCB <<<=============================
  void CSliderScrollProcCB(Widget w, XtPointer This, XtPointer position)
  {
    int pos = (int)position;
    ((vSliderCmd*)This)->ScrollProcCB(pos);
  }
}
