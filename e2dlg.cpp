//=========================================================================//
//-------------------------------------------------------------------------//
// e2dlg.cpp -- Source for Options dialog class                            //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-1999  Claudio Lanconelli                            //
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

#include <stdio.h>
#include "e2app.h"
#include "e2dlg.h"
#include "e2cmdw.h"
#include <v/vnotice.h>

#include "types.h"
#include "i2cbus.h"

#include "cmdenum.h"

//@V@:BeginIDs
enum {
	blk0 = 1000,
	blk1,
	blk2,
	frmPortsel,
	rdbComPort,
	rdbLptPort,
	frmInterf,
	cbxInterf,
	frmComsel,
	rdbCom1,
	rdbCom2,
	rdbCom3,
	rdbCom4,
	frmInterfL,
	cbxInterfL,
	frmLptsel,
	rdbLpt1,
	rdbLpt2,
	rdbLpt3,

	frmPolsep, 
	frmPolsel, 
	lblPol1, 
	chkPol1, 
	chkPol2, 
	chkPol3, 
	chkPol4, 

	btnTestDlg		// add your id's here
};
//@V@:EndIds

static char *interfList[] = {
	//Serial interfaces list
	" SI Prog API ",
	" SI Prog I/O ",
	" Easy I2CBus ",
	0
};

static char *interfListL[] = {
	//Parallel interfaces list
	" Easy I2CBus ",
	"   Avr ISP   ",
	0
};

