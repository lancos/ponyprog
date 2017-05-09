//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
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

#ifndef fuseMDLG_H
#define fuseMDLG_H

#include <QDialog>
#include <QObject>
#include <QVector>
#include <QCheckBox>

#include "ui_fusedlg.h"
#include "ui_blockdlg.h"

#include "e2cmdw.h"


#define LOCKPACKSIZE    8
#define FUSEPACKSIZE    LOCKPACKSIZE


typedef struct
{
	long type;

	uint8_t lockenable3;
	QString locklabel3[LOCKPACKSIZE];
	uint8_t lockenable2;
	QString locklabel2[LOCKPACKSIZE];
	uint8_t lockenable1;
	QString locklabel1[LOCKPACKSIZE];
	uint8_t lockenable0;
	QString locklabel0[LOCKPACKSIZE];

	uint16_t fuseenable3;
	QString fuselabel3[FUSEPACKSIZE];
	uint16_t fuseenable2;
	QString fuselabel2[FUSEPACKSIZE];
	uint16_t fuseenable1;
	QString fuselabel1[FUSEPACKSIZE];
	uint16_t fuseenable0;
	QString fuselabel0[FUSEPACKSIZE];
} FuseBit;



class fuseModalDialog : public QDialog, public cTranslator, public Ui::FuseDialog
{
	Q_OBJECT
  public:               //---------------------------------------- public
	fuseModalDialog(e2CmdWindow *bw, e2AppWinInfo *p, bool readonly = false, const QString &msg = "");
	virtual ~fuseModalDialog();             // Destructor

  private slots:
	void onOk();
	void onRead();
	void onProg();

  protected:    //--------------------------------------- protected

  private:
	void setTextWidgets();
	void initWidgets(const QString &msg, long type, bool readonly);
	int eep_FindFuses(long type);

  private:              //--------------------------------------- private
	QVector<QCheckBox *> chkFuse;
	QVector<QCheckBox *> chkLock;

	uint32_t lock, fuse;

	e2AppWinInfo *awip;

	bool write;
	bool read;
};



