CLS 
REM Configure.

ECHO %path%
SET QTDIR=C:/Qt/Qt5.9.1
ECHO %qtdir%
REM PATH=c:/Program Files/CMake/bin;c:/Qt/Qt5.9.1/Tools/mingw530_32/bin/;%PATH%

rd /s /q "build"
mkdir "build"

cd build

cmake -G "MinGW Makefiles" ^
-DCMAKE_PREFIX_PATH="%QTDIR%/5.9.1/mingw53_32/;%QTDIR%/5.9.1/mingw53_32/lib/;%QTDIR%/Tools/mingw530_32/i686-w64-mingw32/lib/" ^
-DCMAKE_MODULE_PATH="%QTDIR%/5.9.1/mingw53_32/lib/cmake/;" ^
-DCMAKE_C_COMPILER="gcc"  ^
-DCMAKE_CXX_COMPILER="c++" ^
-DCMAKE_MAKE_PROGRAM="mingw32-make" ^
-DCMAKE_BUILD_TYPE=Debug ..

echo "Compile sources"

mingw32-make 

mingw32-make package-binary-inno

cd ..
