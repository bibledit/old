#!/bin/bash


# Post-compilation cleanup of the bibledit library for Windows.


pushd ../webroot


# Clean out stuff no longer needed.
find . -name "*.h" -delete
find . -name "*.cpp" -delete
find . -name "*.c" -delete
rm *.m4
rm -r autom*cache
rm bibledit
rm compile
rm config.*
rm configure*
rm depcomp
rm dev
rm install-sh
rm missing
rm reconfigure
rm valgrind
find . -name ".deps" -exec rm -r "{}" \; > /dev/null 2>&1
find . -name ".dirstamp" -delete
rm Makefile*
rm -f server
rm stamp-h1


popd


say Post compilation for Windows is ready
