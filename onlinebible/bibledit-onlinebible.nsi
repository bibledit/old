; bibledit-onlinebible.nsi
;
;
; It will install bibleditonlinebible.exe into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "Bibledit-OnlineBible"

; The file to write
OutFile "bibledit-onlinebible-setup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Bibledit\OnlineBible

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Bibledit\OnlineBible" "Install_Dir"

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
Section "Bibledit-OnlineBible"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "bibleditonlinebible.exe"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Bibledit\OnlineBible "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditOnlineBible" "DisplayName" "Bibledit-OnlineBible"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditOnlineBible" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditOnlineBible" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditOnlineBible" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Bibledit-OnlineBible"
  CreateShortCut "$SMPROGRAMS\Bibledit-OnlineBible\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Bibledit-OnlineBible\Bibledit-OnlineBible.lnk" "$INSTDIR\bibleditonlinebible.exe" "$INSTDIR\bibledit-onlinebible.ico" "$INSTDIR\bibleditonlinebible.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditOnlineBible"
  DeleteRegKey HKLM SOFTWARE\Bibledit\OnlineBible

  ; Remove files and uninstaller
  Delete $INSTDIR\bibleditonlinebible.exe
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Bibledit-OnlineBible\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Bibledit-OnlineBible"
  RMDir "$INSTDIR"

SectionEnd
