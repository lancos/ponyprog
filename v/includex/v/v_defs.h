//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

#ifndef V_DEFS_H
#define V_DEFS_H

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                                                                         //
// VV         VV                                       VV         VV       //
//  VV       VV     V - A Portable C++ GUI Framework    VV       VV        //
//   VV     VV           designed and written by         VV     VV         //
//    VV   VV                                             VV   VV          //
//     VV VV              Bruce E. Wampler, Ph.D.          VV VV           //
//      VVV            e-mail: bruce@objectcentral.com      VVV            //
//       V                                                   V             //
//                                                                         //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                                                                         //
// v_defs.h - Standard V definitions -- X Version                          // 
//                                                                         //
// Copyright (C) 1995-2000   Bruce E. Wampler                              //
//                                                                         //
// This file is part of the V C++ GUI Framework.                           //
//                                                                         //
// This library is free software; you can redistribute it and/or           //
// modify it under the terms of the GNU Lesser General Public              //
// License as published by the Free Software Foundation; either            //
// version 2.1 of the License, or (at your option) any later version.      //
//                                                                         //
// This library is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
// Library General Public License for more details.                        //
//                                                                         //
// You should have received a copy of the GNU Lesser General Public        //
// License along with this library (see COPYING.LIB); if not, write to the //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
// The author, Bruce E. Wampler, hereby grants you permission              //
// to treat the executable of any application you develop that uses        //
// the V library as a "work that uses the library" as long as you include  //
// an acknowledgement of V, and a link to V at www.objectcentral.com       //
// somewhere in the program or its documentation.  You may use the V       //
// library with any application, free or commercial, as long as you        //
// follow the other terms of the LGPL, excluding Section 6.                //
//                                                                         //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
// VCONST can be defined as const, but that will require changes
// to apps. We will wait until V 2.0 for that to happen officially,
// but for now, this is available, at least on some classes.
// VMUTABLE is used in conjunction with VCONST.
#define VCONST
#define VMUTABLE
//#define VCONST const
//#define VMUTABLE mutable

#ifndef vDEBUG		// force DEBUG for now
#define vDEBUG
#endif

#define Motif		// Motif by default!

#ifdef Athena

#undef Motif		// undef Motif for Athena

#ifdef NO_V_3D
#undef USE3D
#else
#define USE3D		// by default for Athena
#endif

#endif

#ifdef vDEBUG		// We are using old C stdio lib for debug info
#include <stdio.h>
#endif

#define V_VersMajor 1
#define V_VersMinor 25 
#define V_Version "V 1.25 - 15 Sep 2000"
#define V_VersionX 1

// These should be defined as blanks for some C++ compilers

#define BEGIN_EXTERN_C extern "C" {
#define END_EXTERN_C }
#ifdef __GNUC__
  #if (__GNUC_MINOR__==7)
    #undef BEGIN_EXTERN_C
    #undef END_EXTERN_C
    #define BEGIN_EXTERN_C
    #define END_EXTERN_C
  #endif
#endif


BEGIN_EXTERN_C
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
END_EXTERN_C

    // system error functions
    extern void vSysWarning(VCONST char* msg);
    extern void vSysError(VCONST char* msg);

    // Define basic types

    typedef unsigned int vKey;		// Value for a Key
    typedef int ItemVal;		// Value for an Item (menu,button, etc)
//    typedef unsigned int CmdAttribute;	// Attributes of commands
    typedef unsigned long CmdAttribute;	// Attributes of commands (1.24)
    typedef int FontId;			// Id for a font

#define vRGB (v,x,y,z)  v.r = x ; v.g = y ; v.z = b;
#define vpRGB (v,x,y,z)  v->r = x ; v->g = y ; v->z = b;
const int maxFileNameSize = 256;

    typedef struct DebugMask
      {
	unsigned
	System : 1,			// System debug messages
	CmdEvents : 1,			// Show command events (buttons, etc.)
	MouseEvents : 1,		// Show mouse events
	WindowEvents : 1,		// Window events (resize, etc.)
	Build : 1,			// Define/Build window
	BadVals: 1,			// Error values
	Misc : 1,			// Misc stuff
	Text : 1,			// Text events
	Constructor : 1,		// Show constructors
	Destructor : 1,			// Show destructors
	User: 1,			// Debug user events
	UserApp1 : 1,			// Level 1 User App
	UserApp2 : 1,			// Level 2 User App
	UserApp3 : 1;
      } DebugMask;

    extern DebugMask DebugState;	// allow everyone to see DebugState

// Define some macros to make adding debugging stuff easy

#ifdef vDEBUG
#define UserDebug(u,v) if (DebugState.u && DebugState.User) fprintf(stderr,v);
#define UserDebug1(u,v,w) if (DebugState.u && DebugState.User) fprintf(stderr,v,w);
#define UserDebug2(u,v,w,x) if (DebugState.u && DebugState.User) fprintf(stderr,v,w,x);
#define UserDebug3(u,v,w,x,z) if (DebugState.u && DebugState.User) fprintf(stderr,v,w,x,z);

