
--[[
%% @module: 主城特殊逻辑
%% @author: swordman sue
%% @created: 2016-07-15
--]]

Modules = Modules or {}

Modules.SpMaincityModule = Modules.SpMaincityModule or BaseClass(Modules.SplogicModule)

function Modules.SpMaincityModule:__init()
end

function Modules.SpMaincityModule:__delete()
end

--初始化
function Modules.SpMaincityModule:Initialize()
	Modules.SplogicModule.Initialize(self)
end

--释放
function Modules.SpMaincityModule:Dispose()
	Modules.SplogicModule.Dispose(self)
end

--注册事件
function Modules.SpMaincityModule:RegistAllEvents()
end

--注册协议
function Modules.SpMaincityModule:RegistAllProtocols()
	Modules.SplogicModule.RegistAllProtocols(self)
end

--进入GamePlaying状态
function Modules.SpMaincityModule:GamePlayingStartHandle()
	Modules.SplogicModule.GamePlayingStartHandle(self)

	--打开审核主界面
	local channel_type = CPPSdkTools.GetCPChannelType()
	local config_channel_setting = CPPGameLib.GetConfig("ChannelSetting", channel_type, false)	
	if CPPSdkTools.IsInExamine() and config_channel_setting and config_channel_setting.use_examine_maincity_type and config_channel_setting.use_examine_maincity_type ~= 4 then
		CallModuleFunc(ModuleType.POLYTROPIC_MAINCITY, "OpenView", BaseViewType.POLYTROPIC_MAINCITY, function()
			--销毁登陆相关界面
			CallModuleFunc(ModuleType.LOGIN, "DumpViews")
		end)

	--打开主界面
	else
		CallModuleFunc(ModuleType.MAIN_CITY, "OpenView", BaseViewType.MAIN_CITY, function()
			--销毁登陆相关界面
			CallModuleFunc(ModuleType.LOGIN, "DumpViews")

			if not CPPSdkTools.IsInExamine() then
				--首次进入游戏，触发新手剧情
				if CallModuleFunc(ModuleType.ROLE,"GetPropValue", Macros.Game.RolePropType.LEVEL) <= 1 then
					CallModuleFunc(ModuleType.STORY, "Start", 0)		

				--指引恢复检测
				else
					CallModuleFunc(ModuleType.LEAD, "CheckRecovery")			
				end

				--检测推送
				if CallModuleFunc(ModuleType.PUSH, "GetOpenKey") ~= true and 
					not CallModuleFunc(ModuleType.LEAD, "IsInLead") and not CallModuleFunc(ModuleType.STORY, "IsInStory") then
					CallModuleFunc(ModuleType.PUSH, "Transfer")
				end
			end
		end)
	end
	
    --设置运行帧率
    -- CPPSceneManager:SetGameFrame(Macros.Global.FPS_MainCity)
end

function Modules.SpMaincityModule:GamePlayingStopHandle()
	CallModuleFunc(ModuleType.MAIN_CITY, "CloseView")
	

	Modules.SplogicModule.GamePlayingStopHandle(self)
end

--加载资源
function Modules.SpMaincityModule:LoadRes()
	self.__is_res_loaded = true
end

function Modules.SpMaincityModule:IsNeedLoadMap()
    return false
end

--进入后台是否暂停
function Modules.SpMaincityModule:IsPauseWhenDeActive()
	return false
end

function Modules.SpMaincityModule:IsShowPauseDialog()
	return false
end

--处理进入切场景状态
function Modules.SpMaincityModule:HandleEnterSceneChangeState(callback)
	if callback then callback() end
end

--处理离开切场景状态
function Modules.SpMaincityModule:HandleQuitSceneChangeState(callback)
	if callback then callback() end
end
