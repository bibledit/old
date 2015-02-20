#!/bin/bash

export IPHONEOS_DEPLOYMENT_TARGET="6.0"
export CC="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"

ARCH="armv7"
export CFLAGS="-arch ${ARCH} -pipe -Os -gdwarf-2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"
export LDFLAGS="-arch ${ARCH} -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"
make distclean
./configure --disable-shared --enable-static --host="${ARCH}-apple-darwin" --enable-threaded-resolver --with-darwinssl
make -j `sysctl -n hw.logicalcpu_max`
cp lib/.libs/libcurl.a "${HOME}/Desktop/libcurl-${ARCH}.a"
cp include/curl/curlbuild.h ~/Desktop/curlbuild32.h

ARCH="armv7s"
export CFLAGS="-arch ${ARCH} -pipe -Os -gdwarf-2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"
export LDFLAGS="-arch ${ARCH} -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"
make distclean
./configure --disable-shared --enable-static --host="${ARCH}-apple-darwin" --enable-threaded-resolver --with-darwinssl
make -j `sysctl -n hw.logicalcpu_max`
cp lib/.libs/libcurl.a "${HOME}/Desktop/libcurl-${ARCH}.a"

ARCH="arm64"
export IPHONEOS_DEPLOYMENT_TARGET="7.0"
export CFLAGS="-arch ${ARCH} -pipe -Os -gdwarf-2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"
export LDFLAGS="-arch ${ARCH} -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"
make distclean
./configure --disable-shared --enable-static --host="aarch64-apple-darwin" --enable-threaded-resolver --with-darwinssl
make -j `sysctl -n hw.logicalcpu_max`
cp lib/.libs/libcurl.a "${HOME}/Desktop/libcurl-${ARCH}.a"
cp include/curl/curlbuild.h ~/Desktop/curlbuild64.h

ARCH="i386"
export IPHONEOS_DEPLOYMENT_TARGET="6.0"
export CFLAGS="-arch ${ARCH} -pipe -Os -gdwarf-2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk"
export CPPFLAGS="-D__IPHONE_OS_VERSION_MIN_REQUIRED=${IPHONEOS_DEPLOYMENT_TARGET%%.*}0000"
export LDFLAGS="-arch ${ARCH} -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk"
make distclean
./configure --disable-shared --enable-static --host="${ARCH}-apple-darwin" --enable-threaded-resolver --with-darwinssl
make -j `sysctl -n hw.logicalcpu_max`
cp lib/.libs/libcurl.a "${HOME}/Desktop/libcurl-${ARCH}.a"

ARCH="x86_64"
export IPHONEOS_DEPLOYMENT_TARGET="7.0"
export CFLAGS="-arch ${ARCH} -pipe -Os -gdwarf-2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk"
export CPPFLAGS="-D__IPHONE_OS_VERSION_MIN_REQUIRED=${IPHONEOS_DEPLOYMENT_TARGET%%.*}0000"
export LDFLAGS="-arch ${ARCH} -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk"
make distclean
./configure --disable-shared --enable-static --host="${ARCH}-apple-darwin" --enable-threaded-resolver --with-darwinssl
make -j `sysctl -n hw.logicalcpu_max`
cp lib/.libs/libcurl.a "${HOME}/Desktop/libcurl-${ARCH}.a"

lipo -create -output "${HOME}/Desktop/libcurl.a" "${HOME}/Desktop/libcurl-armv7.a" "${HOME}/Desktop/libcurl-armv7s.a" "${HOME}/Desktop/libcurl-arm64.a" "${HOME}/Desktop/libcurl-i386.a" "${HOME}/Desktop/libcurl-x86_64.a"
