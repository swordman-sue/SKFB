
--统计相关
StatisticsTool = StatisticsTool or BaseClass()

local header_list = {"Content-Type=application/json"}

local setup_http_params

--客户端事件跟踪类型
StatisticsTool.EventType4LossRate = 
{
	--前几个由引擎触发
	--....

	--脚本逻辑启动
	START_SCRIPT = 7,
	--加载第一批初始脚本
	LOAD_INIT_SCRIPT_I = 8,
	--开始资源解压和更新
	UNZIP_AND_UPDATE_RES = 9,
	--开始公共资源加载
	LOAD_COMMON_RES = 10,
	--加载第二批初始脚本
	LOAD_INIT_SCRIPT_II = 11,
	--取服务器配置
	REQ_SERVER_CONFIG = 12,
	--取服务器列表
	REQ_SERVER_LIST = 13,
	--连接服务器
	CONNECT_SERVER = 14,
	--打开登录界面
	OPEN_LOGIN_VIEW = 15,
	--打开SDK登录界面
	OPEN_SDK_LOGIN_VIEW = 16,
	--游戏登录成功
	LOGIN_SUCCESS = 17,
	--进入游戏
	START_GAME = 18,
}

function StatisticsTool.GetInstance()
	if not g_statistics_tool then
		g_statistics_tool = StatisticsTool.New()
	end
	return g_statistics_tool
end

function StatisticsTool.DelInstance()
	if g_statistics_tool then
		g_statistics_tool:DeleteMe()
		g_statistics_tool = nil
	end
end

function StatisticsTool:__init()
	self.__module_map = {}
	self.__module_inited = false

	--读取客户端事件跟踪data_id
	self.__data_id_of_event_track_for_loss_rate = tonumber(CPPResManager:GetSetting(Macros.Setting.DataIDOfEventTrackForLossRate)) or os.time()
end

function StatisticsTool:__delete()
	if self.__statistics_player_online_timer then
		GlobalTimerQuest:CancelQuest(self.__statistics_player_online_timer)
		self.__statistics_player_online_timer = nil
	end	
end

--统计系统操作
function StatisticsTool:SystemOperate(module_type, view_type)
	if not StartConfig.ActionStatisticsURL then
		return
	end

	if not self.__module_inited then
		local config_system_entrys = CPPGameLib.GetConfig("SystemEntry", nil, nil, true)
		if config_system_entrys then
			self.__module_inited = true
			
			for k, v in pairs(config_system_entrys) do
				if v.module_type then
					self.__module_map[v.module_type] = self.__module_map[v.module_type] or {}
					self.__module_map[v.module_type][v.view_type or v.module_type] = k
				end
			end
		end
	end
	
	local system_id = self:__GetSystemID(module_type, view_type)
	if not system_id then
		return
	end

	local monitorCnf = CPPGameLib.GetConfig( "MonitorCount" , system_id , false )
	if monitorCnf then
		local get_param = {}
		get_param.role_id = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo").role_id
		get_param.system_id = system_id
		
		local urlmsg_data = {}
		urlmsg_data.url = StartConfig.ActionStatisticsURL
		urlmsg_data.get_param = get_param
		urlmsg_data.forbid_retry = true
		urlmsg_data.open_loading = false
		urlmsg_data.show_error = false
		SendUrlMsg(urlmsg_data)
	end
end

--开启统计玩家在线的计时器
function StatisticsTool:StartTimerForStatisticsPlayerOnline()
	--统计玩家在线
	local statistics_player_online_cd = 5*60
	if StartConfig.StatisticsAppID and StartConfig.StatisticsURL then
		self.__statistics_player_online_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()
			self:__StatisticsPlayerOnline()
		end), statistics_player_online_cd, -1)
	end	
end

