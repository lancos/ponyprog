; PonyProg2000 setup script.
; Copyright by Claudio Lanconelli 1999-2007

[Setup]
AppName=PonyProg2000
AppPublisher=LancOS 
AppPublisherURL=http://www.LancOS.com/
AppUpdatesURL=http://ponyprog.sourceforge.net
AppVersion=2.08c
AppVerName=PonyProg2000 v2.08c
AppCopyright=Copyright © 1997-2013 by Claudio Lanconelli
DefaultDirName={pf}\PonyProg2000
DefaultGroupName=PonyProg
Compression=lzma/max
SolidCompression=no
LicenseFile=license.txt
ChangesAssociations=yes
PrivilegesRequired=admin
;run in 32bit mode only due to dlportio.sys
;ArchitecturesAllowed=x86

;I never tested with WinNT 3.51, may be we need to replace 3.51 with 4.00
MinVersion=0,5.0

[Files]
Source: "PONYPROG2000.EXE"; DestDir: "{app}"
Source: "PONYPROG2000.INI"; DestDir: "{app}"; Permissions: users-modify
Source: "PONYPROG2000.HTML"; DestDir: "{app}"
Source: "OKSOUND.WAV"; DestDir: "{app}"
Source: "LANCOS.JPG"; DestDir: "{app}"
Source: "24C65FUSE.JPG"; DestDir: "{app}"
Source: "AT89S8252FUSE.JPG"; DestDir: "{app}"
Source: "AVR4433FUSE.JPG"; DestDir: "{app}"
Source: "DEVMISSING.JPG"; DestDir: "{app}"
Source: "FILLDLG.JPG"; DestDir: "{app}"
Source: "PIC16F84FUSE.JPG"; DestDir: "{app}"
Source: "PROG-EDIT.JPG"; DestDir: "{app}"
Source: "PROG-NOTE.JPG"; DestDir: "{app}"
Source: "PROGOPTIONS.JPG"; DestDir: "{app}"
Source: "PROG-SAVEAS.JPG"; DestDir: "{app}"
Source: "PROG-SETUP.JPG"; DestDir: "{app}"
Source: "RDPROGRESS.JPG"; DestDir: "{app}"
Source: "SERNUMDLG.JPG"; DestDir: "{app}"
Source: "README.TXT"; DestDir: "{app}"; Flags: isreadme
Source: "INSTALLDRIVER.EXE"; DestDir: "{app}"
Source: "INPOUT32.DLL"; DestDir: "{sys}"; Flags: sharedfile promptifolder
Source: "PonyProg.url"; DestDir: "{app}"

[UninstallDelete] 
Type: files; Name: "{app}\BOGOMIPS.OUT"

[Icons]
Name: "{group}\PonyProg2000"; Filename: "{app}\PONYPROG2000.EXE"; WorkingDir: "{app}"
Name: "{group}\Visit LancOS PonyProg Web Site"; Filename: "{app}\PonyProg.url"

[Run] 
Filename: "{app}\INSTALLDRIVER.EXE"; Parameters: "install"

;Program that need to be executed only under WinNT/Win2000
[UninstallRun] 
;Filename: "{app}\LOADDRIVER.EXE"; Parameters: "remove"

[Registry]
Root: HKCR; Subkey: ".e2p"; ValueType: string; ValueName: ""; ValueData: "PonyProgFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "PonyProgFile"; ValueType: string; ValueName: ""; ValueData: "PonyProgrammer File"; Flags: uninsdeletekey
Root: HKCR; Subkey: "PonyProgFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\PONYPROG2000.EXE,0" 
Root: HKCR; Subkey: "PonyProgFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\PONYPROG2000.EXE"" ""%1""" 

Root: HKCR; Subkey: ".e2s"; ValueType: string; ValueName: ""; ValueData: "PonyProgScript"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "PonyProgScript"; ValueType: string; ValueName: ""; ValueData: "PonyProgrammer Script"; Flags: uninsdeletekey
Root: HKCR; Subkey: "PonyProgScript\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\PONYPROG2000.EXE,0" 
Root: HKCR; Subkey: "PonyProgScript\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\PONYPROG2000.EXE"" ""%1""" 
; "shell\open\command" is the registry key that specifies the program to
; execute when a file of the type is double-clicked in Explorer. The
; surrounding quotes are in the command line so it handles long filenames
; correctly.
