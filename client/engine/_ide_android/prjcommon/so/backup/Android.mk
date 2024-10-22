LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := skyx

ifeq ($(USE_ARM_MODE),1)
LOCAL_ARM_MODE := arm
endif

LOCAL_SRC_FILES := main.cpp \
                ../../../sdz/classes/AppDelegate.cpp \
				../../../sdz/filesystem/fileasync.cpp \
				../../../sdz/filesystem/fileasynchelper.cpp \
				../../../sdz/filesystem/filehelper.cpp \
				../../../sdz/filesystem/filesystemex.cpp \
				../../../sdz/filesystem/fileversion.cpp \
				../../../sdz/filesystem/setting.cpp \
				../../../sdz/gameengine/gameengine.cpp \
				../../../sdz/httpclient/httpclient.cpp \
				../../../sdz/httpmanager/httpmanager.cpp \
				../../../sdz/httpmanager/httpstruct.cpp \
				../../../sdz/httpmanager/httpconnection.cpp \
				../../../sdz/json/src/json_writer.cpp \
				../../../sdz/json/src/json_value.cpp \
				../../../sdz/json/src/json_reader.cpp \
				../../../sdz/location/location.cpp \
				../../../sdz/luaengine/luacall.cpp \
				../../../sdz/luaengine/luaengine.cpp \
				../../../sdz/luaengine/luaregister.cpp \
				../../../sdz/luaengine/luatinker/lua_tinker.cpp \
				../../../sdz/luaengine/luamessageadapter.cpp \
				../../../sdz/luaengine/luaerrorreport.cpp \
				../../../sdz/luaengine/luaprofile.cpp \
				../../../sdz/msgpush/msgpush.cpp \
				../../../sdz/platform/android/fileasynchandler.cpp \
				../../../sdz/platform/android/msgasynchandler.cpp \
				../../../sdz/platform/android/msgdata.cpp \
				../../../sdz/platform/android/sdktools.cpp \
				../../../sdz/platform/android/systemtools.cpp \
				../../../sdz/platform/android/voice.cpp \
				../../../sdz/platform/android/locationtools.cpp \
				../../../sdz/platform/android/view/webview.cpp \
				../../../sdz/platform/android/basesdkjnihelper.cpp \
				../../../sdz/platform/android/pushtools.cpp \
				../../../sdz/resmanager/resmanager.cpp \
				../../../sdz/resmanager/installer.cpp \
				../../../sdz/sdkhandle/sdkhandle.cpp \
				../../../sdz/sound/soundmanager.cpp \
				../../../sdz/sound/voicehelper.cpp \
				../../../sdz/sound/voicemanager.cpp \
				../../../sdz/zqcommon/strutil.cpp \
				../../../sdz/zqcommon/logutil.cpp \
				../../../sdz/cocos2dxext/texturecahceex.cpp \
				../../../sdz/cocos2dxext/ziputilsex.cpp \
				../../../sdz/zip/zip.cpp \
				../../../sdz/netadapter/netadapter.cpp \
				../../../sdz/statemachine/gamestate.cpp \
				../../../sdz/statemachine/gamestatemachine.cpp \
				../../../sdz/timer/timerquest.cpp \
				../../../sdz/view/gamelayer.cpp \
				../../../sdz/view/gamescene.cpp \
				../../../sdz/view/nodeunity.cpp \
				../../../sdz/view/scenemanager.cpp \
				../../../sdz/view/skeletondatacache.cpp \
				../../../sdz/view/skeletondatacache_ssdz.cpp \
				../../../sdz/view/viewhelper.cpp \
				../../../sdz/view/widgetunity.cpp \
				../../../sdz/view/action/actionmanager.cpp \
				../../../sdz/view/action/actiontimelineex.cpp \
				../../../sdz/view/font/font.cpp \
				../../../sdz/view/font/fontmanager.cpp \
				../../../sdz/view/font/richitem.cpp \
				../../../sdz/view/font/richitemparser.cpp \
				../../../sdz/view/font/stringconv.cpp \
				../../../sdz/view/render/batchrenderer.cpp \
				../../../sdz/view/render/textureutil.cpp \
				../../../sdz/view/render/colortype.cpp \
				../../../sdz/view/startup/startlogic.cpp \
				../../../sdz/view/startup/startup.cpp \
				../../../sdz/view/startup/selectlanguagelayer.cpp \
				../../../sdz/view/uiwidget/uipageviewex.cpp \
				../../../sdz/view/uiwidget/uilistviewex.cpp \
				../../../sdz/view/uiwidget/uibuttonex.cpp \
				../../../sdz/view/uiwidget/uitextex.cpp \
				../../../sdz/view/uiwidget/uiimageviewex.cpp \
				../../../sdz/view/uiwidget/uiloadingbarex.cpp \
				../../../sdz/view/uiwidget/uitextfieldex.cpp \
				../../../sdz/view/uiwidget/uilayoutex.cpp \
				../../../sdz/view/uiwidget/uicheckboxex.cpp \
				../../../sdz/view/uiwidget/uitextatlasex.cpp \
				../../../sdz/view/uiwidget/uisliderex.cpp \
				../../../sdz/view/uiwidget/uitouchgroupex.cpp \
				../../../sdz/view/uiwidget/uiscrollviewex.cpp \
				../../../sdz/view/widget/edittext.cpp \
				../../../sdz/view/widget/edittextex.cpp \
				../../../sdz/view/widget/labelex.cpp \
				../../../sdz/view/widget/graphiclabel.cpp \
				../../../sdz/view/widget/graphicrender.cpp \
				../../../sdz/view/widget/scale9spriteex.cpp \
				../../../sdz/view/widget/imagebutton.cpp \
				../../../sdz/view/widget/spriteex.cpp \
				../../../sdz/view/widget/imagesprite.cpp \
				../../../sdz/view/widget/particlesprite.cpp \
				../../../sdz/view/widget/progress9control.cpp \
				../../../sdz/view/widget/progresscontrol.cpp \
				../../../sdz/view/widget/progresstimer.cpp \
				../../../sdz/view/widget/richtext.cpp \
				../../../sdz/view/widget/skeletonanimation.cpp \
				../../../sdz/view/widget/clippingnodeex.cpp \
				../../../sdz/fribidi/fribidi.c \
				../../../sdz/fribidi/fribidi-arabic.c \
				../../../sdz/fribidi/fribidi-bidi.c \
				../../../sdz/fribidi/fribidi-bidi-types.c \
				../../../sdz/fribidi/fribidi-brackets.c \
				../../../sdz/fribidi/fribidi-char-sets.c \
				../../../sdz/fribidi/fribidi-char-sets-cap-rtl.c \
				../../../sdz/fribidi/fribidi-char-sets-cp1255.c \
				../../../sdz/fribidi/fribidi-char-sets-cp1256.c \
				../../../sdz/fribidi/fribidi-char-sets-iso8859-6.c \
				../../../sdz/fribidi/fribidi-char-sets-iso8859-8.c \
				../../../sdz/fribidi/fribidi-char-sets-utf8.c \
				../../../sdz/fribidi/fribidi-deprecated.c \
				../../../sdz/fribidi/fribidi-joining.c \
				../../../sdz/fribidi/fribidi-joining-types.c \
				../../../sdz/fribidi/fribidi-mirroring.c \
				../../../sdz/fribidi/fribidi-run.c \
				../../../sdz/fribidi/fribidi-shape.c \
				../../../cocos2dxlib/extensions/particle/attribute.cpp \
				../../../cocos2dxlib/extensions/particle/dynamicattribute.cpp \
				../../../cocos2dxlib/extensions/particle/particle.cpp \
				../../../cocos2dxlib/extensions/particle/particledatamanager.cpp \
				../../../cocos2dxlib/extensions/particle/particlehelper.cpp \
				../../../cocos2dxlib/extensions/particle/particlesystem.cpp \
				../../../cocos2dxlib/extensions/particle/simplespline.cpp \
				../../../cocos2dxlib/extensions/particle/strconv.cpp \
				../../../cocos2dxlib/extensions/particle/strutil.cpp \
				../../../cocos2dxlib/extensions/particle/technique.cpp \
				../../../cocos2dxlib/extensions/particle/xmlreader.cpp \
				../../../cocos2dxlib/extensions/particle/affector/affector.cpp \
				../../../cocos2dxlib/extensions/particle/affector/colour.cpp \
				../../../cocos2dxlib/extensions/particle/affector/deflector.cpp \
				../../../cocos2dxlib/extensions/particle/affector/elasticity.cpp \
				../../../cocos2dxlib/extensions/particle/affector/linearforce.cpp \
				../../../cocos2dxlib/extensions/particle/affector/pathfollower.cpp \
				../../../cocos2dxlib/extensions/particle/affector/randomiser.cpp \
				../../../cocos2dxlib/extensions/particle/affector/rotation.cpp \
				../../../cocos2dxlib/extensions/particle/affector/scale.cpp \
				../../../cocos2dxlib/extensions/particle/affector/suction.cpp \
				../../../cocos2dxlib/extensions/particle/affector/texanim.cpp \
				../../../cocos2dxlib/extensions/particle/affector/vortex.cpp \
				../../../cocos2dxlib/extensions/particle/emitter/box.cpp \
				../../../cocos2dxlib/extensions/particle/emitter/circle.cpp \
				../../../cocos2dxlib/extensions/particle/emitter/emitter.cpp \
				../../../cocos2dxlib/extensions/particle/emitter/line.cpp \
				../../../cocos2dxlib/extensions/particle/emitter/point.cpp \
				../../../cocos2dxlib/extensions/particle/emitter/position.cpp \
				../../../cocos2dxlib/extensions/particle/emitter/sphere.cpp \
				../../../cocos2dxlib/extensions/particle/render/billboard.cpp \
				../../../cocos2dxlib/extensions/particle/render/emittedps.cpp \
				../../../cocos2dxlib/extensions/particle/render/entity.cpp \
				../../../cocos2dxlib/extensions/particle/render/renderer.cpp \
				../../../cocos2dxlib/extensions/particle/render/rendererbuffer.cpp \
				../../../cocos2dxlib/extensions/particle/render/ribbon.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCAnimationState.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCAnimationTrack.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCEnt3D.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCMyMesh.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCMeshManager.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCMeshSerializer.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCMeshSkeletonAnimation.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCResObjManager.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCSerializer.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCMySkeleton3D.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCSkeleton3DManager.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCSkeleton3DSerializer.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCSkeletonData.cpp \
				../../../cocos2dxlib/extensions/particle/render/mesh/CCSkeletonInstance3D.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../sdz \
					$(LOCAL_PATH)/../../../sdz/classes \
					$(LOCAL_PATH)/../../../sdz/view/font \
					$(LOCAL_PATH)/../../../sdz/luaengine \
					$(LOCAL_PATH)/../../../sdz/fribidi \
					$(LOCAL_PATH)/../../../libs/include \
					$(LOCAL_PATH)/../../../libs/include/luascript \
					$(LOCAL_PATH)/../../../libs/include/luascript/lua \
					$(LOCAL_PATH)/../../../libs/include/network \
					$(LOCAL_PATH)/../../../cocos2dxlib \
					$(LOCAL_PATH)/../../../cocos2dxlib/cocos/audio/include \
					$(LOCAL_PATH)/../../../cocos2dxlib/cocos/editor-support \
					$(LOCAL_PATH)/../../../cocos2dxlib/cocos/ui \
					$(LOCAL_PATH)/../../../cocos2dxlib/extensions \
					$(LOCAL_PATH)/../../../cocos2dxlib/external

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
LOCAL_WHOLE_STATIC_LIBRARIES += coengine_static

include $(BUILD_SHARED_LIBRARY)

$(call import-add-path,$(LOCAL_PATH)/../../..)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2dxlib)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2dxlib/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2dxlib/external)
	
$(call import-module,editor-support/spine)
$(call import-module,libs_src)
	
		