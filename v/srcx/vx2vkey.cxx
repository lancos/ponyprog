//===============================================================
// vx2vkey.cxx - routines for converting V key codes
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/v_defs.h>
#include <v/vkeys.h>
#include <v/vx2vkey.h>
#include <X11/keysym.h>
#ifndef Athena
#include <Xm/VirtKeys.h>
#endif


#ifndef XK_KP_Home		// X11R5 vs. X11R6
#define XK_KP_Home XK_Home
#define XK_KP_Left XK_Left
#define XK_KP_Up XK_Up
#define XK_KP_Right XK_Right
#define XK_KP_Down XK_Down
#define XK_KP_Page_Up XK_Prior
#define XK_KP_Page_Down XK_Next
#define XK_KP_End XK_End
#define XK_KP_Insert XK_Insert
#define XK_KP_Delete XK_Delete
#endif

#ifndef XK_Mode_switch
#define XK_Mode_switch              0xFF7E
#endif

  static vKey XtoV[] =
  {
    XK_BackSpace, vk_BackSpace,  XK_Tab, vk_Tab,  XK_Linefeed, vk_Linefeed,
    XK_Return, vk_Return,  XK_Pause, vk_Pause,  XK_Escape, vk_Escape,
    XK_Delete, vk_Delete,  XK_Home, vk_Home,  XK_Left, vk_Left,
    XK_Insert, vk_Insert,
    XK_Up, vk_Up,  XK_Right, vk_Right,  XK_Down, vk_Down,  XK_Prior, vk_Page_Up,
    XK_Next, vk_Page_Down,  XK_End, vk_End,  XK_Num_Lock, vk_Num_Lock,
    XK_KP_Enter, vk_KP_Enter,  XK_KP_Home, vk_KP_Home,  XK_KP_Left, vk_KP_Left,
    XK_KP_Up, vk_KP_Up,  XK_KP_Right, vk_KP_Right,  XK_KP_Down, vk_KP_Down,
    XK_KP_Page_Up, vk_KP_Page_Up,  XK_KP_Page_Down, vk_KP_Page_Down,
    XK_KP_End, vk_KP_End,  XK_KP_Insert, vk_Insert,  XK_KP_Delete, vk_KP_Delete,
    XK_KP_Equal, vk_KP_Equal,  XK_KP_Multiply, vk_KP_Multiply,
    XK_KP_Add, vk_KP_Add,  XK_KP_Subtract, vk_KP_Subtract,
    XK_KP_Decimal, vk_KP_Decimal,  XK_KP_Divide, vk_KP_Divide,
    XK_KP_0, vk_KP_0,  XK_KP_1, vk_KP_1,  XK_KP_2, vk_KP_2,  XK_KP_3, vk_KP_3,
    XK_KP_4, vk_KP_4,  XK_KP_5, vk_KP_5,  XK_KP_6, vk_KP_6,  XK_KP_7, vk_KP_7,
    XK_KP_8, vk_KP_8,  XK_KP_9, vk_KP_9,  XK_F1, vk_F1,  XK_F2, vk_F2,
    XK_F3, vk_F3,  XK_F4, vk_F4,  XK_F5, vk_F5,  XK_F6, vk_F6,  XK_F7, vk_F7,
    XK_F8, vk_F8,  XK_F9, vk_F9,  XK_F10, vk_F10,  XK_F11, vk_F11,  XK_F12, vk_F12,

    // The following might only apply to Linux, but there probably won't
    // be many using F12-F20 anyway. Linux maps Shift-F1 to F13.
  
    XK_F13, vk_F1,  XK_F14, vk_F2,  XK_F15, vk_F3,  XK_F16, vk_F4,  XK_F17, vk_F5,
    XK_F18, vk_F6,  XK_F19, vk_F7,  XK_F20, vk_F8,  
   
    // The following are modifer keys pressed by themselves
  
    XK_Shift_L, vk_Shift,  XK_Shift_R, vk_Shift,  XK_Control_L, vk_Control,
    XK_Control_R, vk_Control,  XK_Caps_Lock, vk_Caps_Lock,
    XK_Alt_L, vk_Alt,  XK_Alt_R, vk_Alt, XK_Mode_switch, vk_Alt, 
#ifndef Athena
    osfXK_BackSpace, vk_BackSpace,
    osfXK_BeginLine, vk_Home,
    osfXK_Cancel, vk_Escape,
    osfXK_Clear, vk_Delete,
    osfXK_Delete, vk_Delete,
    osfXK_Down, vk_Down,
    osfXK_EndLine, vk_End,
    osfXK_Help, vk_F1,
    osfXK_Insert, vk_Insert,
    osfXK_Left, vk_Left,
    osfXK_PageDown, vk_Page_Down,
    osfXK_PageUp, vk_Page_Up,
    osfXK_Right, vk_Right,
    osfXK_Up, vk_Up,
#endif
    0, 0
  };


//===============================>>> MapXtoVkey <<<================================
  vKey MapXtoVkey(unsigned int Xkey, unsigned int& shift)
  {
    // Map an X Window System keyvalue to the internal code used by V

    unsigned int tmp;
    vKey vtmp;

    if (Xkey > 0 && Xkey <= 255)	// Normal & ISO ascii character
      {
	if (shift == 0)			// no ctrl or alt or shift, so return
	    return Xkey;

	if (shift == VKM_Shift)		// only shifted - kill the flag
	  {
	    shift = 0;			// kill the shift flag
	    if (Xkey == XK_Tab)
		return vk_BackTab;	// special case
	    return Xkey;		// return actual value
	  }
	else if (shift == VKM_Ctrl || shift == (VKM_Ctrl | VKM_Shift))
	// control-X or ctrl-shift-X
	  {
	    if (Xkey >= '@' && Xkey <= '_')
	      {
		shift &= VKM_Shift;		// kill the ctrl flag
		vtmp = Xkey - '@';	// normal range
	      }
	    else if (Xkey >= '`' && Xkey <= '~')
	      {
		shift &= VKM_Shift;		// kill the ctrl flag
		vtmp = Xkey - '`';	// normal range
	      }
	    else
		vtmp = Xkey;		// just let it map;

	    return vtmp;
	  }
	// else it is combination of Alt, Shift, cTRL
	return Xkey;
      }

    // To here, then need to map
    for (int ix = 0 ; XtoV[ix] != 0 ; ix += 2 )
      {
	// Search the XtoV table
	if (XtoV[ix] == Xkey)
	  {
	    return XtoV[ix+1];
	  }
      }
    return vk_None;
  }
