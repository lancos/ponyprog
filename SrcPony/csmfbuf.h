//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2025   Claudio Lanconelli                           //
//                                                                         //
//  https://github.com/lancos/ponyprog                                        //
//                                                                         //
//-------------------------------------------------------------------------//
//                                                                         //
// This program is free software; you can redistribute it and/or           //
// modify it under the terms of the GNU  General Public License            //
// as published by the Free Software Foundation; either version2 of        //
// the License, or (at your option) any later version.                     //
//                                                                         //
// This program is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
// General Public License for more details.                                //
//                                                                         //
// You should have received a copy of the GNU  General Public License      //
// along with this program (see LICENSE);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//=========================================================================//

#ifndef _CSMFBUF_H
#define _CSMFBUF_H

// Include standard V files as needed

#include "types.h"
#include "filebuf.h"

#include "device.h"

class csmFileBuf : public FileBuf
{
  public:

	csmFileBuf(e2AppWinInfo *wininfo = 0);
	//      virtual ~csmFileBuf();

	virtual int Load(int loadtype = ALL_TYPE, long relocation_offfset = 0);
	virtual int Save(int savetype = ALL_TYPE, long relocation_offfset = 0);

  protected:

  private:

};
#endif
