#ifndef	INCLUDE_STRING_TABLE_H
#define	INCLUDE_STRING_TABLE_H

#define	STR_LANGUAGE_CODE	"english"

//Fill this with your reference, if it's empty "" it will be invisible
#define	MSG_TRANSLATORCREDITS	"Translator Credits:"
#define	MSG_TRANSLATORNAME		""

#define	STR_APPNAME_EXT		"Serial Device Programmer"
#define	STR_APPDOWNLOAD1	"Download last version of"
#define	STR_APPDOWNLOAD2	"at the address:"

#define	STR_NONAME		"No Name"

//Button captions
#ifdef	WIN32
#define	STR_BTNOK		" &OK "
#define	STR_BTNCANC		" &Cancel "
#define	STR_BTNHELP		" &Help "
#define	STR_BTNPROBE		" &Probe "
#else
#define STR_BTNOK		"  OK "
#define	STR_BTNCANC		"  Cancel "
#define	STR_BTNHELP		"  Help "
#define	STR_BTNPROBE		"  Probe "
#endif
#define	STR_BTNCHECKHW	" Hardware Check "

#define	STR_BTNNOTE		"Note"
#define	STR_BTNEDIT		"Edit"


//Dialog messages
#define	STR_MSGVERSION		"Version"
#define	STR_MSGYES		"Yes"
#define	STR_MSGNO		"No"
#define	STR_MSGOK		"Ok"
#define	STR_MSGTEST		"Test"
#define	STR_MSGFAILED		"Failed"
#define	STR_ABOUT		"About"
#define	STR_MSGPAGE		"Page"
#define	STR_MSGUNKNOWN		"Unknown"
#define STR_VGUIABOUT		"This program uses the V library.\nThe V library is copyright by Bruce E.Wampler.\nYou can find it and related license at\nhttp://www.objectcentral.com"
#define	STR_BUFCHANGED		"Buffer \"%s\" changed. Save it before closing?"
#define	STR_BUFCHANGED2		"Buffer changed. Save it before opening?"
#define	STR_NOTHINGSAVE		"Nothing to save"
#define	STR_NOTHINGLOAD		"Nothing to load"
#define	STR_NOTHINGPRINT	"Nothing to print"
#define	STR_NOTHINGWRITE	"Nothing to write"
#define	STR_NOTHINGVERIFY	"Nothing to verify"
#define	STR_BUSCALIBRA1		"Bus timing calibration.\nBe sure there are no application running other than "
#define	STR_BUSCALIBRA2		"\n(the CPU and hard disk have to be idle)\nThe calibration may take a couple of seconds.\nDo you want to run calibration now?"
#define	STR_BUSCALIBRAOK	"Calibration OK"
#define	STR_BUSCALIBRAFAIL	"Calibration failed"
#define	STR_MSGNEEDCALIB	"You need to run Calibration from the Options menu\nbefore any read/write operations"
#define	STR_MSGNEEDSETUP	"You need to run Setup from the Options menu\nbefore any read/write operations"
#define	STR_MSGREADING		"Reading..."
#define	STR_MSGREADOK		"Read successful\nDevice Size:"
#define	STR_ASKWRITE		"Are you sure you want to write the device?\nAll previous content will be lost"
#define	STR_MSGWRITING		"Writing..."
#define	STR_MSGVERIFING		"Verifying..."
#define	STR_MSGWRITEOK		"Write successful"
#define	STR_MSGWRITEFAIL	"Write failed"
#define	STR_MSGERASING		"Erasing..."
#define	STR_MSGERASEOK		"Erase successful"
#define	STR_MSGVERIFYFAIL1	"Cannot verify, device missing or bad device"
#define	STR_MSGVERIFYFAIL2	"Verify Failed"
#define	STR_MSGVERIFYOK		"Verify successful"
#define	STR_MSGPROGRAMOK	"Program succesful"
#define	STR_MSGPROGRAMFAIL	"Program Failed"
#define	STR_MSGDEVRESET		"Device reset"
#define	STR_MSGWRITINGSEC	"Writing security bits..."
#define	STR_MSGWRITESECFAIL	"Security bits write failed"
#define	STR_MSGREADINGSEC	"Reading security bits..."
#define	STR_MSGREADSECFAIL	"Security bits read failed"
#define	STR_MSGREADINGFUSE	"Reading configuration bits..."
#define	STR_MSGREADFUSEFAIL	"Configuration bits read failed"
#define	STR_MSGWRITINGFUSE	"Writing configuration bits..."
#define	STR_MSGWRITEFUSEFAIL	"Configuration bits write failed"

