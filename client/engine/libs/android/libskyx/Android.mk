LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libskyx
LOCAL_SRC_FILES := ../release/libskyx.so
include $(PREBUILT_SHARED_LIBRARY)