//@V@:BeginDialogCmd DefaultCmds
static DialogCmd DefaultCmds[] =
{
	{C_Label, lblMainMsg, 0,"X",NoList,CA_MainMsg,isSens,NoFrame, 0, 0},
	{C_Frame,frmPortsel,0,"COM/LPT interface",NoList,CA_None,isSens,NoFrame,0,lblMainMsg},
	{C_RadioButton,rdbComPort,0," Serial  ",NoList,CA_None,isSens,frmPortsel,0,0},
	{C_Blank,blk0,0,"  ",NoList,CA_None,isSens,frmPortsel,rdbComPort,0},
	{C_Blank,blk1,0,"  ",NoList,CA_None,isSens,frmPortsel,blk0,0},
	{C_Blank,blk2,0,"  ",NoList,CA_None,isSens,frmPortsel,blk1,0},
	{C_RadioButton,rdbLptPort,0," Parallel  ",NoList,CA_None,isSens,frmPortsel,blk2,0},

	{C_Frame,frmInterf,0," Interface select ",NoList,CA_NoBorder,isSens,NoFrame,0,frmPortsel},
	{C_ComboBox,cbxInterf,0," Interface type ",(void*)interfList,CA_None,isSens,frmInterf,0,0},

	{C_Frame,frmComsel,0," COM Port select ",NoList,CA_None,isSens,frmInterf,0,cbxInterf},
	{C_RadioButton,rdbCom1,0," COM1  ",NoList,CA_None,isSens,frmComsel,0,0},
	{C_RadioButton,rdbCom2,0," COM2  ",NoList,CA_None,isSens,frmComsel,0,rdbCom1},
	{C_RadioButton,rdbCom3,0," COM3  ",NoList,CA_None,isSens,frmComsel,rdbCom1,0},
	{C_RadioButton,rdbCom4,0," COM4  ",NoList,CA_None,isSens,frmComsel,rdbCom1,rdbCom1},

	{C_Frame,frmInterfL,0," Interface select ",NoList,CA_NoBorder,isSens,NoFrame,frmInterf,frmPortsel},
	{C_ComboBox,cbxInterfL,0," Interface type ",(void*)interfListL,CA_None,isSens,frmInterfL,0,0},

	{C_Frame,frmLptsel,0," LPT Port select ",NoList,CA_None,isSens,frmInterfL,0,cbxInterfL},
	{C_RadioButton,rdbLpt1,0," LPT1  ",NoList,CA_None,isSens,frmLptsel,0,0},
	{C_RadioButton,rdbLpt2,0," LPT2  ",NoList,CA_None,isSens,frmLptsel,0,rdbLpt1},
	{C_RadioButton,rdbLpt3,0," LPT3  ",NoList,CA_None,isSens,frmLptsel,rdbLpt1,0},

	{C_Frame,frmPolsel,0," Polarity  select ",NoList,CA_None,isSens,NoFrame,0,frmInterf}, 
	{C_Label,lblPol1,0," Select Polarity of the Control lines",NoList,CA_None,isSens,frmPolsel,0,0}, 
	{C_CheckBox,chkPol1,0," Invert Reset ",NoList,CA_None,isSens,frmPolsel,0,lblPol1}, 
	{C_CheckBox,chkPol2,0," Invert SCKL  ",NoList,CA_None,isSens,frmPolsel,0,chkPol1}, 
	{C_CheckBox,chkPol3,0," Invert D-IN  ",NoList,CA_None,isSens,frmPolsel,chkPol1,lblPol1}, 
	{C_CheckBox,chkPol4,0," Invert D-OUT ",NoList,CA_None,isSens,frmPolsel,chkPol1,chkPol1}, 

	{C_Button,M_Cancel,0,
#ifdef	_WINDOWS
		" &Cancel ",
#else
		"  Cancel ",
#endif
			NoList,CA_None,isSens,NoFrame,0,frmPolsel}, 
	{C_Button,M_OK,0,
#ifdef	_WINDOWS
		" &OK ",
#else
		"  OK ",
#endif
			NoList,CA_DefaultButton,isSens,NoFrame,M_Cancel,frmPolsel}, 

	{C_Button,btnTestDlg,0,
#ifdef	_WINDOWS
		" &Test ",
#else
		"  Test ",
#endif
			NoList,CA_None,isSens,NoFrame,M_OK,frmPolsel}, 

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd


//=========================>>> e2Dialog::e2Dialog <<<====================
e2Dialog::e2Dialog(vBaseWindow* bw, char* title) :
		vDialog(bw, 0, title),
			lpt_no(2),
			com_no(3)
{
	UserDebug(Constructor,"e2Dialog::e2Dialog()\n")

	_myCmdWin = (e2CmdWindow*) bw;

	AddDialogCmds(DefaultCmds);		// add the predefined commands
}

//======================>>> e2Dialog::~e2Dialog <<<======================
e2Dialog::~e2Dialog()
{
	UserDebug(Destructor,"e2Dialog::~e2Dialog() destructor\n")
}

//======================>>> e2Dialog::DialogDisplayed <<<================
void e2Dialog::DialogDisplayed()
{
	interf_type = THEAPP->GetInterfaceType();
	port_no = THEAPP->GetPort();

	UpdateDialog(1);

//	SetDialogPosition(300, 10);

	vDialog::DialogDisplayed();
}

#define	OPENONLY	1

//====================>>> e2Dialog::UpdateDialog <<<=======================
void e2Dialog::UpdateDialog(int init, int type)
{
	extern int TypeToInterfVector(HInterfaceType type);
	extern int TypeToInterfIndex(HInterfaceType type);

	if (init)
		type = TypeToInterfVector(interf_type);

	// care for the polarity control check boxes 

	if (THEAPP->GetPolarity() & RESETINV) 
		SetValue(chkPol1,1,Checked); 
	if (THEAPP->GetPolarity() & CLOCKINV) 
		SetValue(chkPol2,1,Checked); 
	if (THEAPP->GetPolarity() & DININV) 
		SetValue(chkPol3,1,Checked); 
	if (THEAPP->GetPolarity() & DOUTINV) 
		SetValue(chkPol4,1,Checked); 

	if (type)
	{
		SetValue(rdbLptPort, 1, Value);
		if (init)
		{
			lpt_no = port_no;
			SetValue(cbxInterfL, TypeToInterfIndex(interf_type), Value);
			SetValue(rdbLpt1 + lpt_no - 1, 1, Value);
			SetValue(rdbCom1 + com_no - 1, 1, Value);
		}
		
		int k;
		for (k = 0; k < 3; k++)
		{
			if ( Test(k+1, OPENONLY) == OK )
				SetValue(rdbLpt1 + k, 1, Sensitive);
			else
				SetValue(rdbLpt1 + k, 0, Sensitive);
		}
		SetValue(cbxInterfL, 1, Sensitive);
		for (k = 0; k < 4; k++)
			SetValue(rdbCom1 + k, 0, Sensitive);
		SetValue(cbxInterf, 0, Sensitive);
	}
	else
	{
		SetValue(rdbComPort, 1, Value);
		if (init)
		{
			com_no = port_no;
			SetValue(cbxInterf, TypeToInterfIndex(interf_type), Value);
			SetValue(rdbCom1 + com_no - 1, 1, Value);
			SetValue(rdbLpt1 + lpt_no - 1, 1, Value);
		}
		
		int k;
		for (k = 0; k < 4; k++)
		{
			if ( Test(k+1, OPENONLY) == OK )
				SetValue(rdbCom1 + k, 1, Sensitive);
			else
				SetValue(rdbCom1 + k, 0, Sensitive);
		}
		SetValue(cbxInterf, 1, Sensitive);
		for (k = 0; k < 3; k++)
			SetValue(rdbLpt1 + k, 0, Sensitive);
		SetValue(cbxInterfL, 0, Sensitive);
	}
}

int e2Dialog::Test(int p, int open_only) const
{
	HInterfaceType old_interf = THEAPP->GetInterfaceType();
	int test;

	if (p == 0)
		p = port_no;

	if (interf_type != old_interf)
	{
		THEAPP->SetInterfaceType(interf_type);
		test = THEAPP->TestPort(p, open_only);
		THEAPP->SetInterfaceType(old_interf);
	}
	else
		test = THEAPP->TestPort(p, open_only);

	return test;
}

//====================>>> e2Dialog::DialogCommand <<<=======================
void e2Dialog::DialogCommand(ItemVal id, ItemVal retval, CmdType ctype)
{
	extern HInterfaceType VindexToInterfType(int vector, int index);
	
	// Dialog commands to here

	UserDebug2(CmdEvents,"e2Dialog::DialogCommand(id:%d, val:%d)\n",id, retval)

	vNoticeDialog note(this);
	char str[64];

	switch (id)		// We will do some things depending on value
	{
	case rdbComPort:
	case rdbLptPort:
	  {
		if (retval)
		{
			int vector = id - rdbComPort;
			int index = vector ? GetValue(cbxInterfL) : GetValue(cbxInterf);

			interf_type = VindexToInterfType(vector, index);
			port_no = vector ? lpt_no : com_no;
			UpdateDialog(0, vector);
//			UpdateDialog(1);
		}
		break;
	  }

	case rdbLpt1:
	case rdbLpt2:
	case rdbLpt3:
	  {
		if (retval)
			lpt_no = port_no = id - rdbLpt1 + 1;
		break;
	  }

	case rdbCom1:
	case rdbCom2:
	case rdbCom3:
	case rdbCom4:
	  {
		if (retval)
			com_no = port_no = id - rdbCom1 + 1;
		break;
	  }

	case chkPol1: 
	 { 
	        if (retval) 
	                THEAPP->SetPolarity(THEAPP->GetPolarity()| RESETINV); 
	        else 
	                THEAPP->SetPolarity(THEAPP->GetPolarity()& ~RESETINV); 
	        break; 
	 } 
	case chkPol2: 
	 { 
	        if (retval) 
	                THEAPP->SetPolarity(THEAPP->GetPolarity() | CLOCKINV); 
	        else                 
	                THEAPP->SetPolarity(THEAPP->GetPolarity() & ~CLOCKINV); 
	        break;               
	 }                          
	case chkPol3:                
	 {                          
	        if (retval)          
	                THEAPP->SetPolarity(THEAPP->GetPolarity() | DININV); 
	        else                 
	                THEAPP->SetPolarity(THEAPP->GetPolarity() & ~DININV); 
	        break;               
	 }                          
	case chkPol4:                
	 {                          
	        if (retval)          
	                THEAPP->SetPolarity(THEAPP->GetPolarity() | DOUTINV); 
	        else                                              
	                THEAPP->SetPolarity(THEAPP->GetPolarity() & ~DOUTINV); 
	        break; 
	 } 

	case cbxInterf:
	  {
		interf_type = VindexToInterfType(0, retval);
		UpdateDialog(0, 0);
		break;
	  }

	case cbxInterfL:
	  {
		interf_type = VindexToInterfType(1, retval);
		UpdateDialog(0, 1);
		break;
	  }

	case M_OK:
	  {
		int set_port = FALSE;

		if (port_no != THEAPP->GetPort())
			set_port = TRUE;
		if (interf_type != THEAPP->GetInterfaceType())
		{
			THEAPP->ClosePort();
			THEAPP->SetInterfaceType(interf_type);
			set_port = TRUE;
		}
		if (set_port)
			THEAPP->SetPort(port_no);

		//Store values in the INI file
		THEAPP->SetParInterfType(interf_type);
		THEAPP->SetParPortNo(port_no);
		THEAPP->SetPolarityControl(THEAPP->GetPolarity()); 
	  	break;
	  }

	//@V@:Case: btnTestDlg
	case btnTestDlg:
	  {
		sprintf(str, "Test %s", Test() ? "Failed" : "Ok");
		note.Notice(str);
		break;
	  }	//@V@:EndCase
	}

	vDialog::DialogCommand(id,retval,ctype);
}


/********************************** PROGRESS DIALOG ********************************/

static CommandObject ProgressDlg[] = {
	{C_Label, lblMainMsg, 0,"X",NoList,CA_MainMsg,isSens,NoFrame, 0, 0},
	{C_ProgressBar, pbrProgress, 0, "", NoList, CA_Horizontal|CA_Percent,isSens,NoFrame, 0, lblMainMsg},	// Horiz, with label
	{C_Button, M_Cancel, 0,
#ifdef	_WINDOWS
		"&Abort",
#else
		" Abort",
#endif
			NoList,CA_None,isSens,NoFrame,0,pbrProgress},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};

e2ProgressDialog::e2ProgressDialog(vBaseWindow* bw, char* title) :
		vDialog(bw, 0, title),
			pbr_value(0)
{
	UserDebug(Constructor,"e2ProgressDialog::e2ProgressDialog()\n")

	_myCmdWin = (e2CmdWindow*) bw;
	last_msg[0] = '\0';

	AddDialogCmds(ProgressDlg);
}

e2ProgressDialog::~e2ProgressDialog()
{
	UserDebug(Destructor,"e2ProgressDialog::~e2ProgressDialog() destructor\n")
}

void e2ProgressDialog::ShowDialog(char *msg)
{
	if (msg == 0)
		msg = last_msg;
	vDialog::ShowDialog(msg);
}

void e2ProgressDialog::UpdateDialog(int val, char *msg)
{
	pbr_value = val;
	SetValue(pbrProgress,val,Value);    // The horizontal bar
	if (msg)
	{
		UserDebug(UserApp2,"e2ProgressDialog::UpdateDialog() closing...\n")

		CloseDialog();

		UserDebug1(UserApp2,"e2ProgressDialog::UpdateDialog() showing... (%s)\n", msg)

		ShowDialog(msg);
	}
}

void e2ProgressDialog::DialogDisplayed()
{
//	SetValue(pbrProgress,pbr_value,Value);
	vDialog::DialogDisplayed();
}

void e2ProgressDialog::DialogCommand(ItemVal id, ItemVal retval, CmdType ctype)
{
	// Dialog commands to here

	UserDebug2(CmdEvents,"e2ProgressDialog::DialogCommand(id:%d, val:%d)\n",id, retval)


	switch (id)		// We will do some things depending on value
	{
	case M_Cancel:
	  {
		THEAPP->SetAbortFlag();
	  	break;
	  }
	}

	vDialog::DialogCommand(id,retval,ctype);
}
