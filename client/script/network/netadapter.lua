
--[[
%% @module: 网络适配器
%% @author: swordman sue
%% @created: 2016-07-26
--]]

Net = Net or {}

Net.NetAdapter = Net.NetAdapter or BaseClass(EventAndTimerWrapper)

--协议loading超时时间
Net.NetAdapter.NetLoadingTimeout = 8

local get_protocol_regist_id

--协议接收处理超时时间(每帧)
local handle_protocols_time_threshold = 1 / Macros.Global.FPS_Default / 2

function Net.NetAdapter.CreateInstance()
	if not GlobalNetAdapter then
		GlobalNetAdapter = Net.NetAdapter.New()
	end
end

function Net.NetAdapter.DelInstance()
	if GlobalNetAdapter then
		GlobalNetAdapter:DeleteMe()
		GlobalNetAdapter = nil
	end	
end

function Net.NetAdapter:__init()
	--服务器连接信息
	self.__server_info = nil
	self.__is_connected = false
	self.__net_id = 0xffffffff
	self.__is_asyc_connecting = false
	self.__async_connect_handle = -1
	self.__initiative_disconnect = false

	--协议注册表
	self.__protocol_regist_map = {}
	--协议注册查询表
	self.__protocol_regist_lookup_map = {}

	--接收协议队列
	self.__protocols_recv_queue = {}

	--协议loading表
	self.__protocol_wait_map = {}

	--协议池
	self.__protocol_pool = Net.ProtocolPool.New()

	--记录网络状态(0 无，1 WIFI，2 2G网络，3 3G网络，4 4G网络，5 其他)
	self.__network_state = CPPSystemTools.GetNetWorkState()

	--设置网络状态改变回调
	CPPSystemTools.SetNetworkStateChangeNotifyFunc(GlobalCallbackMgr:AddCallBackFunc(function()		
		self:__CheckNetworkState()
	end))

    GlobalRunner:AddRunObj(self, Runner.Priority.RP_NET_ADAPTER)
end

function Net.NetAdapter:__delete()
	self.__server_info = nil
	self.__protocol_regist_map = nil
	self.__protocol_regist_lookup_map = nil
	self.__protocols_recv_queue = nil

	if self.__protocol_pool then
		self.__protocol_pool:DeleteMe()
		self.__protocol_pool = nil
	end

	self:ResetNetLoading(true)
	self:Disconnect(true)

    GlobalRunner:RemoveRunObj(self)    
end

function Net.NetAdapter:Update(now_time, elapse_time)
	--处理协议接收
	self:__HandleRecv()

	--检测网络有效性
	self:__CheckNetworkValid(elapse_time)

	--延时自动重连
	if self.__delay_reconnect_info and self.__delay_reconnect_info.exec_time <= now_time then
		GlobalStateMgr:ToConnectServerState()										
		self.__delay_reconnect_info = nil
	end

	--TODO：定时向服务器验证IP是否变化，若发生改变，则断线重连
end

function Net.NetAdapter:SetIPAddress(var)
	self.__ip = var
end

function Net.NetAdapter:GetIPAddress()
	return self.__ip or ""
end

function Net.NetAdapter:SetServerInfo(host_name, host_port)
	self.__server_info = self.__server_info or {}
	self.__server_info.host_name = host_name
	self.__server_info.host_port = host_port
end

function Net.NetAdapter:GetServerInfo()
	return self.__server_info
end

