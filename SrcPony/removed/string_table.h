#ifndef INCLUDE_STRING_TABLE_H
#define INCLUDE_STRING_TABLE_H

#include <QString>
#include <QVector>

namespace Translator
{
#if 0
//  EK 2017
// TODO
// we need this enumerator to build later the translation table as static QString array (max size STR_ENUM_SIZE) array

static QString engText[];

enum TRCODE
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
	STR_MSGYES,
	STR_MSGNO,
	STR_MSGOK,
	STR_MSGTEST,
	STR_MSGFAILED,
	STR_ABOUT,
	STR_MSGPAGE,
	STR_MSGUNKNOWN,
	STR_VGUIABOUT,
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
	STR_LBLCOM2,
	STR_LBLCOM3,
	STR_LBLCOM4,
	STR_LBLUSBSELECT,
	STR_LBLUSB1,
	STR_LBLUSB2,
	STR_LBLUSB3,
	STR_LBLUSB4,
	STR_LBLUSB5,
	STR_LBLUSB6,
	STR_LBLUSB7,
	STR_LBLUSB8,
	STR_LBLLPTSELECT,
	STR_LBLLPT1,
	STR_LBLLPT2,
	STR_LBLLPT3,
	STR_LBLLPT4,
	STR_LBLSELPOLARITY,
	STR_LBLINVRESET,
	STR_LBLINVSCK,
	STR_LBLINVDATAIN,
	STR_LBLINVDATAOUT,

	//Tool tips

	STR_TTCOM1,
	STR_TTCOM2,
	STR_TTCOM3,
	STR_TTCOM4,
	STR_TTUSB1,
	STR_TTUSB2,
	STR_TTUSB3,
	STR_TTUSB4,
	STR_TTUSB5,
	STR_TTUSB6,
	STR_TTUSB7,
	STR_TTUSB8,
	STR_TTLPT1,
	STR_TTLPT2,
	STR_TTLPT3,


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
	STR_BTNABORT,
	STR_BTNRETRY,
	STR_BTNIGNORE,

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
	STR_ENUM_SIZE
}
#endif


static QString STR_LANGUAGE_CODE   =  "english";

//Fill this with your reference, if it's empty = "" it will be invisible
static QString MSG_TRANSLATORCREDITS  = "Translator Credits:";
static QString MSG_TRANSLATORNAME              = "";

static QString STR_APPNAME_EXT         = "Serial Device Programmer";
static QString STR_APPDOWNLOAD1        = "You can find last version of";
static QString STR_APPDOWNLOAD2        = "at the address:";

static QString STR_NONAME              = "No Name";

//Button captions
#ifdef  WIN32
static QString STR_BTNOK               = " &OK ";
static QString STR_BTNCANC             = " &Cancel ";
static QString STR_BTNHELP             = " &Help ";
static QString STR_BTNPROBE            = " &Probe ";
#else
static QString STR_BTNOK               = "  OK ";
static QString STR_BTNCANC             = "  Cancel ";
static QString STR_BTNHELP             = "  Help ";
static QString STR_BTNPROBE            = "  Probe ";
#endif
static QString STR_BTNCHECKHW  = " Hardware Check ";

static QString STR_BTNNOTE             = "Note";
static QString STR_BTNEDIT             = "Edit";


