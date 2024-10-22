
--[[
%% @module: 登陆模块
%% @author: swordman sue
%% @created: 2016-07-15
%% @description: 负责登陆相关
--]]

Modules = Modules or {}

Modules.LoginModule = Modules.LoginModule or BaseClass(BaseModule)

--心跳包间隔(秒)
local heart_beat_interval = 30

--服务器分区区间
local server_zone_region = 10

local server_status_img_map = 
{
	[Macros.Game.ZoneStatus.HOT] = "hot",
	[Macros.Game.ZoneStatus.NEW] = "new",
	[Macros.Game.ZoneStatus.MAINTAIN] = "maintain",
}

local function parse_server_info(server_info)
	server_info.id = tonumber(server_info.id) or 0
	server_info.group = math.ceil(server_info.id / server_zone_region)
	server_info.name = server_info.name or ""
	server_info.status = tonumber(server_info.status) or Macros.Game.ZoneStatus.HOT
	server_info.open_time = tonumber(server_info.open_time) or 0
	server_info.maintain_time = tonumber(server_info.maintain_time) or 0
	server_info.is_recommend = tonumber(server_info.recommend) == Macros.Global.TRUE
	server_info.preview_time = tonumber(server_info.preview_time) or 0

	--所有NEW标记，在开服2天后自动设置为HOT
	if server_info.status == Macros.Game.ZoneStatus.NEW then
		local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		if server_time - server_info.open_time >= TimeTool.SINGLE_DAY_SECOND * 2 then
			server_info.status = Macros.Game.ZoneStatus.HOT
		end
	end
end

function Modules.LoginModule:__init()
end

function Modules.LoginModule:__delete()
	self:DumpViews()
	self:StopHeartBeatTimer()

	if not CPPSdkTools.IsSelfLoginSystem() then
		CPPSdkTools.SetLuaLoginCallback(0)
	end
end

--销毁登陆相关界面
function Modules.LoginModule:DumpViews()
	local view_type_map = 
	{
		[BaseViewType.LOGIN] = true,
		[BaseViewType.LOGIN_SERVER_LIST] = true,
	}
	GlobalViewMgr:CloseViews(view_type_map, BaseView.CloseMode.DESTROY, true, true)
end

--获取区服状态图标
function Modules.LoginModule:GetServerStatusImgData(status)
	local img_name = server_status_img_map[status] or "nil"
	return Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LOGIN, img_name, true)
end

--获取区名
function Modules.LoginModule:GetServerZoneName(zone_index)
	if zone_index < 1 then
		return "nil"
	end

	local str = CPPGameLib.GetString("login_server_zone_name")
	return string.format(str, (zone_index-1)*server_zone_region+1, zone_index*server_zone_region)
end

--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
function Modules.LoginModule:HandleServerConnected(result)
end

function Modules.LoginModule:HandleServerDisconnected()
	self.__login_info = nil
	self.__acc_record_value = nil
	self.__acc_record_count = nil

	self:StopHeartBeatTimer()
end

--处理错误码通知
function Modules.LoginModule:HandleErrorNotify(error_code)
	local config_error_code = CPPGameLib.GetConfig("ErrorCode", error_code)
	if not config_error_code then
		return
	end

	--账号在别处登陆，主动断开
	if error_code == 1002 or 
		--您已经被GM强行下线
		error_code == 1012 then
		GlobalNetAdapter:Disconnect(nil, config_error_code.err_tips)

	--角色名已被使用	
	elseif error_code == 1003 then
		self:CreateRoleReq(CallModuleFunc(ModuleType.SUNDRY, "GetRandomRoleName"), Macros.Game.Sex.MALE)

	--检测到外挂，主动断开
	elseif error_code == 20014 then
		GlobalNetAdapter:Disconnect(nil, config_error_code.err_tips)		
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
function Modules.LoginModule:LoginAccountReq_PB()
	local protocol = GlobalNetAdapter:GetProtocol( Net.ProtocolNo.C2S_LOGIN_ACCOUNT_REQ_PB )
	protocol.platform = 666
	protocol.user_id = "LSH"	
	GlobalNetAdapter:OnSend(protocol)
end

function Modules.LoginModule:LoginAccountResp_PB(protocol_data)
	CPPGameLib.PrintTable("skfb", protocol_data, "LoginAccountResp_PB")
end

--请求登陆
function Modules.LoginModule:LoginAccountReq(account, password, type, code)
	password = password or "test"
	type = type or  0

	self.__login_info = self.__login_info or {}
	self.__login_info.type = type
	self.__login_info.password = password
	self.__last_login_servers = nil

	local protocol = GlobalNetAdapter:GetProtocol( Net.ProtocolNo.C2S_LOGIN_ACCOUNT_REQ )
	protocol.platform_type = CPPSystemTools.GetPlatType()
	protocol.cp_channel_id = CPPSdkTools.GetCPChannelType()
	protocol.sdk_channel_id = CPPSdkTools.GetSDKChannelType()
	protocol.type = type
	protocol.account = account
	protocol.password = password
	protocol.code = code or 0
	protocol.ip = GlobalNetAdapter:GetIPAddress()
	protocol.device_id = CPPSystemTools.GetDeviceId()
	GlobalNetAdapter:OnSend(protocol)
	LogTool.LogInfo("[LoginModule:LoginAccountReq] 请求登陆")
