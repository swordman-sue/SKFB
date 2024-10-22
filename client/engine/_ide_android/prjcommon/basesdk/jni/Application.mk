# The ARMv7 is significanly faster due to the use of the hardware FPU
#APP_ABI := armeabi armeabi-v7a
#APP_PLATFORM := android-8

APP_STL := gnustl_static
APP_CPPFLAGS := -frtti -fexceptions -DCC_FIX_ARTIFACTS_BY_STRECHING_TEXEL=1 
APP_PLATFORM :=android-14
APP_ABI := armeabi-v7a

APP_OPTIM := release
ifeq ($(APP_OPTIM),debug)
     APP_CPPFLAGS += -O0 -g3 -Wall -D_DEBUG -DCOCOS2D_DEBUG=1 
else
     APP_CPPFLAGS += -DNDEBUG
endif