#define STR_MSGSCRIPTERROR	"Script error at line"
#define	STR_MSGSCRIPTBADCOMMAND  "Bad command"
#define	STR_MSGSCRIPTBADARGUMENT "Bad argument"
#define	STR_MSGSCRIPTARGMISSING	 "Argument missing"
#define	STR_MSGDEFAULTPAUSE	"Do you want to continue?"

#define	STR_BUFEMPTY		"Buffer empty"
#define	STR_OPNOTSUP		"Operation not supported"
#define	STR_MSGINSPARAM		"Insert parameters"
#define	STR_MSGBADPARAM		"Bad parameters"
#define	STR_MSGACCEPTCMD	"Accepting commands..."
#define	STR_MSGOPENFILE		"Open device content file"
#define	STR_MSGOPENPROGFILE	"Open program (FLASH) content file"
#define	STR_MSGOPENDATAFILE	"Open data (EEPROM) content file"
#define	STR_MSGFILENOTFOUND	"File not found"
#define	STR_MSGFILESAVEFAIL	"File save failed"
#define	STR_MSGFILESAVEAS	"Save Device Content File As"
#define	STR_MSGFILESAVEPROG	"Save Program (FLASH) Content File As"
#define	STR_MSGFILESAVEDATA	"Save Data (EEPROM) Content File As"
#define	STR_MSGCLOSEWINEXIT	"Close last window: do you want to exit?"
#define	STR_MSGCLOSEWINSAVE	"Buffer changed. Save it before closing?"
#define	STR_MSGOPENSCRIPT	"Open script file"


//Menu entries

//Main Menu
#define	STR_MENUFILE		"&File"	
#define	STR_MENUEDIT		"&Edit"
#define	STR_MENUDEVICE		"&Device"
#define	STR_MENUCMD			"&Command"
#define	STR_MENUSCRIPT		"Scri&pt"
#define	STR_MENUUTIL		"&Utility"
#define	STR_MENUSETUP		"&Setup"
#define	STR_MENUOPTIONS		"&Options"
#define	STR_MENUQMARK		"&?"

//Menu File
#define	STR_NEWWIN		"&New Window"
#define	STR_OPEN		"&Open Device File..."
#define	STR_OPENFLASH		"Open &Program (FLASH) File..."
#define	STR_OPENDATA		"Open &Data (EEPROM) File..."
#define	STR_SAVE		"&Save Device File"
#define	STR_SAVEAS		"Save Device File &As..."
#define	STR_SAVEFLASH		"Save Program (&FLASH) File As..."
#define	STR_SAVEDATA		"Save Data (&EEPROM) File As..."
#define	STR_RELOAD		"&Reload Files"
#define	STR_PRINT		"Prin&t..."
#define	STR_CLOSE		"&Close"
#define	STR_EXIT		"E&xit"

//Menu Edit
#define	STR_EDITNOTE		"Edit &Note..."
#define	STR_EDITBUFENA		"Edit Bu&ffer enabled"

//Menu Command
#define	STR_READALL		"&Read All"
#define	STR_READPROG		"Read Program (FLASH)"
#define	STR_READDATA		"Read Data (EEPROM)"
#define	STR_READSECBITS		"Read &Security and Configuration Bits"
#define	STR_WRITEALL		"&Write All"
#define	STR_WRITEPROG		"Write Program (&FLASH)"
#define	STR_WRITEDATA		"Write Data (EEPROM)"
#define	STR_WRITESECBITS	"Wr&ite Security and Configuration Bits"
#define	STR_VERIFYALL		"&Verify All"
#define	STR_VERIFYPROG		"Verify Program (FLASH)"
#define	STR_VERIFYDATA		"Verify Data (EEPROM)"
#define	STR_VERIFYSECBITS	"Verif&y Security and Configuration Bits"
#define	STR_ERASE		"&Erase"
#define	STR_GETINFO		"&GetInfo"
#define	STR_RESET		"Rese&t"
#define	STR_PROGRAM		"&Program"
#define	STR_PROGOPTION		"Program Options..."