#define SysDebug(u,v) if (DebugState.u && DebugState.System) fprintf(stderr,v);
#define SysDebug1(u,v,w) if (DebugState.u && DebugState.System) fprintf(stderr,v,w);
#define SysDebug2(u,v,w,x) if (DebugState.u && DebugState.System) fprintf(stderr,v,w,x);
#define SysDebug3(u,v,w,x,z) if (DebugState.u && DebugState.System) fprintf(stderr,v,w,x,z);

#else

#define UserDebug(u,v) // Nothing
#define UserDebug1(u,v,w) // Nothing
#define UserDebug2(u,v,y,x) // Nothing
#define UserDebug3(u,v,y,x,z) // Nothing

#define SysDebug(u,v) // Nothing
#define SysDebug1(u,v,w) // Nothing
#define SysDebug2(u,v,w,x) // Nothing
#define SysDebug3(u,v,w,x,z) // Nothing
#endif


    enum WindowType { WINDOW, CMDWINDOW, DIALOG };  // Types of windows

    // How Items can be set
    enum ItemSetType {Checked, Sensitive, Value, Hidden, ChangeList,
		ChangeListPtr, Red, Green, Blue, ChangeColor, SetToFirst}; 

    enum CmdType	// types of dialog commands
      {
	C_EndOfList,			// Used to denote end of command list
	C_ArrowButton,			// An arrow button
	C_Blank,			// filler to help RightOfs, Belows work
	C_BoxedLabel,			// Boxed label
	C_Button,			// Button
	C_CheckBox,			// Checked Item
	C_ColorButton,			// A colored button
	C_ColorLabel,			// A colored label
	C_Custom,			// A custom control
	C_ComboBox,			// Popup combo list
	C_Frame,			// General purpose frame
	C_Icon,				// a display only Icon
	C_IconButton,			// a command button Icon
	C_Label,			// Regular text label
	C_List,				// List of items (scrollable)
	C_Menu,				// Menu item
	C_ProgressBar,			// Bar to show progress
	C_RadioButton,			// Radio button
	C_Slider,			// Slider to enter value
	C_Spinner,			// a spinner box
	C_TextEdit,			// Text edit box
	C_TextIn,			// Text input field
	C_Text,				// wrapping text out
        C_ToggleButton, 	      	// A toggle button
	C_ToggleFrame,			// A toggle frame 
	C_ToggleIconButton,		// a toggle command button Icon
	C_ZZZ				// make my life easy
      };

    enum PaneType	// Kinds of Panes
      {
	P_Canvas,			// Drawing Canvas
	P_TextCanvas,			// TextCanvas
	P_Menu,				// Menu
	P_Commands,			// Commands
	P_Icons,			// Icon Bar
	P_Buttons,			// Button Bar
	P_Status			// Status Bar
      };


    const CmdAttribute CA_None = 0;		// No special attributes
    const CmdAttribute CA_MainMsg = 0x01;	// Gets replacement message
    const CmdAttribute CA_DefaultButton = 0x02;	// Special Default Button

    const CmdAttribute CA_Large = 0x04;		// Command larger than normal
    const CmdAttribute CA_Small = 0x08;		// Command smaller than normal
    const CmdAttribute CA_Hidden = 0x10;	// Starts out hidden
    const CmdAttribute CA_Horizontal = 0x20;	// Horizontal orentation
    const CmdAttribute CA_Vertical = 0x40;	// Vertical orentation
    const CmdAttribute CA_NoNotify = 0x100;	// Don't notify on all events
    const CmdAttribute CA_NoLabel = 0x100;	// No label (ProgressCmd only)
    const CmdAttribute CA_Simple = 0x100;	// backward compatible
    const CmdAttribute CA_Password = 0x100;	// for text in control only
    const CmdAttribute CA_Text = 0x200;		// A Spin/Text command
    const CmdAttribute CA_TextInNotify = 0x200;	// Notify when TextIn changes
    const CmdAttribute CA_NoBorder = 0x400;	// No border (frames, status bar)
    const CmdAttribute CA_NoSpace = 0x800;	// No space between widgets
    const CmdAttribute CA_ListWidth = 0x800;	// Special for lists


    const CmdAttribute CA_PerControl = 0x1000;	// Specific to different controls
    const CmdAttribute CA_Percent = 0x1000;	// Show % for ProgressCmd
    const CmdAttribute CA_Size = 0x1000;        // size specified in size element

    // New attributes - require 32 bit, so CmdAttribute is now a long!
    const CmdAttribute CA_Flat = 0x10000;		// flat icon buttons

    // These definitions go here because the user app should not need
    // to know about menus and buttons - only the static data structures
    // needed to define those things - and so shouldn't have to include
    // the header file for each menu or command object

    // definitions for dialog box command objects

