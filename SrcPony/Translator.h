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
// along with this program (see LICENSE);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//=========================================================================//

#ifndef LANGUAGE_HEADER
#define LANGUAGE_HEADER

#include <QString>
#include <QVector>



class cTranslator
{
  public:
	bool loadTranslation(const QString fname);
  public:
	static QString translate(int id);
	static QString engText[];
  private:
	QString convertString(const QString &s);
	//         Q_DISABLE_COPY(cTranslator);

  private:
	static QVector<QString> translateTable;
};

//
// all enums TR_CONSTS are for direct access to the text from engText or translateTable
// not over maps or text id's
//
enum TR_CONSTS
{
	STR_LANGUAGE_CODE = 0,
	MSG_TRANSLATORCREDITS,
	MSG_TRANSLATORNAME,

	STR_APPNAME_EXT,
	STR_APPDOWNLOAD1,
	STR_APPDOWNLOAD2,

	STR_NONAME,
	STR_BTNOK,
	STR_BTNCANC,
	STR_BTNHELP,
	STR_BTNPROBE,

	STR_BTNCHECKHW,

	STR_BTNNOTE,
	STR_BTNEDIT,


	//Dialog messages
	STR_MSGVERSION,
	STR_YES,
	STR_NO,
	STR_OK,
	STR_TEST,
	STR_MSGFAILED,
	STR_ABOUT,
	STR_MSGPAGE,
	STR_MSGUNKNOWN,
	STR_BUFCHANGED,
	STR_BUFCHANGED2,
	STR_NOTHINGSAVE,
	STR_NOTHINGLOAD,
	STR_NOTHINGPRINT,
	STR_NOTHINGWRITE,
	STR_NOTHINGVERIFY,
	STR_BUSCALIBRA1,
	STR_BUSCALIBRA2,
	STR_BUSCALIBRAOK,
	STR_BUSCALIBRAFAIL,
	STR_MSGNEEDCALIB,
	STR_MSGNEEDSETUP,
	STR_MSGREADING,
	STR_MSGREADOK,
	STR_ASKWRITE,
	STR_MSGWRITING,
	STR_MSGVERIFING,
	STR_MSGWRITEOK,
	STR_MSGWRITEFAIL,
	STR_MSGERASING,
	STR_MSGERASEOK,
	STR_MSGVERIFYFAIL1,
	STR_MSGVERIFYFAIL2,
	STR_MSGVERIFYOK,
	STR_MSGPROGRAMOK,
	STR_MSGPROGRAMFAIL,
	STR_MSGDEVRESET,
	STR_MSGWRITINGSEC,
	STR_MSGWRITESECFAIL,
	STR_MSGREADINGSEC,
	STR_MSGREADSECFAIL,
	STR_MSGREADINGFUSE,
	STR_MSGREADFUSEFAIL,
	STR_MSGWRITINGFUSE,
	STR_MSGWRITEFUSEFAIL,

	STR_MSGSCRIPTERROR,
	STR_MSGSCRIPTBADCOMMAND,
	STR_MSGSCRIPTBADARGUMENT,
	STR_MSGSCRIPTARGMISSING,
	STR_MSGDEFAULTPAUSE,

	STR_BUFEMPTY,
	STR_OPNOTSUP,
	STR_MSGINSPARAM,
	STR_MSGBADPARAM,
	STR_MSGACCEPTCMD,
	STR_MSGOPENFILE,
	STR_MSGOPENPROGFILE,
	STR_MSGOPENDATAFILE,
	STR_MSGFILENOTFOUND,
	STR_MSGFILESAVEFAIL,
	STR_MSGFILESAVEAS,
	STR_MSGFILESAVEPROG,
	STR_MSGFILESAVEDATA,
	STR_MSGCLOSEWINEXIT,
	STR_MSGCLOSEWINSAVE,
	STR_MSGOPENSCRIPT,


	//Menu entries

	//Main Menu
	STR_MENUFILE,
	STR_MENUEDIT,
	STR_MENUDEVICE,
	STR_MENUCMD,
	STR_MENUSCRIPT,
	STR_MENUUTIL,
	STR_MENUSETUP,
	STR_MENUOPTIONS,
	STR_MENUQMARK,

