::::Self elevating batch file. Found on http://stackoverflow.com/questions/5944180/how-do-you-run-a-command-as-an-administrator-from-the-windows-command-line
<!-- : --- Self-Elevating Batch Script ---------------------------
@whoami /groups | find "S-1-16-12288" > nul && goto :admin
set "ELEVATE_CMDLINE=cd /d "%~dp0" & call "%~f0" %*"
cscript //nologo "%~f0?.wsf" //job:Elevate & exit /b

-->
<job id="Elevate"><script language="VBScript">
  Set objShell = CreateObject("Shell.Application")
  Set objWshShell = WScript.CreateObject("WScript.Shell")
  Set objWshProcessEnv = objWshShell.Environment("PROCESS")
  strCommandLine = Trim(objWshProcessEnv("ELEVATE_CMDLINE"))
  objShell.ShellExecute "cmd", "/c " & strCommandLine, "", "runas"
</script></job>
:admin -----------------------------------------------------------

@echo off
echo Running as elevated user.
echo Script file : %~f0
echo Arguments   : %*
echo Working dir : %cd%
echo.
:: administrator commands here
:: e.g., run shell as admin
whoami
SET INSTALLSRC=%~dp0
SET INSTALLSRC=%INSTALLSRC:~0,-1%
SET VERSION=%1
"%INSTALLSRC%\Bibledit.exe" -o"C:\program files\" -y

copy "C:\program files\biblEdit-%VERSION%\bibledit.lnk" "C:\Users\Public\Desktop"
del "%INSTALLSRC%\bibledit.exe"
::mshta.exe vbscript:Execute("msgbox ""Installation Complete"",0,""BiblEdit"":close")
::exit
exit
cmd /k
::exit