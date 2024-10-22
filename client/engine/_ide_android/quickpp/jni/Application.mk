APP_STL := gnustl_static
NDK_TOOLCHAIN_VERSION=clang

APP_CFLAGS := -DHAVE_STRINGIZE
APP_CPPFLAGS := -frtti -fexceptions -DANDROID -DCC_FIX_ARTIFACTS_BY_STRECHING_TEXEL=1 -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -DLUABIND_CPP0x=1 -std=c++11 -fsigned-char
APP_LDFLAGS := -latomic
APP_BUILD_SCRIPT := ../prjcommon/jni/Android.mk
APP_PLATFORM := android-14
APP_ABI := armeabi-v7a

APP_OPTIM := release
ifeq ($(APP_OPTIM),debug)
     APP_CPPFLAGS += -O0 -g3 -Wall -D_DEBUG -DCOCOS2D_DEBUG=1
else
     APP_CPPFLAGS += -DNDEBUG
endif