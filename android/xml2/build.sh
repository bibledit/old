#!/bin/bash


# Run this script from the directory where it is located: ./build.sh
# Prerequisites:
# Android SDK for Mac OS X.


# The libxml2 source was downloaded from:
# https://github.com/android/platform_external_libxml2
# and placed in the "jni" folder.

# While compiling it cannot find libicu, so disable the following line:
# jni/include/libxml/xmlversion.h.in:#define LIBXML_ICU_ENABLED

# File "Application.mk" was added to the "jni" folder.

# The NDK for at least one processor architecture does not have the "rand_r"
# function, so disable the following line in jni/config.h:
# #define HAVE_RAND_R 1

# Remove LOCAL_CLANG := true from jni/Android.mk.


# Export environment variables to find the Android SDK and NDK tools.
export ANDROID_HOME=~/scr/android-sdk-macosx
export PATH=$PATH:~/scr/android-sdk-macosx/platform-tools:~/scr/android-sdk-macosx/tools:~/scr/android-ndk-r10e


# Build native code.
ndk-build clean
ndk-build


# Copy libraries into place.
cp -r obj ..