	//Menu File
	STR_NEWWIN,
	STR_OPEN,
	STR_OPENFLASH,
	STR_OPENDATA,
	STR_SAVE,
	STR_SAVEAS,
	STR_SAVEFLASH,
	STR_SAVEDATA,
	STR_RELOAD,
	STR_PRINT,
	STR_CLOSE,
	STR_EXIT,

	//Menu Edit
	STR_EDITNOTE,
	STR_EDITBUFENA,

	//Menu Command
	STR_READALL,
	STR_READPROG,
	STR_READDATA,
	STR_READSECBITS,
	STR_WRITEALL,
	STR_WRITEPROG,
	STR_WRITEDATA,
	STR_WRITESECBITS,
	STR_VERIFYALL,
	STR_VERIFYPROG,
	STR_VERIFYDATA,
	STR_VERIFYSECBITS,
	STR_ERASE,
	STR_GETINFO,
	STR_RESET,
	STR_PROGRAM,
	STR_PROGOPTION,

	//Menu Options
	STR_INTERFSETUP,
	STR_CALIBRATION,
	STR_REMOTEMODE,

	//Menu Utility
	STR_CLEARBUF,
	STR_FILLBUF,
	STR_DOUBLEBANK,
	STR_BYTESWAP,
	STR_DOWNSERNUM,
	STR_SERNUMCONF,

	//Menu Help
	STR_MENUHELP,
	STR_MENUABOUT,

	//Menu Options
	STR_MENULOG,

	//Menu Script
	STR_EDITSCR,
	STR_LOADSCR,
	STR_AUTOLOADSCR,
	STR_RUNSCR,

	//Tool Bar

	//Dialog Messages
	STR_MSGDEVTYPE,

	//Tool tips
	STR_TTOPENFILE,
	STR_TTSAVEFILE,
	STR_TTPRINT,
	STR_TTRELOAD,
	STR_TTREADALL,
	STR_TTREADPROG,
	STR_TTREADDATA,
	STR_TTREADSEC,
	STR_TTWRITEALL,
	STR_TTWRITEPROG,
	STR_TTWRITEDATA,
	//          STR_TTWRITESEC  = "Write Security and Configuration Bits"
	STR_TTVERIFYPROG,
	STR_TTVERIFYDATA,
	STR_TTVERIFYSEC,

	STR_TTSELFAMILY,
	STR_TTSELTYPE,

	STR_TTEDITNOTE,

	//Error Messages
	STR_DEVNOTRESP,
	STR_DEVBADTYPE,
	STR_DEVUNKNOWN,
	STR_DEVLOCKED,
	STR_OPABORTED,
	STR_OPENFAILED,
	STR_ACCDENIED,
	STR_NOTINST,
	STR_HWERROR,
	STR_BUSBUSY,
	STR_I2CNOACK,
	STR_I2CNODEV,
	STR_I2CTIMEOUT,
	STR_I2CSTOPERR,
	STR_WRITEERR,
	STR_BLANKCHECKERR,
	STR_ERRNO,
	STR_SCRIPTERROR,




	//----Setup dialog

	//Dialog title
	STR_DLGIOSETUP,
	STR_MSGINTSETUP,

	//Dialog messages
	STR_LBLCOMLPT,
	STR_LBLSERIAL,
	STR_LBLPARALLEL,
	STR_LBLINTERFSEL,
	STR_LBLINTERFTYPE,
	STR_LBLCOMSELECT,
	STR_LBLCOM1,
	STR_LBLUSBSELECT,
	STR_LBLUSB1,
	STR_LBLLPTSELECT,
	STR_LBLLPT1,
	STR_LBLSELPOLARITY,
	STR_LBLINVRESET,
	STR_LBLINVSCK,
	STR_LBLINVDATAIN,
	STR_LBLINVDATAOUT,

	//Tool tips

	STR_TTCOM1,
	STR_TTUSB1,
	STR_TTLPT1,

	//----Fill dialog

	//Dialog title
	STR_LBLFILLBUF,

	//Dialog messages: NB should be of the same size (number of char)
	STR_LBLFROM,
	STR_LBLTO,
	STR_LBLVALUE,