function Net.NetAdapter:Connect(timeout, for_auto_connect)
    --支持采用protobuf协议进行的网络交互
    if MessageAdapter.USE_PROTOBUF_FOR_NET_MSG then
    	local buffer = Net.ProtobufStruct.RegistPBData("PbHead.MsgHead")
    	if buffer then
    		MessageAdapter.SetupPBDataForNetMsgHead(buffer)
    	else
		    LogTool.LogError("[Net.NetAdapter:Connect] MsgHead buffer invalid")
    	end
    end	

	if not self.__server_info then
	    LogTool.LogError("[Net.NetAdapter:Connect] 请先设置服务器IP、端口信息")
		return false
	end

	if self.__is_connected or self.__is_asyc_connecting then
		LogTool.LogError("[Net.NetAdapter:Connect] 连接服务器中断：is_connected:%s is_asyc_connecting:%s", 
			tostring(self.__is_connected), tostring(self.__is_asyc_connecting))
		return false
	end

	local ret
    self.__async_connect_timeout = timeout or 5000
	ret, self.__async_connect_handle = MessageAdapter.Connect(self.__server_info.host_name, self.__server_info.host_port, self.__async_connect_timeout)
    LogTool.LogInfo("[Net.NetAdapter:Connect] 开始连接服务器: ip: %s, port: %d", self.__server_info.host_name, self.__server_info.host_port)

	self.__is_asyc_connecting = true
	self:__HandleConnectLoading(true, for_auto_connect)

	if not ret then
		self:OnConnected(Macros.Global.FALSE, self.__async_connect_handle, 0xffffffff)
		return false
	end

	self:FireNextFrame(Macros.Event.Net.SERVER_CONNECTING)

	return true
end

function Net.NetAdapter:OnConnected(result, handle, net_id)
	if handle == self.__async_connect_handle then
		result = result == Macros.Global.TRUE

        LogTool.LogInfo("[Net.NetAdapter:OnConnected] 连接服务器结果：result:%s net_id:%d", tostring(result), net_id)		

		self.__is_connected = result
		self.__net_id = net_id
		self.__is_asyc_connecting = false
		self.__async_connect_handle = -1
        self.__initiative_disconnect = false
		self.__protocals_recv_queue = {}
		
        self:__HandleConnectLoading(false)

		self:FireNextFrame(Macros.Event.Net.SERVER_CONNECTED, result)
	end
end

function Net.NetAdapter:Disconnect(initiative, tips)
	if self.__is_connected then
		self.__initiative_disconnect = initiative
		self.__disconnect_tips = tips
		MessageAdapter.Disconnect(self.__net_id)
	end
end

function Net.NetAdapter:OnDisconnect(net_id)	
	if self.__net_id == net_id then
        LogTool.LogInfo("[Net.NetAdapter:OnDisconnect] 断开连接:%d", net_id)		

		self.__is_connected = false
		self.__net_id = 0xffffffff
		self.__protocols_recv_queue = {}
		self:ResetNetLoading()
		self:CheckReconnectAlert(true)

		self:FireNextFrame(Macros.Event.Net.SERVER_DISCONNECTED, self.__initiative_disconnect)
	end
end

function Net.NetAdapter:OnSend(protocol, ignore_net_loading)
	if not protocol then
		return
	end

	--未连接服务器，过滤
	if not self.__is_connected then
		return
	end

	--已进入后台，先把协议缓存起来
	if self.__is_in_background then
		if not MessageAdapter.USE_PROTOBUF_FOR_NET_MSG then
			protocal = CPPGameLib.CopyTblDeep(protocal)
		end
		protocal.ignore_net_loading = ignore_net_loading
		self.__protocal_cache_list_in_background = self.__protocal_cache_list_in_background or {}
		table.insert(self.__protocal_cache_list_in_background, protocal)
		return
	end

    --支持采用protobuf协议进行的网络交互
    if MessageAdapter.USE_PROTOBUF_FOR_NET_MSG then
    	protocol = protocol.pb_struct
    end

    self:__OnSend(protocol, ignore_net_loading)
end

function Net.NetAdapter:OnRecv(protocol_no, net_id)
	if not self.__is_connected then
		return
	end

	if self.__net_id ~= net_id then
		LogTool.LogError("[Net.NetAdapter:OnRecv] 参数net_id无效(self.__net_id:%d net_id:%d)", self.__net_id, net_id)
		return
	end

	--处理协议loading
	self:__HandleNetLoadingResp(protocol_no)

	local protocol = self:GetProtocol(protocol_no, true)
	if protocol then
		local ret, protocol_data = protocol:Decode(MessageAdapter)
		if ret then
			table.insert(self.__protocols_recv_queue, protocol_data)
		else
			table.insert(self.__protocols_recv_queue, protocol_data)
			LogTool.LogError("[Net.NetAdapter:OnRecv] 协议接收失败(protocol_no:%d)", protocol_no)
		end
	end
