//	fusemdlg.h:	Header for fuseModalDialog class
//=======================================================================

#ifndef fuseMDLG_H
#define fuseMDLG_H

#include <v/vmodald.h>

#include "e2cmdw.h"

class fuseModalDialog : public vModalDialog
{
  public:		//---------------------------------------- public
	fuseModalDialog(vBaseWindow* bw, char* title = "Fuse & Lock bits");
	virtual ~fuseModalDialog();		// Destructor
	virtual void DialogCommand(ItemVal,ItemVal,CmdType); // action selected
	virtual int fuseAction(char* msg, long type, int &lock, int &fuse, int readonly = 0);

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

	int write;
	e2CmdWindow *cmdw;
};
#endif