end

--登陆反馈
function Modules.LoginModule:LoginAccountResp(protocol_data)
	LogTool.LogInfo("[LoginModule:LoginAccountResp] 请求登陆响应")		

	if not self.__login_info then
		return
	end
	self.__login_info.account = protocol_data.account
	self.__login_info.server_list = protocol_data.server_list

	local view = self:GetView()
	if view and view:IsOpen() then
		view:LoginAccountResp()
	end	

	if self.__login_info.type == Macros.Game.LoginType.USER_REGIST or 
		self.__login_info.type == Macros.Game.LoginType.HANDSET_REGIST then
		--注册成功
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_enrollSucceed"))	
	else
		--登陆成功
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_loginSucceed"))	
	end

	--保存信息
	self:__AddRecentAccount(protocol_data.account, self.__login_info.password)

	self:FireNextFrame(Macros.Event.GameState.LOGIN_ACCOUNT_SUCCESS, protocol_data.is_bind_mobile == Macros.Global.TRUE, self.__login_info.type == Macros.Game.LoginType.ONEKEY_LOGIN)	

	--客户端事件跟踪(统计流失率)
	StatisticsTool.GetInstance():ReportEventTrackForLossRate(StatisticsTool.EventType4LossRate.LOGIN_SUCCESS)											
end

--SDK登陆请求
function Modules.LoginModule:LoginAccountReq_SDK(sdk_login_data)
	local account = (sdk_login_data.user_id and sdk_login_data.user_id ~= "") and sdk_login_data.user_id or "account"
	local token = (sdk_login_data.token and sdk_login_data.token ~= "") and sdk_login_data.token or "token"
	if account == "null" or account == "(null)" or token == "null" or token == "(null)" then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_sdk_login_data_invalid"))
		return
	end

	self.__login_info = self.__login_info or {}
	self.__login_info.sdk_login_token = sdk_login_data.token or ""
	self.__login_info.is_visitor = sdk_login_data.is_visitor == Macros.Global.TRUE

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SDK_LOGIN_REQ)
	protocol.platform_type = CPPSystemTools.GetPlatType()
	protocol.cp_channel_id = CPPSdkTools.GetCPChannelType()
	protocol.sdk_channel_id = CPPSdkTools.GetSDKChannelType()
	protocol.account = account
	protocol.token = token
	protocol.context = (sdk_login_data.context and sdk_login_data.context ~= "") and sdk_login_data.context or 0
	protocol.ip = GlobalNetAdapter:GetIPAddress()
	protocol.device_id = CPPSystemTools.GetDeviceId()
	LogTool.LogInfo("[LoginModule:LoginAccountReq_SDK] 请求SDK登陆 platform_type:%d cp_channel_type:%d sdk_channel_type:%d account:%s token:%s context:%s ip:%s device_id:%s", 
		protocol.platform_type, protocol.cp_channel_id, protocol.sdk_channel_id, protocol.account, protocol.token, protocol.context,protocol.ip,protocol.device_id)	

	GlobalNetAdapter:OnSend(protocol)
end

--SDK登陆反馈
function Modules.LoginModule:LoginAccountResp_SDK(protocol_data)
	LogTool.LogInfo("[LoginModule:LoginAccountResp_SDK] 请求SDK登陆响应")	
	
	if not self.__login_info then
		return
	end
	self.__login_info.account = protocol_data.account
	self.__login_info.server_list = protocol_data.server_list

	local view = self:GetView()
	if view and view:IsOpen() then
		view:LoginAccountResp()
	end	

	--登陆成功
	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_loginSucceed"))	

	self:FireNextFrame(Macros.Event.GameState.LOGIN_ACCOUNT_SUCCESS)	

	--Adjust事件跟踪
	CPPSdkTools.ReportAdjustEventTrack(Macros.SDK.AdjustEvent.LOGIN_SUCCESS)
	if self.__login_info.is_visitor then
		CPPSdkTools.ReportAdjustEventTrack(Macros.SDK.AdjustEvent.LOGIN_VISITOR)
	else
		CPPSdkTools.ReportAdjustEventTrack(Macros.SDK.AdjustEvent.LOGIN_FACEBOOK)
	end

	--客户端事件跟踪(统计流失率)
	StatisticsTool.GetInstance():ReportEventTrackForLossRate(StatisticsTool.EventType4LossRate.LOGIN_SUCCESS)											