//Dialog messages
static QString STR_MSGVERSION          = "Version";
static QString STR_MSGYES              = "Yes";
static QString STR_MSGNO               = "No";
static QString STR_MSGOK               = "Ok";
static QString STR_MSGTEST             = "Test";
static QString STR_MSGFAILED           = "Failed";
static QString STR_ABOUT               = "About";
static QString STR_MSGPAGE             = "Page";
static QString STR_MSGUNKNOWN          = "Unknown";
static QString STR_VGUIABOUT           = "This program uses the V library.\nThe V library is copyright by Bruce E.Wampler.\nYou can find it and related license at\nhttp://www.objectcentral.com";
static QString STR_BUFCHANGED          = "Buffer \"%s\" changed. Save it before closing?";
static QString STR_BUFCHANGED2         = "Buffer changed. Save it before opening?";
static QString STR_NOTHINGSAVE         = "Nothing to save";
static QString STR_NOTHINGLOAD         = "Nothing to load";
static QString STR_NOTHINGPRINT        = "Nothing to print";
static QString STR_NOTHINGWRITE        = "Nothing to write";
static QString STR_NOTHINGVERIFY       = "Nothing to verify";
static QString STR_BUSCALIBRA1         = "Bus timing calibration.\nBe sure there are no application running other than ";
static QString STR_BUSCALIBRA2         = "\n(the CPU and hard disk have to be idle)\nThe calibration may take a couple of seconds.\nDo you want to run calibration now?";
static QString STR_BUSCALIBRAOK        = "Calibration OK";
static QString STR_BUSCALIBRAFAIL      = "Calibration failed";
static QString STR_MSGNEEDCALIB        = "You need to run Calibration from the Options menu\nbefore any read/write operations";
static QString STR_MSGNEEDSETUP        = "You need to run Setup from the Options menu\nbefore any read/write operations";
static QString STR_MSGREADING          = "Reading...";
static QString STR_MSGREADOK           = "Read successful\nDevice Size:";
static QString STR_ASKWRITE            = "Are you sure you want to write the device?\nAll previous content will be lost";
static QString STR_MSGWRITING          = "Writing...";
static QString STR_MSGVERIFING         = "Verifying...";
static QString STR_MSGWRITEOK          = "Write successful";
static QString STR_MSGWRITEFAIL        = "Write failed";
static QString STR_MSGERASING          = "Erasing...";
static QString STR_MSGERASEOK          = "Erase successful";
static QString STR_MSGVERIFYFAIL1      = "Cannot verify, device missing or bad device";
static QString STR_MSGVERIFYFAIL2      = "Verify Failed";
static QString STR_MSGVERIFYOK         = "Verify successful";
static QString STR_MSGPROGRAMOK        = "Program succesful";
static QString STR_MSGPROGRAMFAIL      = "Program Failed";
static QString STR_MSGDEVRESET         = "Device reset";
static QString STR_MSGWRITINGSEC       = "Writing security bits...";
static QString STR_MSGWRITESECFAIL     = "Security bits write failed";
static QString STR_MSGREADINGSEC       = "Reading security bits...";
static QString STR_MSGREADSECFAIL      = "Security bits read failed";
static QString STR_MSGREADINGFUSE      = "Reading configuration bits...";
static QString STR_MSGREADFUSEFAIL     = "Configuration bits read failed";
static QString STR_MSGWRITINGFUSE      = "Writing configuration bits...";
static QString STR_MSGWRITEFUSEFAIL    = "Configuration bits write failed";

static QString STR_MSGSCRIPTERROR      = "Script error at line";
static QString STR_MSGSCRIPTBADCOMMAND  = "Bad command";
static QString STR_MSGSCRIPTBADARGUMENT = "Bad argument";
static QString STR_MSGSCRIPTARGMISSING  = "Argument missing";
static QString STR_MSGDEFAULTPAUSE     = "Do you want to continue?";

static QString STR_BUFEMPTY            = "Buffer empty";
static QString STR_OPNOTSUP            = "Operation not supported";
static QString STR_MSGINSPARAM         = "Insert parameters";
static QString STR_MSGBADPARAM         = "Bad parameters";
static QString STR_MSGACCEPTCMD        = "Accepting commands...";
static QString STR_MSGOPENFILE         = "Open device content file";
static QString STR_MSGOPENPROGFILE     = "Open program (FLASH) content file";
static QString STR_MSGOPENDATAFILE     = "Open data (EEPROM) content file";
static QString STR_MSGFILENOTFOUND     = "File not found";
static QString STR_MSGFILESAVEFAIL     = "File save failed";
static QString STR_MSGFILESAVEAS       = "Save Device Content File As";
static QString STR_MSGFILESAVEPROG     = "Save Program (FLASH) Content File As";
static QString STR_MSGFILESAVEDATA     = "Save Data (EEPROM) Content File As";
static QString STR_MSGCLOSEWINEXIT     = "Close last window: do you want to exit?";
static QString STR_MSGCLOSEWINSAVE     = "Buffer changed. Save it before closing?";
static QString STR_MSGOPENSCRIPT       = "Open script file";


//Menu entries

//Main Menu
static QString STR_MENUFILE            = "&File";
static QString STR_MENUEDIT            = "&Edit";
static QString STR_MENUDEVICE          = "&Device";
static QString STR_MENUCMD                     = "&Command";
static QString STR_MENUSCRIPT          = "Scri&pt";
static QString STR_MENUUTIL            = "&Utility";
static QString STR_MENUSETUP           = "&Setup";
static QString STR_MENUOPTIONS         = "&Options";
static QString STR_MENUQMARK           = "&?";

