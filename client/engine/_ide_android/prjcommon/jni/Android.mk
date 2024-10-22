LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := skyxmod
LOCAL_SRC_FILES := main.cpp
LOCAL_C_INCLUDES := 				
					
LOCAL_EXPORT_LDLIBS := -llog \
					   -lz \
					   -lGLESv2

LOCAL_LDLIBS := -lGLESv2 \
				-llog \
				-lz

# 链接动态库
LOCAL_SHARED_LIBRARIES := libskyx

include $(BUILD_SHARED_LIBRARY) 

# 预编译库路径
$(call import-add-path, $(LOCAL_PATH)/../../../libs/android)

$(call import-module,libskyx)
