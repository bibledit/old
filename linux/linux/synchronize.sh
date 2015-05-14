#!/bin/bash


# This script runs in a Terminal on OS X.
# It refreshes the bibledit library sources.
# It synchronizes all relevant data to a Linux host.


rsync -av --exclude "*.o" --exclude ".deps" --exclude xcode --exclude ".dirstamp" --exclude "*~" --exclude "autom4te.cache" --exclude "*.a" ../../lib/ ..
rm ../valgrind
rm ../unittest
rm -f ../server
rsync -av .. 192.168.2.12:./bibledit


exit

Linux dependencies, write them in an installation document.

sudo apt-get install build-essential git zip pkgconf libxml2-dev libsqlite3-dev libcurl4-openssl-dev libssl-dev libgtk-3-dev