end

function Net.NetAdapter:IsNetworkValid()
	return self.__is_network_valid
end

function Net.NetAdapter:IsConnected()
	return self.__is_connected
end

function Net.NetAdapter:IsAsyncConnecting()
	return self.__is_asyc_connecting
end
   
function Net.NetAdapter:GetNetId()
	return self.__net_id
end

--获取协议对象
function Net.NetAdapter:GetProtocol(protocol_no, for_recv)
	return self.__protocol_pool:GetProtocol(protocol_no, for_recv)
end

--绑定协议
function Net.NetAdapter:RegisterProtocol(protocol_no, callback)
	local protocol_regist_id = get_protocol_regist_id()

	--记录查询表
	self.__protocol_regist_lookup_map[protocol_regist_id] = protocol_no

	--记录注册表
	self.__protocol_regist_map[protocol_no] = self.__protocol_regist_map[protocol_no] or {}		
	self.__protocol_regist_map[protocol_no][protocol_regist_id] = callback

	return protocol_regist_id
end

--解绑协议
function Net.NetAdapter:UnRegisterProtocol(protocol_regist_id)
	--移除查询记录
	local protocol_no = self.__protocol_regist_lookup_map[protocol_regist_id]
	self.__protocol_regist_lookup_map[protocol_regist_id] = nil

	--移除注册记录
	if protocol_no and self.__protocol_regist_map[protocol_no] then
		self.__protocol_regist_map[protocol_no][protocol_regist_id] = nil
	end
end

--重置协议loading
function Net.NetAdapter:ResetNetLoading(cleanup)
	if self.__protocol_wait_map then
		for _, wait_info in pairs(self.__protocol_wait_map) do
			if wait_info.timer then
				GlobalTimerQuest:CancelQuest(wait_info.timer)
			end
			if wait_info.dlg then
				wait_info.dlg:DeleteMe()
			end
		end
	end

	if cleanup then
		self.__protocol_wait_map = nil
	else
		self.__protocol_wait_map = {}
	end

	CallModuleFunc(ModuleType.LOADING, "DumpNetLoadingView")
end

--检测断线重连
function Net.NetAdapter:CheckReconnectAlert(is_delay)
	if Macros.SDK.Constants.is_sdk_logining then
		--若SDK登录中，则在SDK登录反馈后，再进行检测
		return
	end

	--非主动断开，提示重连
	if not self.__is_connected and not self.__initiative_disconnect then
		local login_info = CallModuleFunc(ModuleType.LOGIN, "GetLoginInfo")
		if not login_info then
			--未请求登录，则自动重连
			if not is_delay then
				GlobalStateMgr:ToConnectServerState()								
			else
				self.__delay_reconnect_info = {}
				self.__delay_reconnect_info.exec_time = Macros.Global.NowTime + 1
			end
			return true
		end

		self:__PauseWhenDisconnected()

		--已请求登录，弹出断线重连提示
	    LogicGUI.TemplAffirmTips.New({
	    	type = LogicGUI.TemplAffirmTipsType.YES,
	        content = self.__disconnect_tips or CPPGameLib.GetString("common_disconnected_tips"),
	        confirm_func = function()
	        	self:HandleReconnect()
	        end,
	       	show_close_btn = false,
	        render_queue = Scene.RenderQueue.GRQ_UI_MAX,
	    })
		self.__initiative_disconnect = false
		self.__disconnect_tips = nil			

		return true
	end
end

--处理断线重连
function Net.NetAdapter:HandleReconnect()
	CallModuleFunc(ModuleType.LOGIN, "Logout")
end

--进入前台
function Net.NetAdapter:OnEnterForeground()
	self.__is_in_background = false

	--把缓存的协议重新发送
	if self.__protocal_cache_list_in_background then
		for _, protocal in ipairs(self.__protocal_cache_list_in_background) do
			self:OnSend(protocal, protocal.ignore_net_loading)
		end
		self.__protocal_cache_list_in_background = nil
	end
	
	--检测网络状态变化
	self:__CheckNetworkState()
end

