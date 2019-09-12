; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "XeroTuningDashboard"
#define MyAppVersion "0.2"
#define MyAppPublisher "ErrorCodeXero"
#define MyAppURL "http://www.wilsonvillerobotics.com/"
#define MyAppExeName "XeroDash.exe"
#define MyUserName "ButchGriffin"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{AD3E84E5-2695-496E-8AB2-C5FC363D0520}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=C:\cygwin64\home\{#MyUserName}\rtools\xerodash\setup\license.txt
; Remove the following line to run in administrative install mode (install for all users.)
PrivilegesRequired=lowest
OutputDir=C:\cygwin64\home\{#MyUserName}\rtools\xerodash\
OutputBaseFilename=xerodash
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\cygwin64\home\{#MyUserName}\rtools\xerodash\x64\Release\*.*"; DestDir: "{app}"; Flags: ignoreversion 64bit recursesubdirs
Source: "C:\Qt\5.13.1\msvc2017_64\plugins\platforms\qwindows.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "C:\Qt\5.13.1\msvc2017_64\bin\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Qt\5.13.1\msvc2017_64\bin\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Qt\5.13.1\msvc2017_64\bin\Qt5Charts.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Qt\5.13.1\msvc2017_64\bin\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

