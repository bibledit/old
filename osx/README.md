Bibledit OS X
=============

Bibledit for OS X source code repository and developer assistance


Introduction


Bibledit has been working on OS X in various forms:
* Bibledit-Gtk written for Linux and ported to Mac OS X.
* Bibledit-Web written in PHP and adapted to run on Mac OS X.


The current port of Bibledit for OS X consists of the following parts:
* A native Bibledit OS X app.
* The bibledit library written in C++, compiled for OS X.
* The system web browser displaying pages through the bibledit library. 


The bibledit library depends on libcurl. Bibledit should be linked with some version of libcurl. Not all versions of libcurl work equally well.

1. OS X 10.10 SDK libcurl.dylib libcurl.4.dylib: It initially works but after a few minutes of synchronizing with a Bibledit server it starts to time out, and stops working.

2. Macports /opt/local/lib/libcurl.4.dylib: Works well.

3. One solution was provided in http://curl.haxx.se/mail/archive-2014-10/0072.html
Download the latest version of curl from http://curl.haxx.se/download.html
$ ./configure --disable-shared --with-darwinssl --enable-threaded-resolver
$ make -j 4
Copy the compiled curl, which is located in ./src/curl to you /usr/bin/
Note: in the make command the sign "`" is indeed back quote, not single quote.
This libcurl.a also times out.
It proves that the timeout is not in libcurl itself, but in one of its dependencies.

4. Another solution would be to link with the static library (libcurl.a) of a Macports installation, and then all the macports dependent *.a libraries to Bibledit to include them.
This works well: No timeout / works on clean Mac OS X.


Building the Bibledit library:
Run script "build.sh".


Building and distributing the app:
Remove all *.o files from webroot (find webroot -name "*.o" -delete)
Rename /opt/local to something else so it cannot link against any of those libraries.
Open the Xcode project.
Build it.
Copy the Bibledit product from Xcode to another location.
Test it from the other location.
Archive the app from Xcode, and submit it to the Apple App Store.


History:

Distributing the app:
Create a dmg in Disk Utility.
Add Bibledit.app to it.
Set the correct background image, and view options.
Create a link to /Applications, and add this link.
Convert the dmg to read-only.

Create a package through software "Packages" available from http://s.sudre.free.fr/Software/Packages/about.html.
