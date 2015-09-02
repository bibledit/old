#!/bin/bash


# Download http://www.sqlite.org/sqlite-amalgamation-3071502.zip
# Unpack it and put sqlite.h/c in the "jni" folder.


# Run this script from the directory where it is located: ./build.sh
# Prerequisites:
# Android SDK for Mac OS X.


# Export environment variables to find the Android SDK and NDK tools.
export ANDROID_HOME=~/scr/android-sdk-macosx
export PATH=$PATH:~/scr/android-sdk-macosx/platform-tools:~/scr/android-sdk-macosx/tools:~/scr/android-ndk-r10e


# Build native code.
ndk-build clean
ndk-build


# Copy libraries into place.
cp -r obj ..


# Information about building sqlite:
# https://github.com/stockrt/sqlite3-android
# http://stackoverflow.com/questions/5523067/sqlite-with-android-ndk
# https://github.com/lookout/sqlite-android/blob/master/jni/Android.mk
