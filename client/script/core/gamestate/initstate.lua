
--[[
%% @module: 初始化状态
%% @author: swordman sue
%% @created: 2016-07-14
%% @description: 连接服务器、登陆、选择服务器、创建角色、进入游戏
--]]

GameState = GameState or {}

--[[
初始化主状态
--]]
GameState.InitState = GameState.InitState or BaseClass(GameState.BaseState)

function GameState.InitState:__init()
	self.__name = "init_state"
end

function GameState.InitState:StateEnter(state_name)	
end

function GameState.InitState:StateUpdate(state_name, elapse_time)
end

function GameState.InitState:StateQuit(state_name)
end

--[[
资源更新状态
--]]
GameState.ResourceUpdateState = GameState.ResourceUpdateState or BaseClass(GameState.BaseState)

function GameState.ResourceUpdateState:__init()
	self.__name = "resource_update"
end

function GameState.ResourceUpdateState:StateEnter(state_name)
	self.__is_finished = false

	--客户端事件跟踪(统计流失率)
	StatisticsTool.GetInstance():ReportEventTrackForLossRate(StatisticsTool.EventType4LossRate.UNZIP_AND_UPDATE_RES)	

	CallModuleFunc(ModuleType.RES_UPDATE, "Start")

	local story_id_for_launch_examine = ConfigAdapter.Common.GetStoryIDForLaunchExamine()
	if CPPSdkTools.IsInExamine() and story_id_for_launch_examine then
		--审核状态下，屏蔽启动流程，播放启动剧情
		CallModuleFunc(ModuleType.STORY, "GetType")
		StartConfig.SkipStory = false
		CallModuleFunc(ModuleType.STORY, "Start", story_id_for_launch_examine, nil, function()
			StartConfig.SkipStory = true
		end)		
	else
		--非审核状态下，显示启动流程
		CallModuleFunc(ModuleType.RES_UPDATE, "OpenView", BaseViewType.RES_UPDATE)
	end
end

function GameState.ResourceUpdateState:StateUpdate(state_name, elapse_time)
	if not self.__is_finished and
		CallModuleFunc(ModuleType.RES_UPDATE, "IsFinished") then
		self.__is_finished = true

		local start_callback = GlobalStateMgr:GetStartCallback()
		if start_callback then
			start_callback()
			GlobalStateMgr:SetStartCallback(nil)
		end		
	end
end

function GameState.ResourceUpdateState:StateQuit(state_name)
end

--[[
连接服务器状态
--]]
GameState.ConnectServerState = GameState.ConnectServerState or BaseClass(GameState.BaseState)

function GameState.ConnectServerState:__init()
	self.__name = "connect_server"
end

function GameState.ConnectServerState:StateEnter(state_name)
	local req_server_config
	local req_server_list, update_server_list, server_list

	--连接服务器
	local function connect_server()
		--客户端事件跟踪(统计流失率)
		StatisticsTool.GetInstance():ReportEventTrackForLossRate(StatisticsTool.EventType4LossRate.CONNECT_SERVER)								

		self.__connect_event_handle = self:Bind(Macros.Event.Net.SERVER_CONNECTED, function(result)
			if result then
				--时间同步后，再切换到登陆状态
				local is_time_sync = CallModuleFunc(ModuleType.TIME_SYNC, "IsSync")
				if is_time_sync then
					update_server_list()
					GlobalStateMgr:ToLoginState()		
				else
					self.__timesync_event_handle = self:Bind(Macros.Event.TimeSyncModule.TIME_SYNC_FIRST, function()
						update_server_list()
						GlobalStateMgr:ToLoginState()
					end)
				end
			end
		end)

		if not GlobalNetAdapter:IsConnected() then
			--连接服务器
			GlobalNetAdapter:Connect(5000)
		else
			--先断开，再连接
			self.__disconnect_event_handle = self:Bind(Macros.Event.Net.SERVER_DISCONNECTED, function()
				if self.__disconnect_event_handle then
					self:UnBind(self.__disconnect_event_handle)
					self.__disconnect_event_handle = nil
				end
				GlobalNetAdapter:Connect(5000)								
			end)
			GlobalNetAdapter:Disconnect(true)
		end
	end

	--获取服务器列表
	req_server_list = function()
		if not CallModuleFunc(ModuleType.LOGIN, "GetServerMap") then
			--客户端事件跟踪(统计流失率)
			StatisticsTool.GetInstance():ReportEventTrackForLossRate(StatisticsTool.EventType4LossRate.REQ_SERVER_LIST)										

			local is_in_examine = CPPSdkTools.IsInExamine()
			local urlmsg_data = {}
			urlmsg_data.url = is_in_examine and StartConfig.ServerListURLExamine or StartConfig.ServerListURL
			urlmsg_data.open_loading = false
			urlmsg_data.auto_retry = true
			urlmsg_data.timeout = 10000
			urlmsg_data.auto_retry_max_count = 2	
			urlmsg_data.call_plat_api = StartConfig.CallPlatAPIToggle
			urlmsg_data.handle_func = function(msg)
				LogTool.LogInfo("[GameState.ConnectServerState:StateEnter] 获取服务器列表成功")
				server_list = msg.server_list
				connect_server()
			end
			urlmsg_data.error_notify_func = function(error_msg)
				--客户端事件跟踪过程中，上报导致卡死的错误信息
				StatisticsTool.GetInstance():ReportErrorInfoForEventTrack(error_msg)															
			end
			SendUrlMsg(urlmsg_data)

			self:FireNextFrame(Macros.Event.GameState.REQUEST_SERVER_LIST)	

		else
			connect_server()
		end	
	end
	update_server_list = function()
		if server_list then
			CallModuleFunc(ModuleType.LOGIN, "SetServerList", server_list)
			server_list = nil
		end
	end

	--获取服务器信息
	if not GlobalNetAdapter:GetServerInfo() then
		req_server_config = function()		
			--客户端事件跟踪(统计流失率)
			StatisticsTool.GetInstance():ReportEventTrackForLossRate(StatisticsTool.EventType4LossRate.REQ_SERVER_CONFIG)									
		
			local is_in_examine = CPPSdkTools.IsInExamine()
			local urlmsg_data = {}
			urlmsg_data.url = is_in_examine and StartConfig.ServerConfigExamine or StartConfig.ServerConfig			
			urlmsg_data.open_loading = false
			urlmsg_data.auto_retry = true
			urlmsg_data.timeout = 10000
			urlmsg_data.auto_retry_max_count = 2
			urlmsg_data.call_plat_api = StartConfig.CallPlatAPIToggle
			urlmsg_data.handle_func = function(msg, data)
				LogTool.LogInfo("[GameState.ConnectServerState:StateEnter] 获取服务器配置成功")

				if msg.status == Macros.Game.ServerStatus.MAINTAIN then
					--服务器维护中
				    LogicGUI.TemplAffirmTips.New({
				    	type = LogicGUI.TemplAffirmTipsType.YES,
				        content = msg.msg or CPPGameLib.GetString("login_server_maintain_tips"),
				        confirm_text = CPPGameLib.GetString("common_retry"),
				        confirm_func = function()
				        	req_server_config()
				        end,
				       	show_close_btn = false,
				    })					
					return
				end
				GlobalNetAdapter:SetIPAddress(data.ip)
				GlobalNetAdapter:SetServerInfo(msg.ip, msg.port)

				--活动公告url
				Macros.Global.NOTICEURL = msg.activity_notice_url

				req_server_list()
			end
			urlmsg_data.error_notify_func = function(error_msg)
				--客户端事件跟踪过程中，上报导致卡死的错误信息
				StatisticsTool.GetInstance():ReportErrorInfoForEventTrack(error_msg)															
			end
			SendUrlMsg(urlmsg_data)

			self:FireNextFrame(Macros.Event.GameState.REQUEST_SERVER_CONFIG)
		end
		req_server_config()

	else
		req_server_list()
	end
