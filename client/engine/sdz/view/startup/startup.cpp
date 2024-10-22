#include "startup.h"

#include <math.h>

#include "cocos2d.h"
#include "cocostudio/WidgetReader/WidgetReader.h"

#include "view/scenemanager.h"
#include "view/gamelayer.h"
#include "view/widget/spriteex.h"
#include "view/widget/scale9spriteex.h"
#include "view/widget/progress9control.h"
#include "view/widget/skeletonanimation.h"
#include "view/viewhelper.h"
#include "view/uiwidget/uitextex.h"

#include "platform/sdktools.h"
#include "platform/systemtools.h"

#include "luaengine/luaengine.h"
#include "gameengine/gameengine.h"

#include "sound/soundmanager.h"
#include "resmanager/resmanager.h"
#include "filesystem/filesystemex.h"
#include "filesystem/fileversion.h"
#include "filesystem/filehelper.h"
#include "filesystem/setting.h"
#include "httpmanager/httplistener.h"

#include "zqcommon/strutil.h"
#include "zqcommon/logutil.h"
#include "zqcommon/gamedef.h"
#include "json/json.h"

#include "particle/strconv.h"

extern "C"
{
#include "lua/lua.h"
}

using namespace cocos2d;
using std::min;

// 动画配置单元
namespace {
struct AnimPos
{
	int anim_id;
	int x;
	int y;
};
}

StartUpScene* StartUpScene::start_scene = NULL;
StartUpScene::BTN_CALLBACK StartUpScene::btn_confirm_func = NULL;
StartUpScene::BTN_CALLBACK StartUpScene::btn_cancel_func = NULL;

StartUpScene::StartUpScene()
	: m_block(false)
	, m_progress_val(0.0f)
	, m_cur_scirpt_index(0)
	, m_network_speed(0)
	, m_last_download_size(0)
	, m_last_download_time(0)
	, m_last_http_id(0)
	, m_keypad_enable(false)
	, m_loading_text(0)
	, m_stop_progress(false)
	, m_bAutoRetry(false)
	, m_hAutoRetryCount(0)
	, m_fAutoRetryPassTime(0)
{
	SceneManager::GetInstance()->GetDesignSize(&m_design_size);
	m_child_layer_width = m_design_size.width;
	m_child_layer_height = m_design_size.height;
}

StartUpScene* StartUpScene::Node()
{
	StartUpScene *game_scene = new StartUpScene();

	if(!game_scene->init())
	{
		CC_SAFE_DELETE(game_scene);
		return NULL;
	}

	game_scene->autorelease();

	return game_scene;
}

class StatisticsListener : public HttpListener
{
public:
	enum StatisticsType
	{
		ST_OPEN_FIRST_TIME = 0,
		ST_OPEN,
	};

	StatisticsListener(StartUpScene *startup, StatisticsType type) 
		: m_startup(startup)
		, m_type(type)
	{
	}

	virtual void OnCallBack(unsigned int http_id, const RequestTask& request_task)
	{
		switch (m_type)
		{
		case StatisticsListener::ST_OPEN_FIRST_TIME:
			m_startup->OnSendOpenFirstTimeStatistics(request_task.result, request_task.ret_code, request_task.data_ptr, request_task.data_len);
			break;

		case StatisticsListener::ST_OPEN:
			m_startup->OnSendOpenStatistics(request_task.result, request_task.ret_code, request_task.data_ptr, request_task.data_len);
			break;

		default:
			break;
		}
	};

	virtual void Free()
	{
		delete this;
	}

protected:
	StartUpScene *m_startup;
	StatisticsType m_type;
};

//统计玩家第一次打开应用方法
void StartUpScene::SendOpenFirstTimeStatistics(float delta)
{
	const std::string& is_send = Setting::GetInstance()->Get("IsSendOpenFirstTimeStatistics");
	if (is_send != "")
	{
		return;
	}

	std::string statistic_url = LuaEngine::GetInstance()->GetConfig("StatisticsURL");
	if (statistic_url == "")
	{
		return;
	}

	Json::Value context;
	context["deviceid"] = Json::Value(SystemTools::GetDeviceId());
	context["channelid"] = Json::Value(SdkTools::GetChannelId());
	if (SystemTools::GetSystemType() == "apple")
	{
		context["idfa"] = Json::Value(SystemTools::GetIdentifierForAdvertisers());
		context["idfv"] = Json::Value(SystemTools::GetIdentifierForVendor());
	}
	Json::Value root;
	root["appid"] = Json::Value(LuaEngine::GetInstance()->GetConfig("StatisticsAppID"));
	root["context"] = context;
	Json::FastWriter json_writer;
	std::string json_str = json_writer.write(root);

	HttpInfo hi;
	hi.url = statistic_url + "/receive/rest/install";
	hi.time_out_milliseconds = 15000;
	hi.post_param_str = json_str;
	hi.header_vec.push_back("Content-Type=application/json");

	StatisticsListener *listener = new StatisticsListener(this, StatisticsListener::ST_OPEN_FIRST_TIME);
	HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_PLAT_MANAGER_PHP, hi, listener);
}