end

--验证码请求 消息Id: 20018
function Modules.LoginModule:AuthenticationCodeReq( info )
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_AUTHENTICATION_CODE_REQ)
	protocol.mobile = info
	GlobalNetAdapter:OnSend(protocol)
end

--验证码响应 消息Id: 20019
function Modules.LoginModule:AuthenticationCodeResp( info )
end

--手机号绑定 消息Id: 20020
function Modules.LoginModule:MobileBindReq( info )
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_MOBILE_BIND_REQ)
	protocol.account = info.account
	protocol.code = info.code
	protocol.mobile = info.mobile
	GlobalNetAdapter:OnSend(protocol)
end

--手机号绑定 消息Id: 20021
function Modules.LoginModule:MobileBindResp( info )
end

--请求选服
function Modules.LoginModule:SelectServerReq(server_id)
	if not self.__login_info or self.__login_info.is_selected_server then
		return
	end

	LogTool.LogInfo("[LoginModule:SelectServerReq] 请求进入服务器(server_id:%d)", server_id)	

	self.__login_info.last_server_id = server_id
	self.__login_info.to_select_server = true

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LOGIN_SELECT_SERVER_REQ)
	protocol.select_server_id = server_id
	GlobalNetAdapter:OnSend(protocol)
end

--选服反馈
function Modules.LoginModule:SelectServerResp(protocol_data)
	LogTool.LogInfo("[LoginModule:SelectServerResp] 请求进入服务器响应(have_role:%d)", protocol_data.have_role)	

	if self.__login_info then
		if not self.__login_info.to_select_server then
			return
		end
		self.__login_info.to_select_server = nil
		self.__login_info.is_selected_server = true
	end

	if protocol_data.have_role ~= Macros.Global.TRUE then
		--若该服无角色，则自动创建
		self:CreateRoleReq(CallModuleFunc(ModuleType.SUNDRY, "GetRandomRoleName"), Macros.Game.Sex.MALE)
	end
end

--请求创角
function Modules.LoginModule:CreateRoleReq(role_name, sex, type)
	type = type or Macros.Game.RoleCreateType.NORMAL
	LogTool.LogInfo("[LoginModule:CreateRoleReq] 请求创角(role_name:%s sex:%d type:%d)", role_name, sex, type)	

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LOGIN_CREATE_ROLE_REQ)
	protocol.role_name = role_name
	protocol.sex = sex
	protocol.type = type
	GlobalNetAdapter:OnSend(protocol)
end

--请求创角反馈
function Modules.LoginModule:CreateRoleResp(protocol_data)
	if self.__login_info then
		self.__login_info.is_create_role = true
	end
end

--上传创建角色
function Modules.LoginModule:ReportCreateRole()
	if self.__login_info and self.__login_info.is_create_role then
		self.__login_info.is_create_role = nil
		CPPSdkTools.ReportCreateRole()
	end
end

--请求改名
function Modules.LoginModule:ChangeRoleNameReq(role_name)
	LogTool.LogInfo("[LoginModule:ChangeRoleNameReq] 请求修改角色名(role_name:%s)", role_name)	

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LOGIN_CHANGE_ROLE_NAME_REQ)
	protocol.role_name = role_name
	GlobalNetAdapter:OnSend(protocol)
end

--请求改名反馈(0=表示成功,1=名称已被使用,2=有敏感字)
function Modules.LoginModule:ChangeRoleNameResp(protocol_data)
	LogTool.LogInfo("[LoginModule:ChangeRoleNameResp] 请求修改角色名响应(result:%d role_name:%s)", protocol_data.result, protocol_data.role_name)

	if protocol_data.result == 0 then
		self:DumpViews()
		CallModuleFunc(ModuleType.ROLE, "SetName", protocol_data.role_name)

		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_renameSucceed"))
		
	elseif protocol_data.result == 1 then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_role_name_existed"))
	elseif protocol_data.result == 2 then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_role_name_invalid"))
	end
end

