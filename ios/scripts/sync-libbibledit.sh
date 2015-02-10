#!/bin/bash

# Sychronizes the libbibledit source tree to iOS and cleans it up.

rsync -av --delete ../../lib/ ../webroot

pushd ../webroot

make distclean

find . -name "*.h" -delete

find . -name "*.cpp" -delete

find . -name "*.c" -delete

find . -name "*.php" -delete

rm Make*
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
rm -rf .deps

popd
