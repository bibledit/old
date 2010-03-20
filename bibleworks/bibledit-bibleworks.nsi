; bibledit-bibleworks.nsi
;
;
; It will install bibleditbibleworks.exe into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "Bibledit-BibleWorks"

; The file to write
OutFile "bibledit-bibleworks-setup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Bibledit\BibleWorks

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Bibledit\BibleWorks" "Install_Dir"

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
Section "Bibledit-BibleWorks"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "bibleditbibleworks.exe"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Bibledit\BibleWorks "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditBibleWorks" "DisplayName" "Bibledit-BibleWorks"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditBibleWorks" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditBibleWorks" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditBibleWorks" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Bibledit-BibleWorks"
  CreateShortCut "$SMPROGRAMS\Bibledit-BibleWorks\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Bibledit-BibleWorks\Bibledit-BibleWorks.lnk" "$INSTDIR\bibleditbibleworks.exe" "$INSTDIR\bibledit-bibleworks.ico" "$INSTDIR\bibleditbibleworks.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BibleditBibleWorks"
  DeleteRegKey HKLM SOFTWARE\Bibledit\BibleWorks

  ; Remove files and uninstaller
  Delete $INSTDIR\bibleditbibleworks.exe
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Bibledit-BibleWorks\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Bibledit-BibleWorks"
  RMDir "$INSTDIR"

SectionEnd