--进入游戏反馈
function Modules.LoginModule:EnterGameResp()
	LogTool.LogInfo("[LoginModule:EnterGameResp] 进入游戏响应")	

	--启动心跳包计时器
	self:StartHeartBeatTimer()

	------------------------------------------------------
	--请求相关数据
	------------------------------------------------------
	--请求背包数据
	CallModuleFunc(ModuleType.Pack, "ItemListReq")	

	--请求装备数据
	CallModuleFunc(ModuleType.EQUIP, "EquipmentItemListReq")

	--请求圣器数据
	CallModuleFunc(ModuleType.TREASURE, "TreasureListReq")

	--请求卡牌数据
	CallModuleFunc(ModuleType.DIVINATION, "CardListReq")

	--请求占卜数据
	CallModuleFunc(ModuleType.DIVINATION, "PlayerZhanbuDataReq")

	--请求收藏书列表
	CallModuleFunc(ModuleType.DIVINATION, "CollectionBookListReq")

	--请求精灵数据
	CallModuleFunc(ModuleType.SPIRIT, "SpiritDataReq")

	--请求世界之心数据
	CallModuleFunc(ModuleType.WORLD_HEART, "WorldHeartDataReq")

	--请求公会技能
	CallModuleFunc(ModuleType.UNION , "GuildSkillListReq") 

	--请求主线副本数据
	CallModuleFunc(ModuleType.DUNGEON, "FBMainDataReq")

	--请求英雄副本数据
	CallModuleFunc(ModuleType.DUNGEON, "FBHeroDataReq")

	--请求精英副本数据
	CallModuleFunc(ModuleType.DUNGEON, "FBEliteDataReq")

	--请求日常副本
	CallModuleFunc(ModuleType.DUNGEON, "FBDailyDataReq")

	--聊天记录
	CallModuleFunc(ModuleType.CHAT, "ChatRecordListReq" )

	--称号
	CallModuleFunc( ModuleType.TITLE , "RoleTitleInfoReq" )

	--商城
	CallModuleFunc(ModuleType.SHOP, "ShopBuyRecordRequest")

	--爬塔
	CallModuleFunc(ModuleType.TOWER , "FbTowerDataReq")

	--寻访遗迹
	CallModuleFunc(ModuleType.MANOR, "PlayerLandDataReq")

	--夺宝进度
	CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "plunderedTreasuresDataReq")

	--世界任务
	CallModuleFunc(ModuleType.PUSH_WORLDLEVEL, "WorldLevelTaskDataReq")

	--原石
	CallModuleFunc(ModuleType.GEM, "GemListReq")

	--直购特惠
	CallModuleFunc(ModuleType.DIRECT_BUY_SHOP, "HotSaleGoodsDataReq")

	--领奖中心
	CallModuleFunc(ModuleType.ACCEPT_PRIZE_CENTER, "RewardCenterListReq")

	--初始化已开启的活动
	CallModuleFunc(ModuleType.ACTIVITY, "InitActivitys")
	--请求活动相关协议
	CallModuleFunc(ModuleType.ACTIVITY, "RequestProtocolsForStartGame")

	------------------------------------------------------
	------------------------------------------------------

	--请求开始游戏（必须放在最后）
	self:StartGameReq()
end

--请求心跳包
function Modules.LoginModule:HeartBeatReq()
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HEART_BEAT)
	protocol.value = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	GlobalNetAdapter:OnSend(protocol)

	LogTool.LogInfo("skfb", "[Modules.LoginModule:HeartBeatReq] 请求心跳包")	
end

function Modules.LoginModule:HeartBeatResp(protocol_data)
end

--请求开始游戏
function Modules.LoginModule:StartGameReq()
	LogTool.LogInfo("[LoginModule:StartGameReq] 开始游戏请求")		

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_START_GAME_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--开始游戏反馈
function Modules.LoginModule:StartGameResp(protocol_data)
	local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID")
	LogTool.LogInfo("[LoginModule:StartGameResp] 开始游戏反馈，角色ID：%d", role_id)
			
	--检测逻辑层版本更新
	local cur_client_version = Cookies.Get(nil, Macros.Setting.ClientVersion)
	if cur_client_version ~= "" and tonumber(cur_client_version) ~= protocol_data.client_version then
		Cookies.Set(nil, Macros.Setting.ClientVersion, protocol_data.client_version)

		--强制更新提示
		ShowGameDialog(
			CPPGameLib.GetString("common_info"), 
			CPPGameLib.GetString("common_detected_update"), 
			function()
				CPPGameEngine:CloseApp()
			end)
	else 	
		--进入EnterGame状态
		GlobalStateMgr:ToEnterGameState()
	end
end

--游戏中强行用户更新功能
function Modules.LoginModule:ClientForceUpdateResp(protocol_data)
	--强制断开连接
	GlobalNetAdapter:Disconnect(true)

	--强制更新提示
	ShowGameDialog(
		CPPGameLib.GetString("common_info"), 
		CPPGameLib.GetString("common_detected_update"), 
		function()
			CPPGameEngine:CloseApp()
		end)
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--处理选择服务器
function Modules.LoginModule:HandleSelectServer(server_id)
	--服务器无效
	local server_info = self:GetServerInfo(server_id)
	if not server_info then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_server_invalid_tips", time_str))			
	end

	--检测服务器是否开启
	if not StartConfig.SuperUser and not self:IsServerOpen(server_id) then
		local time_str = TimeTool.GetTimeFormat(server_info.open_time, 12)
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_server_open_tips", time_str))			
		return
	end

	--检测服务器是否维护
	if not StartConfig.SuperUser and self:IsServerMataining(server_id) then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_server_maintain_tips"))			
		return
	end

	self:SelectServerReq(server_id)