end

function GameState.ConnectServerState:StateUpdate(state_name, elapse_time)
end

function GameState.ConnectServerState:StateQuit(state_name)
	if self.__timesync_event_handle then
		self:UnBind(self.__timesync_event_handle)
		self.__timesync_event_handle = nil
	end

	if self.__connect_event_handle then
		self:UnBind(self.__connect_event_handle)		
		self.__connect_event_handle = nil
	end

	if self.__disconnect_event_handle then
		self:UnBind(self.__disconnect_event_handle)		
		self.__disconnect_event_handle = nil
	end	
end

--[[
登陆状态
--]]
GameState.LoginState = GameState.LoginState or BaseClass(GameState.BaseState)

function GameState.LoginState:__init()
	self.__name = "login"
end

function GameState.LoginState:StateEnter(state_name)
	self.__has_opened_view = false
end

function GameState.LoginState:StateUpdate(state_name, elapse_time)
	if not self.__has_opened_view then
		local story_id_for_launch_examine = ConfigAdapter.Common.GetStoryIDForLaunchExamine()
		if CPPSdkTools.IsInExamine() and 
			story_id_for_launch_examine and 
			CallModuleFunc(ModuleType.STORY, "IsInStory", story_id_for_launch_examine) then
			--审核状态下，播放启动剧情中，过滤之
			return
		end

		self.__has_opened_view = true
		local sub_type = GlobalStateMgr:GetLoginStateSubType()
		CallModuleFunc(ModuleType.LOGIN, "OpenView", BaseViewType.LOGIN, sub_type, function()
			CallModuleFunc(ModuleType.RES_UPDATE, "CloseView")
		end)	
	end
end

function GameState.LoginState:StateQuit(state_name)
end

--[[
服务器列表状态
--]]
GameState.ServerListState = GameState.ServerListState or BaseClass(GameState.BaseState)

function GameState.ServerListState:__init()
	self.__name = "server_list"
end

function GameState.ServerListState:StateEnter(state_name)
	CallModuleFunc(ModuleType.LOGIN, "OpenView", BaseViewType.LOGIN_SERVER_LIST)
end

function GameState.ServerListState:StateUpdate(state_name, elapse_time)
end

function GameState.ServerListState:StateQuit(state_name)
	GlobalViewMgr:CloseViews({[BaseViewType.LOGIN_SERVER_LIST] = true}, nil, true, true)
end

--[[
进入游戏状态
--]]
GameState.EnterGameState = GameState.EnterGameState or BaseClass(GameState.BaseState)

function GameState.EnterGameState:__init()
	self.__name = "enter_game"
end

function GameState.EnterGameState:StateEnter(state_name)
	--申请统计所有系统的红点
	CallModuleFunc(ModuleType.RED_MGR, "StatisticsAllRed")
	
	--上传进入游戏到SDK
	CPPSdkTools.ReportEnterGame()

	--开启统计玩家在线的计时器
	StatisticsTool.GetInstance():StartTimerForStatisticsPlayerOnline()

	--客户端事件跟踪(统计流失率)
	StatisticsTool.GetInstance():ReportEventTrackForLossRate(StatisticsTool.EventType4LossRate.START_GAME)												
end

function GameState.EnterGameState:StateUpdate(state_name, elapse_time)
	--打开主界面
	GlobalScene:ChangeScene(Macros.Game.FBType.MAIN_CITY)
end

function GameState.EnterGameState:StateQuit(state_name)
end
