#ifndef STARTUP_H
#define STARTUP_H

#include "startlogic.h"
#include "view/struct.h"
#include "view/gamescene.h"
#include "view/widget/progress9control.h"
#include "view/widget/labelex.h"
#include "selectlanguagelayer.h"

#include <map>

class StartUpScene: public GameScene
{
private:
	struct ProgressGoingItem
	{
		ProgressGoingItem() : stage(0), is_need_download(false), max_progress(0.0f)
		{

		}
		int stage;
		bool is_need_download;
		float max_progress;
	};

public:
	typedef void (StartUpScene::*BTN_CALLBACK)(void);

public:
	StartUpScene();
	static StartUpScene* Node();

	static void OnMsgDlgBtn(int);

	virtual void onEnter();
	virtual void onExit();
	virtual void update(float delta);
	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	void keyBackClicked();
	void CheckStartProgress(float time);
	void ConfirmRetry();
	void ConfirmUpdatePackage();
	void CancelUpdatePackage();
	void ProgressGoing(float time);

	//统计玩家第一次打开应用方法
	void SendOpenFirstTimeStatistics(float delta);			
	void OnSendOpenFirstTimeStatistics(bool result, int ret_code, const char* data_ptr, unsigned int len);

	//统计玩家打开应用方法
	void SendOpenStatistics(float delta);
	void OnSendOpenStatistics(bool result, int ret_code, const char* data_ptr, unsigned int len);

public:
	static StartUpScene *start_scene;
	static BTN_CALLBACK btn_confirm_func;		//确定回调函数
	static BTN_CALLBACK btn_cancel_func;		//取消回调函数

private:
	void SelectLanguage();
	void ShowLogoUI();
	void ShowSdkUI();
	void ShowDownLoadUI();
	void CreateMsgDlg(const std::string&, BTN_CALLBACK, BTN_CALLBACK cancel_func = NULL, const std::string &confirm_str = "", const std::string &cancel_str = "");
	std::string GetString(const std::string &key, const std::string& default_value = "");
	void SetDownLoadingText(const ProgressGoingItem& item, const char* context);

	void Start();
	void EnterMain();
	void CheckRetry(const StartLogic::LogicProgressItem& progress_item);

private:
	StartLogic m_logic;
	std::map<int, std::string> m_stage_text_map;
	bool m_block;									// 阻塞标识
	StartLogic::LogicProgress m_failed_progress;	// 记录出错的阶段

	float m_progress_val;							// 当前进度条量
	unsigned int m_cur_scirpt_index;				// 第几个scirpt.ios
	std::vector<ProgressGoingItem> m_progress_list;	// 进度推进历史关键点

	float m_network_speed;					// 网络速度
	std::vector<float>	m_network_speed_list;
	ZQ::UINT32 m_last_download_size;		// 已下载数据
	float m_last_download_time;				// 统计网速的时间
	unsigned int m_last_http_id;			// 下载文件标识

	bool m_keypad_enable;					// 是否开启监听键盘事件
	COCOSize m_design_size;					// 设备屏幕大小
	GameLayer *m_child_layer;				// 子层
	float m_child_layer_width;				// 全屏子层宽度
	float m_child_layer_height;				// 全屏子层高度

	Scale9SpriteEx* m_loading_bg;
	LabelEx * m_loading_text;			
	bool m_stop_progress;

	SelectLanguageLayer* selectlanguagelayer; //选择语言弹窗

	bool m_bAutoRetry;
	short m_hAutoRetryCount;
	//进入准备自动重试状态时长(秒)
	float m_fAutoRetryPassTime;
};

#endif
