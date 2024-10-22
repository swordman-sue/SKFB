--[[
%% @module: 时间同步
%% @author: swordman sue
%% @created: 2016-07-15
--]]

Modules = Modules or {}

Modules.TimeSyncModule = Modules.TimeSyncModule or BaseClass(BaseModule)

local start_sync_times = 4
local start_sync_interval = 5
local sync_interval = 30

function Modules.TimeSyncModule:__init()
	self:__Reset()
end

function Modules.TimeSyncModule:__delete()
	self.__is_sync = false
	self.__sync_data = nil
	self:__ClearTimer()
	if self.noticeTime then
		GlobalTimerQuest:CancelQuest( self.noticeTime )
		self.noticeTime = nil
	end
end

function Modules.TimeSyncModule:HandleServerConnected(result)
	if result then
		self:__StartTimer()
	end
end

function Modules.TimeSyncModule:HandleServerDisconnected()
	self:__Reset()
	self:__ClearTimer()
end

--返回服务器时间(秒)
function Modules.TimeSyncModule:GetServerTime()
	if not self.__is_sync then
		LogTool.LogError("[Modules.TimeSyncModule:GetServerTime] 请先同步服务器时间")
		return TimeTool.GetTime()
	end

	local elapse_time_milli = CPPGameEngine:GetNowTimeMilli() - self.__sync_data.org_local_time_millisecond
	local server_time_second = math.floor(self.__sync_data.org_server_time_second + elapse_time_milli/1000)
	return server_time_second
end

function Modules.TimeSyncModule:ServerTimeResp(protocol_data)
	local now_time_milli = CPPGameEngine:GetNowTimeMilli()
	local latency = (now_time_milli - self.__sync_data.last_req_time) * 0.5
	self.__sync_data.last_req_time = 0	

	--记录客户端与服务端的时间差
	local local_time = TimeTool.GetTimeStampFromDate(protocol_data.year + 1900, protocol_data.month + 1, protocol_data.day, protocol_data.hour, protocol_data.minute, protocol_data.second, true)
	TimeTool.delta_between_local_and_server = protocol_data.server_time - local_time

	--记录协议返回最快的那次服务器时间
	if not self.__sync_data.latency or latency < self.__sync_data.latency then
		self.__sync_data.latency = latency
		self.__sync_data.org_server_time_second = protocol_data.server_time + self.__sync_data.latency/1000
		self.__sync_data.org_local_time_millisecond = now_time_milli
	end

	--首次同步派发事件
	if not self.__is_sync then
		self.__is_sync = true
		self:FireNextFrame(Macros.Event.TimeSyncModule.TIME_SYNC_FIRST)
	end

	if self.__sync_data.sync_time == start_sync_times then
		self:__ClearTimer()
		self.__timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()  
			self:__ServerTimeReq()
		end), sync_interval, -1)
	end
end

function Modules.TimeSyncModule:IsSync()
	return self.__is_sync
end

function Modules.TimeSyncModule:__Reset()
	self.__sync_data = self.__sync_data or {}
	self.__sync_data.last_req_time = 0
end

function Modules.TimeSyncModule:__ServerTimeReq()
	if self.__sync_data.last_req_time ~= 0 then
		return
	end

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SERVER_TIME_REQ)
	GlobalNetAdapter:OnSend(protocol, self.__is_sync)
	self.__sync_data.last_req_time = CPPGameEngine:GetNowTimeMilli()

end

--开始同步时间戳
function Modules.TimeSyncModule:__StartTimer()
	self.__sync_data.sync_time = 0
	
	local func = function()
		self.__sync_data.sync_time = self.__sync_data.sync_time + 1
		self:__ServerTimeReq()
	end
	func()

	if self.__timer then
		GlobalTimerQuest:CancelQuest(self.__timer)
	end
	self.__timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(func), start_sync_interval, -1)
end

--结束同步时间戳
function Modules.TimeSyncModule:__ClearTimer()	
    if self.__timer then
        GlobalTimerQuest:CancelQuest(self.__timer)
        self.__timer = nil
    end
end
