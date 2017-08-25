; PonyProg setup script.
; Copyright by Claudio Lanconelli 1999-2017

[Setup]
AppName={#APPNAME}
AppPublisher=LancOS 
AppPublisherURL=http://www.LancOS.com/
AppUpdatesURL=http://ponyprog.sourceforge.net
AppVersion={#APPVERSION}
AppVerName={#APPNAME} {#APPVERSION}
AppCopyright={#APPCOPYRIGHT}
DefaultDirName={pf}\{#APPNAME}
DefaultGroupName={#APPNAME}
Compression=lzma2/max
SolidCompression=yes
LicenseFile=license.txt
ChangesAssociations=yes
PrivilegesRequired=admin
;run in 32bit mode only due to dlportio.sys (no more true with inpout32.dll)
;ArchitecturesAllowed=x86
OutputBaseFilename={#APPNAME}-setup

;I never tested with WinNT 3.51, may be we need to replace 3.51 with 4.00
MinVersion=5.0

[Files]
Source: "{#APPNAME}.exe"; DestDir: "{app}"
Source: "*.dll"; DestDir: "{app}"
Source: "audio\*"; DestDir: "{app}\audio"
Source: "bearer\*"; DestDir: "{app}\bearer"
Source: "iconengines\*"; DestDir: "{app}\iconengines"
Source: "imageformats\*"; DestDir: "{app}\imageformats"
Source: "mediaservice\*"; DestDir: "{app}\mediaservice"
Source: "platforms\*"; DestDir: "{app}\platforms"
Source: "playlistformats\*"; DestDir: "{app}\playlistformats"
Source: "printsupport\*"; DestDir: "{app}\printsupport"
Source: "translations\*"; DestDir: "{app}\translations"

Source: "lang\*"; DestDir: "{app}\lang"

;Source: "PONYPROG.INI"; DestDir: "{app}"; Permissions: users-modify
;Source: "PONYPROG.HTML"; DestDir: "{app}"
Source: "OKSOUND.WAV"; DestDir: "{app}"
Source: "LANCOS.JPG"; DestDir: "{app}"
;Source: "24C65FUSE.JPG"; DestDir: "{app}"
;Source: "AT89S8252FUSE.JPG"; DestDir: "{app}"
;Source: "AVR4433FUSE.JPG"; DestDir: "{app}"
;Source: "DEVMISSING.JPG"; DestDir: "{app}"
;Source: "FILLDLG.JPG"; DestDir: "{app}"
;Source: "PIC16F84FUSE.JPG"; DestDir: "{app}"
;Source: "PROG-EDIT.JPG"; DestDir: "{app}"
;Source: "PROG-NOTE.JPG"; DestDir: "{app}"
;Source: "PROGOPTIONS.JPG"; DestDir: "{app}"
;Source: "PROG-SAVEAS.JPG"; DestDir: "{app}"
;Source: "PROG-SETUP.JPG"; DestDir: "{app}"
;Source: "RDPROGRESS.JPG"; DestDir: "{app}"
;Source: "SERNUMDLG.JPG"; DestDir: "{app}"
;Source: "README.TXT"; DestDir: "{app}"; Flags: isreadme
;Source: "INSTALLDRIVER.EXE"; DestDir: "{app}"
;Source: "INPOUT32.DLL"; DestDir: "{sys}"; Flags: sharedfile promptifolder
;Source: "PonyProg.url"; DestDir: "{app}"

[UninstallDelete] 
Type: files; Name: "{app}\BOGOMIPS.OUT"

;[Icons]
;Name: "{group}\PonyProg"; Filename: "{app}\PONYPROG.EXE"; WorkingDir: "{app}"
;Name: "{group}\Visit LancOS PonyProg Web Site"; Filename: "{app}\PonyProg.url"

;[Run] 
;Filename: "{app}\INSTALLDRIVER.EXE"; Parameters: "install"; StatusMsg: "Installing I/O driver..."

;[UninstallRun] 
;Filename: "{app}\INSTALLDRIVER.EXE"; Parameters: "remove"

;[Registry]
;Root: HKCR; Subkey: ".e2p"; ValueType: string; ValueName: ""; ValueData: "PonyProgFile"; Flags: uninsdeletevalue
;Root: HKCR; Subkey: "PonyProgFile"; ValueType: string; ValueName: ""; ValueData: "PonyProgrammer File"; Flags: uninsdeletekey
;Root: HKCR; Subkey: "PonyProgFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\PONYPROG.EXE,0" 
;Root: HKCR; Subkey: "PonyProgFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\PONYPROG.EXE"" ""%1""" 

;Root: HKCR; Subkey: ".e2s"; ValueType: string; ValueName: ""; ValueData: "PonyProgScript"; Flags: uninsdeletevalue
;Root: HKCR; Subkey: "PonyProgScript"; ValueType: string; ValueName: ""; ValueData: "PonyProgrammer Script"; Flags: uninsdeletekey
;Root: HKCR; Subkey: "PonyProgScript\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\PONYPROG.EXE,0" 
;Root: HKCR; Subkey: "PonyProgScript\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\PONYPROG2000.EXE"" ""%1""" 
; "shell\open\command" is the registry key that specifies the program to
; execute when a file of the type is double-clicked in Explorer. The
; surrounding quotes are in the command line so it handles long filenames
; correctly.
