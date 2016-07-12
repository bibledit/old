echo Build script from within Windows.

echo Setting the path so it can also find the Cygwin binaries.
set PATH=C:\cygwin\bin;%PATH%

echo Deleting possible existing Bibledit installation.
del /F /S /Q C:\bibledit > nul

echo Building the Bibledit core library from within Cygwin bash.
bash /cygdrive/e/dev/bibledit/windows/build.sh

echo Copy the Bibledit Windows GUI into place.
copy /Y C:\bibledit.exe\bibledit.exe C:\bibledit

echo Create the bibleditsetup.exe.
E:
cd \
cd dev\bibledit\windows
"C:\Program Files (x86)\Inno Setup 5\Compil32.exe" /cc package.iss

echo Copying the result.
robocopy C:\bibledit E:\bibledit-windows /MIR > nul
