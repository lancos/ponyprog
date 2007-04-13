#include <windows.h>		// for Win 32 stuff

//======================>>> WinMain <<<======================================

int  AppMain(int, char**);
int  CMain(HANDLE , HANDLE , LPSTR , int );
void vRegisterAppMain( int (*)(int, char**) );

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine, int nCmdShow)
{
   // pass the address of your AppMain to the V library.
   vRegisterAppMain(AppMain);

   // call CMain as in the static library.
   return CMain(hInstance, hPrevInstance, lpszCmdLine, nCmdShow);
}