void StartUpScene::OnSendOpenFirstTimeStatistics(bool result, int ret_code, const char* data_ptr, unsigned int len)
{
	static int send_num = 0;
	if (!result)
	{
		LogUtil::LogError("[OnSendOpenFirstTimeStatistics failure][ret_code:%d]", ret_code);
		if (send_num < 2)
		{
			this->scheduleOnce(schedule_selector(StartUpScene::SendOpenFirstTimeStatistics), 4.0f);
		}
		return;
	}

	Json::Value root;
	Json::Reader json_reader;
	std::string json_str(data_ptr, len);
	if (!json_reader.parse(json_str, root))
	{
		LogUtil::LogError("[OnSendOpenFirstTimeStatistics JsonDecodeError][data:%s len:%d]", data_ptr, len);
		return;
	}

	if (!root["status"].isInt())
	{
		LogUtil::LogError("[OnSendOpenFirstTimeStatistics ReturnCodeError status is not int]");
		return;
	}

	int status = root["status"].asInt();
	if (status == 0)
	{
		Setting::GetInstance()->Set("IsSendOpenFirstTimeStatistics", "true");
		return;
	}

	LogUtil::LogError("[OnSendOpenFirstTimeStatistics ReturnCodeError][status:%d, json_str:%s]", status, json_str.c_str());
}

//统计玩家打开应用方法
void StartUpScene::SendOpenStatistics(float delta)
{
	std::string statistic_url = LuaEngine::GetInstance()->GetConfig("StatisticsURL");
	if (statistic_url == "")
	{
		return;
	}

	Json::Value context;
	context["deviceid"] = Json::Value(SystemTools::GetDeviceId());
	context["channelid"] = Json::Value(SdkTools::GetChannelId());
	context["devicetype"] = Json::Value(SystemTools::GetDeviceModel());
	if (SystemTools::GetSystemType() == "apple")
	{
		context["idfa"] = Json::Value(SystemTools::GetIdentifierForAdvertisers());
		context["idfv"] = Json::Value(SystemTools::GetIdentifierForVendor());
	}
	Json::Value root;
	root["appid"] = Json::Value(LuaEngine::GetInstance()->GetConfig("StatisticsAppID"));
	root["context"] = context;
	Json::FastWriter json_writer;
	std::string json_str = json_writer.write(root);

	HttpInfo hi;
	hi.url = statistic_url + "/receive/rest/startup";
	hi.time_out_milliseconds = 15000;
	hi.post_param_str = json_str;
	hi.header_vec.push_back("Content-Type=application/json");

	StatisticsListener *listener = new StatisticsListener(this, StatisticsListener::ST_OPEN);
	HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_PLAT_MANAGER_PHP, hi, listener);
}

void StartUpScene::OnSendOpenStatistics(bool result, int ret_code, const char* data_ptr, unsigned int len)
{
	static int send_num = 0;
	if (!result)
	{
		LogUtil::LogError("[OnSendOpenStatistics failure][ret_code:%d]", ret_code);
		if (send_num < 2)
		{
			this->scheduleOnce(schedule_selector(StartUpScene::SendOpenStatistics), 4.0f);
		}
		return;
	}

	Json::Value root;
	Json::Reader json_reader;
	std::string json_str(data_ptr, len);
	if (!json_reader.parse(json_str, root))
	{
		LogUtil::LogError("[OnSendOpenStatistics JsonDecodeError][data:%s len:%d]", data_ptr, len);
		return;
	}

	if (!root["status"].isInt())
	{
		LogUtil::LogError("[OnSendOpenStatistics ReturnCodeError status is not int]");
		return;
	}

	int status = root["status"].asInt();
	if (status != 0)
	{
		LogUtil::LogError("[OnSendOpenStatistics ReturnCodeError][status:%d, json_str:%s]", status, json_str.c_str());
	}
}