--进入后台
function Net.NetAdapter:OnEnterBackground()
	self.__is_in_background = true
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--检测网络有效性
function Net.NetAdapter:__CheckNetworkValid(elapse_time)
	local interval = 2
	self.__check_net_available_elapse_time = self.__check_net_available_elapse_time or interval
	self.__check_net_available_elapse_time = self.__check_net_available_elapse_time + elapse_time
	if self.__check_net_available_elapse_time < interval then
		return
	end
	self.__check_net_available_elapse_time = self.__check_net_available_elapse_time - interval

	self.__is_network_valid = CPPSystemTools.IsNetworkAvailable()
	if self.__is_network_valid then
		self.in_network_alert = false
		return
	end

	if self.in_network_alert then
		return
	end
	self.in_network_alert = true

	LogTool.LogInfo("Net.NetAdapter:__CheckNetworkValid do network alert")

	--网络无效，断开连接
	self:Disconnect(nil, CPPGameLib.GetString("common_network_invalid_tips"))

	--派发网络无效事件
	self:FireNextFrame(Macros.Event.Net.NETWORK_INVALID_NOTIFY)

	--提示网络无效弹窗
	CPPSystemTools.CheckNetWork(CPPGameLib.GetString("common_bad_network"), CPPGameLib.GetString("common_bad_network_setting"), CPPGameLib.GetString("common_bad_network_cancel"))
end

--检测网络状态变化(如：网络制式变化)
function Net.NetAdapter:__CheckNetworkState()
	local old_network_state = self.__network_state or 0
	self.__network_state = CPPSystemTools.GetNetWorkState()

	LogTool.LogInfo("Net.NetAdapter:__CheckNetworkState old_state:%d new_state:%d", old_network_state, self.__network_state)

	if old_network_state ~= 0 and old_network_state ~= self.__network_state then
		--切换网络，断开连接
		self:Disconnect(nil, CPPGameLib.GetString("common_network_change_tips"))

		--派发网络状态变化
		self:FireNextFrame(Macros.Event.Net.NETWORK_STATE_CHANGED_NOTIFY)
	end
end

function Net.NetAdapter:__OnSend(protocol, ignore_net_loading)
	--编码
	if protocol:Encode(MessageAdapter) then
		--发送
		if MessageAdapter.Send(self.__net_id) then
			--处理协议loading
			self:__HandleNetLoadingReq(protocol, ignore_net_loading)
			--派发事件
			self:FireNextFrame(Macros.Event.Net.ON_SEND_PROTOCOL, protocol.protocol_no)
		else
			LogTool.LogError("[Net.NetAdapter:OnSend] 协议发送失败(protocol_no:%s)", protocol.protocol_no)		
		end
	else
		LogTool.LogError("[Net.NetAdapter:OnSend] 协议编码失败(protocol_no:%s)", protocol.protocol_no)		
	end
end

--处理协议接收
function Net.NetAdapter:__HandleRecv()
	self.__protocols_recv_start_time = CPPGameEngine:GetNowTime()

	--接收
	local protocol_data
	while #self.__protocols_recv_queue > 0 do
		protocol_data = table.remove(self.__protocols_recv_queue, 1)

		local callback_list = self.__protocol_regist_map[protocol_data.protocol_no]
		if callback_list then
			for _, callback in pairs(callback_list) do
				callback(protocol_data)
			end
		end

		protocol_data.protocol_no = nil

		if CPPGameEngine:GetNowTime() - self.__protocols_recv_start_time > handle_protocols_time_threshold then
			return
		end		
	end
end

--断线后暂停
function Net.NetAdapter:__PauseWhenDisconnected()
	if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInMainCity") and
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetCurStatus") == Modules.SplogicStatus.RUN then
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "PauseEx")
	end	
end

--处理协议loading
function Net.NetAdapter:__HandleNetLoadingReq(protocol, ignore_net_loading)
	if ignore_net_loading then
		return
	end

	local wait_info = self.__protocol_wait_map[protocol.protocol_no]
	local protocol_no_resp = Net.ProtocolLoadingMap[protocol.protocol_no]
	if not wait_info and protocol_no_resp then
		wait_info = {}
		wait_info.protocol = protocol
		wait_info.protocol_no_resp = protocol_no_resp
		wait_info.timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
			wait_info.timer = nil
			self:__HandleNetLoadingTimeout(protocol.protocol_no)
		end), Net.NetAdapter.NetLoadingTimeout)		

		self:__AddNetLoadingWaitInfo(protocol.protocol_no, wait_info)
	end
