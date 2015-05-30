# Which processor architectures to build for.
# APP_ABI := all
# APP_ABI := x86
# APP_ABI := armeabi
APP_ABI := armeabi-v7a

#  Enable C++11 and exceptions
APP_CPPFLAGS += -std=c++11 -fexceptions

# Instruct to use the static GNU STL implementation
# http://www.kandroid.org/ndk/docs/CPLUSPLUS-SUPPORT.html
APP_STL := gnustl_shared

LOCAL_C_INCLUDES += ${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.9/include
