#ifndef WEB_VIEW_H
#define WEB_VIEW_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include <string>

class WebView : public cocos2d::CCNode
{
public:

	// 缓存类型
	enum CACHE_TYPE
	{
        CacheTypeUseProtocolCachePolicy = 0,
		CacheTypeReturnCacheDataElseLoad = 1,		// Return Cache Data Else Load
		CacheTypeReturnCacheDataDontLoad,			// Return Cache Data
		CacheTypeReloadIgnoringLocalCacheData,		// Ignoring Cache
		CacheTypeClearHistory,						// Clear Cache
		CacheTypeDefault = CacheTypeUseProtocolCachePolicy,	 // Use Protocol Cache Policy
	};
    typedef struct _viewPoint {
        int x;
        int y;
    } ViewPoint;
    
    
    bool init();
    
    // {{0, 0}, {960 640}}
    // x <0, 960>    y <0, 640>
    void SetCenter(int x, int y);
    //    width <1, 960>    height<1, 640>
    void SetSize(int width, int height);
    
    void SetCacheType(CACHE_TYPE cache_type);
    
    void SetUrl(std::string url);
    
    void SetTimeout(int timeout) {m_request_timeout = timeout;}
    
    int Show();
    
    void Close(int tag);
    
    void Refresh(int tag);
    
    void SetCloseCallback(ZQ::UINT32 func_id) {m_lua_close_func_id = func_id;}
    
    static WebView * CreateWebView(std::string url, CACHE_TYPE cache_type, int timeout);
    
    // 关闭所有webview
    static void CloseAll();

protected:
    virtual ~WebView();
    
    CACHE_TYPE m_cache_type;
    ViewPoint m_top_left_point;
    ViewPoint m_center;
	int m_width, m_height;
	int m_request_timeout;
    std::string m_url;
	ZQ::UINT32 m_lua_close_func_id;
private:
	WebView();
};

#endif