--[[
客户端事件跟踪(统计流失率)
--]]
function StatisticsTool:ReportEventTrackForLossRate(type)
	if CPPSystemTools.IsWin32() then
		return
	end

	if not StartConfig.EventTrackForLossRateURL then
		return
	end

	if self.__type_record_of_event_track_for_loss_rate and 
		self.__type_record_of_event_track_for_loss_rate >= type then
		return
	end
	self.__type_record_of_event_track_for_loss_rate = type

	local post_param = {}
	setup_http_params(self, post_param)

	if type == StatisticsTool.EventType4LossRate.START_GAME then
		local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
		post_param.account = role_info.account
		post_param.server_id = role_info.server_id
		post_param.role_id = role_info.role_id

		LogTool.LogInfo("[StatisticsTool:ReportEventTrackForLossRate] memory_size:%d", post_param.mem_size)
	end


	local urlmsg_data = {}
	urlmsg_data.url = StartConfig.EventTrackForLossRateURL
	urlmsg_data.post_param = post_param
	urlmsg_data.forbid_retry = true
	urlmsg_data.open_loading = false
	urlmsg_data.show_error = false
	SendUrlMsg(urlmsg_data)
end

--客户端事件跟踪过程中，上报导致卡死的错误信息
function StatisticsTool:ReportErrorInfoForEventTrack(error_msg)
	if CPPSystemTools.IsWin32() then
		return
	end	

	if not StartConfig.ErrorInfoForEventTrackURL then
		return
	end

	if not self.__type_record_of_event_track_for_loss_rate or self.__type_record_of_event_track_for_loss_rate == StatisticsTool.EventType4LossRate.START_GAME then
		return
	end

	local post_param = {}
	setup_http_params(self, post_param)
	post_param.error_msg = error_msg

	local urlmsg_data = {}
	urlmsg_data.url = StartConfig.ErrorInfoForEventTrackURL
	urlmsg_data.post_param = post_param
	urlmsg_data.forbid_retry = true
	urlmsg_data.open_loading = false
	urlmsg_data.show_error = false
	SendUrlMsg(urlmsg_data)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function StatisticsTool:__GetSystemID(module_type, view_type)
	if self.__module_map and self.__module_map[module_type] then
		return self.__module_map[module_type][view_type]
	end
end

--统计玩家在线
function StatisticsTool:__StatisticsPlayerOnline()
	local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
	if role_info then
		local statistics_info = {}
		statistics_info.appid = StartConfig.StatisticsAppID or ""	
		statistics_info.who = tostring(role_info.account)
		local context = {}
		context.deviceid = CPPSystemTools.GetDeviceId()
		context.serverid = tostring(role_info.server_id)
		context.channelid = tostring(CPPSdkTools.GetChannelId())
		context.level = tostring(role_info.level)
		statistics_info.context = context --CPPGameLib.JSON.encode(context)
		local act_url = (StartConfig.StatisticsURL or "").."/receive/rest/heartbeat"

		local urlmsgex_data = {}
		urlmsgex_data.url = act_url
		urlmsgex_data.header_list = header_list
		urlmsgex_data.post_param = statistics_info
		urlmsgex_data.forbid_retry = true
		urlmsgex_data.open_loading = false
		SendUrlMsgEx(urlmsgex_data)
	end
end

--初始化HTTP参数
setup_http_params = function(obj_self, param)
	local by_engine = tonumber(CPPResManager:GetSetting("ReportEventTrackForLossRateByEngine")) == Macros.Global.TRUE	

	param.app_key = by_engine and 20160603 or 20160602
	param.data_id = obj_self.__data_id_of_event_track_for_loss_rate
	param.deviceid = string.format("%s_%s", CPPSystemTools.GetSystemType(), CPPSystemTools.GetDeviceId())
	param.cp_channel_type = CPPSdkTools.GetCPChannelType()
	param.is_new = tonumber(CPPResManager:GetSetting(Macros.Setting.IsFirstStarted))
	param.device_model = CPPSystemTools.GetDeviceModel()
	param.mem_size = CPPSystemTools.GetMemorySize()
	param.app_version = CPPSystemTools.GetAppVersion()
	param.os = CPPSystemTools.GetSystemType()
	param.type = obj_self.__type_record_of_event_track_for_loss_rate
end