//Menu Options
#define	STR_INTERFSETUP		"&Interface Setup..."
#define	STR_CALIBRATION		"&Calibration"
#define	STR_REMOTEMODE		"&Remote Mode"

//Menu Utility
#define	STR_CLEARBUF	"&Clear Buffer"
#define	STR_FILLBUF	"&Fill Buffer..."
#define	STR_DOUBLEBANK	"&Double Bank"
#define	STR_BYTESWAP	"&Byte Swap"
#define	STR_DOWNSERNUM	"Set Serial&Number"
#define	STR_SERNUMCONF	"SerialNumber C&onfig..."

//Menu Help
#define	STR_MENUHELP	"&Help"
#define	STR_MENUABOUT	"&About"

//Menu Options
#define	STR_MENULOG		"&Log File..."

//Menu Script
#define	STR_EDITSCR		"&Edit..."
#define	STR_LOADSCR		"&Load and Run..."
#define	STR_AUTOLOADSCR	"&Auto Load"
#define	STR_RUNSCR		"&Run"

//Tool Bar

//Dialog Messages
#define	STR_MSGDEVTYPE	"Dev.Type"

//Tool tips
#define	STR_TTOPENFILE	"Open Device File"
#define	STR_TTSAVEFILE	"Save Device File"
#define	STR_TTPRINT	"Print Buffer"
#define	STR_TTRELOAD	"Reload Files"
#define	STR_TTREADALL	"Read Device"
#define	STR_TTREADPROG	"Read Program Memory (FLASH)"
#define	STR_TTREADDATA	"Read Data Memory (EEPROM)"
#define	STR_TTREADSEC	"Read Security and Configuration Bits"
#define	STR_TTWRITEALL	"Write Device"
#define	STR_TTWRITEPROG	"Write Program Memory (FLASH)"
#define	STR_TTWRITEDATA	"Write Data Memory (EEPROM)"
//#define	STR_TTWRITESEC	"Write Security and Configuration Bits"
#define	STR_TTVERIFYPROG "Verify Program Memory (FLASH)"
#define	STR_TTVERIFYDATA "Verify Data Memory (EEPROM)"
#define	STR_TTVERIFYSEC	"Verify Security and Configuration Bits"

#define	STR_TTSELFAMILY	"Select device family"
#define	STR_TTSELTYPE	"Select device type"

#define	STR_TTEDITNOTE	"Edit device notes"


//Error Messages
#define	STR_DEVNOTRESP	"Device Not Responding"
#define	STR_DEVBADTYPE	"Bad device, select the correct type"
#define	STR_DEVUNKNOWN	"Device missing or unknown device"
#define	STR_DEVLOCKED	"Unable to probe, bad device or device locked"
#define	STR_OPABORTED	"User abort"
#define	STR_OPENFAILED	"Communication port not available"
#define	STR_ACCDENIED	"I/O access denied. You must be root to perform I/O"
#define	STR_NOTINST	"Programmer board not found"
#define	STR_HWERROR	"Hardware error, is the board properly connected?"
#define	STR_BUSBUSY	"Bus busy or hardware error"
#define	STR_I2CNOACK	"Missing Acknowledge from the device"
#define	STR_I2CNODEV	"Missing device"
#define	STR_I2CTIMEOUT	"Timeout on the bus"
#define	STR_I2CSTOPERR	"I2CBus Stop condition error (bad timing?)"
#define	STR_WRITEERR	"Write error"
#define	STR_BLANKCHECKERR "Blank check error"
#define	STR_ERRNO	"Error n."
#define	STR_SCRIPTERROR	"Script error"




//----Setup dialog

//Dialog title
#define	STR_DLGIOSETUP		"I/O port setup"
#define	STR_MSGINTSETUP		"Interface board Setup"

