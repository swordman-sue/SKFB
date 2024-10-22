LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := coengine_static

LOCAL_MODULE_FILENAME := libcoengine

ifeq ($(USE_ARM_MODE),1)
LOCAL_ARM_MODE := arm
endif

# 配置自己的源文件目录和源文件后缀名
MY_FILES_PATH := $(LOCAL_PATH)/common \
				 $(LOCAL_PATH)/luascript \
				 $(LOCAL_PATH)/network \
				 $(LOCAL_PATH)/pbc
 				 
MY_FILES_SUFFIX := %.cpp %.c

# 递归遍历目录下的所有的文件
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# 获取相应的源文件
MY_ALL_FILES := $(foreach src_path,$(MY_FILES_PATH), $(call rwildcard,$(src_path),*.*) ) 
MY_SRC_LIST  := $(filter $(MY_FILES_SUFFIX),$(MY_ALL_FILES)) 
MY_SRC_LIST  := $(MY_SRC_LIST:$(LOCAL_PATH)/%=%)

# 去除字串的重复单词
define uniq =
  $(eval seen :=)
  $(foreach _,$1,$(if $(filter $_,${seen}),,$(eval seen += $_)))
  ${seen}
endef

# 递归遍历获取所有目录
MY_ALL_DIRS := $(dir $(foreach src_path,$(MY_FILES_PATH), $(call rwildcard,$(src_path),*/) ) )
MY_ALL_DIRS := $(call uniq,$(MY_ALL_DIRS))

# 赋值给NDK编译系统
LOCAL_SRC_FILES := $(MY_SRC_LIST)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../libs/include \
					$(LOCAL_PATH)/../libs/include/common \
					$(LOCAL_PATH)/../libs/include/luascript/lua \
					$(LOCAL_PATH)/../libs/include/network \
					$(LOCAL_PATH)/../libs/include/network/basicnetwork \
					$(LOCAL_PATH)/../libs/include/pbc

						
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../libs/include \
						   $(LOCAL_PATH)/../libs/include/common \
						   $(LOCAL_PATH)/../libs/include/luascript/lua \
						   $(LOCAL_PATH)/../libs/include/network \
						   $(LOCAL_PATH)/../libs/include/network/basicnetwork \
						   $(LOCAL_PATH)/../libs/include/pbc

LOCAL_EXPORT_LDLIBS := -llog \
                       -lm \
                       -landroid
		   
include $(BUILD_STATIC_LIBRARY)
