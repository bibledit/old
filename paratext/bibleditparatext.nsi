; bibledit-paratext.nsi
;
;
; It will install bibleditparatext.exe into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "Bibledit-Paratext"

; The file to write
OutFile "bibledit-paratext-setup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Bibledit\Paratext

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Bibledit\Paratext" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Bibledit-Paratext"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "bibleditparatext.exe"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Bibledit\Paratext "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditParatext" "DisplayName" "Bibledit-Paratext"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditParatext" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditParatext" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditParatext" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Bibledit-Paratext"
  CreateShortCut "$SMPROGRAMS\Bibledit-Paratext\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Bibledit-Paratext\Bibledit-Paratext.lnk" "$INSTDIR\bibleditparatext.exe" "$INSTDIR\bibledit-paratext.ico" "$INSTDIR\bibleditparatext.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditParatext"
  DeleteRegKey HKLM SOFTWARE\Bibledit\Paratext

  ; Remove files and uninstaller
  Delete $INSTDIR\bibleditparatext.exe
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Bibledit-Paratext\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Bibledit-Paratext"
  RMDir "$INSTDIR"

SectionEnd