//Menu File
static QString STR_NEWWIN              = "&New Window";
static QString STR_OPEN                = "&Open Device File...";
static QString STR_OPENFLASH           = "Open &Program (FLASH) File...";
static QString STR_OPENDATA            = "Open &Data (EEPROM) File...";
static QString STR_SAVE                = "&Save Device File";
static QString STR_SAVEAS              = "Save Device File &As...";
static QString STR_SAVEFLASH           = "Save Program (&FLASH) File As...";
static QString STR_SAVEDATA            = "Save Data (&EEPROM) File As...";
static QString STR_RELOAD              = "&Reload Files";
static QString STR_PRINT               = "Prin&t...";
static QString STR_CLOSE               = "&Close";
static QString STR_EXIT                = "E&xit";

//Menu Edit
static QString STR_EDITNOTE            = "Edit &Note...";
static QString STR_EDITBUFENA          = "Edit Bu&ffer enabled";

//Menu Command
static QString STR_READALL             = "&Read All";
static QString STR_READPROG            = "Read Program (FLASH)";
static QString STR_READDATA            = "Read Data (EEPROM)";
static QString STR_READSECBITS         = "Read &Security and Configuration Bits";
static QString STR_WRITEALL            = "&Write All";
static QString STR_WRITEPROG           = "Write Program (&FLASH)";
static QString STR_WRITEDATA           = "Write Data (EEPROM)";
static QString STR_WRITESECBITS        = "Wr&ite Security and Configuration Bits";
static QString STR_VERIFYALL           = "&Verify All";
static QString STR_VERIFYPROG          = "Verify Program (FLASH)";
static QString STR_VERIFYDATA          = "Verify Data (EEPROM)";
static QString STR_VERIFYSECBITS       = "Verif&y Security and Configuration Bits";
static QString STR_ERASE               = "&Erase";
static QString STR_GETINFO             = "&GetInfo";
static QString STR_RESET               = "Rese&t";
static QString STR_PROGRAM             = "&Program";
static QString STR_PROGOPTION          = "Program Options...";

//Menu Options
static QString STR_INTERFSETUP         = "&Interface Setup...";
static QString STR_CALIBRATION         = "&Calibration";
static QString STR_REMOTEMODE          = "&Remote Mode";

//Menu Utility
static QString STR_CLEARBUF    = "&Clear Buffer";
static QString STR_FILLBUF     = "&Fill Buffer...";
static QString STR_DOUBLEBANK  = "&Double Bank";
static QString STR_BYTESWAP    = "&Byte Swap";
static QString STR_DOWNSERNUM  = "Set Serial&Number";
static QString STR_SERNUMCONF  = "SerialNumber C&onfig...";

//Menu Help
static QString STR_MENUHELP    = "&Help";
static QString STR_MENUABOUT   = "&About";

//Menu Options
static QString STR_MENULOG             = "&Log File...";

//Menu Script
static QString STR_EDITSCR             = "&Edit...";
static QString STR_LOADSCR             = "&Load and Run...";
static QString STR_AUTOLOADSCR = "&Auto Load";
static QString STR_RUNSCR              = "&Run";

//Tool Bar

//Dialog Messages
static QString STR_MSGDEVTYPE  = "Dev.Type";

//Tool tips
static QString STR_TTOPENFILE  = "Open Device File";
static QString STR_TTSAVEFILE  = "Save Device File";
static QString STR_TTPRINT     = "Print Buffer";
static QString STR_TTRELOAD    = "Reload Files";
static QString STR_TTREADALL   = "Read Device";
static QString STR_TTREADPROG  = "Read Program Memory (FLASH)";
static QString STR_TTREADDATA  = "Read Data Memory (EEPROM)";
static QString STR_TTREADSEC   = "Read Security and Configuration Bits";
static QString STR_TTWRITEALL  = "Write Device";
static QString STR_TTWRITEPROG = "Write Program Memory (FLASH)";
static QString STR_TTWRITEDATA = "Write Data Memory (EEPROM)";
//static QString       STR_TTWRITESEC  = "Write Security and Configuration Bits"
static QString STR_TTVERIFYPROG = "Verify Program Memory (FLASH)";
static QString STR_TTVERIFYDATA = "Verify Data Memory (EEPROM)";
static QString STR_TTVERIFYSEC = "Verify Security and Configuration Bits";

