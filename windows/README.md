## Bibledit for Windows

### The current code for Bibledit for Windows is at https://github.com/bibledit/bibledit-windows.

Description of the old folder and code:

build.sh: Script to update and build the Bibledit core library from within a Cygwin shell.

build.bat: Script to run build.sh from within Windows.

gui: Contains the graphical user interface version in C#. Depends on the library.

package.iss: The Inno Setup package script.


The MinGW variant being developed has these files:
mingw.sh: The builder script.
mingw.txt: MinGW build environment.
The MinGW variant does not currently work.


The Windows Visual Studio variant being developed has these files:
visualstudio.sh: The script that prepares the visual studio project.
visualstudio.txt: Developer information.