void StartUpScene::onEnter()
{
	Scene::onEnter();
    
    if (!SystemTools::IsNetworkAvailable())
    {
        this->scheduleOnce(schedule_selector(StartUpScene::SendOpenFirstTimeStatistics), 5.0f);
        this->scheduleOnce(schedule_selector(StartUpScene::SendOpenStatistics), 5.0f);
    }
    else
    {
        SendOpenFirstTimeStatistics(0);
        SendOpenStatistics(0);
    }

    //初始化语言相关
	SelectLanguage();

	//注册检查进度的函数
	schedule(schedule_selector(StartUpScene::update), 0.05f);

	std::string print_load_log = LuaEngine::GetInstance()->GetConfig("PrintLoadLog");
	if (print_load_log == "true")
	{
		CCTextureCache::getInstance()->print_load_log = true;
	}

	//拷贝启动资源到下载目录
	std::string cur_app_version = Setting::GetInstance()->Get("StartCoverVersion");
	bool cover_start = (cur_app_version != SystemTools::GetAppVersionCode());
	if(!FileSystemEx::GetInstance()->CopyDirFromInitialToDownload("resource/start", "resource/start", cover_start))
	{
		LogUtil::LogError("[StartUpScene::onEnter CopyError][path:%s]", "resource/start");
	}

	if(!FileSystemEx::GetInstance()->CopyDirFromInitialToDownload("trdres", "trdres", cover_start))
	{
		LogUtil::LogError("[StartUpScene::onEnter CopyError][path:%s]", "trdres");
	}
	Setting::GetInstance()->Set("StartCoverVersion", SystemTools::GetAppVersionCode());

	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(StartUpScene::onKeyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(StartUpScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	m_child_layer = GameLayer::Node();
	m_child_layer->SetPosition(m_design_size.width / 2, m_design_size.height / 2);
	AddLayer(m_child_layer);

	std::string skip_logo = LuaEngine::GetInstance()->GetConfig("SkipLogo");
	if (skip_logo != "true")
	{
		//显示公司logo界面
		ShowLogoUI();
	}
	else
	{
		//跳过公司logo闪屏
		ShowSdkUI();
	}
}

void StartUpScene::onExit()
{
	Scene::onExit();
	unschedule(schedule_selector(StartUpScene::update));
}

//返回键
void StartUpScene::keyBackClicked()
{
	SdkTools::ExitSdk(GetString("is_exit_game", "exit game?").c_str());
}

void StartUpScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* unused_event)
{
	CC_UNUSED_PARAM(unused_event);
}

void StartUpScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* unused_event)
{
	CC_UNUSED_PARAM(unused_event);
	if (keyCode == EventKeyboard::KeyCode::KEY_BACK)
	{
		keyBackClicked();
	}
#if CC_ENABLE_SCRIPT_BINDING
	if (kScriptTypeNone != _scriptType)
	{
		KeypadScriptData data(keyCode, this);
		ScriptEvent event(kKeypadEvent, &data);
		ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
	}
#endif
}

void StartUpScene::ShowLogoUI()
{
	SpriteEx *bk_label = SpriteEx::CreateWithImageFile("resource/start/bg_cp.sdz");
	float scaleX = m_design_size.width / GameType::SCREEN_DESIGN_WIDTH_DEF;
	float scaleY = m_design_size.height / GameType::SCREEN_DESIGN_HEIGHT_DEF;
	bk_label->setScale(std::max(scaleX, scaleY));
	m_child_layer->AddChild(bk_label);

	//淡入淡出效果
	cocos2d::ActionInterval *fade_delay = cocos2d::DelayTime::create(2.0);
	cocos2d::ActionInterval *fade_out = cocos2d::FadeOut::create(0.25);
	cocos2d::CallFunc *fade_func = cocos2d::CallFunc::create(this, cocos2d::SEL_CallFunc(&StartUpScene::ShowSdkUI));

	cocos2d::FiniteTimeAction *fade_callfunc_action = cocos2d::CCSequence::create(fade_delay, fade_out, fade_func, NULL);
	bk_label->runAction(fade_callfunc_action);
}

void StartUpScene::ShowSdkUI()
{
	m_child_layer->RemoveAllChild(true);

	std::string sdk_start_image = SdkTools::GetStartImagePath();
	if(sdk_start_image == "")
	{
		Start();
		return;
	}
	
	if(!FileSystemEx::GetInstance()->IsDownloadPathFileExist(sdk_start_image))
	{
		if(!FileSystemEx::GetInstance()->CopyFileFromInitialToDownload(sdk_start_image, sdk_start_image, 0, 0))
		{
			Start();
			return;
		}
	}
	
	SpriteEx *bk_label = SpriteEx::CreateWithImageFile(sdk_start_image);
	bk_label->setOpacity(0);
	m_child_layer->AddChild(bk_label);

	//淡入淡出效果
	cocos2d::ActionInterval *fade_in = cocos2d::FadeIn::create(0.25);
	cocos2d::ActionInterval *fade_delay = cocos2d::DelayTime::create(1.5);
	cocos2d::ActionInterval *fade_out = cocos2d::FadeOut::create(0.5);
	cocos2d::CallFunc *fade_func = cocos2d::CallFunc::create(this, cocos2d::SEL_CallFunc(&StartUpScene::Start));
	cocos2d::FiniteTimeAction *fade_callfunc_action = cocos2d::Sequence::create(fade_in, fade_delay, fade_out, fade_func, NULL);
	bk_label->runAction(fade_callfunc_action);
}

