#!/bin/bash


# Run this script from the directory where it is located: ./gdb.
# Prerequisites:
# Android SDK for Mac OS X.
# Apache Ant: port install apache-ant.


# In the AndroidManifest.xml, in the <application> element, enter the following property:
# android:debuggable="true"



# Export environment variables to find the Android SDK and NDK tools.
export ANDROID_HOME=~/scr/android-sdk-macosx
export PATH=$PATH:~/scr/android-sdk-macosx/platform-tools:~/scr/android-sdk-macosx/tools:~/scr/android-ndk-r10e


# https://software.intel.com/en-us/android/articles/how-to-test-and-debug-ndk-based-android-applications
ndk-gdb --start org.bibledit.android/.MainActivity --force --verbose
