//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: cmdenum.h,v 1.11 2013/11/05 13:23:01 lancos Exp $
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

#ifndef _CMDENUM_H
#define _CMDENUM_H

//@V@:BeginIDs
enum
{
	idAskToSave = 100, // Dummy Command
	idCloseAllDialog,
	idEnableToolBar,

	m_Options,              // Options menu
	m_Log,
	m_Setup,                // Setup menu
	m_InterfSetup,
	m_Calibration,  // Calibration menu
	m_Device,
	m_Command,
	m_Read,
	m_Write,
	m_Verify,
	m_Info,
	m_Erase,
	m_Reset,
	m_Help,                 // Help menu
	m_About,                // About
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
	m_Reload,
	m_Program,
	m_ProgOptions,
	m_SerialNumConfig,
	m_SetSerialNum,

	m_OscCalibConfig,
	m_OscCalibRead,

	m_HalfOpen,
	m_OpenProg,
	m_OpenData,

	m_HalfSave,
	m_SaveProg,
	m_SaveData,

	m_HalfRead,
	m_ReadProg,
	m_ReadData,

	m_HalfWrite,
	m_WriteProg,
	m_WriteData,

	m_HalfVerify,
	m_VerifyProg,
	m_VerifyData,

	m_Script,
	m_EditScript,
	m_AutoLoadScript,
	m_LoadScript,
	m_RunScript,
	m_RunFile,

	//List of device menu Item
	m_I2Cbus8,
	m_I2Cbus16,
	m_at17xx,
	m_SPI,
	m_MicroWire,
	m_AVR,
	m_at89s,
	m_PIC16,
	m_PIC12,
	m_imbus,
	m_sde25xx,
	m_x2444xx,

	//AutoTag
	//List of device type menu Item
	m_24XX,
	m_2401a,
	m_2401b,
	m_2402,
	m_2404,
	m_2408,
	m_2416,
	m_24325,
	m_24645,
	m_24XXX,
	m_24E16,
	m_2432,
	m_2464,
	m_24128,
	m_24256,
	m_24512,

	m_9306,
	m_9346,
	m_9356,
	m_9357,
	m_9366,
	m_9376,
	m_9386,

	m_9306_8,
	m_9346_8,
	m_9356_8,
	m_9357_8,
	m_9366_8,
	m_9376_8,
	m_9386_8,

	//      m_at90sXXXX,
	m_at90s1200,
	m_at90s2313,
	m_at90s4414,
	m_at90s8515,
	m_atmega8515,
	m_atmega8535,
	m_atmega603,
	m_atmega103,
	m_atmega8,
	m_atmega16,
	m_atmega128,
	m_atmega64,
	m_atmega161,
	m_atmega162,
	m_atmega163,
	m_atmega169,
	m_atmega32,
	m_atmega323,
	m_at90s2323,
	m_at90s2333,
	m_at90s2343,
	m_at90s4433,
	m_at90s4434,
	m_at90s8534,
	m_at90s8535,
	//      m_attiny10,
	//      m_attiny11,
	m_attiny12,
	m_attiny13,
	m_attiny15,
	m_attiny22,
	m_attiny26,
	//      m_attiny28,
	m_attiny2313,
	m_attiny25, m_attiny45, m_attiny85,
	m_attiny261, m_attiny461, m_attiny861,
	m_atmega48, m_atmega88, m_atmega168, m_atmega328, // new ATmega328 (RG 22.06.2012)
	m_atmega164, m_atmega324, m_atmega644,
	m_atmega640, m_atmega1280, m_atmega1281,
	m_atmega2560, m_atmega2561,
	m_at90can32, m_at90can64, m_at90can128,

	m_at89s8252,
	m_at89s8253,
	m_at89s51,
	m_at89s52,
	m_at89s53,

	m_pic1683,
	m_pic1684,
	m_pic1684A,
	m_pic16870,
	m_pic16871,
	m_pic16872,
	m_pic16873,
	m_pic16874,
	m_pic16876,
	m_pic16877,

	m_pic16873A,
	m_pic16874A,
	m_pic16876A,
	m_pic16877A,
	m_pic16627,
	m_pic16628,

	m_pic12508,
	m_pic12509,
	m_pic12508A,
	m_pic12509A,
	m_pic12518,
	m_pic12519,
	m_pic12671,
	m_pic12672,
	m_pic12673,
	m_pic12674,

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

	m_mda2062,
	m_nvm3060,

	m_at1765,
	m_at17128,
	m_at17256,
	m_at17512,
	m_at17010,

	m_s2430,
	m_x2444,
	m_x2445,
#if 0
	btnHelp,

	frmToolBar,
	frmToolBar2,
	frmToolBar3,
	frmToolBar4,

	icbRead,                // Tool Bar Read
	icbReadEep,
	icbReadFlash,
	icbWrite,
	icbWriteEep,
	icbWriteFlash,
	icbVerify,
	icbVerifyEep,
	icbVerifyFlash,
	icbLock,
	icbOpen,
	icbOpenEep,
	icbOpenFlash,
	icbSave,
	icbSaveEep,
	icbSaveFlash,
	icbErase,
	icbReload,
	icbPrint,
	icbEdit,
	icbHelp,                // Tool Bar Help

	icbNew,
	icbScript,
	icbProgram,
	icbSernum,
	icbSetup,

	icoChip,

	lblEEPType,
	cbxEEPType,             // EEPROM type
	lblEEPSubType,
	cbxEEPSubType,
	btnChange,
	lblStringID,
	txtStringID,    // Identificativo della EEPROM
	lblComment,
	txtComment,             // Commento
	lblTestStat,    // Status Bar test
	lblCurTime,             // Date/Time field
	lblEEPInfo,             // EEPROM Info
	lblLegend,

	//About Modal dialog
	icoPonyProg,
	frmAbout1,
	lblAbout0,
	lblAbout1,
	lblAbout2,
	frmAboutVgui,
	icoVgui,
	txtVgui,

	btnAboutHelp,
	btnAboutVgui,
	chkSound,
	chkSkipMsg,

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
	blkBlank1,
	blkBlank2,
	blkBlank3,
	blkBlank4,
	blkBlank5,
	blkBlank6,
	blkBlank7,

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
#endif

	blkLast                 // Last item
};
//@V@:EndIDs

#endif