void StartUpScene::ShowDownLoadUI()
{
	m_child_layer->RemoveAllChild(true);

	// 背景
    std::string bg_path = FileSystemEx::GetInstance()->GetResourcePath("trdres/bg_resupdate.jpg");
	if (bg_path.empty())
	{
        bg_path = "resource/start/bg_resupdate.sdz";
	}
	SpriteEx *bk_label = SpriteEx::CreateWithImageFile(bg_path);
	float scaleX = m_design_size.width / GameType::SCREEN_DESIGN_WIDTH_DEF;
	float scaleY = m_design_size.height / GameType::SCREEN_DESIGN_HEIGHT_DEF;
	bk_label->setScale(std::max(scaleX, scaleY));
	m_child_layer->AddChild(bk_label);

	// 从配置读取动画数据
	std::vector<AnimPos> anim_list;
	if (0 != LuaEngine::GetInstance()->CallFunction("require", "s", "resource/start/startup"))
	{
		LogUtil::LogError("[LuaEngine::CallLuaStartUpFunc RequireError][require lua file startup.cnf error]");
	}
	else
	{
		lua_State* stat_up_lua_state = LuaEngine::GetInstance()->GetLuaState();
		lua_getglobal(stat_up_lua_state, "StartUp");
		if(!lua_istable(stat_up_lua_state, -1))
		{
			lua_pop(stat_up_lua_state, 1);
			LogUtil::LogError("[StartUpScene::ShowDownLoadUI GetConfigTableError] StartUp");
			return;
		}

		lua_getfield(stat_up_lua_state, -1, "anim_list");
		if(!lua_istable(stat_up_lua_state, -1))
		{
			lua_pop(stat_up_lua_state, 2);
			LogUtil::LogError("[StartUpScene::ShowDownLoadUI GetConfigTableError] anim_list");
			return;
		}

		lua_pushnil(stat_up_lua_state);
		while(0 != lua_next(stat_up_lua_state, -2))
		{
			lua_getfield(stat_up_lua_state, -1, "id");
			lua_getfield(stat_up_lua_state, -2, "x");
			lua_getfield(stat_up_lua_state, -3, "y");

			AnimPos ap;
			ap.y = (int)lua_tointeger(stat_up_lua_state, -1);
			ap.x = (int)lua_tointeger(stat_up_lua_state, -2);
			ap.anim_id = (int)lua_tointeger(stat_up_lua_state, -3);
			anim_list.push_back(ap);

			lua_pop(stat_up_lua_state, 4);
			
		}
		lua_pop(stat_up_lua_state, 2);
	}

	// 加入动画列表
	std::vector<AnimPos>::iterator iter = anim_list.begin();
	std::vector<AnimPos>::iterator end = anim_list.end();
	for (; iter != end; ++iter)
	{
		AnimPos& ap= *iter;
		char buff[128];
		sprintf(buff, "%sstart/%d.ssdz", "resource/", ap.anim_id);
		SkeletonAnimationEx* pSpine = SkeletonAnimationEx::CreateWithFile(buff);
		if (pSpine != NULL)
		{
			pSpine->setPositionX(float(ap.x));
			pSpine->setPositionY(float(ap.y));
			pSpine->SetAnimation(0, "animation", true);
			pSpine->SetToPlayTime(0.25);
			pSpine->StopAnimation();
			bk_label->addChild(pSpine);
		}
	}

	// 游戏名logo
    std::string logo_path = FileSystemEx::GetInstance()->GetResourcePath("trdres/logo.png");
	if (logo_path.empty())
	{
        logo_path = "resource/start/logo.png";
	}
    SpriteEx* logo_bg = SpriteEx::CreateWithImageFile(logo_path);
	logo_bg->SetAnchorPoint(0.5, 1);
	logo_bg->setPosition(Vec2(7, m_child_layer_height / 2 - 22));
	m_child_layer->AddChild(logo_bg);

	// 进度条
	float loading_bg_width = m_child_layer_width * 0.75;
	m_loading_bg = Scale9SpriteEx::CreateWithImageFile("resource/start/bar1.png");
	m_loading_bg->SetContentSize(loading_bg_width, 36);
	m_loading_bg->setVisible(!SystemTools::IsApptorePackage());
	m_loading_bg->setPosition(0, -m_child_layer_height / 2 + 33);
	m_child_layer->AddChild(m_loading_bg);

	m_loading_text = LabelEx::CreateWithString(m_stage_text_map[StartLogic::LP_Invalid], Label::DEF_FONT_NAME, 20, loading_bg_width, 24);
	m_loading_text->SetStroke(true);
	m_loading_text->SetStrokeColor(68, 44, 12, 255);
	m_loading_text->setHorizontalAlignment(TextHAlignment::LEFT);
	m_loading_text->SetAnchorPoint(0, 0.5);
	
	m_loading_text->SetPosition(-loading_bg_width / 2, -m_child_layer_height / 2 + 67);
	m_child_layer->AddChild(m_loading_text);

	// 背景音乐
	bool b_play = true;
	float f_volume = 0.3f;
	std::string str_music_volume = Setting::GetInstance()->Get("BgMusicVolume");
	std::string play_bgm = LuaEngine::GetInstance()->GetConfig("PlayStartupBGM");
	if (play_bgm == "true")
	{
		ZQ::StrUtil::StringToBaseType(str_music_volume, f_volume);
		SoundManager::GetInstance()->PlayBGMusic("resource/start/2.sdzm", true);
		SoundManager::GetInstance()->SetBGMusicVolume(f_volume);
	}
}

