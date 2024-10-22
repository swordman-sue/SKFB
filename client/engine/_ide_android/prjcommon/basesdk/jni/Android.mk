LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)  
LOCAL_MODULE := Bugly
LOCAL_SRC_FILES := ../../../../libs/android/libbugly/libBugly.so
include $(PREBUILT_SHARED_LIBRARY)