const ItemVal NoList = 0;	//  no attributes, items is null list;
const ItemVal NoFrame = 0;	// no frame used

    typedef struct CommandObject
      {
	CmdType cmdType;	// what kind of item is this
	ItemVal cmdId;		// unique id for the item
	ItemVal retVal;		// value returned when picked (might be < 0)
	VCONST char* title;		// string
	void* itemList;		// a list of stuff to use for the cmd
	CmdAttribute attrs;	// bit map of attributes
	unsigned
	  Sensitive : 1;	// if item is sensitive or not
	ItemVal cFrame;		// Frame used for an item
	ItemVal cRightOf;	// Item placed left of this id
	ItemVal cBelow;		// Item placed below this one
	int size;		// Used for sizes
	VCONST char* tip;		// tip string
      } CommandObject;

#define DialogCmd CommandObject	// for backward compatibility

    typedef struct vMenu
      {
	VCONST char* label;		// The label on the menu
	ItemVal menuId;		// A User assigned unique value
	unsigned
	 sensitive : 1,		// If item is sensitive or not
	 checked : 1;		// If item is checked or not
	VCONST char* keyLabel;		// Label for an accelerator key
	vKey accel;		// Value of accelerator key
	vMenu* SubMenu;		// Ptr to a submenu
	unsigned int kShift;	// Shift state of accel key
      } vMenu;

    typedef struct vStatus	// for status bars
      {
	VCONST char* label;		// label of button
	ItemVal statId;		// value
	CmdAttribute attrs;	// bit map of attributes
	unsigned
	 sensitive : 1;		// if button is sensitive or not
	int width;		// if need to specify width (0 => default)
      } vStatus;

    // symbols for defining menus

const int noKey = 0;
const int noKeyLbl = 0;
const int isSens = 1;
const int notSens = 0;
const int isChk = 1;
const int notChk = 0;
const int noSub = 0;
const int notUsed = 0;
const int noIcon = 0;

    // standard menu definitions
const ItemVal M_File = 32000;
const ItemVal M_Edit = 32001;
const ItemVal M_Search = 32002;
const ItemVal M_Help = 32003;
const ItemVal M_Window = 32004;
const ItemVal M_Options = 32005;
const ItemVal M_Tools = 32006;
const ItemVal M_Font = 32007;
const ItemVal M_View = 32008;
const ItemVal M_Format = 32009;
const ItemVal M_Insert = 32010;
const ItemVal M_Test = 32011;

const ItemVal M_Line = 32090;

const ItemVal M_New = 32100;
const ItemVal M_Open = 32101;
const ItemVal M_Close = 32102;
const ItemVal M_Save = 32103;
const ItemVal M_SaveAs = 32104;
const ItemVal M_Print = 32105;
const ItemVal M_PrintPreview = 32106;
const ItemVal M_About = 32107;
const ItemVal M_Exit = 32108;
const ItemVal M_CloseFile = 32109;	// V:1.13

const ItemVal M_Cut = 32110;
const ItemVal M_Copy = 32111;
const ItemVal M_Paste = 32112;
const ItemVal M_Delete = 32113;
const ItemVal M_Clear = 32114;

const ItemVal M_UnDo = 32120;
const ItemVal M_SetDebug = 32121;
const ItemVal M_SaveClose = 32122;
const ItemVal M_Abandon = 32123;

const ItemVal M_Find = 32130;
const ItemVal M_FindAgain = 32131;
const ItemVal M_Replace = 32132;
const ItemVal M_Preferences = 32135;
const ItemVal M_FontSelect = 32136;

const ItemVal M_User0 = 32140;
const ItemVal M_User1 = 32141;
const ItemVal M_User2 = 32142;
const ItemVal M_User3 = 32143;
const ItemVal M_User4 = 32144;
const ItemVal M_User5 = 32145;
const ItemVal M_User6 = 32146;
const ItemVal M_User7 = 32147;
const ItemVal M_User8 = 32148;
const ItemVal M_User9 = 32149;

const ItemVal M_Cancel = 32150;
const ItemVal M_Done = 32151;
const ItemVal M_OK = 32152;
const ItemVal M_Yes = 32153;
const ItemVal M_No = 32154;
const ItemVal M_All = 32155;
const ItemVal M_None = 32156;

const ItemVal M_TextInChange = 32158;
const ItemVal M_TextInLeaveFocus = 32159;

// Definitions for drawing

    typedef struct vPoint	// a point
      {
	short x, y;		// X version
      } vPoint;

    typedef struct vRect
      {
	short x,y,w,h;
      } vRect;

    typedef struct vLine
      {
	short x,y,xend,yend;
      } vLine;

// for brushes and pens

const int vSolid = 1;
const int vTransparent = 2;

// for brushes

const int vHorizontalHatch = 3;
const int vVerticleHatch = 4;
const int vVerticalHatch = 4;
const int vLeftDiagonalHatch = 5;
const int vRightDiagonalHatch = 6;
const int vCrossHatch = 7;
const int vDiagonalCrossHatch = 8;

// for pens
const int vDash = 9;
const int vDot = 10;
const int vDashDot = 11;

// for filling
const int vAlternate = 12;
const int vWinding = 13;
#endif
