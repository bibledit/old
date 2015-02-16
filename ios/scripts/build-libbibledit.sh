#!/bin/bash

# Build libbibledit on OS X for iOS.

sh sterilize-libbibledit.sh
sh compile-for.sh armv7 iPhoneOS 32

sh sterilize-libbibledit.sh
sh compile-for.sh armv7s iPhoneOS 32

sh sterilize-libbibledit.sh
sh compile-for.sh arm64 iPhoneOS 64

sh sterilize-libbibledit.sh
sh compile-for.sh i386 iPhoneSimulator 32

sh sterilize-libbibledit.sh
sh compile-for.sh x86_64 iPhoneSimulator 64

cp ../../lib/library/bibledit.h ~/Desktop

echo Creating fat library file
lipo -create -output ~/Desktop/libbibledit.a ~/Desktop/libbibledit-armv7.a ~/Desktop/libbibledit-armv7s.a ~/Desktop/libbibledit-arm64.a ~/Desktop/libbibledit-i386.a ~/Desktop/libbibledit-x86_64.a
lipo -info ~/Desktop/libbibledit.a

echo Copying library into place
mv ~/Desktop/libbibledit.a ../lib

echo Restore library to default state
pushd ../../lib
./configure
make clean > /dev/null
popd