static QString STR_TTSELFAMILY = "Select device family";
static QString STR_TTSELTYPE   = "Select device type";

static QString STR_TTEDITNOTE  = "Edit device notes";


//Error Messages
static QString STR_DEVNOTRESP  = "Device Not Responding";
static QString STR_DEVBADTYPE  = "Bad device, select the correct type";
static QString STR_DEVUNKNOWN  = "Device missing or unknown device";
static QString STR_DEVLOCKED   = "Unable to probe, bad device or device locked";
static QString STR_OPABORTED   = "User abort";
static QString STR_OPENFAILED  = "Communication port not available";
static QString STR_ACCDENIED   = "I/O access denied. You must be root to perform I/O";
static QString STR_NOTINST     = "Programmer board not found";
static QString STR_HWERROR     = "Hardware error, is the board properly connected?";
static QString STR_BUSBUSY     = "Bus busy or hardware error";
static QString STR_I2CNOACK    = "Missing Acknowledge from the device";
static QString STR_I2CNODEV    = "Missing device";
static QString STR_I2CTIMEOUT  = "Timeout on the bus";
static QString STR_I2CSTOPERR  = "I2CBus Stop condition error (bad timing?)";
static QString STR_WRITEERR    = "Write error";
static QString STR_BLANKCHECKERR = "Blank check error, need to erase";
static QString STR_ERRNO       = "Error n.";
static QString STR_SCRIPTERROR = "Script error";




//----Setup dialog

//Dialog title
static QString STR_DLGIOSETUP          = "I/O port setup";
static QString STR_MSGINTSETUP         = "Interface board Setup";

//Dialog messages
static QString STR_LBLCOMLPT           = "COM/LPT interface";
static QString STR_LBLSERIAL           = " Serial  ";
static QString STR_LBLPARALLEL         = " Parallel  ";
static QString STR_LBLINTERFSEL        = " Interface select ";
static QString STR_LBLINTERFTYPE       = " Interface type ";
static QString STR_LBLCOMSELECT        = " COM Port select ";
#ifdef _WINDOWS
static QString STR_LBLCOM1             = " COM1  ";
static QString STR_LBLCOM2             = " COM2  ";
static QString STR_LBLCOM3             = " COM3  ";
static QString STR_LBLCOM4             = " COM4  ";
#else
static QString STR_LBLCOM1             = " ttyS0  ";
static QString STR_LBLCOM2             = " ttyS1  ";
static QString STR_LBLCOM3             = " ttyS2  ";
static QString STR_LBLCOM4             = " ttyS3  ";
#endif
static QString STR_LBLUSBSELECT        = " USB Port select ";
#ifdef _WINDOWS
static QString STR_LBLUSB1             = " USB1  ";
static QString STR_LBLUSB2             = " USB2  ";
static QString STR_LBLUSB3             = " USB3  ";
static QString STR_LBLUSB4             = " USB4  ";
static QString STR_LBLUSB5             = " USB5  ";
static QString STR_LBLUSB6             = " USB6  ";
static QString STR_LBLUSB7             = " USB7  ";
static QString STR_LBLUSB8             = " USB8  ";
#else
static QString STR_LBLUSB1             = " ttyS0  ";
static QString STR_LBLUSB2             = " ttyS1  ";
static QString STR_LBLUSB3             = " ttyS2  ";
static QString STR_LBLUSB4             = " ttyS3  ";
static QString STR_LBLUSB5             = " ttyS4  ";
static QString STR_LBLUSB6             = " ttyS5  ";
static QString STR_LBLUSB7             = " ttyS6  ";
static QString STR_LBLUSB8             = " ttyS7  ";
#endif
static QString STR_LBLLPTSELECT        = " LPT Port select ";
static QString STR_LBLLPT1             = " LPT1  ";
static QString STR_LBLLPT2             = " LPT2  ";
static QString STR_LBLLPT3             = " LPT3  ";
static QString STR_LBLLPT4             = " LPT4  ";
static QString STR_LBLSELPOLARITY      = " Select Polarity of the Control lines";
static QString STR_LBLINVRESET         = " Invert Reset ";
static QString STR_LBLINVSCK           = " Invert SCKL  ";
static QString STR_LBLINVDATAIN        = " Invert D-IN  ";
static QString STR_LBLINVDATAOUT       = " Invert D-OUT ";

