echo Build script from within Windows.

echo Setting the path so it can also find the Cygwin binaries.
set PATH=C:\cygwin\bin;%PATH%

echo Deleting possible existing Bibledit installation.
del /F /S /Q C:\bibledit

echo Building the Bibledit core library from within Cygwin bash.
bash /cygdrive/e/dev/bibledit/windows/build.sh

echo Copying the result.
robocopy C:\bibledit E:\bibledit-windows /MIR
