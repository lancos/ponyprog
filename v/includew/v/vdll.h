//===============================================================
// vdll.h - This file has differences needed for dll/nondll versions
//
// Copyright (C) 1995-2000  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================
#ifndef VDLL_H
#define VDLL_H
//#define BUILD_V_DLL
// These definitions contributed by Franco Bez <franco.bez@gmx.de> V 1.24
#if defined (BUILD_V_DLL)
# define V_EXPORT __declspec(dllexport)
# else
# if defined (USE_V_DLL)
#  define V_EXPORT __declspec(dllimport)
# else
#  define V_EXPORT  
# endif 
#endif 

#endif 
