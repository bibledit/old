#!/bin/bash

# Build libbibledit on OS X for iOS.

sh sterilize.sh
sh compile-for.sh armv7 iPhoneOS 32

sh sterilize.sh
sh compile-for.sh armv7s iPhoneOS 32

sh sterilize.sh
sh compile-for.sh arm64 iPhoneOS 64

sh sterilize.sh
sh compile-for.sh i386 iPhoneSimulator 32

sh sterilize.sh
sh compile-for.sh x86_64 iPhoneSimulator 64
