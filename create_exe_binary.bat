@cls
@echo on
@echo "Configure."

if "%1"=="" setlocal

set QTDIR=C:\Qt\5.15.2\mingw81_32
set "INNOPATH=C:\Program Files (x86)\Inno Setup 6"
set "PATH=%QTDIR%\bin;C:\Qt\Tools\mingw810_32\bin;%PATH%;%INNOPATH%"

rd /s /q "build"
cmake -E make_directory build || exit /b 1

cd build

cmake -G "MinGW Makefiles" ^
-DCMAKE_C_COMPILER="gcc" ^
-DCMAKE_CXX_COMPILER="c++" ^
-DCMAKE_MAKE_PROGRAM="mingw32-make" ^
-DUSE_DEBUGGER=OFF ^
-DUSE_QT5=ON ^
-DCMAKE_PREFIX_PATH="C:/libftdi1-1.5_devkit_x86_x64_19July2020" ^
-DLIBUSB_SKIP_VERSION_CHECK=ON .. || exit /b 1
cmake --build . -j%NUMBER_OF_PROCESSORS% --target all || exit /b 1

cd ..

qmake ponyprog.pro -spec win32-g++ FTDIPATH=C:/libftdi1-1.5_devkit_x86_x64_19July2020 || exit /b 1

@echo "Compile sources"

mingw32-make -j%NUMBER_OF_PROCESSORS% || exit /b 1
