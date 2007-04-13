//===============================================================
// vkeys.h - Defintions for V keys
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VKEYS_H
#define VKEYS_H

#define vk_None		0x0	/* no key */

    // Masks for various shift modes

#define VKM_Shift 1
#define VKM_Ctrl 4
#define VKM_Alt 8

#define vk_BackSpace		0x08
#define vk_Tab			0x09
#define vk_Linefeed		0x0A
#define vk_Return		0x0D
#define vk_Pause		0x13
#define vk_Escape		0x1B
#define vk_Delete		0x7F

#define vk_BackTab		0xFF09

#define vk_Home			0xFF50
#define vk_Left			0xFF51
#define vk_Up			0xFF52
#define vk_Right		0xFF53
#define vk_Down			0xFF54
#define vk_Page_Up		0xFF55
#define vk_Page_Down		0xFF56
#define vk_End			0xFF57
#define vk_Insert		0xFF58

#define vk_Num_Lock		0xFF7F

#define vk_KP_Enter		0xFF0D
#define vk_KP_Home		0xFF95
#define vk_KP_Left		0xFF96
#define vk_KP_Up		0xFF97
#define vk_KP_Right		0xFF98
#define vk_KP_Down		0xFF99
#define vk_KP_Page_Up		0xFF9A
#define vk_KP_Page_Down		0xFF9B
#define vk_KP_End		0xFF9C
#define vk_KP_Insert		0xFF9E
#define vk_KP_Delete		0xFF7F
#define vk_KP_Equal		0xFF3D
#define vk_KP_Multiply		0xFF2A
#define vk_KP_Add		0xFF2B
#define vk_KP_Subtract		0xFF2D
#define vk_KP_Decimal		0xFF2E
#define vk_KP_Divide		0xFF2F

#define vk_KP_0			0xFF30
#define vk_KP_1			0xFF31
#define vk_KP_2			0xFF32
#define vk_KP_3			0xFF33
#define vk_KP_4			0xFF34
#define vk_KP_5			0xFF35
#define vk_KP_6			0xFF36
#define vk_KP_7			0xFF37
#define vk_KP_8			0xFF38
#define vk_KP_9			0xFF39


#define vk_F1			0xFFC1
#define vk_F2			0xFFC2
#define vk_F3			0xFFC3
#define vk_F4			0xFFC4
#define vk_F5			0xFFC5
#define vk_F6			0xFFC6
#define vk_F7			0xFFC7
#define vk_F8			0xFFC8
#define vk_F9			0xFFC9
#define vk_F10			0xFFCA
#define vk_F11			0xFFCB
#define vk_F12			0xFFCC

// The following are modifer keys pressed by themselves

#define vk_Shift		0xFFE1
#define vk_Control		0xFFE4
#define vk_Caps_Lock		0xFFE5
#define vk_Alt			0xFFE9

#define vk_IsModifier(x) (x>=vk_Shift && x<=vk_Alt)

#define vk_space               0x020
#define vk_asciitilde          0x07e
#endif