//Tool tips

static QString STR_TTCOM1              = "Select serial port COM1";
static QString STR_TTCOM2              = "Select serial port COM2";
static QString STR_TTCOM3              = "Select serial port COM3";
static QString STR_TTCOM4              = "Select serial port COM4";
static QString  STR_TTUSB2              = "Select serial port USB2";
static QString  STR_TTUSB3              = "Select serial port USB3";
static QString  STR_TTUSB4              = "Select serial port USB4";
static QString  STR_TTUSB5              = "Select serial port USB5";
static QString  STR_TTUSB6              = "Select serial port USB6";
static QString  STR_TTUSB7              = "Select serial port USB7";
static QString  STR_TTUSB8              = "Select serial port USB8";
static QString STR_TTLPT1              = "Select parallel port LPT1";
static QString STR_TTLPT2              = "Select parallel port LPT2";
static QString STR_TTLPT3              = "Select parallel port LPT3";


//----Fill dialog

//Dialog title
static QString STR_LBLFILLBUF          = "Fill Buffer";

//Dialog messages: NB should be of the same size (number of char)
static QString STR_LBLFROM             = " From  ";
static QString STR_LBLTO               = " To    ";
static QString STR_LBLVALUE            = " Value ";



//----Fuse dialog

//Button captions
#ifdef  WIN32
static QString STR_BTNCLRALL           = " C&lear All ";
static QString STR_BTNSETALL           = " &Set All ";
static QString STR_BTNWRITE            = " &Write ";
static QString STR_BTNREAD             = " &Read ";
#else
static QString STR_BTNCLRALL           = " Clear All ";
static QString STR_BTNSETALL           = " Set All ";
static QString STR_BTNWRITE            = " Write ";
static QString STR_BTNREAD             = " Read ";
#endif

//Dialog title
static QString STR_MSGFUSEDLG          = "Configuration and Security bits";

//Dialog messages: NB should be of the same size (number of char)
static QString STR_MSGFIRSTBLK         = " First Block to Lock ";
static QString STR_MSGNUMBLOCK         = " N.of Blocks to Lock ";
static QString STR_MSGHIGHENDBLK       = " High endurance block";

//Tool tips
static QString STR_TTCLRALL            = "All bits to '0'";
static QString STR_TTSETALL            = "All bits to '1'";
static QString STR_TTWRITE             = "Write bits to the device";
static QString STR_TTREAD              = "Read bits from the device";


//----Info dialog

//Button captions
#ifdef  WIN32
static QString STR_BTNCLOSE            = " &Close ";
#else
static QString STR_BTNCLOSE            = "  Close ";
#endif

//Dialog title
static QString STR_MSGDEVINFO          = "Device Info";

//Dialog messages
static QString STR_MSGBANKROLL         = "Bank roll-over capability:";
static QString STR_MSGSIZE             = "Size:";
static QString STR_MSGCRC              = "CRC:";
static QString STR_MSGSECBLOCK         = "Security block:";
static QString STR_HIGHENDURAN         = "High endurance block:";


//----Char Edit dialog

//Dialog title
static QString STR_MSGEDITBUG          = "Edit Buffer";

//Dialog messages: NB should be of the same size (number of char)
static QString STR_MSGHEX              = " Hex     ";
static QString STR_MSGDECIMAL          = " Decimal ";
static QString STR_MSGCHAR             = " Char    ";

static QString STR_MSGINSNEWVAL        = "Insert the new value";
static QString STR_MSGENTERTEXT        = "Enter text from the address";

//Tool tips
static QString STR_TTHEX               = "Hexadecimal value";
static QString STR_TTDECIMAL           = "Decimal value";
static QString STR_TTCHAR              = "Char value";


//----Program Option dialog

//Dialog title
static QString STR_MSGPROGOPT          = "Program Options";

//Dialog messages
static QString STR_MSGRELOAD           = "Reload Files";
static QString STR_MSGREADPROG         = "Read Program memory (FLASH)";
static QString STR_MSGREADDATA         = "Read Data memory (EEPROM)";
static QString STR_MSGREADSERNUM       = "Read Seriial Number";
static QString STR_MSGBYTESWAP         = "Byte Swap";
static QString STR_MSGSERNUM           = "Set Serial Number";
static QString STR_MSGERASE            = "Erase";
static QString STR_MSGWRITEPROG        = "Write Program memory (FLASH)";
static QString STR_MSGWRITEDATA        = "Write Data memory (EEPROM)";
static QString STR_MSGWRITESEC         = "Write Security and Config bits";
static QString STR_MSGVERIFYSEC        = "Verify Security and Config bits";



