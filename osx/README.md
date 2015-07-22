# Bibledit OS X

Bibledit for OS X source code repository and developer assistance

## Introduction

Bibledit has been working on OS X in various forms:
* Bibledit-Gtk written for Linux and ported to Mac OS X.
* Bibledit-Web written in PHP and adapted to run on Mac OS X.

The current port of Bibledit for OS X consists of the following parts:
* A native Bibledit OS X app with a WebView.
* The bibledit library written in C++, compiled for OS X.

## Sandboxing

With the app sandbox enabled in Xcode, the app cannot write to the webroot folder in the app's Resources. To deal with this, the app copies the webroot folder to the documents folder within the sandbox. The sandbox is at ~/Library/Containers/org.bibledit.osx/

## libcurl

The bibledit library depends on libcurl. Bibledit should be linked with a working version of libcurl. 

* Linking with OS X 10.10 SDK libcurl.dylib: It initially works but after a few minutes of synchronizing with Bibledit Cloud it starts to time out, and then stops working.

* Linking with the static library (libcurl.a) of a Macports installation, and add all the macports dependent *.a libraries to the Bibledit Xcode project to include them: This works well. No timeouts occur.

## Building libbibledit

Run script "build.sh".

## Building and distributing the app

* Rename /opt/local to something else so it cannot link against any of those libraries.
* Open the Bibledit Xcode project.
* Clean it.
* Build it.
* Copy the Bibledit product from Xcode to another location.
* Test it from the other location.
* Test it on a clean OS X installation.
* Archive the app from Xcode, and submit it to the Apple App Store.
* Have it reviewed by Apple.
* On release, immediately test it on a clean OS X installation.

## Compliance with Mac App Store

Upon submission to the store the Bibledit app was rejected on the grounds that the interface was not of sufficient quality. It just had a button to open Safari to display the web app. The solution to this was to integrate a WebView into the Bibledit app.
Upon a second submission to the store, the app was rejected on the grounds that it accessed '/Library/Managed Preferences/Guest/com.apple.familycontrols.contentfilter.plist'. After an investigation into this, it appeared that the Bibledit app itself was not accessing this location, but that the integrated WebView did it. The WebView is a component from Apple, thus access to this location is outside Bibledit's control.
Upon a third submission to the store, the app was rejected because "The app uses 'osx' in the menu item names in a manner that is not consistent with Apple's trademark guidelines." The solution was to update the names of the menu items.
After a submission to the store, the app was rejected for the following reason: "This app uses one or more entitlements which do not have matching functionality within the app. Apps should have only the minimum set of entitlements necessary for the app to function properly. Please remove all entitlements that are not needed by your app and submit an updated binary for review, including the following: com.apple.security.network.server". The following explanation was given: The app includes a web server on port 8080 to function properly. The GUI of the app is a Web View that connects to the included web server to display the app's interface and to interact with the app. After removing the "com.apple.security.network.server" entitlement, the app ceases to work.
After a submission to the store, the app was rejected for the following reason: "We have found that when the user closes the main application window there is no menu item to re-open it.". The solution was to quit the app when the main window is closed.

## History

Distributing the app:
Create a dmg in Disk Utility.
Add Bibledit.app to it.
Set the correct background image, and view options.
Create a link to /Applications, and add this link.
Convert the dmg to read-only.

