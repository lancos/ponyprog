CLS 
@echo off
ECHO "Configure."

ECHO %path%

setlocal

SET myCmakePath=c:/Program Files/CMake/bin/
SET myMinGWPath=c:/Qt/Qt5.9.1/Tools/mingw530_32/bin/
SET myQtPath=C:/Qt/Qt5.9.1

PATH=%myCmakePath%;%myMinGWPath%;%PATH%

rd /s /q "build-win"
mkdir "build-win"

cd "build-win"

cmake -G "MinGW Makefiles" ^
-DCMAKE_PREFIX_PATH="%myQtPath%/5.9.1/mingw53_32/;%myQtPath%/5.9.1/mingw53_32/lib/;%myQtPath%/Tools/mingw530_32/i686-w64-mingw32/lib/" ^
-DCMAKE_MODULE_PATH="%myQtPath%/5.9.1/mingw53_32/lib/cmake/;" ^
-DCMAKE_C_COMPILER="gcc"  ^
-DCMAKE_CXX_COMPILER="c++" ^
-DCMAKE_MAKE_PROGRAM="mingw32-make" ^
-DUSE_DEBUGGER="OFF" ..

ECHO "Compile sources"

mingw32-make 

mingw32-make package-binary-nsis

cd ..
