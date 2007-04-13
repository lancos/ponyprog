#include <v/vapp.h>
#include <stdlib.h>

//======================>>> main <<<======================================
  int main(int argc, char** argv)
  {
    int Xargc = argc;
    int retcode;

    theApp->initialize(Xargc,argv);	// Create top level widget

    if ((retcode = AppMain(Xargc,argv)) != 0)	// call the app main program
	exit(retcode);

    theApp->doXEventLoop();		// And enter the event loop
  }