end

--开启心跳包计时器
function Modules.LoginModule:StartHeartBeatTimer()
	self:HeartBeatReq()
	
	self:StopHeartBeatTimer()
	if not self.__heart_beat_timer then
		self.__heart_beat_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()
			self:HeartBeatReq()
		end), heart_beat_interval, -1)
	end
end

--清除心跳包计时器
function Modules.LoginModule:StopHeartBeatTimer()
	if self.__heart_beat_timer then
		GlobalTimerQuest:CancelQuest(self.__heart_beat_timer)
		self.__heart_beat_timer = nil
	end
end

--设置服务器列表
function Modules.LoginModule:SetServerList(list)
	self.__server_map = {}
	self.__zone_server_map = {}
	self.__server_id_list = {}

	for _, server_info in ipairs(list) do
		parse_server_info(server_info)
		self.__server_map[server_info.id] = server_info
		self.__zone_server_map[server_info.group] = self.__zone_server_map[server_info.group] or {}
		table.insert(self.__zone_server_map[server_info.group], server_info)

		table.insert(self.__server_id_list, server_info.id)
	end
end

--获取服务器列表
function Modules.LoginModule:GetServerMap()
	return self.__server_map
end

--获取服务器ID列表
function Modules.LoginModule:GetServerIDList()
	return self.__server_id_list
end

--获取服务器信息(server_id为空则返回最近登陆的服务器信息)
function Modules.LoginModule:GetServerInfo(server_id)
	if not self.__server_map then
		return nil
	end

	server_id = server_id or self:GetLastLoginServerID()
	return self.__server_map[server_id]
end

--判断服务器是否开启
function Modules.LoginModule:IsServerOpen(server_id)
	local server_info = self:GetServerInfo(server_id)
	if not server_info then
		return
	end

	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	return server_time >= server_info.open_time
end

--判断服务器是否维护中
function Modules.LoginModule:IsServerMataining(server_id)
	local server_info = self:GetServerInfo(server_id)
	if not server_info then
		return
	end

	if server_info.status ~= Macros.Game.ZoneStatus.MAINTAIN then
		return
	end

	if server_info.maintain_time <= 0 then
		return true
	end

	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	return server_time < server_info.maintain_time
end

--判断服务器是否显示
function Modules.LoginModule:IsServerVisual(server_id)
	local server_info = self:GetServerInfo(server_id)
	if not server_info then
		return
	end

	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	if server_info.preview_time == 0 then
		--开服后才可见
		return server_time >= server_info.open_time
	else
		--开服前多少秒可见
		return server_time >= server_info.open_time - server_info.preview_time
	end
end

--获取分区列表
function Modules.LoginModule:GetZones(is_for_visual)
	if not self.__zone_server_map then
		return nil
	end

	if not self.__zones then
		self.__zones = {}
		local data
		for zone_index, _ in pairs(self.__zone_server_map) do
			data = {}
			data.index = zone_index
			data.name = self:GetServerZoneName(zone_index)
			table.insert(self.__zones, data)
		end
		table.sort(self.__zones, SortTools.KeyLowerSorter("index"))
	end

	if not is_for_visual then
		return self.__zones
	else
		local visual_zones = {}
		for _, zone_info in ipairs(self.__zones) do
			local zone_servers = self:GetZoneServers(zone_info.index)
			local server_info = zone_servers[1]
			if server_info and self:IsServerVisual(server_info.id) then
				table.insert(visual_zones, zone_info)
			end
		end
		return visual_zones
	end
end

--获取某分区的服务器列表
function Modules.LoginModule:GetZoneServers(zone_index, is_for_visual)
	if not self.__zone_server_map then
		return nil
	end

	local zone_servers = self.__zone_server_map[zone_index]
	if not is_for_visual then
		return zone_servers
	else
		local visual_servers = {}
		for _, server_info in ipairs(zone_servers) do
			if self:IsServerVisual(server_info.id) then
				table.insert(visual_servers, server_info)
			end
		end
		return visual_servers
	end
end

--获取最近登陆的服务器列表
function Modules.LoginModule:GetLastLoginServers()
	if not self.__last_login_servers and self.__server_map and 
		self.__login_info and self.__login_info.server_list then
		self.__last_login_servers = {}

		local server_info
		for _, last_server_info in pairs(self.__login_info.server_list) do
			server_info = CPPGameLib.CopyTbl(self.__server_map[last_server_info.serverid])
			if server_info then
				server_info.head_icon = last_server_info.head_icon
				server_info.role_level = last_server_info.level
				server_info.role_name = last_server_info.role_name
				table.insert(self.__last_login_servers, server_info)
			end
		end
	end
	return self.__last_login_servers
end

