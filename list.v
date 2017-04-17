//==========================================================================================================================
PonyProg-2.08d/e2app.cpp
PonyProg-2.08d/e2app.h

keine ui elemente 
//==========================================================================================================================
PonyProg-2.08d/e2dlg.cpp
PonyProg-2.08d/e2dlg.h

//@V@:BeginDialogCmd DefaultCmds
static DialogCmd DefaultCmds[] =
{
	{C_Label, lblMainMsg, 0,"X",NoList,CA_MainMsg,isSens,NoFrame, 0, 0},

	{C_Frame,frmPortsel,0,STR_LBLCOMLPT,NoList,CA_None,isSens,NoFrame,0,lblMainMsg},
	{C_RadioButton,rdbComPort,0,STR_LBLSERIAL,NoList,CA_None,isSens,frmPortsel,0,0},
	{C_Blank,blk0,0,"  ",NoList,CA_None,isSens,frmPortsel,rdbComPort,0},
	{C_Blank,blk1,0,"  ",NoList,CA_None,isSens,frmPortsel,blk0,0},
	{C_Blank,blk2,0,"  ",NoList,CA_None,isSens,frmPortsel,blk1,0},
	{C_RadioButton,rdbLptPort,0,STR_LBLPARALLEL,NoList,CA_None,isSens,frmPortsel,blk2,0},

	{C_Frame,frmInterf,0,STR_LBLINTERFSEL,NoList,CA_NoBorder,isSens,NoFrame,0,frmPortsel},
	{C_ComboBox,cbxInterf,0,STR_LBLINTERFTYPE,(void*)interfList,CA_None,isSens,frmInterf,0,0},

	{C_Frame,frmComsel,0,STR_LBLCOMSELECT,NoList,CA_None,isSens,frmInterf,0,cbxInterf},
	{C_RadioButton,rdbCom1,0,STR_LBLCOM1,NoList,CA_None,isSens,frmComsel,0,0,		0, STR_TTCOM1},
	{C_RadioButton,rdbCom2,0,STR_LBLCOM2,NoList,CA_None,isSens,frmComsel,0,rdbCom1, 0, STR_TTCOM2},
	{C_RadioButton,rdbCom3,0,STR_LBLCOM3,NoList,CA_None,isSens,frmComsel,rdbCom1,0, 0, STR_TTCOM3},
	{C_RadioButton,rdbCom4,0,STR_LBLCOM4,NoList,CA_None,isSens,frmComsel,rdbCom1,rdbCom1, 0, STR_TTCOM4},

	{C_Frame,frmInterfL,0,STR_LBLINTERFSEL,NoList,CA_NoBorder,isSens,NoFrame,frmInterf,frmPortsel},
	{C_ComboBox,cbxInterfL,0,STR_LBLINTERFTYPE,(void*)interfListL,CA_None,isSens,frmInterfL,0,0},

	{C_Frame,frmLptsel,0,STR_LBLLPTSELECT,NoList,CA_None,isSens,frmInterfL,0,cbxInterfL},
	{C_RadioButton,rdbLpt1,0,STR_LBLLPT1,NoList,CA_None,isSens,frmLptsel,0,0,		0, STR_TTLPT1},
	{C_RadioButton,rdbLpt2,0,STR_LBLLPT2,NoList,CA_None,isSens,frmLptsel,0,rdbLpt1,	0, STR_TTLPT2},
	{C_RadioButton,rdbLpt3,0,STR_LBLLPT3,NoList,CA_None,isSens,frmLptsel,rdbLpt1,0,	0, STR_TTLPT3},

	{C_Frame,frmPolsel,0,STR_LBLSELPOLARITY,NoList,CA_None,isSens,NoFrame,0,frmInterf},
	{C_Label,lblPol1,0,STR_LBLSELPOLARITY,NoList,CA_None,isSens,frmPolsel,0,0},
	{C_CheckBox,chkPol1,0,STR_LBLINVRESET,NoList,CA_None,isSens,frmPolsel,0,lblPol1},
	{C_CheckBox,chkPol2,0,STR_LBLINVSCK,NoList,CA_None,isSens,frmPolsel,0,chkPol1},
	{C_CheckBox,chkPol3,0,STR_LBLINVDATAIN,NoList,CA_None,isSens,frmPolsel,chkPol1,lblPol1},
	{C_CheckBox,chkPol4,0,STR_LBLINVDATAOUT,NoList,CA_None,isSens,frmPolsel,chkPol1,chkPol1},

	{C_Button,M_Cancel,0,STR_BTNCANC, NoList,CA_None,isSens,NoFrame,0,frmPolsel},
	{C_Button,M_OK,0,STR_BTNOK,	NoList,CA_DefaultButton,isSens,NoFrame,M_Cancel,frmPolsel},

	{C_Button,btnTestDlg,0,STR_BTNPROBE,NoList,CA_None,isSens,NoFrame,M_OK,frmPolsel},
//	{C_Button,btnCheckHw,0,STR_BTNCHECKHW,NoList,CA_None,isSens,NoFrame,btnTestDlg,frmPolsel},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd

/********************************** PROGRESS DIALOG ********************************/

static CommandObject ProgressDlg[] = {
	{C_Label, lblMainMsg, 0,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",NoList,CA_MainMsg,isSens,NoFrame, 0, 0},
	{C_ProgressBar, pbrProgress, 0, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", NoList, CA_Horizontal|CA_Percent,isSens,NoFrame, 0, lblMainMsg},	// Horiz, with label
	{C_Button, M_Cancel, 0, STR_BTNABORT, NoList,CA_None,isSens,NoFrame,0,pbrProgress},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};


//==========================================================================================================================
PonyProg-2.08d/e2awinfo.cpp
PonyProg-2.08d/e2awinfo.h

keine ui elemente 
//==========================================================================================================================
PonyProg-2.08d/e2cmdw.cpp
PonyProg-2.08d/e2cmdw.h

//@V@:BeginCmdPane ToolBar
static CommandObject ToolBar[] =
{
	{C_Frame,frmToolBar,0,"",NoList,CA_None,isSens,NoFrame,0,0},

	{C_IconButton,icbNew,0,"New",&newI,CA_None,isSens,frmToolBar,0,0, 0,STR_TTNEWWIN},

	{C_Blank,blkBlank1,0," ",NoList,CA_None,isSens,frmToolBar,icbNew,0},

	{C_IconButton,icbOpen,0,"Open",&openI,CA_None,isSens,frmToolBar,blkBlank1,0, 0,STR_TTOPENFILE},
	{C_IconButton,icbOpenFlash,0,"Open Flash",&openflashI,CA_None,isSens,frmToolBar,icbOpen,0, 0,STR_TTOPENPROG},
	{C_IconButton,icbOpenEep,0,"Open Eep",&openeepI,CA_None,isSens,frmToolBar,icbOpenFlash,0, 0,STR_TTOPENDATA},

	{C_Blank,blkBlank2,0," ",NoList,CA_None,isSens,frmToolBar,icbOpenEep,0},

	{C_IconButton,icbSave,0,"Save",&saveI,CA_None,isSens,frmToolBar,blkBlank2,0, 0,STR_TTSAVEFILE},
	{C_IconButton,icbSaveFlash,0,"Save Flash",&saveflashI,CA_None,isSens,frmToolBar,icbSave,0, 0,STR_TTSAVEPROG},
	{C_IconButton,icbSaveEep,0,"Save Eep",&saveeepI,CA_None,isSens,frmToolBar,icbSaveFlash,0, 0,STR_TTSAVEDATA},

	{C_Blank,blkBlank3,0," ",NoList,CA_None,isSens,frmToolBar,icbSaveEep,0},

	{C_IconButton,icbReload,0,"Reload",&reloadI,CA_None,isSens,frmToolBar,blkBlank3,0, 0,STR_TTRELOAD},
	{C_IconButton,icbScript,0,"Script",&scriptI,CA_None,isSens,frmToolBar,icbReload,0, 0,STR_TTSCRIPT},
	{C_IconButton,icbPrint,0,"Print",&printI,CA_None,isSens,frmToolBar,icbScript,0, 0,STR_TTPRINT},
	{C_IconButton,icbSetup,0,"Setup",&setupI,CA_None,isSens,frmToolBar,icbPrint,0, 0,STR_TTSETUP},
	{C_IconButton,icbHelp,0,"Help",&helpI,CA_None,isSens,frmToolBar,icbSetup,0, 0,STR_ABOUT " " APPNAME},

	{C_Frame,frmToolBar2,0,"",NoList,CA_None,isSens,NoFrame,0,0},
//	{C_Label, lblEEPType,0,"   " STR_MSGDEVTYPE,NoList,CA_None,isSens,frmToolBar2,0,0},
	{C_Icon,icoChip,0,STR_MSGDEVTYPE,&chipI,CA_None,isSens,frmToolBar2,0,0},

	{C_ComboBox,cbxEEPType,0,"Chip Type",comboList1,CA_None,isSens,frmToolBar2,icoChip,0, 0,STR_TTSELFAMILY},
//	{C_Label, lblEEPSubType,0," ",NoList,CA_None,isSens,frmToolBar2,cbxEEPType,0},
	{C_ComboBox,cbxEEPSubType,0,"Chip SubType",comboList2,CA_None,isSens,frmToolBar2,cbxEEPType,0, 0,STR_TTSELTYPE},

//	{C_Label, lblLegend,0,"Legend",NoList,CA_NoBorder,isSens,NoFrame,0,0, 0, "Program Memory = Blue;  Data Memory = Green"},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndCmdPane

//@V@:BeginCmdPane ToolBar2
static CommandObject ToolBar2[] =
{
	{C_Frame,frmToolBar3,0,"",NoList,CA_None,isSens,NoFrame,0,0},

	{C_IconButton,icbRead,0,"Read All",&readI,CA_None,isSens,frmToolBar3,0,0,  0,STR_TTREADALL},
	{C_IconButton,icbReadFlash,0,"Read Flash",&readflashI,CA_None,isSens,frmToolBar3,icbRead,0,  0,STR_TTREADPROG},
	{C_IconButton,icbReadEep,0,"Read Eep",&readeepI,CA_None,isSens,frmToolBar3,icbReadFlash,0,  0,STR_TTREADDATA},

	{C_Blank,blkBlank2,0," ",NoList,CA_None,isSens,frmToolBar3,icbReadEep,0},

	{C_IconButton,icbWrite,0,"Write",&writeI,CA_None,isSens,frmToolBar3,blkBlank2,0, 0,STR_TTWRITEALL},
	{C_IconButton,icbWriteFlash,0,"Write Flash",&writeflashI,CA_None,isSens,frmToolBar3,icbWrite, 0, 0,STR_TTWRITEPROG},
	{C_IconButton,icbWriteEep,0,"Write Eep",&writeeepI,CA_None,isSens,frmToolBar3,icbWriteFlash,0, 0,STR_TTWRITEDATA},

	{C_Blank,blkBlank3,0," ",NoList,CA_None,isSens,frmToolBar3,icbWriteEep,0},

	{C_IconButton,icbErase,0,"Erase",&eraseI,CA_None,isSens,frmToolBar3,blkBlank3,0, 0, STR_TTERASE},
	{C_IconButton,icbLock,0,"Lock",&lockI,CA_None,isSens,frmToolBar3,icbErase,0, 0, STR_TTSECBITS},
	{C_IconButton,icbSernum,0,"Serial Number",&sernumI,CA_None,isSens,frmToolBar3,icbLock,0, 0, STR_MSGSERNUM},
	{C_IconButton,icbProgram,0,"Program",&programI,CA_None,isSens,frmToolBar3,icbSernum,0, 0,STR_TTPROGRAM},

	{C_Frame,frmToolBar4,0,"",NoList,CA_None,isSens,NoFrame,0,0},

	{C_IconButton,icbEdit,0,"Edit Note",&editI,CA_None,isSens,frmToolBar4,0,0, 0,STR_TTEDITNOTE},
	{C_Text,txtComment,0,"",(void *)"X                                                                 ",CA_None,isSens,frmToolBar4,icbEdit,0, 0,STR_MSGNOTE},
#ifndef	_WINDOWS
	//Workaround for X11 to allocate space for C_Text
	{C_Label,lblComment,0,STR_BTNNOTE "X                                                                 ",NoList,CA_Hidden,isSens,frmToolBar4,icbEdit,0},
#endif
	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndCmdPane

#define	STATUSBAR_FORM	"    Size     0 Bytes     CRC  0000h      "
#define	STATUSBAR_PRINT	"    Size %5ld Bytes     CRC  %04Xh     %c"

//@V@:BeginStatPane StatBar
static vStatus StatBar[] =
{
	{APPNAME "   ", lblTestStat, CA_NoBorder, isSens, 0},
	{"                           ", lblStringID, CA_NoBorder, isSens, 0},
	{STATUSBAR_FORM, lblEEPInfo, CA_NoBorder, isSens, 0},

	{0,0,0,0,0}
};
//@V@:EndStatPane


DialogCmd Notes[] =
{
#ifdef	_LINUX_
	{C_Label,lblMainMsg,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},
#else
	{C_Blank,lblMainMsg,0," ",NoList,CA_None,isSens,NoFrame,0,0},
#endif

	{C_Label, lblStrID,0,STR_MSGCHIPID,NoList,CA_None,isSens,NoFrame,0,lblMainMsg},
	{C_TextIn,txiStrID,0,"",NoList,CA_None,isSens,NoFrame,lblStrID,lblMainMsg,32},

	{C_Label, lblCommnt,0,STR_MSGNOTE,NoList,CA_None,isSens,NoFrame,0,lblStrID},
	{C_TextIn,txiCommnt,0,"",NoList,CA_None,isSens,NoFrame,lblCommnt,lblStrID,80},

	{C_Button, M_Cancel, 0,STR_BTNCANC,	NoList,CA_None, isSens,NoFrame,0, lblCommnt},
	{C_Button, M_OK, 0,STR_BTNOK, NoList, CA_DefaultButton, isSens, NoFrame, M_Cancel, lblCommnt},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};



//==========================================================================================================================
PonyProg-2.08d/aboutmdlg.cpp
PonyProg-2.08d/aboutmdlg.h

static CommandObject AboutVguiDlg[] = {
	{C_Frame,frmAboutVgui,0,"V GUI",NoList,CA_NoBorder,isSens,NoFrame,0,0},
	{C_Icon,icoVgui,0,"V GUI",&aboutI,CA_None,isSens,frmAboutVgui,0,0},
	{C_Text,txtVgui,0,STR_VGUIABOUT,NoList,CA_NoBorder,isSens,frmAboutVgui,icoVgui,0},

	{C_Button,M_OK,0,STR_BTNOK,NoList,CA_DefaultButton,isSens,NoFrame,0,frmAboutVgui},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};

static DialogCmd AboutDlg[] = {
	{C_Text,lblAbout0,0,APPNAME " - " APPNAME_EXT "\n    " STR_MSGVERSION "  " APPVERSION "  " __DATE__,
			NoList,CA_NoBorder,isSens,NoFrame,icoPonyProg,0},
	{C_Frame,frmAbout1,0,"About",NoList,CA_NoBorder,isSens,NoFrame,0,lblAbout0},
	{C_Icon,icoPonyProg,0,APPNAME,&ponyI,CA_None,isSens,frmAbout1,0,0},
	{C_Text,lblAbout1,0,
		"Copyright (C) " COPYRIGHTYEAR "  by  " AUTHORNAME "\n\n" STR_APPDOWNLOAD1 " " APPNAME " " STR_APPDOWNLOAD2 "\n" AUTHORWEB,
			NoList,CA_None,isSens,frmAbout1,icoPonyProg,0},
	{C_Text,lblAbout2,0,
		MSG_TRANSLATORCREDITS "\n" MSG_TRANSLATORNAME,
			NoList,CA_None,isSens,frmAbout1,icoPonyProg,lblAbout1},
	{C_Button,M_OK,0,STR_BTNOK,	NoList,CA_DefaultButton,isSens,NoFrame,0,frmAbout1},

	{C_Button,btnAboutHelp,0,STR_BTNHELP,NoList,CA_None,isSens,NoFrame,M_OK,frmAbout1},

	{C_Button,btnAboutVgui,0,STR_ABOUT " V lib",NoList,CA_None,isSens,NoFrame,btnAboutHelp,frmAbout1},
#ifdef	WIN32
	{C_Blank,blkBlank,0,"      ",NoList,CA_None,isSens,NoFrame,btnAboutVgui,frmAbout1},
	{C_CheckBox,chkSound,0,STR_LBLNOSOUND,NoList,CA_None,isSens,NoFrame,blkBlank,frmAbout1},
	{C_CheckBox,chkSkipMsg,0,STR_LBLSKIPMSG,NoList,CA_None,isSens,NoFrame,chkSound,frmAbout1},
#else
	{C_CheckBox,chkSkipMsg,0,STR_LBLSKIPMSG,NoList,CA_None,isSens,NoFrame,btnAboutVgui,frmAbout1},
#endif
	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};

//==========================================================================================================================
PonyProg-2.08d/progoption.cpp
PonyProg-2.08d/progoption.h


//@V@:BeginDialogCmd DefaultCmds
static DialogCmd ProgDialogCmds[] =
{
	{C_Label, lblMainMsg, 0,"X",NoList,CA_MainMsg,isSens,NoFrame, 0, 0},

	{C_Frame, frmFuses,0,"Fuses",NoList,CA_None,isSens,NoFrame,0,lblMainMsg},

	{C_CheckBox, idRELOAD, 0,STR_MSGRELOAD,NoList,CA_None,isSens,frmFuses,0,0},
	{C_CheckBox, idREADFLASH, 0,STR_MSGREADPROG,NoList,CA_None,isSens,frmFuses,0,idRELOAD},
	{C_CheckBox, idREADEEP, 0,STR_MSGREADDATA,NoList,CA_None,isSens,frmFuses,0,idREADFLASH},
	{C_CheckBox, idBYTESWAP, 0,STR_MSGBYTESWAP,NoList,CA_None,isSens,frmFuses,0,idREADEEP},
	{C_CheckBox, idSETID, 0,STR_MSGSERNUM,NoList,CA_None,isSens,frmFuses,0,idBYTESWAP},
	{C_CheckBox, idREADOSCCAL, 0,STR_READOSCCALIB,NoList,CA_None,isSens,frmFuses,0,idSETID},
	{C_CheckBox, idERASE, 0,STR_MSGERASE,NoList,CA_None,isSens,frmFuses,0,idREADOSCCAL},
	{C_CheckBox, idFLASH, 0,STR_MSGWRITEPROG,NoList,CA_None,isSens,frmFuses,0,idERASE},
	{C_CheckBox, idEEPROM, 0,STR_MSGWRITEDATA,NoList,CA_None,isSens,frmFuses,0,idFLASH},
	{C_CheckBox, idLOCK, 0,STR_MSGWRITESEC,NoList,CA_None,isSens,frmFuses,0,idEEPROM},

	{C_Blank, blk1, 0, " ",NoList,CA_None,isSens,NoFrame,0,frmFuses},

	{C_Button,M_Cancel,0, STR_BTNCANC, NoList,CA_None,isSens,NoFrame,0,blk1},
	{C_Button,M_OK,0, STR_BTNOK, NoList,CA_DefaultButton,isSens,NoFrame,M_Cancel,blk1},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd


//==========================================================================================================================
PonyProg-2.08d/fusemdlg.cpp
PonyProg-2.08d/fusemdlg.h


//@V@:BeginDialogCmd DefaultCmds
static DialogCmd FuseDialogCmds[] =
{
	{C_Label, lblMainMsg, 0,"X",NoList,CA_MainMsg,isSens,NoFrame, 0, 0},

	{C_Frame, frmLock,0,"Lock",NoList,CA_None,isSens,NoFrame,0,lblMainMsg},

	{C_Frame, frmLock0,0,"",NoList,CA_NoBorder,isSens,frmLock,0,0},
	{C_CheckBox, chk7_L0, 0," 7",NoList,CA_None,isSens,frmLock0,0,0},
	{C_CheckBox, chk6_L0, 0," 6",NoList,CA_None,isSens,frmLock0,chk7_L0,0},
	{C_CheckBox, chk5_L0, 0," 5",NoList,CA_None,isSens,frmLock0,chk6_L0,0},
	{C_CheckBox, chk4_L0, 0," 4",NoList,CA_None,isSens,frmLock0,chk5_L0,0},
	{C_CheckBox, chk3_L0, 0," 3",NoList,CA_None,isSens,frmLock0,chk4_L0,0},
	{C_CheckBox, chk2_L0, 0," 2",NoList,CA_None,isSens,frmLock0,chk3_L0,0},
	{C_CheckBox, chk1_L0, 0," 1",NoList,CA_None,isSens,frmLock0,chk2_L0,0},
	{C_CheckBox, chk0_L0, 0," 0",NoList,CA_None,isSens,frmLock0,chk1_L0,0},

	{C_Frame, frmLock1,0,"",NoList,CA_NoBorder,isSens,frmLock,0,frmLock0},
	{C_CheckBox, chk7_L1, 0," 7",NoList,CA_None,isSens,frmLock1,0,0},
	{C_CheckBox, chk6_L1, 0," 6",NoList,CA_None,isSens,frmLock1,chk7_L1,0},
	{C_CheckBox, chk5_L1, 0," 5",NoList,CA_None,isSens,frmLock1,chk6_L1,0},
	{C_CheckBox, chk4_L1, 0," 4",NoList,CA_None,isSens,frmLock1,chk5_L1,0},
	{C_CheckBox, chk3_L1, 0," 3",NoList,CA_None,isSens,frmLock1,chk4_L1,0},
	{C_CheckBox, chk2_L1, 0," 2",NoList,CA_None,isSens,frmLock1,chk3_L1,0},
	{C_CheckBox, chk1_L1, 0," 1",NoList,CA_None,isSens,frmLock1,chk2_L1,0},
	{C_CheckBox, chk0_L1, 0," 0",NoList,CA_None,isSens,frmLock1,chk1_L1,0},

	{C_Frame, frmLock2,0,"",NoList,CA_NoBorder,isSens,frmLock,0,frmLock1},
	{C_CheckBox, chk7_L2, 0," 7",NoList,CA_None,isSens,frmLock2,0,0},
	{C_CheckBox, chk6_L2, 0," 6",NoList,CA_None,isSens,frmLock2,chk7_L2,0},
	{C_CheckBox, chk5_L2, 0," 5",NoList,CA_None,isSens,frmLock2,chk6_L2,0},
	{C_CheckBox, chk4_L2, 0," 4",NoList,CA_None,isSens,frmLock2,chk5_L2,0},
	{C_CheckBox, chk3_L2, 0," 3",NoList,CA_None,isSens,frmLock2,chk4_L2,0},
	{C_CheckBox, chk2_L2, 0," 2",NoList,CA_None,isSens,frmLock2,chk3_L2,0},
	{C_CheckBox, chk1_L2, 0," 1",NoList,CA_None,isSens,frmLock2,chk2_L2,0},
	{C_CheckBox, chk0_L2, 0," 0",NoList,CA_None,isSens,frmLock2,chk1_L2,0},

	{C_Frame, frmLock3,0,"",NoList,CA_NoBorder,isSens,frmLock,0,frmLock2},
	{C_CheckBox, chk7_L3, 0," 7",NoList,CA_None,isSens,frmLock3,0,0},
	{C_CheckBox, chk6_L3, 0," 6",NoList,CA_None,isSens,frmLock3,chk7_L3,0},
	{C_CheckBox, chk5_L3, 0," 5",NoList,CA_None,isSens,frmLock3,chk6_L3,0},
	{C_CheckBox, chk4_L3, 0," 4",NoList,CA_None,isSens,frmLock3,chk5_L3,0},
	{C_CheckBox, chk3_L3, 0," 3",NoList,CA_None,isSens,frmLock3,chk4_L3,0},
	{C_CheckBox, chk2_L3, 0," 2",NoList,CA_None,isSens,frmLock3,chk3_L3,0},
	{C_CheckBox, chk1_L3, 0," 1",NoList,CA_None,isSens,frmLock3,chk2_L3,0},
	{C_CheckBox, chk0_L3, 0," 0",NoList,CA_None,isSens,frmLock3,chk1_L3,0},


	{C_Frame, frmFuses,0,"Fuses",NoList,CA_None,isSens,NoFrame,0,frmLock},

	{C_Frame, frmFuses0,0,"",NoList,CA_NoBorder,isSens,frmFuses,0,0},
	{C_CheckBox, chk7_F0, 0," 7",NoList,CA_None,isSens,frmFuses0,0,0},
	{C_CheckBox, chk6_F0, 0," 6",NoList,CA_None,isSens,frmFuses0,chk7_F0,0},
	{C_CheckBox, chk5_F0, 0," 5",NoList,CA_None,isSens,frmFuses0,chk6_F0,0},
	{C_CheckBox, chk4_F0, 0," 4",NoList,CA_None,isSens,frmFuses0,chk5_F0,0},
	{C_CheckBox, chk3_F0, 0," 3",NoList,CA_None,isSens,frmFuses0,chk4_F0,0},
	{C_CheckBox, chk2_F0, 0," 2",NoList,CA_None,isSens,frmFuses0,chk3_F0,0},
	{C_CheckBox, chk1_F0, 0," 1",NoList,CA_None,isSens,frmFuses0,chk2_F0,0},
	{C_CheckBox, chk0_F0, 0," 0",NoList,CA_None,isSens,frmFuses0,chk1_F0,0},

	{C_Frame, frmFuses1,0,"",NoList,CA_NoBorder,isSens,frmFuses,0,frmFuses0},
	{C_CheckBox, chk7_F1, 0," 7",NoList,CA_None,isSens,frmFuses1,0,0},
	{C_CheckBox, chk6_F1, 0," 6",NoList,CA_None,isSens,frmFuses1,chk7_F1,0},
	{C_CheckBox, chk5_F1, 0," 5",NoList,CA_None,isSens,frmFuses1,chk6_F1,0},
	{C_CheckBox, chk4_F1, 0," 4",NoList,CA_None,isSens,frmFuses1,chk5_F1,0},
	{C_CheckBox, chk3_F1, 0," 3",NoList,CA_None,isSens,frmFuses1,chk4_F1,0},
	{C_CheckBox, chk2_F1, 0," 2",NoList,CA_None,isSens,frmFuses1,chk3_F1,0},
	{C_CheckBox, chk1_F1, 0," 1",NoList,CA_None,isSens,frmFuses1,chk2_F1,0},
	{C_CheckBox, chk0_F1, 0," 0",NoList,CA_None,isSens,frmFuses1,chk1_F1,0},

	{C_Frame, frmFuses2,0,"",NoList,CA_NoBorder,isSens,frmFuses,0,frmFuses1},
	{C_CheckBox, chk7_F2, 0," 7",NoList,CA_None,isSens,frmFuses2,0,0},
	{C_CheckBox, chk6_F2, 0," 6",NoList,CA_None,isSens,frmFuses2,chk7_F2,0},
	{C_CheckBox, chk5_F2, 0," 5",NoList,CA_None,isSens,frmFuses2,chk6_F2,0},
	{C_CheckBox, chk4_F2, 0," 4",NoList,CA_None,isSens,frmFuses2,chk5_F2,0},
	{C_CheckBox, chk3_F2, 0," 3",NoList,CA_None,isSens,frmFuses2,chk4_F2,0},
	{C_CheckBox, chk2_F2, 0," 2",NoList,CA_None,isSens,frmFuses2,chk3_F2,0},
	{C_CheckBox, chk1_F2, 0," 1",NoList,CA_None,isSens,frmFuses2,chk2_F2,0},
	{C_CheckBox, chk0_F2, 0," 0",NoList,CA_None,isSens,frmFuses2,chk1_F2,0},

	{C_Frame, frmFuses3,0,"",NoList,CA_NoBorder,isSens,frmFuses,0,frmFuses2},
	{C_CheckBox, chk7_F3, 0," 7",NoList,CA_None,isSens,frmFuses3,0,0},
	{C_CheckBox, chk6_F3, 0," 6",NoList,CA_None,isSens,frmFuses3,chk7_F3,0},
	{C_CheckBox, chk5_F3, 0," 5",NoList,CA_None,isSens,frmFuses3,chk6_F3,0},
	{C_CheckBox, chk4_F3, 0," 4",NoList,CA_None,isSens,frmFuses3,chk5_F3,0},
	{C_CheckBox, chk3_F3, 0," 3",NoList,CA_None,isSens,frmFuses3,chk4_F3,0},
	{C_CheckBox, chk2_F3, 0," 2",NoList,CA_None,isSens,frmFuses3,chk3_F3,0},
	{C_CheckBox, chk1_F3, 0," 1",NoList,CA_None,isSens,frmFuses3,chk2_F3,0},
	{C_CheckBox, chk0_F3, 0," 0",NoList,CA_None,isSens,frmFuses3,chk1_F3,0},

	{C_Blank, blk0, 0, " ",NoList,CA_None,isSens,NoFrame,0,frmFuses},

	{C_Frame, frmHlp, 0,"",NoList,CA_NoBorder,isSens,NoFrame,0,blk0},
	{C_CheckBox, chkHlp1, 1,STR_FUSEDLGNOTESET " (bit = 0)",NoList,CA_None,notSens,frmHlp,0,0},
	{C_CheckBox, chkHlp0, 0,STR_FUSEDLGNOTECLR " (bit = 1)",NoList,CA_None,notSens,frmHlp,chkHlp1,0},

	{C_Label, blk1, 0,STR_LBLFUSEDLGHLP,NoList,CA_None,isSens,NoFrame,0,frmHlp},
//	{C_Blank, blk1, 0, " ",NoList,CA_None,isSens,NoFrame,0,frmHlp},

	{C_Button,M_Cancel,0, STR_BTNCANC,NoList,CA_None,isSens,NoFrame,0,blk1},
	{C_Button,M_OK,0,STR_BTNOK,NoList,CA_DefaultButton,isSens,NoFrame,M_Cancel,blk1},

	{C_Blank, blk2, 0, " ",NoList,CA_None,isSens,NoFrame,M_OK,blk1},

//	{C_Button,btn0,0,STR_BTNCLRALL,	NoList,CA_None,isSens,NoFrame,blk2,blk1,	0, STR_TTCLRALL},
//	{C_Button,btn1,0,STR_BTNSETALL,	NoList,CA_None,isSens,NoFrame,btn0,blk1,	0, STR_TTSETALL},
	{C_Button,btnProg,0,STR_BTNWRITE, NoList,CA_None,isSens,NoFrame,blk2,blk1,	0, STR_TTWRITE},
	{C_Button,btnRead,0,STR_BTNREAD, NoList,CA_None,isSens,NoFrame,btnProg,blk1,	0, STR_TTREAD},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};

static DialogCmd BlockDialogCmds[] =
{
	{C_Label, lblMainMsg, 0,"X",NoList,CA_MainMsg,isSens,NoFrame, 0, 0},

	{C_Frame, frmFuses,0,"Blocks",NoList,CA_None,isSens,NoFrame,0,lblMainMsg},

	{C_Frame, frmFill,0,"",NoList,CA_None,isSens,frmFuses,0,0},
	{C_Label, lblFrom,0, STR_MSGFIRSTBLK,NoList,CA_None,isSens,frmFill,0,0},
	{C_Label, lblTo,  0, STR_MSGNUMBLOCK,NoList,CA_None,isSens,frmFill,0,lblFrom},
	{C_Label, lblVal, 0, STR_MSGHIGHENDBLK,NoList,CA_None,isSens,frmFill,0,lblTo},

	{C_TextIn,txiFrom,0,"",NoList,CA_None,isSens,frmFuses,frmFill,0,8},
	{C_TextIn,txiTo,  0,"",NoList,CA_None,isSens,frmFuses,frmFill,txiFrom,8},
	{C_TextIn,txiVal, 0,"",NoList,CA_None,isSens,frmFuses,frmFill,txiTo,8},

	{C_Blank, blk1, 0, " ",NoList,CA_None,isSens,NoFrame,0,frmFuses},

	{C_Button,M_Cancel,0, STR_BTNCANC,	NoList,CA_None,isSens,NoFrame,0,blk1},
	{C_Button,M_OK,0, STR_BTNOK, NoList,CA_DefaultButton,isSens,NoFrame,M_Cancel,blk1},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};


//==========================================================================================================================
PonyProg-2.08d/sernumdlg.cpp
PonyProg-2.08d/sernumdlg.h


//@V@:BeginDialogCmd DefaultCmds
static DialogCmd DefaultCmds[] =
{
//	{C_Label,lblSNMsg,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},

	{C_Frame, frmSN,0,"Serial Number",NoList,CA_None,isSens,NoFrame,0,0},
	{C_Label, lblLoc, 0, STR_MSGADDRESS, NoList,CA_None,isSens,frmSN,0,0},
	{C_Label, lblLen, 0, STR_MSGSIZE2, NoList,CA_None,isSens,frmSN,0,lblLoc},
	{C_Label, lblVal, 0, STR_MSGVALUE,NoList,CA_None,isSens,frmSN,0,lblLen},

	{C_TextIn,txiLoc, 0,"",NoList,CA_None,isSens,NoFrame,frmSN,0,10,STR_TTSNMEMADDR},
	{C_TextIn,txiLen, 0,"",NoList,CA_None,isSens,NoFrame,frmSN,txiLoc,10,STR_TTSNSIZE},
	{C_TextIn,txiVal, 0,"",NoList,CA_None,isSens,NoFrame,frmSN,txiLen,10,STR_TTSNVALUE},

	{C_CheckBox, chkMemOffset, 0,STR_MSGOFFSET,NoList,CA_None,isSens,NoFrame,0,frmSN, 0,STR_TTSNOFFSET},
	{C_CheckBox, chkAutoInc, 0,STR_MSGSNAUTOINC,NoList,CA_None,isSens,NoFrame,0,chkMemOffset, 0, STR_TTSNAUTOINC},

	{C_Frame,frmFormat,0,STR_MSGFORMAT,NoList,CA_None,isSens,NoFrame,0,chkAutoInc},
	{C_Label, lblFmt, 0, STR_MSGFORMAT, NoList,CA_None,isSens,frmFormat, 0,0},
	{C_RadioButton,rdbLittleEnd,0,STR_MSGLITTLEEND,NoList,CA_None,isSens,frmFormat,0,lblFmt, 0,STR_TTLITTLEEND},
	{C_RadioButton,rdbBigEnd,   0,STR_MSGBIGENDIAN,NoList,CA_None,isSens,frmFormat,rdbLittleEnd,lblFmt, 0,STR_TTBIGENDIAN},

	{C_Button, M_Cancel, 0, STR_BTNCANC, NoList,CA_None, isSens,NoFrame, 0, frmFormat},
	{C_Button, M_OK, 0, STR_BTNOK, NoList, CA_DefaultButton, isSens, NoFrame, M_Cancel, frmFormat},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd


static DialogCmd OscCalibCmds[] =
{
	{C_Frame, frmSN,0,"Osc.Calibration",NoList,CA_None,isSens,NoFrame,0,0},
	{C_Label, lblLoc, 0, STR_MSGADDRESS, NoList,CA_None,isSens,frmSN,0,0},
	{C_Label, lblVal, 0, STR_MSGVALUE,NoList,CA_None,isSens,frmSN,0,lblLoc},

	{C_TextIn,txiLoc, 0,"",NoList,CA_None,isSens,NoFrame,frmSN,0,10,STR_TTSNMEMADDR},
	{C_TextIn,txiVal, 0,"",NoList,CA_None,notSens,NoFrame,frmSN,txiLoc,10,STR_MSGOSCCALIBCFG},

	{C_CheckBox, chkMemOffset, 0,STR_MSGOFFSET,NoList,CA_None,isSens,NoFrame,0,frmSN, 0,STR_TTSNOFFSET},

	{C_Button, M_Cancel, 0, STR_BTNCANC, NoList,CA_None, isSens,NoFrame, 0, chkMemOffset},
	{C_Button, M_OK, 0, STR_BTNOK, NoList, CA_DefaultButton, isSens, NoFrame, M_Cancel, chkMemOffset},
	{C_Button, btnRead,0,STR_BTNREAD, NoList, CA_None, isSens,NoFrame, M_OK, chkMemOffset,	0, STR_READOSCCALIB},


	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd

//==========================================================================================================================
PonyProg-2.08d/infomdlg.cpp
PonyProg-2.08d/infomdlg.h


//@V@:BeginDialogCmd DefaultCmds
static DialogCmd e24xxCmds[] =
{
#ifdef	_LINUX_
	{C_Label,lblMainMsg2,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},
#else
	{C_Blank,lblMainMsg2,0," ",NoList,CA_None,isSens,NoFrame,0,0},
#endif

	{C_Label,lblRllBnk,0,STR_MSGBANKROLL,NoList,CA_None,isSens,NoFrame,0,lblMainMsg2},
	{C_Text,txtRllBnk,0,"",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblMainMsg2},

	{C_Label,lblCRC,0,STR_MSGCRC,NoList,CA_None,isSens,NoFrame,0,lblRllBnk},
	{C_Text,txtCRC,0,"0000h",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblRllBnk},

	{C_Label,lblSize,0,STR_MSGSIZE,NoList,CA_None,isSens,NoFrame,0,lblCRC},
	{C_Text,txtSize,0,"0 Byte",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblCRC},
/**
	{C_Label,lblSecurity,0,STR_MSGSECBLOCK,NoList,CA_None,isSens,NoFrame,0,lblSize},
	{C_Text,txtSecurity,0,"0",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblSize},

	{C_Label,lblHEndurance,0,STR_HIGHENDURAN,NoList,CA_None,isSens,NoFrame,0,lblSecurity},
	{C_Text,txtHEndurance,0,"0",NoList,CA_None,isSens,NoFrame,lblRllBnk,lblSecurity},
**/
	{C_Button,M_OK,0, STR_BTNCLOSE, NoList,CA_DefaultButton,isSens, NoFrame,0,lblSize},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd

static DialogCmd otherCmds[] =
{
#ifdef	_LINUX_
	{C_Label,lblMainMsg2,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},
#else
	{C_Blank,lblMainMsg2,0," ",NoList,CA_None,isSens,NoFrame,0,0},
#endif

	{C_Label,lblSecurity,0,STR_MSGEEPSIZE ":",NoList,CA_None,isSens,NoFrame,0,lblMainMsg2},
	{C_Text,txtSecurity,0,"0 Byte",NoList,CA_None,isSens,NoFrame,lblSecurity,lblMainMsg2},

	{C_Label,lblSize,0,STR_MSGFLASHSIZE ":",NoList,CA_None,isSens,NoFrame,0,lblSecurity},
	{C_Text,txtSize,0,"0 Byte",NoList,CA_None,isSens,NoFrame,lblSecurity,lblSecurity},

	{C_Label,lblCRC,0,STR_MSGCRC,NoList,CA_None,isSens,NoFrame,0,lblSize},
	{C_Text,txtCRC,0,"0000h",NoList,CA_None,isSens,NoFrame,lblSecurity,lblSize},

	{C_Button,M_OK,0, STR_BTNCLOSE, NoList,CA_DefaultButton,isSens, NoFrame,0,lblCRC},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};


/********************************** EDIT DIALOG ********************************/

DialogCmd EditChar[] =
{
	{C_Label,lblEditMsg,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},

	{C_Frame, frmEditChar,0,"Edit Dialog",NoList,CA_None,isSens,NoFrame,0,lblEditMsg},
	{C_Label, lblHexval,0,STR_MSGHEX,NoList,CA_None,isSens,frmEditChar,0,0},
	{C_Label, lblDecval,0,STR_MSGDECIMAL,NoList,CA_None,isSens,frmEditChar,0,lblHexval},
	{C_Label, lblChval,0, STR_MSGCHAR,NoList,CA_None,isSens,frmEditChar,0,lblDecval},

	{C_TextIn,txiHexval,0,"",NoList,CA_None,isSens,NoFrame,frmEditChar,lblEditMsg,5,STR_TTHEX},
	{C_TextIn,txiDecval,0,"",NoList,CA_None,isSens,NoFrame,frmEditChar,txiHexval,5,STR_TTDECIMAL},
	{C_TextIn,txiChval,0,"",NoList,CA_None,isSens,NoFrame,frmEditChar,txiDecval,5,STR_TTCHAR},

	{C_Button, M_Cancel, 0, STR_BTNCANC, NoList,CA_None, isSens,NoFrame, 0, frmEditChar},
	{C_Button, M_OK, 0, STR_BTNOK, NoList, CA_DefaultButton, isSens, NoFrame, M_Cancel, frmEditChar},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};


//************************ EDIT DIALOG2 *******************************
DialogCmd EditChar2[] =
{
	{C_Label,lblEditMsg,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},

	{C_TextIn,txiEditText,0,"",NoList,CA_Large,isSens,NoFrame,0,lblEditMsg,80},

	{C_Button, M_Cancel, 0, STR_BTNCANC, NoList,CA_None, isSens,NoFrame, 0, txiEditText},
	{C_Button, M_OK, 0, STR_BTNOK, NoList, CA_DefaultButton, isSens, NoFrame, M_Cancel, txiEditText},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};


//==========================================================================================================================
PonyProg-2.08d/retrymdlg.cpp
PonyProg-2.08d/retrymdlg.h


//@V@:BeginDialogCmd DefaultCmds
static DialogCmd DefaultCmds[] =
{
//#ifdef	_LINUX_
//	{C_Label,lblMainMsg,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},
//#else
	{C_Label,lblMainMsg,0," ",NoList,CA_None,isSens,NoFrame,0,0},
//#endif

	{C_Button, M_Cancel, 0, STR_BTNABORT, NoList,CA_DefaultButton, isSens,NoFrame, 0, lblMainMsg},
	{C_Button, M_OK, 0, STR_BTNRETRY, NoList, CA_None, isSens, NoFrame, M_Cancel, lblMainMsg},
	{C_Button, btnIgnore, 0, STR_BTNIGNORE, NoList, CA_None, isSens, NoFrame, M_OK, lblMainMsg},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd

//==========================================================================================================================
PonyProg-2.08d/filldlg.cpp
PonyProg-2.08d/filldlg.h


//@V@:BeginDialogCmd DefaultCmds
static DialogCmd DefaultCmds[] =
{
	{C_Label,lblFillMsg,0,"X",NoList,CA_MainMsg,isSens,NoFrame,0,0},

	{C_Frame, frmFill,0,STR_LBLFILLBUF,NoList,CA_None,isSens,NoFrame,0,lblFillMsg},
	{C_Label, lblFrom,0, STR_LBLFROM,NoList,CA_None,isSens,frmFill,0,0},
	{C_Label, lblTo,  0, STR_LBLTO,NoList,CA_None,isSens,frmFill,0,lblFrom},
	{C_Label, lblVal, 0, STR_LBLVALUE,NoList,CA_None,isSens,frmFill,0,lblTo},

	{C_TextIn,txiFrom,0,"",NoList,CA_None,isSens,NoFrame,frmFill,lblFillMsg,8,"From address"},
	{C_TextIn,txiTo,  0,"",NoList,CA_None,isSens,NoFrame,frmFill,txiFrom,8,"To address"},
	{C_TextIn,txiVal, 0,"",NoList,CA_None,isSens,NoFrame,frmFill,txiTo,8,"Byte value"},

	{C_Button, M_Cancel, 0, STR_BTNCANC, NoList,CA_None, isSens,NoFrame, 0, frmFill},
	{C_Button, M_OK, 0, STR_BTNOK, NoList, CA_DefaultButton, isSens, NoFrame, M_Cancel, frmFill},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd

//==========================================================================================================================
PonyProg-2.08d/rs232int.cpp

keine ui elemente 
//==========================================================================================================================
PonyProg-2.08d/businter.h

keine ui elemente 
//==========================================================================================================================
PonyProg-2.08d/modaldlg_utilities.cpp

keine ui elemente 
//==========================================================================================================================
PonyProg-2.08d/e2cnv.h

keine ui elemente 