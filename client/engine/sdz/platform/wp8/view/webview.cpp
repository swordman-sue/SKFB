#include "platform/webview.h"
#include "luaengine/luaengine.h"

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



// x <0, 960>    y <0, 640>
void WebView::SetCenter(int x, int y)
{

}
//    width <1, 960>    height<1, 640>
void WebView::SetSize(int width, int height)
{

}

void WebView::SetCacheType(CACHE_TYPE cache_type)
{

}

void WebView::SetUrl(std::string url)
{


}

int WebView::Show()
{
	return 0;
}

void WebView::Close(int tag)
{

}

void WebView::Refresh(int tag)
{

}

// πÿ±’À˘”–webview
void WebView::CloseAll()
{


}