//=========================================================================//
//-------------------------------------------------------------------------//
// cmdenum.h -- command id enumeration                                     //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2000  Claudio Lanconelli                            //
//                                                                         //
//  e-mail: lanconel@cs.unibo.it                                           //
//  http://www.cs.unibo.it/~lanconel                                       //
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
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//


#ifndef	_CMDENUM_H
#define	_CMDENUM_H

//@V@:BeginIDs
enum {
	idAskToSave = 100, // Dummy Command
	idCloseAllDialog,
	idEnableToolBar,

	m_Options,		// Options menu
	m_Setup,		// Setup menu
	m_Calibration,	// Calibration menu
	m_Eeprom,
	m_Read,
	m_Write,
	m_Verify,
	m_Erase,
	m_Info,
	m_Reset,
	m_Help,			// Help menu
	m_About,		// About
	m_HIndex,
	m_Note,
	m_EditBuf,
	m_ClearBuf,
	m_FillBuf,
	m_DoubleSize,
	m_Utility,
	m_ByteSwap,
	m_ReadSecurity,
	m_WriteSecurity,
	m_ReadHEndurance,
	m_WriteHEndurance,
	m_SpecBits,
	m_Type,
	m_File1,
	m_File2,
	m_Remote,

	//List of device menu Item
	m_I2Cbus,
	m_SPI,
	m_MicroWire,
	m_AVR,
	m_at89s,
	m_PIC,

	//AutoTag
	//List of device type menu Item
	m_24XX,
	m_2402,
	m_2404,
	m_2408,
	m_2416,
	m_24325,
	m_24645,
	m_24XXX,
	m_2432,
	m_2464,
	m_24128,
	m_24256,
	m_24512,
	m_9306,
	m_9346,
	m_9356,
	m_9366,
	m_9376,
	m_9386,
	m_at90sXXXX,
	m_at90s1200,
	m_at90s2313,
	m_at90s4414,
	m_at90s8515,
	m_atmega603,
	m_atmega103,
	m_at90s2323,
	m_at90s2333,
	m_at90s2343,
	m_at90s4433,
	m_at90s4434,
	m_at90s8534,
	m_at90s8535,
	m_attiny12,
	m_attiny22,

	m_at89s8252,
	m_at89s53,
	m_pic1684,
	m_25010,
	m_25020,
	m_25040,
	m_25080,
	m_25160,
	m_25320,
	m_25640,
	m_25128,
	m_25256,
	m_sde2506,

	frmToolBar,
	frmToolBar2,
	frmToolBar3,
	frmToolBar4,

	icbRead,		// Tool Bar Read
	icbWrite,
	icbVerify,
	icbLock,
	icbOpen,
	icbSave,
	icbReload,
	icbPrint,
	icbHelp,		// Tool Bar Help
	lblEEPType,
	cbxEEPType,		// EEPROM type
	lblEEPSubType,
	cbxEEPSubType,
	btnChange,
	lblStringID,
	txtStringID,	// Identificativo della EEPROM
	lblComment,
	txtComment,		// Commento
	lblTestStat,	// Status Bar test
	lblCurTime,		// Date/Time field
	lblEEPInfo,		// EEPROM Info

	//About Modal dialog
	icoPonyProg,
	frmAbout1,
	lblAbout0,
	lblAbout1,
	lblAbout2,
	frmAboutVgui,
	icoVgui,
	txtVgui,

	//Notes Modal dialog
	lblMainMsg,
	lblStrID,
	txiStrID,
	lblCommnt,
	txiCommnt,

	//Info Modal dialog
	lblMainMsg2,
	lblRllBnk,
	txtRllBnk,
	lblCRC,
	txtCRC,
	lblSize,
	txtSize,
	lblSecurity,
	txtSecurity,
	lblHEndurance,
	txtHEndurance,

	//Retry Modal dialog
	btnIgnore,
	blkBlank,

	//Edit buffer dialog
	frmEditChar,
	lblEditMsg,
	lblHexval,
	txiHexval,
	lblDecval,
	txiDecval,
	lblChval,
	txiChval,
	txiEditText,

	pbrProgress,

	blkLast			// Last item
};
//@V@:EndIDs

#endif
