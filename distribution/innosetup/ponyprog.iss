; PonyProg setup script.
; Copyright by Claudio Lanconelli 1999-2019

[Setup]
AppName={#APPNAME}
AppPublisher=LancOS 
AppPublisherURL=http://www.LancOS.com/
AppUpdatesURL=https://github.com/lancos/ponyprog/releases
AppVersion={#APPVERSION}
AppVerName={#APPNAME} {#APPVERSION}
AppCopyright=(C) 1997-{#APPYEAR} Claudio Lanconelli
DefaultDirName={pf}\{#APPNAME}
DefaultGroupName={#APPNAME}
Compression=lzma2/max
SolidCompression=yes
LicenseFile=license.txt
ChangesAssociations=yes
PrivilegesRequired=admin
;run in 32bit mode only due to dlportio.sys (no more true with inpout32.dll)
;ArchitecturesAllowed=x86
OutputBaseFilename={#APPNAME}-{#APPVERSION}-setup
RestartIfNeededByRun=yes
;AlwaysRestart=yes

MinVersion=6.0

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

;Source: "{#APPNAME}.ini"; DestDir: "{app}"; Permissions: users-modify
Source: "{#APPNAME}.html"; DestDir: "{app}"
Source: "oksound.wav"; DestDir: "{app}"
Source: "*.jpg"; DestDir: "{app}"
Source: "Readme.txt"; DestDir: "{app}"; Flags: isreadme
Source: "PonyProg.url"; DestDir: "{app}"
Source: "installdriver.exe"; DestDir: "{app}"
Source: "inpout32.dll"; DestDir: "{app}"
;Source: "INPOUT32.DLL"; DestDir: "{sys}"; Flags: sharedfile promptifolder
Source: "wdi-simple-64.exe"; DestDir: "{app}"; Flags: replacesameversion promptifolder;
Source: "wdi-simple-32.exe"; DestDir: "{app}"; Flags: replacesameversion promptifolder;

[UninstallDelete] 
Type: files; Name: "{app}\bogomips.out"

[Icons]
Name: "{group}\PonyProg"; Filename: "{app}\{#APPNAME}.exe"; WorkingDir: "{app}"
Name: "{group}\Visit LancOS PonyProg Web Site"; Filename: "{app}\PonyProg.url"

[Run] 
Filename: "{app}\installdriver.exe"; Parameters: "install"; StatusMsg: "Installing legacy COM and LPT I/O driver..."
; call wdi-simple
;
; -n, --name <name>          set the device name
; -f, --inf <name>           set the inf name
; -m, --manufacturer <name>  set the manufacturer name
; -v, --vid <id>             set the vendor ID (VID)
; -p, --pid <id>             set the product ID (PID)
; -i, --iid <id>             set the interface ID (MI)
; -t, --type <driver_type>   set the driver to install
;                            (0=WinUSB, 1=libusb0, 2=libusbK, 3=usbser, 4=custom)
; -d, --dest <dir>           set the extraction directory
; -x, --extract              extract files only (don't install)
; -c, --cert <certname>      install certificate <certname> from the
;                            embedded user files as a trusted publisher
;     --stealth-cert         installs certificate above without prompting
; -s, --silent               silent mode
; -b, --progressbar=[HWND]   display a progress bar during install
;                            an optional HWND can be specified
; -o, --timeout              set timeout (in ms) to wait for any
;                            pending installations
; -l, --log                  set log level (0=debug, 4=none)
; -h, --help                 display usage
;
Filename: "{app}\wdi-simple-64.exe"; Flags: "runhidden"; Parameters: " --name PonyProgFT --manufacturer ""Eurek srl"" --vid 0x0403 --pid 0x6e38 --progressbar={wizardhwnd} --timeout 120000"; StatusMsg: "Installing PonyProgFT 64bit USB driver (this may take several seconds) ..."; Check: IsWin64
Filename: "{app}\wdi-simple-32.exe"; Flags: "runhidden"; Parameters: " --name PonyProgFT --manufacturer ""Eurek srl"" --vid 0x0403 --pid 0x6e38 --progressbar={wizardhwnd} --timeout 120000"; StatusMsg: "Installing PonyProgFT 32bit USB driver (this may take several seconds) ..."; Check: not IsWin64

[UninstallRun] 
Filename: "{app}\installdriver.exe"; Parameters: "remove"

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