--获取最近登陆的服务器信息
function Modules.LoginModule:GetLastLoginServerID()
	if not self.__server_map then
		return
	end

	if not self.__login_info then
		return
	end

	if self.__login_info.last_server_id then
		return self.__login_info.last_server_id
	end

	local last_server_id = 0
	if self.__login_info.server_list and 
		self.__login_info.server_list[1] then
		last_server_id = self.__login_info.server_list[1].serverid
	end
	return last_server_id
end

--按ID获取最近登陆的服务器
function Modules.LoginModule:GetLastLoginInfoByID( id )
	local last_login_servers = self:GetLastLoginServers()
    for i, v in ipairs(last_login_servers) do
    	if v.id == id then
    		return v
    	end
    end
end

--获取推荐的服务器列表
function Modules.LoginModule:GetRecommendServers()
	if not self.__recommend_servers and self.__server_map then
		self.__recommend_servers = {}

		for server_id, server_info in pairs(self.__server_map) do
			if self:IsServerOpen(server_id) and 
				(server_info.status == Macros.Game.ZoneStatus.HOT or server_info.status == Macros.Game.ZoneStatus.NEW) then
				table.insert(self.__recommend_servers, server_info)
			end
		end		
		table.sort(self.__recommend_servers, function(a, b)
			if not a then
				return true
			elseif not b then
				return false
			elseif a.status > b.status then
				return true
			elseif a.status < b.status then
				return false
			elseif a.id > b.id then
				return true
			else
				return false
			end
		end)	
	end
	return self.__recommend_servers
end

--获取推荐服务器信息
function Modules.LoginModule:GetRecommendServerInfo(is_force)
	if not self.__recommend_server_info and self.__server_map then
		local recommend_list
		for server_id, server_info in pairs(self.__server_map) do
			if (is_force or self:IsServerOpen(server_id)) and server_info.is_recommend then
				recommend_list = recommend_list or {}
				table.insert(recommend_list, server_info)
			end
		end
		if recommend_list then
			table.sort(recommend_list, SortTools.KeyUpperSorter("id"))
			self.__recommend_server_info = recommend_list[1]
		end
	end
	return self.__recommend_server_info
end

--SDK登录界面
function Modules.LoginModule:LoginSDK(params)
	if CPPSdkTools.IsSelfLoginSystem() then	
		return
	end

	--SDK初始化失败，弹出重启游戏信息框
    local is_sdk_init_failed = tonumber(Cookies.Get(nil, Macros.Setting.IsSDKInitFailed)) or Macros.Global.FALSE
    if is_sdk_init_failed == Macros.Global.TRUE then
		ShowGameDialog(
			CPPGameLib.GetString("common_error"), 
			CPPGameLib.GetString("common_sdk_init_failed"), 
			function()
				CPPGameEngine:CloseApp()
			end)

		--客户端事件跟踪过程中，上报导致卡死的错误信息
		StatisticsTool.GetInstance():ReportErrorInfoForEventTrack("[Modules.LoginModule:LoginSDK] sdk initialize failed")																	
    	return
    end

	--SDK未初始化，弹出重启游戏信息框
	if not CPPSdkTools.IsInited() then		
		ShowGameDialog(
			CPPGameLib.GetString("common_error"), 
			CPPGameLib.GetString("common_sdk_init_failed"), 
			function()
				CPPGameEngine:CloseApp()
			end)

		--客户端事件跟踪过程中，上报导致卡死的错误信息
		StatisticsTool.GetInstance():ReportErrorInfoForEventTrack("[Modules.LoginModule:LoginSDK] sdk has not initialized")																	
		return
	end

	--登录账号
	if Modules.LoginModule.sdk_login_data then
		self:SDKLoginResp(Modules.LoginModule.sdk_login_data)
		return
	end

	--切换账号
	if Modules.LoginModule.sdk_login_data_for_switch_account then
		self:__RegistSDKLoginCallback()
		self:SDKLoginResp(Modules.LoginModule.sdk_login_data_for_switch_account)
		Modules.LoginModule.sdk_login_data_for_switch_account = nil
		return
	end

	if self.__sdk_loading_timer then
		GlobalTimerQuest:CancelQuest(self.__sdk_loading_timer)
	end	
	self.__sdk_loading_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function() 
		self.__sdk_loading_timer = nil

		--关闭loading界面
		GlobalViewMgr:CloseViews({[BaseViewType.UI_LOADING_II] = true}, nil, true, true)		        
	end), 10)

	--打开loading界面
	CallModuleFunc(ModuleType.LOADING, "OpenView", BaseViewType.UI_LOADING_II)

	--监听SDK登录相关回调
	self:__RegistSDKLoginCallback()

	--打开SDK登陆界面
	CPPSdkTools.LoginPanelEx(params)
end

