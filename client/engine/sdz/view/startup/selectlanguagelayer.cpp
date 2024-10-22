#include "selectlanguagelayer.h"
#include "view/uiwidget/uiimageviewex.h"
#include "view/uiwidget/uitextex.h"
#include "luaengine/luaengine.h"
#include "httpmanager/httpmanager.h"
#include "platform/sdktools.h"
#include "platform/systemtools.h"
#include "resmanager/resmanager.h"
#include "view/scenemanager.h"
#include "gameengine/gameengine.h"
#include "zqcommon/strutil.h"
#include <iostream>
#include <sstream>
#include "cocostudio/WidgetReader/WidgetReader.h"


SelectLanguageLayer::SelectLanguageLayer() 
:selectedcheckbox(nullptr)
{
}


SelectLanguageLayer::~SelectLanguageLayer()
{
}

SelectLanguageLayer* SelectLanguageLayer::Create()
{
	SelectLanguageLayer* layer = new SelectLanguageLayer();
	if (layer && layer->init())
	{
		//layer->setContentSize(Size::Size(0, 0));
		layer->autorelease();
		return layer;
	}
	else
	{
		CC_SAFE_DELETE(layer);
		return nullptr;
	}
}

void SelectLanguageLayer::onEnter()
{
	Layer::onEnter();
}

void SelectLanguageLayer::onExit()
{
	Layer::onExit();
}

void SelectLanguageLayer::show(std::string language_list_param[], selectlanguage_callback call_back)
{
	call_back_fun = call_back;

	UIImageViewEx *bg = UIImageViewEx::CreateWithImage("resource/start/tipbk.png");
	bg->SetScale9Enabled(true);
	bg->SetCapInsets(40, 40, 30, 30);
	bg->SetContentSize(250, 250);
	this->addChild(bg);

	for (int i=0; i<LANGUAGE_LEN; i++)
	{
		float pos_x = -80;
		float pos_y = 100 - i * 50;
		if (language_list_param[i] != "")
		{
			this->language_list[i] = language_list_param[i];
			UICheckBoxEx * checkbox = UICheckBoxEx::CreateWithImage("resource/start/tick_bk.png", "resource/start/tick.png");
			checkbox->SetPosition(pos_x, pos_y);
			checkbox->setTag(i);
			this->addChild(checkbox);
			checkbox->addEventListener(CC_CALLBACK_2(SelectLanguageLayer::OnSelectEvent, this));
			if (i == 0)
			{
				checkbox->SetSelectedState(true);
				selectedcheckbox = checkbox;
			}

			//FontLabelEx *label = FontLabelEx::CreateWithString(" ", FontType, 20, 120, 24);
			//label->SetString(LuaEngine::GetInstance()->GetLanguageConfig(language_list[i]));
			//label->SetStroke(true);
			//label->SetStrokeColor(68, 44, 12, 255);
			//label->setHorizontalAlignment(TextHAlignment::LEFT);
			UITextEx * label = UITextEx::CreateWithString(LuaEngine::GetInstance()->GetLanguageConfig(language_list[i]), cocostudio::WidgetReader::DefaultFontName, 20);
			label->SetPosition(pos_x + 90, pos_y);
			this->addChild(label);
		}
	}

	surebtn = UIButtonEx::CreateWithImage("resource/start/small_btn.png");
	std::string language = language_list[0];
	surebtn->SetText(LuaEngine::GetInstance()->GetLanguageConfig(language + "_" + "next"));
	surebtn->SetPosition(0, -80);
	surebtn->SetFontSize(20);
	surebtn->addTouchEventListener(CC_CALLBACK_2(SelectLanguageLayer::OnTouchEvent, this));
	this->addChild(surebtn);
}

void SelectLanguageLayer::OnSelectEvent(cocos2d::Ref *pSender, cocos2d::ui::CheckBox::EventType eventType)
{
	if (cocos2d::ui::CheckBox::EventType::SELECTED == eventType)
	{
		if(selectedcheckbox != pSender)
		{
			selectedcheckbox->SetSelectedState(false);
			selectedcheckbox = (UICheckBoxEx*)pSender;
		}
	}
	else
	{
		if (selectedcheckbox == pSender)
		{
			selectedcheckbox->SetSelectedState(true);
		}
	}
	std::string language = this->language_list[selectedcheckbox->getTag()];
	std::string next_str = LuaEngine::GetInstance()->GetLanguageConfig(language + "_" + "next");
	surebtn->SetText(next_str);
}

void SelectLanguageLayer::OnTouchEvent(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		{
			call_back_fun(this->language_list[selectedcheckbox->getTag()]);
			this->NoticePhp();
		}
		break;
	default:
		break;
	}
}

