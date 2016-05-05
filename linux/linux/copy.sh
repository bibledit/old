#!/bin/bash


# This script runs in a Terminal on OS X.
# It synchronizes the sources for Bibledit for Linux to /tmp/bibledit-linux


LINUXSOURCE=`dirname $0`
cd $LINUXSOURCE
BIBLEDITLINUX=/tmp/bibledit-linux
echo Synchronizing relevant source code to $BIBLEDITLINUX
mkdir -p $BIBLEDITLINUX
rsync --archive --delete ../../lib $BIBLEDITLINUX/
rsync --archive --delete ../../linux $BIBLEDITLINUX/
echo Done