--SDK登陆成功回调
function Modules.LoginModule:SDKLoginResp(json_str)
    LogTool.LogInfo("[Modules.LoginModule:SDKLoginResp] %s", json_str)

    Modules.LoginModule.sdk_login_data = json_str

	--检测断线重连
    if GlobalNetAdapter:CheckReconnectAlert() then
    	return
    end

	local data, pos, err = CPPGameLib.JSON.decode(json_str)
	if err then
		LogTool.LogError("[Modules.LoginModule:SDKLoginResp] json解析错误 err:%s json_str:%s", err, json_str)
	else
		--SDK登陆请求
		self:LoginAccountReq_SDK(data)
	end
end

--注销账号
function Modules.LoginModule:Logout(logout_sdk)
	if CPPSdkTools.IsSelfLoginSystem() then
		GlobalGame:Relogin()
	else
		CPPSdkTools.Logout()
	end
end

--SDK注销成功回调
function Modules.LoginModule:SDKLogoutResp(data)
    LogTool.LogInfo("[Modules.LoginModule:SDKLogoutResp]")

    Modules.LoginModule.sdk_login_data = nil    

	CPPSdkTools.ReportLogout()

	GlobalGame:Relogin()
end

--SDK切换账号成功回调
function Modules.LoginModule:SDKSwitchAccountResp(data)	
    LogTool.LogInfo("[Modules.LoginModule:SDKSwitchAccountResp]")

    Modules.LoginModule.sdk_login_data = nil    
	Modules.LoginModule.sdk_login_data_for_switch_account = data

	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
    if not role_info then
    	--未进入游戏，自动重连
		GlobalStateMgr:ToConnectServerState()			
    else
    	--已进入游戏，退出到登陆界面
    	GlobalGame:Relogin()
    end
end

--切换账号处理
function Modules.LoginModule:SwitchAccount()
	if CPPSdkTools.IsSelfLoginSystem() then	
		--自有登陆系统
		self.__tochange_account_status = true
		GlobalStateMgr:ToConnectServerState()					
	else
		--SDK登陆系统
		CPPSdkTools.SwitchAccountEx()
	end
end

--获取登录信息
function Modules.LoginModule:GetLoginInfo()
	return self.__login_info
end

--修改登录信息
function Modules.LoginModule:ModifyLoginInfo(key, value)
	if self.__login_info then
		self.__login_info[key] = value
	end
end

--获得当前账号
function Modules.LoginModule:GetAccount()
	return self.__login_info and self.__login_info.account or ""
end

function Modules.LoginModule:GetSDKLoginToken()
	return self.__login_info and self.__login_info.sdk_login_token or ""
end

--是否切换账号状态
function Modules.LoginModule:IsSwitchAccount()
	return self.__tochange_account_status
end

--重置切换账号状态
function Modules.LoginModule:ResetSwitchAccount()
	self.__tochange_account_status = nil
end

--获得最近登陆的账号密码
function Modules.LoginModule:GetLastAccountInfo()
	local last_login_account = CPPGameLib.SplitEx( Cookies.Get( nil ,Macros.Setting.LastLoginAccount) , "<GH>")[1]
	local last_login_password = CPPGameLib.SplitEx( Cookies.Get( nil ,Macros.Setting.LastLoginPassword) , "<GH>")[1]

	local type = nil
	if last_login_account and last_login_account ~= "" then
		type = Macros.Game.LoginType.USER_LOGIN 
	end

	return last_login_account , last_login_password , type 
end

--移除最近登录的账号记录
function Modules.LoginModule:DelRecentAccount( account )
	local accountLst = CPPGameLib.SplitEx( Cookies.Get( nil ,Macros.Setting.LastLoginAccount) , "<GH>")
	local passwordLst = CPPGameLib.SplitEx( Cookies.Get( nil ,Macros.Setting.LastLoginPassword) , "<GH>")
	local t1 = {}
	for i , v in ipairs( accountLst ) do
		if v ~= account then
			table.insert( t1 , { account = v , password = passwordLst[i] } )
		end
	end
	self:__SaveRecentAccount( t1 )
	GlobalEventSystem:FireNextFrame(Macros.Event.GameState.LOGIN_ACCOUNT_REMOVE )
end

--获取最近登录的账号记录
function Modules.LoginModule:GetRecentAccountList()
	local accountLst = CPPGameLib.SplitEx( Cookies.Get( nil ,Macros.Setting.LastLoginAccount) , "<GH>")
	local passwordLst = CPPGameLib.SplitEx( Cookies.Get( nil ,Macros.Setting.LastLoginPassword) , "<GH>")
	local t1 = {}
	for i = 1 , #accountLst do
		table.insert( t1 , { account = accountLst[i] , password = passwordLst[i] } )
	end
	return t1
end

