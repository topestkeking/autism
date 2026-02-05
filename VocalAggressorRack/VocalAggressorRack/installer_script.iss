; Vocal Aggressor Rack - Inno Setup Script
; This script generates a professional Windows installer for your VST3 plugin.

[Setup]
AppName=Vocal Aggressor Rack
AppVersion=1.0.0
DefaultDirName={commoncf}\VST3\Vocal Aggressor Rack.vst3
DefaultGroupName=Vocal Aggressor Rack
UninstallDisplayIcon={app}\VocalAggressorRack.vst3
Compression=lzma2
SolidCompression=yes
OutputDir=Installer
OutputBaseFilename=VocalAggressorRack_Setup
; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on 64-bit Windows.
ArchitecturesInstallIn64BitMode=x64

[Files]
; IMPORTANT: This assumes you have built the "Release" version in Visual Studio.
; Update the 'Source' path below to point to where Visual Studio saves your compiled .vst3 folder.

; Using Visual Studio 2022 path (Default):
Source: "Builds\VisualStudio2022\x64\Release\VST3\VocalAggressorRack.vst3\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs

; Note: If you are using Visual Studio 2026, uncomment the line below and comment out the one above.
; Source: "Builds\VisualStudio2026\x64\Release\VST3\VocalAggressorRack.vst3\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs

[Messages]
WelcomeLabel2=This will install the Vocal Aggressor Rack VST3 plugin on your computer.

[Code]
// No special code needed for a standard VST3 install
