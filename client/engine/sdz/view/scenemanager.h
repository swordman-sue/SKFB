#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include "struct.h"

class GameScene;

class DefaultKeyDelegate : public cocos2d::Node
{
public:
	DefaultKeyDelegate();
	~DefaultKeyDelegate();

	void SetKeypadEnabled(bool is_enabled);
	void SetLuaBackKey(ZQ::UINT32 back_fun_id){ m_lua_back_fun_id = back_fun_id; }
	void SetLuaMenuKey(ZQ::UINT32 menu_fun_id){ m_lua_menu_fun_id = menu_fun_id; }

protected:
	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* unused_event);
	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* unused_event);

protected:
	bool		m_is_enabled;
	ZQ::UINT32	m_lua_back_fun_id;		//返回键回调
	ZQ::UINT32	m_lua_menu_fun_id;		//菜单键回调
	cocos2d::EventListenerKeyboard*		m_listener;

// start 支持遥控器 by liwei 15-02-03
public:
	void keyArrowClicked(ZQ::UINT32 arrow_fun_id){ m_lua_arrow_fun_id = arrow_fun_id; }
	void keyEnterClicked(ZQ::UINT32 enter_fun_id){ m_lua_enter_fun_id = enter_fun_id; }

protected:
	ZQ::UINT32	m_lua_arrow_fun_id;		//方向键回调
	ZQ::UINT32	m_lua_enter_fun_id;		//Win32的Enter键或遥控器的OK键回调
// end
};


class DefaultTouchDelegate :public cocos2d::Node
{
public:
	DefaultTouchDelegate();
	~DefaultTouchDelegate();
	virtual bool onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);
	virtual void onTouchMoved(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);
	virtual void onTouchEnded(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);
	virtual void onTouchCancelled(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);

public:
	void SetLuaTouchDown(ZQ::UINT32 down_fun_id){m_lua_down_fun_id = down_fun_id;}
	void SetLuaTouchMove(ZQ::UINT32 move_fun_id){m_lua_move_fun_id = move_fun_id;}
	void SetLuaTouchUp(ZQ::UINT32 up_fun_id){m_lua_up_fun_id = up_fun_id;}
	void SetLuaTouchCancel(ZQ::UINT32 cancel_fun_id){m_lua_cancel_fun_id = cancel_fun_id;}
	void SetTouchEnable(bool enable);
	void SetSwallow(bool enable);

protected:
	bool		m_is_enabled;
	ZQ::UINT32	m_lua_down_fun_id;		//触摸点击回调
	ZQ::UINT32	m_lua_move_fun_id;		//触摸移动回调
	ZQ::UINT32	m_lua_up_fun_id;		//触摸松开回调
	ZQ::UINT32	m_lua_cancel_fun_id;	//触摸取消回调
	bool		m_swallow;				//是否吞噬
	cocos2d::EventListenerTouchOneByOne*	m_listener;
};

//用于场景管理的的类
class SceneManager
{
public:
	static SceneManager* GetInstance();
	~SceneManager();

private:
	SceneManager();

public:  
	//获取当前运行场景
	GameScene* GetRunningScene();
	
	//从栈中移除场景
	void PopScene();
	
	//从栈中加入场景
	void PushScene(GameScene* scene);
	
	//替换当前运行的场景
	void ReplaceScene(GameScene* scene);
	
	//以第一个场景运行
	void RunWithScene(GameScene* scene);
	
	//获取屏幕适配方案
	unsigned int GetResolutionPolicy();

	//获取设计分辨率尺寸
	void GetDesignSize(COCOSize* size);
    
	//获取设备分辨率尺寸
	void GetScreenSize(COCOSize* size);

	//获取可视化区域
	void GetVisibleRect(cocos2d::Rect* rect);

    //获取IPhone安全区域
    void GetSafeAreaRect(cocos2d::Rect* rect);

	//显示消息提示框
	void ShowMessage(const std::string &msg, const std::string &title, ZQ::UINT32 fun_id);
	
	//设置是否响应触摸消息
	void SetEnableTouchEvent(bool enable);

	//设置游戏质量
	void SetQualityLevel(float quality_level);

	//获取游戏质量
	float GetQualityLevel();

	//设置游戏帧数
	void SetGameFrame(float frame);

	//获取游戏帧数
	double GetGameFrame();

	//设置按键的回调
	void SetLuaBackKey(ZQ::UINT32 back_fun_id);
	void SetLuaMenuKey(ZQ::UINT32 menu_fun_id);
	void SetLuaArrowKey(ZQ::UINT32 arrow_fun_id);
	void SetLuaEnterKey(ZQ::UINT32 enter_fun_id);
	//设置是否响应键盘消息
	void SetKeypadEnabled(bool bEnabled);

	//运行开始界面
	void RunStartUpScene();

	//设置触摸回调
	void SetLuaTouchDown(ZQ::UINT32 down_fun_id);
	void SetLuaTouchMove(ZQ::UINT32 move_fun_id);
	void SetLuaTouchUp(ZQ::UINT32 up_fun_id);
	void SetLuaTouchCancel(ZQ::UINT32 cancel_fun_id);
	void SetTouchSwallow(bool is_swallow);

	void Pause();
	void Resume();

	void SetDeltaSpeedTime(float speed_time);
	float GetDeltaSpeedTime();

private:
	cocos2d::Director			*m_director;
	DefaultKeyDelegate			m_key_delegate;
	DefaultTouchDelegate		m_touch_delegate;

	cocos2d::Size				m_screen_size;
	cocos2d::Size				m_design_size;
};

#endif