const QVector<FuseBit> eep_fusebits =
{
	//  { AT90S0000,
	//      0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
	//      0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },

	{
		AT90S1200,
		// lock
		0x00, {0}, // L3
		0x00, {0}, // L2
		0x00, {0}, // L1
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0}, // L0
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		AT90S2313,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		AT90S4414,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		AT90S8515,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},

	{
		AT90S4434,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x01, {0, 0, 0, 0, 0, 0, 0, "FSTRT "}
	},
	{
		AT90S8535,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x01, {0, 0, 0, 0, 0, 0, 0, "FSTRT "}
	},

	{
		AT90S2333,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x1f, {0, 0, "SPIEN ", "BODLEVEL ", "BODEN ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		AT90S4433,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x1f, {0, 0, "SPIEN ", "BODLEVEL ", "BODEN ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},

	{
		AT90S2323,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x01, {0, 0, 0, 0, 0, 0, 0, "FSTRT "}
	},
	{
		AT90S2343,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x01, {0, 0, 0, 0, 0, 0, 0, "RCEN "}
	},

	{
		AT90S8534,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},

	//  { ATtiny10,
	//      0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
	//      0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },
	//  { ATtiny11,
	//      0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
	//      0x00,{0}, 0x00,{0}, 0x00,{0}, 0x17,{0,0,0,"FSTRT ","RSTDISBL ","CKSEL2 ","CKSEL1 ","CKSEL0 "} },
	{
		ATtiny12,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0xCF, {"BODLEVEL ", "BODEN ", "SPIEN ", "RSTDISBL ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATtiny13,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x03, {0, 0, 0, 0, 0, 0, "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x1E, {0, 0, 0, "SELFPRGEN ", "DWEN ", "BODLEVEL1 ", "BODLEVEL0 ", "RSTDISBL "},
		0x7F, {"SPIEN ", "EESAVE ", "WDTON ", "CKDIV8 ", "SUT1 ", "SUT0 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATtiny15,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0xC3, {"BODLEVEL ", "BODEN ", "SPIEN ", "RSTDISBL ", 0, 0, "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATtiny22,
		0x00, {0}, 0x00, {0}, 0x00, {0}, 0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		0x00, {0}, 0x00, {0}, 0x00, {0}, 0x01, {0, 0, 0, 0, 0, 0, 0, "RCEN "}
	},
	{
		ATtiny26,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x03, {0, 0, 0, 0, 0, 0, "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x07, {0, 0, 0, "RSTDISBL ", "SPIEN ", "EESAVE ", "BODLEVEL ", "BODEN "},
		0xFF, {"PLLCK ", "CKOPT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	//  { ATtiny28,
	//      0x00,{0}, 0x00,{0}, 0x00,{0}, 0x06,{0,0,0,0,0,"Lock2 ","Lock1 ",0},
	//      0x00,{0}, 0x00,{0}, 0x00,{0}, 0x00,{0} },

	{
		ATtiny2313,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x03, {0, 0, 0, 0, 0, 0, "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x01, {0, 0, 0, 0, 0, 0, 0, "SPMEN "},
		0xDE, {"DWEN ", "EESAVE ", "SPIEN ", "WDTON ", "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 ", "RSTDISBL "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATtiny25,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x03, {0, 0, 0, 0, 0, 0, "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x01, {0, 0, 0, 0, 0, 0, 0, "SELFPRGEN "},
		0x5F, {"RSTDISBL ", "DWEN ", "SPIEN ", "WDTON ", "EESAVE ", "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATtiny45,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x03, {0, 0, 0, 0, 0, 0, "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x01, {0, 0, 0, 0, 0, 0, 0, "SELFPRGEN "},
		0x5F, {"RSTDISBL ", "DWEN ", "SPIEN ", "WDTON ", "EESAVE ", "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATtiny85,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x03, {0, 0, 0, 0, 0, 0, "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x01, {0, 0, 0, 0, 0, 0, 0, "SELFPRGEN "},
		0x5F, {"RSTDISBL ", "DWEN ", "SPIEN ", "WDTON ", "EESAVE ", "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATtiny261,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x03, {0, 0, 0, 0, 0, 0, "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x01, {0, 0, 0, 0, 0, 0, 0, "SELFPRGEN "},
		0x5F, {"RSTDISBL ", "DWEN ", "SPIEN ", "WDTON ", "EESAVE ", "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATtiny461,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x03, {0, 0, 0, 0, 0, 0, "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x01, {0, 0, 0, 0, 0, 0, 0, "SELFPRGEN "},
		0x5F, {"RSTDISBL ", "DWEN ", "SPIEN ", "WDTON ", "EESAVE ", "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATtiny861,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x03, {0, 0, 0, 0, 0, 0, "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x01, {0, 0, 0, 0, 0, 0, 0, "SELFPRGEN "},
		0x5F, {"RSTDISBL ", "DWEN ", "SPIEN ", "WDTON ", "EESAVE ", "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega48,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x01, {0, 0, 0, 0, 0, 0, 0, "SELFPRGEN "},
		0x5F, {"RSTDISBL ", "DWEN ", "SPIEN ", "WDTON ", "EESAVE ", "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega88,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x07, {0, 0, 0, 0, 0, "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0x5F, {"RSTDISBL ", "DWEN ", "SPIEN ", "WDTON ", "EESAVE ", "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega168,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x07, {0, 0, 0, 0, 0, "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0x5F, {"RSTDISBL ", "DWEN ", "SPIEN ", "WDTON ", "EESAVE ", "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega328, // new (RG 22.06.2012)
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x07, {0, 0, 0, 0, 0, "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0x5F, {"RSTDISBL ", "DWEN ", "SPIEN ", "WDTON ", "EESAVE ", "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega164,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x07, {0, 0, 0, 0, 0, "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "WDTON ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega324,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x07, {0, 0, 0, 0, 0, "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "WDTON ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega644,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x07, {0, 0, 0, 0, 0, "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "WDTON ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega640,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x07, {0, 0, 0, 0, 0, "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "WDTON ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega1280,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x07, {0, 0, 0, 0, 0, "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "WDTON ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega1281,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x07, {0, 0, 0, 0, 0, "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "WDTON ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega2560,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x07, {0, 0, 0, 0, 0, "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "WDTON ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega2561,
		0x00, {0}, 0x00, {0}, 0x00, {0}, 0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		0x00, {0}, 0x07, {0, 0, 0, 0, 0, "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 "}, 0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "WDTON ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "}, 0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		AT90CAN32,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x0E, {0, 0, 0, 0, "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 ", "TA0SEL "},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "WDTON ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		AT90CAN64,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x0E, {0, 0, 0, 0, "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 ", "TA0SEL "},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "WDTON ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		AT90CAN128,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x0E, {0, 0, 0, 0, "BODLEVEL2 ", "BODLEVEL1 ", "BODLEVEL0 ", "TA0SEL "},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "WDTON ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},

	{
		ATmega603,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x0B, {0, 0, 0, 0, "EESAVE ", 0, "SUT1 ", "SUT0 "}
	},
	{
		ATmega103,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x06, {0, 0, 0, 0, 0, "Lock2 ", "Lock1 ", 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x0B, {0, 0, 0, 0, "EESAVE ", 0, "SUT1 ", "SUT0 "}
	},
	{
		ATmega8,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x5F, {"RSTDISBL ", "WDTON  ", "SPIEN ", "CKOPT ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"BODLEVEL ", "BODEN ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega8515,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0xDF, {"S8515C ", "WDTON  ", "SPIEN ", "CKOPT ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"BODLEVEL ", "BODEN ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega8535,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0xDF, {"S8535C ", "WDTON  ", "SPIEN ", "CKOPT ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"BODLEVEL ", "BODEN ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega64,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x03, {0, 0, 0, 0, 0, 0, "M103C ", "WDTON  "},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "CKOPT ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"BODLEVEL ", "BODEN ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega128,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x03, {0, 0, 0, 0, 0, 0, "M103C ", "WDTON  "},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "CKOPT ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"BODLEVEL ", "BODEN ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega16,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "CKOPT ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"BODLEVEL ", "BODEN ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega161,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x7F, {0, "BOOTRST ", "SPIEN ", "BODLEVEL ", "BODEN ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega163,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x07, {0, 0, 0, 0, 0, "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xCF, {"BODLEVEL ", "BODEN ", 0, 0, "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega162,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x1E, {0, 0, 0, "M161C ", "BOD2LEVEL ", "BOD1LEVEL ", "BOD0LEVEL ", 0},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "WDTON  ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega169,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x0E, {0, 0, 0, 0, "BOD2LEVEL ", "BOD1LEVEL ", "BOD0LEVEL ", "RSTDISBL "},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "WDTON  ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"CKDIV8 ", "CKOUT ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega323,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0xCF, {"OCDEN ", "JTAGEN ", "SPIEN ", 0, "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xCF, {"BODLEVEL ", "BODEN ", 0, 0, "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},
	{
		ATmega32,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "BootLock12 ", "BootLock11 ", "BootLock02 ", "BootLock01 ", "Lock2 ", "Lock1 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0xDF, {"OCDEN ", "JTAGEN ", "SPIEN ", "CKOPT ", "EESAVE ", "BOOTSZ1 ", "BOOTSZ0 ", "BOOTRST "},
		0xFF, {"BODLEVEL ", "BODEN ", "SUT1 ", "SUT0 ", "CKSEL3 ", "CKSEL2 ", "CKSEL1 ", "CKSEL0 "}
	},

	{
		AT89S8252,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0xE0, {"LB1 ", "LB2 ", "LB3 ", 0, 0, 0, 0, 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		AT89S8253,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x07, {0, 0, 0, 0, 0, "LB1 ", "LB2 ", "LB3 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x0F, {0, 0, 0, 0, "ClkSel ", "UserRow ", "x2Mode ", "SerProg "}
	},
	{
		AT89S51,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x1C, {0, 0, 0, "LB1 ", "LB2 ", "LB3 ", 0, 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		AT89S52,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x1C, {0, 0, 0, "LB1 ", "LB2 ", "LB3 ", 0, 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		AT89S53,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0xE0, {"LB1 ", "LB2 ", "LB3 ", 0, 0, 0, 0, 0},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},

	{
		PIC12508,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x1F, {0, 0, 0, "MCLRE ", "CP ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC12509,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x1F, {0, 0, 0, "MCLRE ", "CP ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC12508A,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x1F, {0, 0, 0, "MCLRE ", "CP ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC12509A,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x1F, {0, 0, 0, "MCLRE ", "CP ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC12E518,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x1F, {0, 0, 0, "MCLRE ", "CP ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC12E519,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x1F, {0, 0, 0, "MCLRE ", "CP ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},

	{
		PIC12671,
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "CP1 ", "CP0 ", "CP1 ", "CP0 ", "CP1 ", "CP0 "},
		0xFF, {"MCLRE ", "CP1 ", "CP0 ", "PWRTE ", "WDTE ", "FOSC2 ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC12672,
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "CP1 ", "CP0 ", "CP1 ", "CP0 ", "CP1 ", "CP0 "},
		0xFF, {"MCLRE ", "CP1 ", "CP0 ", "PWRTE ", "WDTE ", "FOSC2 ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC12673,
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "CP1 ", "CP0 ", "CP1 ", "CP0 ", "CP1 ", "CP0 "},
		0xFF, {"MCLRE ", "CP1 ", "CP0 ", "PWRTE ", "WDTE ", "FOSC2 ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC12674,
		0x00, {0},
		0x00, {0},
		0x3F, {0, 0, "CP1 ", "CP0 ", "CP1 ", "CP0 ", "CP1 ", "CP0 "},
		0xFF, {"MCLRE ", "CP1 ", "CP0 ", "PWRTE ", "WDTE ", "FOSC2 ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},

	{
		PIC1683,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x1F, {0, 0, 0, "CP ", "PWRTE ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC1684,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x1F, {0, 0, 0, "CP ", "PWRTE ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC1684A,
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x1F, {0, 0, 0, "CP ", "PWRTE ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},

	{
		PIC16870,
		0x00, {0},
		0x00, {0},
		0x3B, {0, 0, "CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "},
		0xFF, {"LVP ", "BODEN ", "CP1 ", "CP0 ", "PWRTE ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC16871,
		0x00, {0},
		0x00, {0},
		0x3B, {0, 0, "CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "},
		0xFF, {"LVP ", "BODEN ", "CP1 ", "CP0 ", "PWRTE ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC16872,
		0x00, {0},
		0x00, {0},
		0x3B, {0, 0, "CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "},
		0xFF, {"LVP ", "BODEN ", "CP1 ", "CP0 ", "PWRTE ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC16873,
		0x00, {0},
		0x00, {0},
		0x3B, {0, 0, "CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "},
		0xFF, {"LVP ", "BODEN ", "CP1 ", "CP0 ", "PWRTE ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC16874,
		0x00, {0},
		0x00, {0},
		0x3B, {0, 0, "CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "},
		0xFF, {"LVP ", "BODEN ", "CP1 ", "CP0 ", "PWRTE ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC16876,
		0x00, {0},
		0x00, {0},
		0x3B, {0, 0, "CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "},
		0xFF, {"LVP ", "BODEN ", "CP1 ", "CP0 ", "PWRTE ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC16877,
		0x00, {0},
		0x00, {0},
		0x3B, {0, 0, "CP1 ", "CP0 ", "RESV ", 0, "WRT ", "CPD "},
		0xFF, {"LVP ", "BODEN ", "CP1 ", "CP0 ", "PWRTE ", "WDTE ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC16873A,
		0x00, {0},
		0x00, {0},
		0x2F, {0, 0, "CP ", 0, "DEBUG ", "WRT1 ", "WRT0 ", "CPD "},
		0xCF, {"LVP ", "BOREN ", 0, 0, "/PWRTEN ", "WDTEN ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC16874A,
		0x00, {0},
		0x00, {0},
		0x2F, {0, 0, "CP ", 0, "DEBUG ", "WRT1 ", "WRT0 ", "CPD "},
		0xCF, {"LVP ", "BOREN ", 0, 0, "/PWRTEN ", "WDTEN ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC16876A,
		0x00, {0},
		0x00, {0},
		0x2F, {0, 0, "CP ", 0, "DEBUG ", "WRT1 ", "WRT0 ", "CPD "},
		0xCF, {"LVP ", "BOREN ", 0, 0, "/PWRTEN ", "WDTEN ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC16877A,
		0x00, {0},
		0x00, {0},
		0x2F, {0, 0, "CP ", 0, "DEBUG ", "WRT1 ", "WRT0 ", "CPD "},
		0xCF, {"LVP ", "BOREN ", 0, 0, "/PWRTEN ", "WDTEN ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC16627,
		0x00, {0},
		0x00, {0},
		0x3D, {0, 0, "CP1 ", "CP0 ", "CP1 ", "CP0 ", 0, "CPD "},
		0xFF, {"LVP ", "BOREN ", "MCLRE ", "FOSC2 ", "/PWRTEN ", "WDTEN ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	},
	{
		PIC16628,
		0x00, {0},
		0x00, {0},
		0x3D, {0, 0, "CP1 ", "CP0 ", "CP1 ", "CP0 ", 0, "CPD "},
		0xFF, {"LVP ", "BOREN ", "MCLRE ", "FOSC2 ", "/PWRTEN ", "WDTEN ", "FOSC1 ", "FOSC0 "},
		// fuse
		0x00, {0},
		0x00, {0},
		0x00, {0},
		0x00, {0}
	}
	//EndTag
};


class blockDialog : public QDialog, public cTranslator, public Ui::BlockDialog
{
	Q_OBJECT
  public:               //---------------------------------------- public
	blockDialog(e2CmdWindow *bw, e2AppWinInfo *p, bool readonly = false, const QString &msg = "");
	virtual ~blockDialog();          // Destructor

  protected:    //--------------------------------------- protected

  private slots:
	void onOk();

  private:              //--------------------------------------- private
	e2AppWinInfo *awip;
	uint32_t lock, fuse;
};

#endif
