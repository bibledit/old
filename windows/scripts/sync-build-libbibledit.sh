#!/bin/bash


# Synchronize and prepare for building the bibledit library for Windows.


pushd ../webroot


# Sychronizes the libbibledit data files in the source tree to here and cleans them up.
rsync -av --delete ../../lib/ .
make distclean


# Configure Bibledit in client mode,
./configure --enable-client --enable-cygwin


# Save the header file.
cp library/bibledit.h ../gui/bibledit


# Compile and build.
make


popd