	//----Fuse dialog

	//Button captions
	STR_BTNCLRALL,
	STR_BTNSETALL,
	STR_BTNWRITE,
	STR_BTNREAD,

	//Dialog title
	STR_MSGFUSEDLG,

	//Dialog messages: NB should be of the same size (number of char)
	STR_MSGFIRSTBLK,
	STR_MSGNUMBLOCK,
	STR_MSGHIGHENDBLK,

	//Tool tips
	STR_TTCLRALL,
	STR_TTSETALL,
	STR_TTWRITE,
	STR_TTREAD,


	//----Info dialog

	//Button captions
	STR_BTNCLOSE,

	//Dialog title
	STR_MSGDEVINFO,

	//Dialog messages
	STR_MSGBANKROLL,
	STR_MSGSIZE,
	STR_MSGCRC,
	STR_MSGSECBLOCK,
	STR_HIGHENDURAN,


	//----Char Edit dialog

	//Dialog title
	STR_MSGEDITBUG,

	//Dialog messages: NB should be of the same size (number of char)
	STR_MSGHEX,
	STR_MSGDECIMAL,
	STR_MSGCHAR,

	STR_MSGINSNEWVAL,
	STR_MSGENTERTEXT,

	//Tool tips
	STR_TTHEX,
	STR_TTDECIMAL,
	STR_TTCHAR,

	//----Program Option dialog

	//Dialog title
	STR_MSGPROGOPT,

	//Dialog messages
	STR_MSGRELOAD,
	STR_MSGREADPROG,
	STR_MSGREADDATA,
	STR_MSGREADSERNUM,
	STR_MSGBYTESWAP,
	STR_MSGSERNUM,
	STR_MSGERASE,
	STR_MSGWRITEPROG,
	STR_MSGWRITEDATA,
	STR_MSGWRITESEC,
	STR_MSGVERIFYSEC,

	//----Serial Number dialog

	//Dialog title
	STR_MSGSERNUMCFG,

	//Dialog messages: NB should be of the same size (number of char)
	STR_MSGADDRESS,
	STR_MSGSIZE2,
	STR_MSGVALUE,
	STR_MSGFORMAT,

	//next 2 probably can't be translated
	STR_MSGLITTLEEND,
	STR_MSGBIGENDIAN,

	STR_MSGOFFSET,

	//Tool tips

	STR_TTSNMEMADDR,
	STR_TTSNSIZE,
	STR_TTSNVALUE,
	STR_TTSNOFFSET,
	STR_TTLITTLEEND,
	STR_TTBIGENDIAN,


	//----Retry dialog

	//Button captions
	STR_ABORT,
	STR_RETRY,
	STR_IGNORE,

	//Dialog title
	STR_MSGALERT,


	//----Notes dialog

	//Dialog title
	STR_MSGDEVNOTE,


	//----Progress dialog

	//Dialog title
	STR_MSGSTATUS,


	//----Hardware check dialog
	STR_MSGHWCHECKDLG,


	//----About dialog
	STR_LBLNOSOUND,

	STR_LBLSKIPMSG,


	//---- Oscillator Calibration Byte
	STR_MSGOSCCALIBCFG,

	STR_READOSCCALIB,
	STR_OSCCALIBOPTION,
	STR_MSGREADCALIBOK,

	STR_MSGCHIPID,
	STR_MSGNOTE,
	STR_MSGEEPSIZE,
	STR_MSGFLASHSIZE,

	STR_MSGSNAUTOINC,
	STR_TTSNAUTOINC,

	STR_MSGBANKROLLOVER,
	STR_BUFCHANGED3,

	STR_TTOPENPROG,
	STR_TTOPENDATA,
	STR_TTSAVEPROG,
	STR_TTSAVEDATA,
	STR_TTNEWWIN,
	STR_TTPROGRAM,
	STR_TTSCRIPT,
	STR_TTSETUP,
	STR_TTERASE,

	STR_SECBITS,
	STR_TTSECBITS,

	STR_LBLFUSEDLGHLP,
	STR_FUSEDLGNOTESET,
	STR_FUSEDLGNOTECLR,
	STR_CALIBRENABLED,
	STR_ENUM_SIZE
};


#endif
