#include "platform/webview.h"
#include "luaengine/luaengine.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "platform/CCCommon.h"
#include "cocos2d.h"
#include <string>

#define CPP_INVOKE_JAVA_ADWEBVIEWUTIL "com/kuyue/webview/AdWebViewUtil"

WebView::WebView()
:m_cache_type(CacheTypeDefault)
,m_width(240)
,m_height(160)
,m_request_timeout(30)
,m_url("")
,m_lua_close_func_id(0)
{
	m_top_left_point.x = 0;
	m_top_left_point.y = 0;
	m_center.x = 0;
	m_center.y = 0;
}

bool WebView::init()
{
	return true;
}

WebView * WebView::CreateWebView(std::string url, CACHE_TYPE cache_type, int timeout)
{
	WebView *pRet = new WebView();
	if(pRet && pRet->init())
	{
		pRet->m_url = url;
		pRet->m_cache_type = cache_type;
		pRet->m_request_timeout = timeout;
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

WebView::~WebView()
{
	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_close_func_id);
}

// È¡Öµ·¶Î§ x <0, 960>    y <0, 640>
void WebView::SetCenter(int x, int y)
{
	m_center.x = x;
	m_center.y = y;
	m_top_left_point.x = m_center.x - m_width/2;
	m_top_left_point.y = m_center.y - m_height/2;
}
//    width <1, 960>    height<1, 640>
void WebView::SetSize(int width, int height)
{
	m_width = width;
	m_height = height;
	m_top_left_point.x = m_center.x - m_width/2;
	m_top_left_point.y = m_center.y - m_height/2;
}

void WebView::SetCacheType(CACHE_TYPE cache_type)
{
	m_cache_type = cache_type;
}

void WebView::SetUrl(std::string url)
{
	m_url = url;
}

int WebView::Show()
{
	int ret_val = 0;
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, CPP_INVOKE_JAVA_ADWEBVIEWUTIL,
			"Show", "(Ljava/lang/String;IIIIII)I");
	if (is_get)
	{
		jstring j_url = method_info.env->NewStringUTF(m_url.c_str());
		jint j_cachetype = (jint)m_cache_type;
		jint j_timeout = (jint)m_request_timeout;
		jint j_x = (jint)m_top_left_point.x;
		jint j_y = (jint)m_top_left_point.y;
		jint j_width = (jint)m_width;
		jint j_height = (jint)m_height;
		ret_val = (int)method_info.env->CallStaticIntMethod(method_info.classID, method_info.methodID, j_url, j_cachetype, j_timeout, j_x, j_y, j_width, j_height);
		method_info.env->DeleteLocalRef(method_info.classID);
		method_info.env->DeleteLocalRef(j_url);
	}
	return ret_val;
}
void WebView::Refresh(int tag)
{
	// refresh by tag
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, CPP_INVOKE_JAVA_ADWEBVIEWUTIL,
			"Refresh", "(I)V");
	if (is_get)
	{
		jint j_tag = tag;
		method_info.env->CallStaticVoidMethod(method_info.classID, method_info.methodID, j_tag);
		method_info.env->DeleteLocalRef(method_info.classID);
	}
}

void WebView::Close(int tag)
{
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, CPP_INVOKE_JAVA_ADWEBVIEWUTIL,
			"Close", "(I)V");
	if (is_get)
	{
		jint j_tag = tag;
		method_info.env->CallStaticVoidMethod(method_info.classID, method_info.methodID, j_tag);
		method_info.env->DeleteLocalRef(method_info.classID);
	}
}

void WebView::CloseAll()
{
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, CPP_INVOKE_JAVA_ADWEBVIEWUTIL,
			"CloseAll", "()V");
	if (is_get)
	{
		method_info.env->CallStaticVoidMethod(method_info.classID, method_info.methodID);
		method_info.env->DeleteLocalRef(method_info.classID);
	}
}
