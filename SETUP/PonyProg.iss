; This is PonyProg setup script.
; Copyright by Claudio Lanconelli 1999-2000

[Setup]
Bits=32
AppName=PonyProg
AppVerName=PonyProg v1.17g
AppCopyright=Copyright © 1997-2000 by Claudio Lanconelli
DefaultDirName={pf}\PonyProg
DefaultGroupName=PonyProg
CompressLevel=9
LicenseFile=license.txt

;I never tested with WinNT 3.51, may be we need to replace 3.51 with 4.00
MinVersion=4,3.51

[Files]
Source: "PONYPROG.EXE"; DestDir: "{app}"
Source: "PONYPROG.INI"; DestDir: "{app}"
Source: "PONYPROG.HTML"; DestDir: "{app}"
Source: "LOADDRIVER.EXE"; DestDir: "{app}"
Source: "LANCOS.JPG"; DestDir: "{app}"
Source: "PROG-EDIT.JPG"; DestDir: "{app}"
Source: "PROG-NOTE.JPG"; DestDir: "{app}"
Source: "PROG-SAVEAS.JPG"; DestDir: "{app}"
Source: "PROG-SETUP.JPG"; DestDir: "{app}"
Source: "README.TXT"; DestDir: "{app}"; Flags: isreadme
Source: "DLPORTIO.DLL"; DestDir: "{sys}"; CopyMode: onlyifdoesntexist; Flags: sharedfile

;Files that need to be installed only under WinNT/Win2000
Source: "DLPORTIO.SYS"; DestDir: "{sys}\DRIVERS"; CopyMode: onlyifdoesntexist; Flags: sharedfile

;Files that need to be installed only under Windows95/98
Source: "LPTCON.VXD"; DestDir: "{sys}"; CopyMode: onlyifdoesntexist; Flags: sharedfile

[UninstallDelete] 
Type: files; Name: "{app}\BOGOMIPS.OUT"

[Icons]
Name: "{group}\PonyProg"; Filename: "{app}\PONYPROG.EXE"

;Program that need to be executed only under WinNT/Win2000
[Run] 
Filename: "{app}\LOADDRIVER.EXE"; Parameters: "install"

;Program that need to be executed only under WinNT/Win2000
[UninstallRun] 
Filename: "{app}\LOADDRIVER.EXE"; Parameters: "remove"

[Registry]
Root: HKCR; Subkey: ".e2p"; ValueType: string; ValueName: ""; ValueData: "PonyProgFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "PonyProgFile"; ValueType: string; ValueName: ""; ValueData: "PonyProgrammer File"; Flags: uninsdeletekey
Root: HKCR; Subkey: "PonyProgFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\PONYPROG.EXE,0" 
Root: HKCR; Subkey: "PonyProgFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\PONYPROG.EXE"" ""%1""" 
; "shell\open\command" is the registry key that specifies the program to
; execute when a file of the type is double-clicked in Explorer. The
; surrounding quotes are in the command line so it handles long filenames
; correctly.
