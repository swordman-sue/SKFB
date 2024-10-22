
--[[
%% @module: 逻辑层启动模块
%% @author: swordman sue
%% @created: 2016-07-07
--]]

function RunCallBackFunc(func_id, ...)
	if GlobalCallbackMgr then
		return GlobalCallbackMgr:RunCallBackFunc(func_id, ...)
	end
end

function DelCallBackFunc(func_id)
	if GlobalCallbackMgr then
		return GlobalCallbackMgr:DelCallBackFunc(func_id)
	end
end

Startup = Startup or BaseClass()

require "cpptools/gamelib"
require "cpptools/logtool"
require "cpptools/sdktools"
require "cpptools/systemtools"
require "cpptools/httphandle"
require "luatools/requiretool"
require "luatools/statisticstool"
require "luatools/callbackmgr"
require "language/langhelper"

LanguageHelper.InitLanguage()

local function get_language_script(script_name)
	return string.format("language/%s/%s", LanguageHelper.language, script_name)
end

local function get_protobuf()
	if MessageAdapter.USE_PROTOBUF_FOR_NET_MSG then
		return "cpptools/protobuf"
	end
end

Startup.FilesForSetup =
{	
	get_language_script("stringtable"),	
	
	"macros/color",
	"macros/event",

	"core/event",
	"core/event_and_timer_wrapper",
	"core/eventsystem",
	"core/layer",
	"core/uilayer",

	"luatools/list",
	"luatools/array",
	"luatools/sortmap",
	"luatools/sorttool",
	"luatools/timetool",
	"luatools/mathtool",
	"luatools/postool",
	"luatools/sizetool",
	"luatools/idtool",
	"luatools/stringtool",
	"luatools/autoreleasetool",	
	"luatools/refobj",

	"resource/pathtool",
	"cpptools/nodeunity",
	"cpptools/widgetunity",
	"cpptools/sdkconstants",
	"cpptools/faceconvhelper",
	"cpptools/cookies",
	"cpptools/msgpush",
	"cpptools/actionmgr",
	"cpptools/asyncloadmgr",

	"gui/util",

	"language/language_filter",

	get_protobuf(),
}

function Startup.CreateInstance()
	if not GlobalStartup then
		GlobalStartup = Startup.New()
	end
end

function Startup.DelInstance()
	if GlobalStartup then
		GlobalStartup:DeleteMe()
		GlobalStartup = nil
	end
end

function Startup:__init()	
	local flag = tonumber(CPPResManager:GetSetting("ReportEventTrackForLossRateByEngine")) or Macros.Global.FALSE
	if flag ~= Macros.Global.TRUE then		
		--兼容老版本
		--记录"游戏是否启动过"的标记
		local have_started = tonumber(CPPResManager:GetSetting(Macros.Setting.HaveStarted)) == Macros.Global.TRUE
		if not have_started then
			CPPResManager:SetSetting(Macros.Setting.HaveStarted, tostring(Macros.Global.TRUE))
		end
		--初始化"是否首次启动"的标记
		CPPResManager:SetSetting(Macros.Setting.IsFirstStarted, tostring(not have_started and Macros.Global.TRUE or Macros.Global.FALSE))
	end

	--初始化回调管理器
	CallBackMgr.CreateInstance()	

	--客户端事件跟踪(统计流失率)
	StatisticsTool.GetInstance():ReportEventTrackForLossRate(StatisticsTool.EventType4LossRate.START_SCRIPT)

	--初始化RequireTool
	RequireToolBatch.CreateInstance()	

	--加载Setup所需脚本
	GlobalRequireToolBatch:LoadFiles({
			file_list = Startup.FilesForSetup, 
			callback = function() 
				self:__Setup()
			end})
end

function Startup:__delete()
	if self.__enter_run_state then
		--销毁Game模块
		Game.DelInstance()		

		--销毁回调管理器
		CallBackMgr.DelInstance()
	end

	GlobalRunner:RemoveRunObj(LogTool)
	GlobalRunner:RemoveRunObj(GlobalTimerQuest)
	GlobalTimerQuest = nil

	--销毁StatisticsTool
	StatisticsTool.DelInstance()

	--销毁RequireTool
	RequireToolBatch.DelInstance()
end

function Startup:Update(now_time, elapse_time)
	if GlobalGame then
		GlobalGame:Update(now_time, elapse_time)
	end
end

--进入前台
function Startup:OnEnterForeground()
	if GlobalTimerQuest then
		GlobalTimerQuest:Resume()
	end
	if GlobalEventSystem then
		GlobalEventSystem:ResumeTimer()
	end
end

--进入后台
function Startup:OnEnterBackground()
	if GlobalTimerQuest then
		GlobalTimerQuest:Pause()
	end
	if GlobalEventSystem then
		GlobalEventSystem:PauseTimer()
	end
end

function Startup:__Setup()
	--初始化设计分辨率、设备分辨率、默认设计分辨率	
	CPPSceneManager:GetDesignSize(Macros.Global.DesignSize)
	CPPSceneManager:GetScreenSize(Macros.Global.ScreenSize)
	Macros.Global.DesignSizeDef = COCOSize(CPPSceneManager.SCREEN_DESIGN_WIDTH_DEF, CPPSceneManager.SCREEN_DESIGN_HEIGHT_DEF)

	--初始化IPhone安全区域
	if CPPSystemTools.IsIOS() then
		local visible_rect = COCORect()
		local safe_area_rect = COCORect()
		CPPSceneManager:GetVisibleRect(visible_rect)
		CPPSceneManager:GetSafeAreaRect(safe_area_rect)

		if not visible_rect:Equals(safe_area_rect) then
			Macros.Global.IphoneSafeAreaRect = COCORect()
			CPPSceneManager:GetSafeAreaRect(Macros.Global.IphoneSafeAreaRect)
		end
	end

	--初始化IOS表情转换映射
	FaceConvHelper.InitFaceConvHelper()	

	--注册系统字体
	CPPFontManager:registerFont(Util.FontType)

	--过滤格式控制字符集(格式控制字符不需要生成字模，且格式控制字符生成字模的话可能在部分机型上导致崩溃)
	local filter_char_set = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31}
	for _, v in ipairs(filter_char_set) do
		CPPFontManager:registerFilterChar(v)
	end

	--响应返回键
	CPPSceneManager:SetKeypadEnabled(true)
	local back_fun_id = GlobalCallbackMgr:AddCallBackFunc(function()
		CPPSdkTools.ExitSdk(CPPGameLib.GetStartString("is_exit_game"))
	end)
	CPPSceneManager:SetLuaBackKey(back_fun_id)

	--响应低内存警报
	local memory_warning_fun_id = GlobalCallbackMgr:AddCallBackFunc(function()
		LogTool.LogInfo("warnning: not enough memeory")
	end)
	CPPSdkTools.SetLuaMemoryWarnCallback(memory_warning_fun_id)	

	--初始化定时器
	GlobalTimerQuest = CPPTimerQuest()
	GlobalRunner:AddRunObj(GlobalTimerQuest, Runner.Priority.RP_TIMER)

	--初始化日志工具
	GlobalRunner:AddRunObj(LogTool)

	--启动系统推送
	-- MsgPushHandle.PushMsgStart()

	--初始化Game模块
	require "game"
	Game.CreateInstance()

	self.__enter_run_state = true	
end
