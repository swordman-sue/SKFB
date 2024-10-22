#include "platform/webview.h"
#import "AdWebViewManager.h"
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

int WebView::Show()
{
    //NSLog(@"%@ %d %d url:%s", [NSString stringWithFormat:@"{{%d,%d},{%d,%d}}", m_top_left_point.x, m_top_left_point.y, m_width, m_height], m_request_timeout, m_cache_type, m_url.c_str());
    int ret_val = [[AdWebViewManager GetInstance] ShowWebUrl:[NSString stringWithUTF8String:m_url.c_str()]
                                         WithCacheType:m_cache_type
                                           WithTimeout:m_request_timeout
                                        WithRectString:[NSString stringWithFormat:@"{{%d,%d},{%d,%d}}", m_top_left_point.x, m_top_left_point.y, m_width, m_height]];
	return ret_val;
}
void WebView::Refresh(int tag)
{
    [[AdWebViewManager GetInstance] Refresh:tag];
}

void WebView::Close(int tag)
{
    [[AdWebViewManager GetInstance] HideWebUrl:tag isdistory:true];
}

void WebView::SetUrl(std::string url)
{
	m_url = url;
}

void WebView::CloseAll()
{
    [[AdWebViewManager GetInstance] HideWebUrl:0 isdistory:true];
}
