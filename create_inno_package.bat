@cls
@echo on

setlocal

call create_exe_binary.bat no_setlocal || exit /b 1

@echo "Create installer"

mingw32-make -j%NUMBER_OF_PROCESSORS% win32setup || exit /b 1

@rem ponyprog.bat is created by make win32setup

call ponyprog.bat || exit /b 1