class SelectlanguageListener : public HttpListener
{
public:
	SelectlanguageListener(SelectLanguageLayer *startup)
		:m_startup(startup)
	{
	}
	virtual void OnCallBack(unsigned int http_id, const RequestTask& request_task)
	{

	};
	virtual void Free()
	{
		delete this;
	}

protected:
	SelectLanguageLayer *m_startup;

};

void SelectLanguageLayer::NoticePhp()
{
	//通知php统计玩家选择语言 by chuxi
	std::string url = LuaEngine::GetInstance()->GetConfig("Manager") + "api/pre_register_statis.php";
	HttpInfo http_info;
	http_info.url = url;
	http_info.url_id = rand();
	http_info.time_out_milliseconds = 60 * 60000;
	std::stringstream ss;
	const std::string& param_key_0 = "language";
	ss << selectedcheckbox->getTag();
	const std::string& param_value_0 = ss.str();
	const std::string& param_key_1 = "plat_name";
	const std::string& param_value_1 = SdkTools::GetSdkName();
	const std::string& param_key_2 = "device_id";
	const std::string& param_value_2 = SystemTools::GetDeviceId();
	const std::string& param_key_3 = "channel";
	const std::string& param_value_3 = SdkTools::GetChannelId();
	const std::string& param_key_4 = "system_type";
	int system_type = 0;
	if (SystemTools::GetSystemType() == "apple")
	{
		system_type = 1;
	}
	else if (SystemTools::GetSystemType() == "android")
	{
		system_type = 2;
	}
	else
	{
		system_type = 3;
	}
	ss << system_type;
	const std::string& param_value_4 = ss.str();
	const std::string& param_key_5 = "package_name";
	const std::string& param_value_5 = SystemTools::GetPackageName();
	const std::string& param_key_6 = "package_size";
	const std::string& param_value_6 = LuaEngine::GetInstance()->GetConfig("Package");
	const std::string& param_key_7 = "state";
	const std::string& param_value_7 = "choose_language";
	const std::string& param_key_8 = "time";
	unsigned int time = GameEngine::GetInstance()->GetStartTimeMilli() + GameEngine::GetInstance()->GetNowTimeMilli();
	ss << time;
	const std::string& param_value_8 = ss.str();
	const std::string& param_key_9 = "ticket";
	const std::string& param_value_9 = ZQ::StrUtil::Md5("jf2" + param_value_8 + "jljaoijwweoiHNAS2fjoihks");
	const std::string& param_key_10 = "classify";
	const std::string& param_value_10 = "pre_register";
	
	memcpy(http_info.post_param_names[0], param_key_0.c_str(), param_key_0.length());
	memcpy(http_info.post_param_values[0], param_value_0.c_str(), param_value_0.length());
	memcpy(http_info.post_param_names[1], param_key_1.c_str(), param_key_1.length());
	memcpy(http_info.post_param_values[1], param_value_1.c_str(), param_value_1.length());
	memcpy(http_info.post_param_names[2], param_key_2.c_str(), param_key_2.length());
	memcpy(http_info.post_param_values[2], param_value_2.c_str(), param_value_2.length());
	memcpy(http_info.post_param_names[3], param_key_3.c_str(), param_key_3.length());
	memcpy(http_info.post_param_values[3], param_value_3.c_str(), param_value_3.length());
	memcpy(http_info.post_param_names[4], param_key_4.c_str(), param_key_4.length());
	memcpy(http_info.post_param_values[4], param_value_4.c_str(), param_value_4.length());
	memcpy(http_info.post_param_names[5], param_key_5.c_str(), param_key_5.length());
	memcpy(http_info.post_param_values[5], param_value_5.c_str(), param_value_5.length());
	memcpy(http_info.post_param_names[6], param_key_6.c_str(), param_key_6.length());
	memcpy(http_info.post_param_values[6], param_value_6.c_str(), param_value_6.length());
	memcpy(http_info.post_param_names[7], param_key_7.c_str(), param_key_7.length());
	memcpy(http_info.post_param_values[7], param_value_7.c_str(), param_value_7.length());
	memcpy(http_info.post_param_names[8], param_key_8.c_str(), param_key_8.length());
	memcpy(http_info.post_param_values[8], param_value_8.c_str(), param_value_8.length());
	memcpy(http_info.post_param_names[9], param_key_9.c_str(), param_key_9.length());
	memcpy(http_info.post_param_values[9], param_value_9.c_str(), param_value_9.length());
	memcpy(http_info.post_param_names[10], param_key_10.c_str(), param_key_10.length());
	memcpy(http_info.post_param_values[10], param_value_10.c_str(), param_value_10.length());
	http_info.post_param_num = 11;
	SelectlanguageListener *listener = new SelectlanguageListener(this);
	HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_PLAT_MANAGER_PHP, http_info, listener);
}