end

--处理协议loading
function Net.NetAdapter:__HandleNetLoadingResp(protocol_no_resp)
	for protocol_no, wait_info in pairs(self.__protocol_wait_map) do
		if wait_info.protocol_no_resp == protocol_no_resp then
			if wait_info.timer then
				GlobalTimerQuest:CancelQuest(wait_info.timer)
			end
			if wait_info.dlg then
				wait_info.dlg:DeleteMe()
			end

			self:__DelNetLoadingWaitInfo(protocol_no)
			break
		end
	end
end

--处理协议loading超时
function Net.NetAdapter:__HandleNetLoadingTimeout(protocol_no)
	local wait_info = self.__protocol_wait_map[protocol_no]
	if wait_info then
	    wait_info.dlg = LogicGUI.TemplAffirmTips.New({
	    	content = CPPGameLib.GetString("common_net_loading_timeout", protocol_no),
	        confirm_text = CPPGameLib.GetString("common_net_loading_reconnect"),
	        confirm_func = function()
	        	--重新请求
	        	self:__DelNetLoadingWaitInfo(protocol_no)
	        	self:OnSend(wait_info.protocol)
	        end,
	        cancel_text = CPPGameLib.GetString("common_net_loading_relogin"),
	       	cancel_func = function()
	       		--重新登陆
	        	self:ResetNetLoading()
	        	self:HandleReconnect()
	       	end,
	       	show_close_btn = false,
	        render_queue = Scene.RenderQueue.GRQ_UI_TOP,		       	
	    })					
	end	
end

function Net.NetAdapter:__AddNetLoadingWaitInfo(protocol_no, wait_info)
	self.__protocol_wait_map[protocol_no] = wait_info
	--打开loading界面
	CallModuleFunc(ModuleType.LOADING, "OpenView", BaseViewType.NET_LOADING)	
end

function Net.NetAdapter:__DelNetLoadingWaitInfo(protocol_no)
	self.__protocol_wait_map[protocol_no] = nil
	--关闭loading界面
	GlobalViewMgr:CloseViews({[BaseViewType.NET_LOADING] = true}, nil, true, true)			
end

--处理连接服务器loading
function Net.NetAdapter:__HandleConnectLoading(for_connect, for_auto_connect)
	if for_connect then
		if not for_auto_connect then
			--重置自动重连次数
			self.__auto_reconnect_count = 0
		end

		--打开loading界面
		CallModuleFunc(ModuleType.LOADING, "OpenView", BaseViewType.NET_CONNECT_LOADING, "is_connect_loading")
	else
		--关闭loading界面
		GlobalViewMgr:CloseViews({[BaseViewType.NET_CONNECT_LOADING] = true}, nil, true, true)		        

		if not self.__is_connected then			
			self.__auto_reconnect_count = self.__auto_reconnect_count or 0
			self.__auto_reconnect_count = self.__auto_reconnect_count + 1
			if self.__auto_reconnect_count <= 2 then
				--自动重连
	        	self:Connect(self.__async_connect_timeout, true)
			else
				--弹出重连提示
			    LogicGUI.TemplAffirmTips.New({
			    	type = LogicGUI.TemplAffirmTipsType.YES,
			    	content = CPPGameLib.GetString("common_connect_failed_tip"),
			        confirm_text = CPPGameLib.GetString("common_retry"),
			        confirm_func = function()
			        	--重新连接
			        	self:Connect(self.__async_connect_timeout)
			        end,
			       	show_close_btn = false,
			        render_queue = Scene.RenderQueue.GRQ_UI_TOP,		       	
			    })						
			end
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
get_protocol_regist_id = function()
	Net.NetAdapter.protocol_regist_id = Net.NetAdapter.protocol_regist_id or 0
 	Net.NetAdapter.protocol_regist_id = Net.NetAdapter.protocol_regist_id + 1
 	return Net.NetAdapter.protocol_regist_id
end

