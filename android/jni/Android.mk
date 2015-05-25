LOCAL_PATH:= $(call my-dir)

# bibledit lib, which will be built statically
#
include $(CLEAR_VARS)

LOCAL_MODULE    := libbibledit
LOCAL_SRC_FILES := bibledit.cpp

include $(BUILD_STATIC_LIBRARY)

# wrapper lib, which will depend on and include the first one
#
include $(CLEAR_VARS)

LOCAL_MODULE    := libjavawrapper
LOCAL_SRC_FILES := javawrapper.c

LOCAL_STATIC_LIBRARIES := libbibledit

include $(BUILD_SHARED_LIBRARY)