//----Serial Number dialog

//Dialog title
static QString STR_MSGSERNUMCFG        = "Serial Number Config";

//Dialog messages: NB should be of the same size (number of char)
static QString STR_MSGADDRESS          = " Address ";
static QString STR_MSGSIZE2            = " Size    ";
static QString STR_MSGVALUE            = " Value   ";
static QString STR_MSGFORMAT           = " Format  ";

//next 2 probably can't be translated
static QString STR_MSGLITTLEEND        = "Little endian";
static QString STR_MSGBIGENDIAN        = "Big endian";

static QString STR_MSGOFFSET           = "Data memory offset";

//Tool tips

static QString STR_TTSNMEMADDR         = "Memory address";
static QString STR_TTSNSIZE            = "Size in bytes";
static QString STR_TTSNVALUE           = "Next value to be used";
static QString STR_TTSNOFFSET          = "Assume the Address relative to Data memory instead of Program memory";
static QString STR_TTLITTLEEND         = "Most significant Byte at high address";
static QString STR_TTBIGENDIAN         = "Most significant Byte at low address";


//----Retry dialog

//Button captions
#ifdef  WIN32
static QString STR_BTNABORT    = " &Abort ";
static QString STR_BTNRETRY    = " &Retry ";
static QString STR_BTNIGNORE   = " &Ignore ";
#else
static QString STR_BTNABORT    = " Abort ";
static QString STR_BTNRETRY    = " Retry ";
static QString STR_BTNIGNORE   = " Ignore ";
#endif

//Dialog title
static QString STR_MSGALERT    = "Alert";


//----Notes dialog

//Dialog title
static QString STR_MSGDEVNOTE  = "Edit Device Notes";


//----Progress dialog

//Dialog title
static QString STR_MSGSTATUS   = "Status";


//----Hardware check dialog
static QString STR_MSGHWCHECKDLG       = "Hardware Check";


//----About dialog
static QString STR_LBLNOSOUND          = "Disable sound";

static QString STR_LBLSKIPMSG          = "Don't show this window on startup";


//---- Oscillator Calibration Byte
static QString STR_MSGOSCCALIBCFG      = "Oscillator Calibration Byte";

static QString STR_READOSCCALIB        = "Read Osc.Calibration Byte";
static QString STR_OSCCALIBOPTION      = "Osc. Calibration Options...";
static QString STR_MSGREADCALIBOK      = "Read Osc.Calibration Byte successful";

static QString STR_MSGCHIPID   = " Chip Id";
static QString STR_MSGNOTE             = " Note  ";
static QString STR_MSGEEPSIZE  = "EEPROM Size";
static QString STR_MSGFLASHSIZE        = "FLASH Size";

static QString STR_MSGSNAUTOINC        = "Auto Increment";
static QString STR_TTSNAUTOINC         = "Increment the serial number after every 'set serial number' operation";

static QString STR_MSGBANKROLLOVER     = "Can't know bank roll-over without modify and write, do you want to write?";
static QString STR_BUFCHANGED3         = "Buffer changed. Save it before reading?";

static QString STR_TTOPENPROG  = "Open Program Memory (FLASH) File";
static QString STR_TTOPENDATA  = "Open Data Memory (EEPROM) File";
static QString STR_TTSAVEPROG  = "Save Program Memory (FLASH) File";
static QString STR_TTSAVEDATA  = "Save Data Memory (EEPROM) File";
static QString STR_TTNEWWIN    = "New Window";
static QString STR_TTPROGRAM   = "Launch program cycle";
static QString STR_TTSCRIPT    = "Execute script";
static QString STR_TTSETUP             = "Open setup dialog";
static QString STR_TTERASE             = "Erase all the device to FF";

static QString STR_SECBITS             = "Security and Configuration &Bits...";
static QString STR_TTSECBITS   = "Security and Configuration Bits";

static QString STR_LBLFUSEDLGHLP       = "Refer to device datasheet, please";
static QString STR_FUSEDLGNOTESET      = "Checked items means programmed";
static QString STR_FUSEDLGNOTECLR      = "UnChecked items means unprogrammed";

}

#endif