--获得随机密码
function Modules.LoginModule:GetRandomPassword( )
	local text = ""
	for i = 1 , 2 do
		text = text .. string.char( MathTool.GetRandom( 48 , 57 ) )
	end

	for i = 1 , 2 do
		text = text .. string.char( MathTool.GetRandom( 65 , 90 ) )
	end

	for i = 1 , 2 do
		text = text .. string.char( MathTool.GetRandom( 97 , 122 ) )
	end

	return text
end

--进入前台处理
function Modules.LoginModule:DoAfterEnterForeground()
	if not self.__enter_background_time then
		return
	end

	--进入后台超过心跳间隔，立即发送一次心跳包并重新计时
	if Macros.Global.NowTime - self.__enter_background_time > heart_beat_interval then
		if self.__heart_beat_timer then
			self:StartHeartBeatTimer()
		end
	end
end

--进入后台处理
function Modules.LoginModule:DoAfterEnterBackground()
	self.__enter_background_time = Macros.Global.NowTime
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--监听SDK登录相关回调
function Modules.LoginModule:__RegistSDKLoginCallback()
	if not CPPSdkTools.IsSelfLoginSystem() and not self.__is_registed_sdk_login_callback then
		self.__is_registed_sdk_login_callback = true

		--SDKLogin回调
		CPPSdkTools.SetLuaLoginCallback(GlobalCallbackMgr:AddCallBackFunc(function(code, json_str)		
			local function close_loading_view()
				if self.__sdk_loading_timer then
					GlobalTimerQuest:CancelQuest(self.__sdk_loading_timer)
					self.__sdk_loading_timer = nil

					--关闭loading界面
					GlobalViewMgr:CloseViews({[BaseViewType.UI_LOADING_II] = true}, nil, true, true)		        
				end
			end

        	Macros.SDK.Constants.is_sdk_logining = false

	        CPPGameLib.Switch(code)
	        {
	        	--登陆成功
	            [Macros.SDK.Constants.SUCCESS_CODE] = function()
	            	close_loading_view()
	                self:SDKLoginResp(json_str)
	            end,

	        	--登陆取消
	            [Macros.SDK.Constants.CANCEL_CODE] = function ()
	            	LogTool.LogError("SDK登陆取消")
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_sdk_login_cancel"))	

	            	close_loading_view()
	            	GlobalNetAdapter:CheckReconnectAlert()
	            end,

	        	--登陆失败
	            [Macros.SDK.Constants.FAILURE_CODE] = function()
	            	LogTool.LogError("SDK登陆失败")
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_sdk_login_failed"))	

	            	close_loading_view()
	            	GlobalNetAdapter:CheckReconnectAlert()
	            end,

	        	--注销成功
	            [Macros.SDK.Constants.LOGOUT_CODE] = function()
	                self:SDKLogoutResp(data)
	            end,

	        	--切换账号成功
	            [Macros.SDK.Constants.SWITCH_ACCOUNT_CODE] = function()
	            	self:SDKSwitchAccountResp(json_str)
	            end,	            
	        }
		end))
	end
end

--添加最近登录的账号记录
function Modules.LoginModule:__AddRecentAccount( account , password )
	--账号列表
	local nativeAccount = CPPGameLib.SplitEx( Cookies.Get( nil ,Macros.Setting.LastLoginAccount) , "<GH>")
	local accountLst = {}
	for i , v in ipairs( nativeAccount ) do
		accountLst[v] = v
	end

	--如果没有而且要存 那就存
	if not accountLst[account]then
		local t2 = CPPGameLib.SplitEx( Cookies.Get( nil ,Macros.Setting.LastLoginPassword) , "<GH>")
		local t3 = {}
		for i , v in ipairs( nativeAccount ) do
			table.insert( t3 , { account = v , password = t2[i] } )
		end
		table.insert( t3 , 1 , { account = account , password = password })
		self:__SaveRecentAccount( t3 )

	--如果有而且要存 那就排序
	elseif accountLst[account] then
		local t2 = CPPGameLib.SplitEx( Cookies.Get( nil ,Macros.Setting.LastLoginPassword) , "<GH>")
		local t3 = {}
		for i, v in ipairs( nativeAccount ) do
			if v ~= account then
				table.insert( t3 , { account = v , password = t2[i] } )
			end
		end
		table.insert(t3, 1, { account = account , password = password })
		self:__SaveRecentAccount( t3 )
	end
end


--保存最近登录的账号记录
function Modules.LoginModule:__SaveRecentAccount( lst )
	local account , password = "" , ""
	local t1 = {}
	--保存上限
	local MAX = 5

	for i = 1 , MAX do
		if lst[i] then
			table.insert( t1 , lst[i] )
		end
	end

	for i , v in ipairs( t1 ) do
		account = account .. v.account .. "<GH>"
		password = password .. v.password .. "<GH>"
	end

	Cookies.Set( nil , Macros.Setting.LastLoginAccount , account )
	Cookies.Set( nil , Macros.Setting.LastLoginPassword , password )
end

