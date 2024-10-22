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

	//ͳ����ҵ�һ�δ�Ӧ�÷���
	void SendOpenFirstTimeStatistics(float delta);			
	void OnSendOpenFirstTimeStatistics(bool result, int ret_code, const char* data_ptr, unsigned int len);

	//ͳ����Ҵ�Ӧ�÷���
	void SendOpenStatistics(float delta);
	void OnSendOpenStatistics(bool result, int ret_code, const char* data_ptr, unsigned int len);

public:
	static StartUpScene *start_scene;
	static BTN_CALLBACK btn_confirm_func;		//ȷ���ص�����
	static BTN_CALLBACK btn_cancel_func;		//ȡ���ص�����

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
	bool m_block;									// ������ʶ
	StartLogic::LogicProgress m_failed_progress;	// ��¼����Ľ׶�

	float m_progress_val;							// ��ǰ��������
	unsigned int m_cur_scirpt_index;				// �ڼ���scirpt.ios
	std::vector<ProgressGoingItem> m_progress_list;	// �����ƽ���ʷ�ؼ���

	float m_network_speed;					// �����ٶ�
	std::vector<float>	m_network_speed_list;
	ZQ::UINT32 m_last_download_size;		// ����������
	float m_last_download_time;				// ͳ�����ٵ�ʱ��
	unsigned int m_last_http_id;			// �����ļ���ʶ

	bool m_keypad_enable;					// �Ƿ������������¼�
	COCOSize m_design_size;					// �豸��Ļ��С
	GameLayer *m_child_layer;				// �Ӳ�
	float m_child_layer_width;				// ȫ���Ӳ���
	float m_child_layer_height;				// ȫ���Ӳ�߶�

	Scale9SpriteEx* m_loading_bg;
	LabelEx * m_loading_text;			
	bool m_stop_progress;

	SelectLanguageLayer* selectlanguagelayer; //ѡ�����Ե���

	bool m_bAutoRetry;
	short m_hAutoRetryCount;
	//����׼���Զ�����״̬ʱ��(��)
	float m_fAutoRetryPassTime;
};

#endif
