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
	ZQ::UINT32	m_lua_back_fun_id;		//���ؼ��ص�
	ZQ::UINT32	m_lua_menu_fun_id;		//�˵����ص�
	cocos2d::EventListenerKeyboard*		m_listener;

// start ֧��ң���� by liwei 15-02-03
public:
	void keyArrowClicked(ZQ::UINT32 arrow_fun_id){ m_lua_arrow_fun_id = arrow_fun_id; }
	void keyEnterClicked(ZQ::UINT32 enter_fun_id){ m_lua_enter_fun_id = enter_fun_id; }

protected:
	ZQ::UINT32	m_lua_arrow_fun_id;		//������ص�
	ZQ::UINT32	m_lua_enter_fun_id;		//Win32��Enter����ң������OK���ص�
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
	ZQ::UINT32	m_lua_down_fun_id;		//��������ص�
	ZQ::UINT32	m_lua_move_fun_id;		//�����ƶ��ص�
	ZQ::UINT32	m_lua_up_fun_id;		//�����ɿ��ص�
	ZQ::UINT32	m_lua_cancel_fun_id;	//����ȡ���ص�
	bool		m_swallow;				//�Ƿ�����
	cocos2d::EventListenerTouchOneByOne*	m_listener;
};

//���ڳ�������ĵ���
class SceneManager
{
public:
	static SceneManager* GetInstance();
	~SceneManager();

private:
	SceneManager();

public:  
	//��ȡ��ǰ���г���
	GameScene* GetRunningScene();
	
	//��ջ���Ƴ�����
	void PopScene();
	
	//��ջ�м��볡��
	void PushScene(GameScene* scene);
	
	//�滻��ǰ���еĳ���
	void ReplaceScene(GameScene* scene);
	
	//�Ե�һ����������
	void RunWithScene(GameScene* scene);
	
	//��ȡ��Ļ���䷽��
	unsigned int GetResolutionPolicy();

	//��ȡ��Ʒֱ��ʳߴ�
	void GetDesignSize(COCOSize* size);
    
	//��ȡ�豸�ֱ��ʳߴ�
	void GetScreenSize(COCOSize* size);

	//��ȡ���ӻ�����
	void GetVisibleRect(cocos2d::Rect* rect);

    //��ȡIPhone��ȫ����
    void GetSafeAreaRect(cocos2d::Rect* rect);

	//��ʾ��Ϣ��ʾ��
	void ShowMessage(const std::string &msg, const std::string &title, ZQ::UINT32 fun_id);
	
	//�����Ƿ���Ӧ������Ϣ
	void SetEnableTouchEvent(bool enable);

	//������Ϸ����
	void SetQualityLevel(float quality_level);

	//��ȡ��Ϸ����
	float GetQualityLevel();

	//������Ϸ֡��
	void SetGameFrame(float frame);

	//��ȡ��Ϸ֡��
	double GetGameFrame();

	//���ð����Ļص�
	void SetLuaBackKey(ZQ::UINT32 back_fun_id);
	void SetLuaMenuKey(ZQ::UINT32 menu_fun_id);
	void SetLuaArrowKey(ZQ::UINT32 arrow_fun_id);
	void SetLuaEnterKey(ZQ::UINT32 enter_fun_id);
	//�����Ƿ���Ӧ������Ϣ
	void SetKeypadEnabled(bool bEnabled);

	//���п�ʼ����
	void RunStartUpScene();

	//���ô����ص�
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
