# Tool chain to use.
# NDK_TOOLCHAIN_VERSION := 4.8

# Which processors to build for.
# APP_ABI := all
# APP_ABI := armeabi x86
APP_ABI := armeabi

#  Enable C++11 and exceptions
APP_CPPFLAGS += -std=c++11 -fexceptions

# Instruct to use the static GNU STL implementation
APP_STL := gnustl_static
# APP_STL := stlport_static

LOCAL_C_INCLUDES += ${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.9/include