//Dialog messages
#define	STR_LBLCOMLPT		"COM/LPT interface"
#define	STR_LBLSERIAL		" Serial  "
#define	STR_LBLPARALLEL		" Parallel  "
#define	STR_LBLINTERFSEL	" Interface select "
#define	STR_LBLINTERFTYPE	" Interface type "
#define	STR_LBLCOMSELECT	" COM Port select "
#define	STR_LBLCOM1		" COM1  "
#define	STR_LBLCOM2		" COM2  "
#define	STR_LBLCOM3		" COM3  "
#define	STR_LBLCOM4		" COM4  "
#define	STR_LBLLPTSELECT	" LPT Port select "
#define	STR_LBLLPT1		" LPT1  "
#define	STR_LBLLPT2		" LPT2  "
#define	STR_LBLLPT3		" LPT3  "
#define	STR_LBLLPT4		" LPT4  "
#define	STR_LBLSELPOLARITY	" Select Polarity of the Control lines"
#define	STR_LBLINVRESET		" Invert Reset "
#define	STR_LBLINVSCK		" Invert SCKL  "
#define	STR_LBLINVDATAIN	" Invert D-IN  "
#define	STR_LBLINVDATAOUT	" Invert D-OUT "

//Tool tips

#define	STR_TTCOM1		"Select serial port COM1"
#define	STR_TTCOM2		"Select serial port COM2"
#define	STR_TTCOM3		"Select serial port COM3"
#define	STR_TTCOM4		"Select serial port COM4"
#define	STR_TTLPT1		"Select parallel port LPT1"
#define	STR_TTLPT2		"Select parallel port LPT2"
#define	STR_TTLPT3		"Select parallel port LPT3"


//----Fill dialog

//Dialog title
#define	STR_LBLFILLBUF		"Fill Buffer"

//Dialog messages: NB should be of the same size (number of char)
#define	STR_LBLFROM		" From  "
#define	STR_LBLTO		" To    "
#define	STR_LBLVALUE		" Value "



//----Fuse dialog

//Button captions
#ifdef	WIN32
#define	STR_BTNCLRALL		" C&lear All "
#define	STR_BTNSETALL		" &Set All "
#define	STR_BTNWRITE		" &Write "
#define	STR_BTNREAD		" &Read "
#else
#define	STR_BTNCLRALL		" Clear All "
#define	STR_BTNSETALL		" Set All "
#define	STR_BTNWRITE		" Write "
#define	STR_BTNREAD		" Read "
#endif

//Dialog title
#define	STR_MSGFUSEDLG		"Configuration and Security bits"

//Dialog messages: NB should be of the same size (number of char)
#define	STR_MSGFIRSTBLK		" First Block to Lock "
#define	STR_MSGNUMBLOCK		" N.of Blocks to Lock "
#define	STR_MSGHIGHENDBLK	" High endurance block"

//Tool tips
#define	STR_TTCLRALL		"All bits to '0'"
#define	STR_TTSETALL		"All bits to '1'"
#define	STR_TTWRITE		"Write bits to the device"
#define	STR_TTREAD		"Read bits from the device"


//----Info dialog

//Button captions
#ifdef	WIN32
#define	STR_BTNCLOSE		" &Close "
#else
#define	STR_BTNCLOSE		"  Close "
#endif

//Dialog title
#define	STR_MSGDEVINFO		"Device Info"

//Dialog messages
#define	STR_MSGBANKROLL		"Bank roll-over capability:"
#define	STR_MSGSIZE		"Size:"
#define	STR_MSGCRC		"CRC:"
#define	STR_MSGSECBLOCK		"Security block:"
#define	STR_HIGHENDURAN		"High endurance block:"


//----Char Edit dialog

//Dialog title
#define	STR_MSGEDITBUG		"Edit Buffer"

//Dialog messages: NB should be of the same size (number of char)
#define	STR_MSGHEX		" Hex     "
#define	STR_MSGDECIMAL		" Decimal "
#define	STR_MSGCHAR		" Char    "

#define	STR_MSGINSNEWVAL	"Insert the new value"
#define	STR_MSGENTERTEXT	"Enter text from the address"

//Tool tips
#define	STR_TTHEX		"Hexadecimal value"
#define	STR_TTDECIMAL		"Decimal value"
#define	STR_TTCHAR		"Char value"


//----Program Option dialog

//Dialog title
#define	STR_MSGPROGOPT		"Program Options"

