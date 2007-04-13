//===============================================================
// vcb2x4.h - defines a 2 row by 8 color button frame
//
// Copyright (C) 1995  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VCB2X4_H
#define VCB2X4_H
#ifndef vC_Size
#define vC_Size 8
#endif
#ifndef vC_Frame
#define vC_Frame NoFrame
#endif
#ifndef vC_RightOf
#define vC_RightOf 0
#endif
#ifndef vC_Below
#define vC_Below 0
#endif
{C_Frame, M_ColorFrame,0,"",NoList,CA_NoSpace,isSens,vC_Frame,vC_RightOf,vC_Below,0},
{C_ColorButton,M_Black, vC_Black,"",&vStdColors[vC_Black],CA_None,isSens,M_ColorFrame,0,0,vC_Size},
{C_ColorButton,M_Red, vC_Red,"",&vStdColors[vC_Red],CA_None,isSens,M_ColorFrame,M_Black,0,vC_Size},
{C_ColorButton,M_Green, vC_Green,"",&vStdColors[vC_Green],CA_None,isSens,M_ColorFrame,M_Red,0,vC_Size},
{C_ColorButton,M_Blue, vC_Blue,"",&vStdColors[vC_Blue],CA_None,isSens,M_ColorFrame,M_Green,0,vC_Size},

{C_ColorButton,M_White, vC_White,"",&vStdColors[vC_White],CA_None,isSens,M_ColorFrame,0,M_Black,vC_Size},
{C_ColorButton,M_Yellow, vC_Yellow,"",&vStdColors[vC_Yellow],CA_None,isSens,M_ColorFrame,M_White,M_Black,vC_Size},
{C_ColorButton,M_Magenta, vC_Magenta,"",&vStdColors[vC_Magenta],CA_None,isSens,M_ColorFrame,M_Yellow,M_Black,vC_Size},
{C_ColorButton,M_Cyan, vC_Cyan,"",&vStdColors[vC_Cyan],CA_None,isSens,M_ColorFrame,M_Magenta,M_Black,vC_Size},
#endif