void StartUpScene::SelectLanguage()
{
	//自动识别区域语言
	std::string auto_district_language = LuaEngine::GetInstance()->GetConfig("AutoDistrictLanguage");
	if (auto_district_language == "true")
	{
		LuaEngine::GetInstance()->CallFunction("AutoDistrictLanguage", "");
	}
	else
	{
		Setting::GetInstance()->Set("DistrictLanguage", LuaEngine::GetInstance()->GetConfig("DistrictLanguage"));
	}

	//区域语言
	std::string district_language = Setting::GetInstance()->Get("DistrictLanguage");
	int pos = district_language.find_first_of("_");
	std::string language = district_language.substr(pos + 1);
	Setting::GetInstance()->Set("Language", language);

	// 检测是否通过TTF创建系统字
	std::string use_system_font_by_ttf = LuaEngine::GetInstance()->GetConfig("UseSystemFontByTTF");
	Label::USE_SYSTEM_FONT_BY_TTF = use_system_font_by_ttf == "true";

	if (Label::USE_SYSTEM_FONT_BY_TTF)
	{
		Label::DEF_FONT_NAME = "zhunyuan";
	}
	else
	{
		Label::DEF_FONT_NAME = "zhunyuan.ttf";
	}

	// 默认字体
	cocostudio::WidgetReader::DefaultFontName = Label::DEF_FONT_NAME;
	Setting::GetInstance()->Set(SETTING_DEFAULT_FONT_NAME, Label::DEF_FONT_NAME);

	// 加载start_string.cnf
	LuaEngine::GetInstance()->LoadStrCnfFromLuaExternal();
}

void StartUpScene::EnterMain()
{
	LuaEngine::GetInstance()->CallLuaMainFunc();
}

void StartUpScene::OnMsgDlgBtn(int btn_id)
{
	if(!start_scene)
	{
		return;
	}

	if(btn_id == 1)
	{
		(start_scene->*btn_confirm_func)();
	}
	else if(btn_id == 0)
	{
		(start_scene->*btn_cancel_func)();
	}
}

void StartUpScene::CreateMsgDlg(const std::string &text, BTN_CALLBACK confirm_func, BTN_CALLBACK cancel_func, const std::string &confirm_str, const std::string &cancel_str)
{
	const char* confirm_btn_str = confirm_str.c_str();
	if(confirm_str == "")
	{
		confirm_btn_str = GetString("confirm", "confirm").c_str();
	}

	const char* cancel_btn_str = NULL;
	if(cancel_func)
	{
		cancel_btn_str = cancel_str.c_str();
		if(cancel_str == "")
		{
			cancel_btn_str = GetString("cancel", "cancel").c_str();
		}
	}

	start_scene = this;
	btn_confirm_func = confirm_func;
	btn_cancel_func = cancel_func;

	SdkTools::ShowDialog(GetString("tips", "tips").c_str(), text.c_str(), confirm_btn_str, cancel_btn_str, OnMsgDlgBtn);
}

std::string StartUpScene::GetString(const std::string &key, const std::string& default_value)
{
	std::string get_value = LuaEngine::GetInstance()->GetStringConfig(key);
	if(get_value != "")
	{
		return get_value;
	}
	return default_value;
}

