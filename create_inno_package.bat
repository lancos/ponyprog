@cls
@echo on

setlocal

call create_exe_binary.bat no_setlocal || exit /b 1

@echo "Create installer"

mingw32-make -j%NUMBER_OF_PROCESSORS% win32setup || exit /b 1

@rem Append exit code check to commands
echo. > build/ponyprog.bat
for /f "delims=" %%i in (ponyprog.bat) do (
	echo %%i ^|^| exit /b 1 >> build/ponyprog.bat
)
move /y build\ponyprog.bat ponyprog.bat

call ponyprog.bat || exit /b 1
