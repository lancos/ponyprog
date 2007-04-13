//===============================================================
// vwin32.h - Special defs for Win32 - Windows version
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VWIN32_H
#define VWIN32_H

#ifdef __WATCOMC__
#ifdef __I86__
#define NEED_VXCRACKERS
#endif
#endif

#ifndef GET_WM_COMMAND_ID
#define NEED_VXCRACKERS
#endif

#ifndef VWIN16
#define MoveTo(a,b,c) MoveToEx(a,b,c,0)
#define SetWindowExt(a,b,c) SetWindowExtEx(a,b,c,0)
#define SetViewportExt(a,b,c) SetViewportExtEx(a,b,c,0)
#else
#ifdef NEED_VXCRACKERS
// For some reason, Watcom doesn't define these in windowsx.h
// on the 16 bit version - they are there for 32 bit.
// Neither does VCC 1.52!
#define GET_WM_COMMAND_ID(wp, lp)   (wp)
#define GET_WM_HSCROLL_CODE(wp, lp) (wp)
#define GET_WM_HSCROLL_POS(wp, lp)  LOWORD(lp)
#define GET_WM_HSCROLL_HWND(wp, lp) (HWND)HIWORD(lp)
#define GET_WM_VSCROLL_CODE(wp, lp) (wp)
#define GET_WM_VSCROLL_POS(wp, lp)  LOWORD(lp)
#define GET_WM_VSCROLL_HWND(wp, lp) (HWND)HIWORD(lp)
#define GET_WM_COMMAND_CMD(wp, lp)  HIWORD(lp)
#endif
#endif
#endif