void StartUpScene::update(float delta)
{
	m_logic.update(delta);

	if (m_logic.IsStarted())
	{
		CheckStartProgress(delta);
	}
}

void StartUpScene::CheckStartProgress(float time)
{
	if(m_block)
	{
		if (m_bAutoRetry)
		{
			//延时进行自动重试
			m_fAutoRetryPassTime += time;
			if (m_fAutoRetryPassTime >= 0.25)
			{
				m_bAutoRetry = false;
				ConfirmRetry();
			}
		}
		return;
	}
	
	StartLogic::LogicProgressItem progress_item;
	if(!m_logic.GetLogicProgress(progress_item))
	{
		ProgressGoing(time);
		return;
	}

	// 流程失败
	if(progress_item.sub_progress == StartLogic::SP_End_Fail)
	{
		m_block = true;
		m_failed_progress = progress_item.progresss;

		LogUtil::LogInfo("[StartUpScene::CheckStartProgress ProgressFail] progresss:%d sub_progress:%d ret_code:%d http_ret_code:%d op_code:%d op_code_desc:%s", 
			progress_item.progresss, progress_item.sub_progress, progress_item.ret_code, progress_item.http_ret_code, progress_item.op_code, progress_item.op_code_desc.c_str());

		if (progress_item.progresss == StartLogic::LP_ScriptIos_DownLoading)
		{
			m_cur_scirpt_index--;
		}

		CheckRetry(progress_item);

		return;
	}

	switch(progress_item.progresss)
	{
	case StartLogic::LP_Cdn_DownLoading:
	case StartLogic::LP_VersionIos_DownLoading:
	case StartLogic::LP_VersionIos_Loading:
		{
            if (SystemTools::IsApptorePackage() &&
                progress_item.progresss != StartLogic::LP_Cdn_DownLoading &&
                progress_item.sub_progress == StartLogic::SP_Start)
            {
                bool is_in_examine = StrConv::parseInt(Setting::GetInstance()->Get("IsInExamine")) == 1;
                m_loading_bg->setVisible(!is_in_examine);
                m_loading_text->setVisible(!is_in_examine);
            }

			ProgressGoingItem item;
			item.stage = progress_item.progresss;
			item.is_need_download = (progress_item.progresss!=StartLogic::LP_VersionIos_Loading);
			item.max_progress = 10.0f*progress_item.progresss;
			m_progress_list.push_back(item);
		}
		break;

	case StartLogic::LP_NewPackage_WaitingForCheck:
		{
			m_block = true;
			int version = FileVersion::GetInstance()->CheckAppVersion();
			if(version == FileVersion::AVS_NEED_UPDATE)
			{
				CreateMsgDlg(GetString("confirm_download_package", "detect new game version, update now?"), &StartUpScene::ConfirmUpdatePackage);
			}
			else if(version == FileVersion::AVS_CAN_IGNORE)
			{
				CreateMsgDlg(GetString("inquiry_download_package", "detect new game version, update now?"), &StartUpScene::ConfirmUpdatePackage, &StartUpScene::CancelUpdatePackage);
			}
			else
			{
				LogUtil::LogError("[StartUpScene::CheckStartProgress CheckAppVersionError][state:%d]", version);
			}
			return;
		}
		break;

	case StartLogic::LP_NewPackage_GetInfo:
		{
			ProgressGoingItem item;
			item.stage = progress_item.progresss;
			item.is_need_download = true;
			item.max_progress = 30.0f;
			m_progress_list.push_back(item);
		}
		break;

	case StartLogic::LP_NewPackage_DownLoading:
		{
			if(progress_item.sub_progress == StartLogic::SP_End_Succ)
			{
				m_logic.InstallSteupPackage();
				m_block = true;
				return;
			}
			else
			{
				ProgressGoingItem item;
				item.stage = progress_item.progresss;
				item.is_need_download = true;
				item.max_progress = 90.0f;
				m_progress_list.push_back(item);
			}
		}
		break;

	case StartLogic::LP_ResPackage_DownLoading:
		{
			ProgressGoingItem item;
			item.stage = progress_item.progresss;
			item.is_need_download = true;
			item.max_progress = 40.0f;
			m_progress_list.push_back(item);
		}
		break;

	case StartLogic::LP_ResPackage_Unziping:
		{
			ProgressGoingItem item;
			item.stage = progress_item.progresss;
			item.max_progress = 50.0f;
			m_progress_list.push_back(item);
		}
		break;

	case StartLogic::LP_ScriptIos_DownLoading:
		{
			unsigned int all_script_num = m_logic.GetNeedDownloadScriptNum();
			++m_cur_scirpt_index;
			ProgressGoingItem item;
			item.stage = progress_item.progresss;
			item.is_need_download = true;
			if (all_script_num==0)
			{
				item.max_progress = 75.0f;
			}
			else
			{
				item.max_progress = 50.0f+25.0f/all_script_num * m_cur_scirpt_index;
			}
			m_progress_list.push_back(item);
		}
		break;

	case StartLogic::LP_ScriptIos_Loading:
		if(progress_item.sub_progress == StartLogic::SP_End_Succ)
		{
			EnterMain();
			m_progress_val = 100.0f;
			return;
		}
		else
		{
			if (m_progress_val < 75.0f)
			{
				m_progress_val = 75.0f;
			}
			ProgressGoingItem item;
			item.stage = progress_item.progresss;
			item.max_progress = 100.0f;
			m_progress_list.push_back(item);
		}
		break;

	case StartLogic::LP_StopProgressGoing:
		if (progress_item.sub_progress == StartLogic::SP_Start)
		{
			m_stop_progress = true;
		}
		else
		{
			m_stop_progress = false;
		}
		break;

	case StartLogic::LP_EnterWithoutUpdate:
		m_stop_progress = true;
		EnterMain();
		break;

	default:
		LogUtil::LogError("[StartUpScene::CheckStartProgress ProgressErorr][progress:%d]", progress_item.progresss);
		break;
	}

	ProgressGoing(time);
}

