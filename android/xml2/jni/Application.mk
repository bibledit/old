LOCAL_PATH := $(call my-dir)

APP_MODULES := xml2

# Some default compiler warnings.
# APP_CFLAGS += -Wall -Wno-unused -Wno-multichar -Wstrict-aliasing=2 -Werror

# Don't export symbols in release build by default
APP_CFLAGS += -fvisibility=hidden
APP_CFLAGS += -fno-exceptions -fmessage-length=0

APP_ABI := all
# APP_ABI := armeabi-v7a
