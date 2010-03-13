@ECHO OFF
REM This wrapper script starts bibledit and the helper processes.

REM The first reasons that made this script necessary was that Gtk got mixed up
REM when a child process was repeatedly run.
REM Later on git was integrated, and called often.
REM Messages have been seen saying that there were too many open files.
REM Both the Gtk based graphical functions ceased to work and the child processed 
REM failed to be spawned.
REM By separating those child processes completely from Bibledit's Gtk, these
REM problems disappeared.
REM Another reason for using helper programs is that Bibledit got big and 
REM sometimes got slower because of the many child processed. 
REM By separating these processed this improved too.

TITLE Bibledit Console

REM The temporal directory. Create it.
MKDIR "%USERPROFILE%\.bibledit_temp" > NUL 2>&1

REM Start Windows Outpost
START /WAIT TASKKILL /T /F /FI "IMAGENAME eq BWOUTPOST.EXE" > NUL 2>&1
START /B /LOW BWOUTPOST.EXE

REM Start bibledit-git, and kill any previous such process.
SET OUTFILEGIT="%USERPROFILE%\.bibledit_temp\git.log"
SET ERRFILEGIT="%USERPROFILE%\.bibledit_temp\git.err.log"
SET OUTFILEOLD="%LOGFILEGIT%.old"
SET ERRFILEOLD="%LOGFILEGIT%.err.old"
MOVE /Y "%OUTFILEGIT%" "%OUTFILEOLD%" > NUL 2>&1
MOVE /Y "%ERRFILEGIT%" "%ERRFILEOLD%" > NUL 2>&1
SET BIBLEDIT_GIT=bibledit-git.exe
START /WAIT TASKKILL /T /F /FI "IMAGENAME eq %BIBLEDIT_GIT%" > NUL 2>&1
START /B /LOW %BIBLEDIT_GIT% > %OUTFILEGIT% 2> %ERRFILEGIT%

REM The log file of bibledit.
REM The output of the processes is piped to the logfile.
REM The previous logfile is be saved.
SET OUTFILE="%USERPROFILE%\.bibledit_temp\bibledit.log"
SET ERRFILE="%USERPROFILE%\.bibledit_temp\bibledit.err.log"
SET OUTFILEOLD="%USERPROFILE%\.bibledit_temp\bibledit.log.old"
SET ERRFILEOLD="%USERPROFILE%\.bibledit_temp\bibledit.log.err.old"
MOVE /Y %LOGFILE% %LOGFILEOLD% > NUL 2>&1
MOVE /Y %ERRFILE% %ERRFILEOLD% > NUL 2>&1
REM Start bibledit-bin with all the commandline arguments.
SET BIBLEDIT_BIN=bibledit-bin.exe
START /WAIT %BIBLEDIT_BIN% %* > %OUTFILE% 2> %ERRFILE%
TASKKILL /T /F /FI "IMAGENAME eq BWOUTPOST.EXE" > NUL 2>&1