void StartUpScene::ConfirmRetry()
{
	m_block = false;
	m_logic.Restart(m_failed_progress);
}

void StartUpScene::ConfirmUpdatePackage()
{
	m_block = false;
	m_logic.StartDownLoad_NewPackage();
}

void StartUpScene::CancelUpdatePackage()
{
	m_block = false;
	m_logic.StartDownLoad_ResPackage();
}

//启动StartLogic
void StartUpScene::Start()
{
	m_stage_text_map[StartLogic::LP_Invalid] = GetString("init", "initing...");
	m_stage_text_map[StartLogic::LP_Cdn_DownLoading] = GetString("get_cdn", "connect to resource server...");
	m_stage_text_map[StartLogic::LP_VersionIos_DownLoading] = GetString("update_version", "update version...");
	m_stage_text_map[StartLogic::LP_VersionIos_Loading] = GetString("load_version", "loading version...");
	m_stage_text_map[StartLogic::LP_ResPackage_DownLoading] = GetString("download_res_package", "download resource package...");
	m_stage_text_map[StartLogic::LP_ResPackage_Unziping] = GetString("unzip_res_package", "unzip resource package...");
	m_stage_text_map[StartLogic::LP_NewPackage_GetInfo] = GetString("get_package_info", "getting package info...");
	m_stage_text_map[StartLogic::LP_NewPackage_DownLoading] = GetString("download_package", "download package...");
	m_stage_text_map[StartLogic::LP_ScriptIos_DownLoading] = GetString("download_script", "update program script...");
	m_stage_text_map[StartLogic::LP_ScriptIos_Loading] = GetString("load_script", "loading program script...");
	m_stage_text_map[StartLogic::LP_COUNT] = GetString("load_start_resource", "loading necessary resource...");

	ShowDownLoadUI();

	//开始启动逻辑
	m_logic.Start();
}

//设置解压/下载信息
void StartUpScene::SetDownLoadingText(const ProgressGoingItem& item, const char* context)
{
	char tmp[128];
	std::string text = "";
	switch (item.stage)
	{
	case StartLogic::LP_ScriptIos_DownLoading:
		sprintf(tmp, "%s(%d/%d),%s", m_stage_text_map[item.stage].c_str(), m_logic.GetHadDownloadScriptIndex(), m_logic.GetNeedDownloadScriptNum(), context);
		text = tmp;
		break;
	case StartLogic::LP_ResPackage_DownLoading:
		sprintf(tmp, "%s(%d/%d),%s", m_stage_text_map[item.stage].c_str(), m_logic.GetHadDwonloadResPackage(), m_logic.GetNeedDownloadResPackage(), context);
		text = tmp;
		break;
	default:
		text = m_stage_text_map[item.stage] + context;
		break;
	}

	m_loading_text->SetString(text);
}

