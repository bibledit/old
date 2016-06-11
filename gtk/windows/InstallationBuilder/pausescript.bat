@echo off
:ping
ping 172.217.4.110 > nul
IF EXIST bibledit.exe (GOTO :ping)