# Bibledit OS X

Bibledit for OS X source code repository and developer assistance

## Introduction

Bibledit has been working on OS X in various forms:
* Bibledit-Gtk written for Linux and ported to Mac OS X.
* Bibledit-Web written in PHP and adapted to run on Mac OS X.

The current port of Bibledit for OS X consists of the following parts:
* A native Bibledit OS X app.
* The bibledit library written in C++, compiled for OS X.
* The system web browser displaying pages through the bibledit library. 

## Sandboxing

With the app sandboxed enabled in Xcode, the app cannot write to the webroot folder in the app's Resources. To deal with this, the app copies the webroot folder to within the sandbox.

## libcurl

The bibledit library depends on libcurl. Bibledit should be linked with some version of libcurl. 

* OS X 10.10 SDK libcurl.dylib libcurl.4.dylib: It initially works but after a few minutes of synchronizing with a Bibledit server it starts to time out, and stops working.

* Link with the static library (libcurl.a) of a Macports installation, and add all the macports dependent *.a libraries to the Bibledit Xcode project to include them. This works well: No timeouts occur.

## Building libbibledit

Run script "build.sh".

## Building and distributing the app

* Rename /opt/local to something else so it cannot link against any of those libraries.
* Open the Bibledit Xcode project.
* Build it.
* Copy the Bibledit product from Xcode to another location.
* Test it from the other location.
* Test it on a clean OS X installation.
* Archive the app from Xcode, and submit it to the Apple App Store.
* Have it reviewed by Apple.
* On release, immediately test it on a clean OS X installation.

## History

Distributing the app:
Create a dmg in Disk Utility.
Add Bibledit.app to it.
Set the correct background image, and view options.
Create a link to /Applications, and add this link.
Convert the dmg to read-only.