//进度推进
void StartUpScene::ProgressGoing(float time)
{
	if (m_stop_progress)
	{
		return;
	}

	if (0 >= m_progress_list.size())
	{
		LogUtil::LogError("[StartUpScene::ProgressGoing VectorLenError][cur_progress:%d progress_list_size:%d]", (int)m_progress_list.size());
		return;
	}

	ProgressGoingItem item = m_progress_list[m_progress_list.size()-1];

	// 更新进度条
	float progress_max = item.max_progress;
	float increase_val = (progress_max - m_progress_val) / 100;
	increase_val = increase_val > 0.01 ? increase_val : 0.01f;
	m_progress_val += increase_val;
	m_progress_val = m_progress_val > progress_max ? progress_max : m_progress_val;
	m_progress_val = m_progress_val < 5.0f ? 5.0f : m_progress_val;
	
	// 更新进度
	char progress_val_str[16] = {0};
	sprintf(progress_val_str, "%.2f", m_progress_val);
	// 更新文字
	if (!item.is_need_download)
	{
        m_loading_text->SetString(m_stage_text_map[item.stage]);
		return;
	}

	unsigned int http_id;
	unsigned int total_len = 0;
	unsigned int cur_len = 0;
	if (item.stage == StartLogic::LP_NewPackage_DownLoading)
	{
		if (!m_logic.GetDownLoadNewPackageProgress(http_id, total_len, cur_len))
		{
			return;
		}
	}
	else if (!m_logic.GetCurDownLoadProgress(http_id, total_len, cur_len))
	{
		return;
	}

	if (m_last_http_id!=http_id)
	{
		m_last_http_id = http_id;
		m_last_download_size = cur_len;
		m_last_download_time=0;
		if (total_len!=0)
		{
			std::string networkspeed_str;
			ZQ::StrUtil::BaseTypeToString((ZQ::UINT32)m_network_speed, networkspeed_str);
			std::string cur_len_str;
			ZQ::StrUtil::BaseTypeToString((cur_len/1024), cur_len_str);
			std::string total_len_str;
			ZQ::StrUtil::BaseTypeToString((total_len/1024), total_len_str);
			std::string context = cur_len_str + "kb/" + total_len_str + "kb[" + networkspeed_str + "kb/s]";
			SetDownLoadingText(item, context.c_str());
		}

		return;
	}

	m_last_download_time+=time;

	if (m_last_download_time<1)
	{
		return;
	}

	float network_speed = (cur_len - m_last_download_size) / (1024 * m_last_download_time);
	m_network_speed_list.push_back(network_speed);
	int size = m_network_speed_list.size();
	if (size > 0)
	{
		if (size > 3)
		{
			m_network_speed_list.erase(m_network_speed_list.begin());
			size = size - 1;
		}
		float total_speed = 0.0f;
		for (int i = 0; i < size; i++)
		{
			total_speed += m_network_speed_list[i];
		}
		m_network_speed = total_speed / size;
	}
	
	if (total_len != 0)
	{
		std::string networkspeed_str;
		ZQ::StrUtil::BaseTypeToString((ZQ::UINT32)m_network_speed, networkspeed_str);
		std::string cur_len_str;
		ZQ::StrUtil::BaseTypeToString((cur_len / 1024), cur_len_str);
		std::string total_len_str;
		ZQ::StrUtil::BaseTypeToString((total_len / 1024), total_len_str);
		std::string context = cur_len_str + "kb/" + total_len_str + "kb[" + networkspeed_str + "kb/s]";
		SetDownLoadingText(item, context.c_str());
	}

	m_last_download_size = cur_len;
	m_last_download_time = 0;
}

//检测重试
void StartUpScene::CheckRetry(const StartLogic::LogicProgressItem& progress_item)
{
	if (m_hAutoRetryCount < 4)
	{
		m_bAutoRetry = true;
		++m_hAutoRetryCount;
		m_fAutoRetryPassTime = 0.0f;
	}
	else
	{
		m_bAutoRetry = false;
		m_hAutoRetryCount = 0;

		std::string ret_code_str;
		ZQ::StrUtil::BaseTypeToString(progress_item.ret_code, ret_code_str);
		std::string http_ret_code_str;
		ZQ::StrUtil::BaseTypeToString(progress_item.http_ret_code, http_ret_code_str);
		std::string op_code_str;
		ZQ::StrUtil::BaseTypeToString(progress_item.op_code, op_code_str);
		std::string progress_str;
		ZQ::StrUtil::BaseTypeToString(progress_item.progresss, progress_str);
		std::string err_msg = GetString("download_error", "download error, please check network");
		err_msg = err_msg +
			"progress(" + progress_str + ")" +
			"ret_code[" + ret_code_str + "]" +
			"http_ret_code[" + http_ret_code_str + "]" +
			"op_code[" + op_code_str + "]" +
			"op_code_desc[" + progress_item.op_code_desc + "]";
		CreateMsgDlg(err_msg, &StartUpScene::ConfirmRetry);

		//客户端事件跟踪过程中，上报导致卡死的错误信息
		LogUtil::ReportErrorInfoForEventTrack("%s", err_msg.c_str());
	}
}