//Dialog messages
#define	STR_MSGRELOAD		"Reload Files"
#define	STR_MSGREADPROG		"Read Program memory (FLASH)"
#define	STR_MSGREADDATA		"Read Data memory (EEPROM)"
#define	STR_MSGREADSERNUM	"Read Seriial Number"
#define	STR_MSGBYTESWAP		"Byte Swap"
#define	STR_MSGSERNUM		"Set Serial Number"
#define	STR_MSGERASE		"Erase"
#define	STR_MSGWRITEPROG	"Write Program memory (FLASH)"
#define	STR_MSGWRITEDATA	"Write Data memory (EEPROM)"
#define	STR_MSGWRITESEC		"Write Security and Config bits"
#define	STR_MSGVERIFYSEC	"Verify Security and Config bits"



//----Serial Number dialog

//Dialog title
#define	STR_MSGSERNUMCFG	"Serial Number Config"

//Dialog messages: NB should be of the same size (number of char)
#define	STR_MSGADDRESS		" Address "
#define	STR_MSGSIZE2		" Size    "
#define	STR_MSGVALUE		" Value   "
#define	STR_MSGFORMAT		" Format  "

//next 2 probably can't be translated
#define	STR_MSGLITTLEEND	"Little endian"
#define	STR_MSGBIGENDIAN	"Big endian"

#define	STR_MSGOFFSET		"Data memory offset"

//Tool tips

#define	STR_TTSNMEMADDR		"Memory address"
#define	STR_TTSNSIZE		"Size in bytes"
#define STR_TTSNVALUE		"Next value to be used"
#define	STR_TTSNOFFSET		"Assume the Address relative to Data memory instead of Program memory"
#define	STR_TTLITTLEEND		"Most significant Byte at high address"
#define	STR_TTBIGENDIAN		"Most significant Byte at low address"


//----Retry dialog

//Button captions
#ifdef	WIN32
#define	STR_BTNABORT	" &Abort "
#define	STR_BTNRETRY	" &Retry "
#define	STR_BTNIGNORE	" &Ignore "
#else
#define	STR_BTNABORT	" Abort "
#define	STR_BTNRETRY	" Retry "
#define	STR_BTNIGNORE	" Ignore "
#endif

//Dialog title
#define	STR_MSGALERT	"Alert"


//----Notes dialog

//Dialog title
#define	STR_MSGDEVNOTE	"Edit Device Notes"


//----Progress dialog

//Dialog title
#define	STR_MSGSTATUS	"Status"


//----Hardware check dialog
#define	STR_MSGHWCHECKDLG	"Hardware Check"


//----About dialog
#define	STR_LBLNOSOUND		"Disable sound"


//---- Oscillator Calibration Byte
#define	STR_MSGOSCCALIBCFG	"Oscillator Calibration Byte"

#define	STR_READOSCCALIB	"Read Osc.Calibration Byte"
#define	STR_OSCCALIBOPTION	"Osc. Calibration Options..."
#define	STR_MSGREADCALIBOK	"Read Osc.Calibration Byte successful"

#define	STR_MSGCHIPID	" Chip Id"
#define STR_MSGNOTE		" Note   "
#define	STR_MSGEEPSIZE	"EEPROM Size"
#define	STR_MSGFLASHSIZE	"FLASH Size"

#define	STR_MSGSNAUTOINC	"Auto Increment"
#define	STR_TTSNAUTOINC		"Increment the serial number after every 'set serial number' operation"

#define	STR_MSGBANKROLLOVER	"Can't know bank roll-over without modify and write, do you want to write?"
#define	STR_BUFCHANGED3		"Buffer changed. Save it before reading?"

#define STR_TTOPENPROG	"Open Program Memory (FLASH) File"
#define STR_TTOPENDATA	"Open Data Memory (EEPROM) File"
#define STR_TTSAVEPROG	"Save Program Memory (FLASH) File"
#define STR_TTSAVEDATA	"Save Data Memory (EEPROM) File"
#define STR_TTNEWWIN	"New Window"
#define STR_TTPROGRAM	"Launch program cycle"
#define STR_TTSCRIPT	"Execute script"
#define STR_TTSETUP		"Open setup dialog"
#define STR_TTERASE		"Erase all the device to FF"

#define STR_SECBITS		"Security and Configuration &Bits..."
#define STR_TTSECBITS	"Security and Configuration Bits"

#define STR_LBLFUSEDLGHLP	"Refer to device datasheet, please"
#define	STR_FUSEDLGNOTESET	"Checked items means programmed"
#define	STR_FUSEDLGNOTECLR	"UnChecked items means unprogrammed"

#endif
