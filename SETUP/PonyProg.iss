; This is PonyProg setup script.
; Copyright by Claudio Lanconelli 1999-2000

[Setup]
Bits=32
AppName=PonyProg
AppVerName=PonyProg v1.17c
AppCopyright=Copyright © 1997-2000 by Claudio Lanconelli
DefaultDirName={pf}\PonyProg
DefaultGroupName=PonyProg
CompressLevel=9
LicenseFile=license.txt
MinVersion=4,3.51

[Files]
Source: "LPTCON.VXD"; DestDir: "{sys}"; CopyMode: onlyifdoesntexist; Flags: sharedfile
Source: "PONYPROG.SYS"; DestDir: "{app}"
Source: "PONYPROG.EXE"; DestDir: "{app}"
Source: "PONYPROG.INI"; DestDir: "{app}"
Source: "PONYPROG.HTML"; DestDir: "{app}"
Source: "LOADDRV.EXE"; DestDir: "{app}"
Source: "LANCOS.JPG"; DestDir: "{app}"
Source: "PROG-EDIT.JPG"; DestDir: "{app}"
Source: "PROG-NOTE.JPG"; DestDir: "{app}"
Source: "PROG-SAVEAS.JPG"; DestDir: "{app}"
Source: "PROG-SETUP.JPG"; DestDir: "{app}"
Source: "README.TXT"; DestDir: "{app}"; Flags: isreadme

[UninstallDelete] 
Type: files; Name: "{app}\BOGOMIPS.OUT"

[Icons]
Name: "{group}\PonyProg"; Filename: "{app}\PONYPROG.EXE"

[Run] 
Filename: "{app}\LOADDRV.EXE"; Parameters: "install name={app}\PONYPROG.SYS"

[UninstallRun] 
Filename: "{app}\LOADDRV.EXE"; Parameters: "remove name={app}\PONYPROG.SYS"

[Registry]
Root: HKCR; Subkey: ".e2p"; ValueType: string; ValueName: ""; ValueData: "PonyProgFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "PonyProgFile"; ValueType: string; ValueName: ""; ValueData: "PonyProgrammer File"; Flags: uninsdeletekey
Root: HKCR; Subkey: "PonyProgFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\PONYPROG.EXE,0" 
Root: HKCR; Subkey: "PonyProgFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\PONYPROG.EXE"" ""%1""" 
; "shell\open\command" is the registry key that specifies the program to
; execute when a file of the type is double-clicked in Explorer. The
; surrounding quotes are in the command line so it handles long filenames
; correctly.
