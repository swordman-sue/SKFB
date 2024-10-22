--[[
%% @module: 紫装推送入口
%% @author: ldx
%% @created: 2018-12-22
--]]

Modules = Modules or {}

Modules.MorrowEntry = Modules.MorrowEntry or BaseClass(Modules.BaseMainCityEntry)

function Modules.MorrowEntry:__init()
	--创建限时活动倒计时
    self:CreateLimitActivityTimeTxt()
	--服务器时间
	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	--紫装领取时间
	local endTime = CallModuleFunc(ModuleType.MORROW,"GetEndTime")
	self.__bg_time:SetVisible((endTime - server_time) > 0)
	self.__activity_txt:SetVisible((endTime - server_time) > 0)
	--计时器
	if (endTime - server_time) > 0 then
		if not self.__morrow_countdown_timer then
			self.__morrow_countdown_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
			function()
				self:MorrowCountDownTimer()
			end), 1, -1)				
		end	
		self:MorrowCountDownTimer()
	end
end

function Modules.MorrowEntry:RegistAllEvents()
	Modules.BaseMainCityEntry.RegistAllEvents(self)
	
	self:BindGlobalEvent(Macros.Event.MorrowModule.MORROW_AWARD, function()
		--更新主界面
		CallModuleFunc(ModuleType.MAIN_CITY, "ResetRegionEntrysBySystemID", self.__system_id)
	end)
end
	
--紫装倒计时
function Modules.MorrowEntry:MorrowCountDownTimer()
	--服务器时间
	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	--紫装领取时间
	local endTime = CallModuleFunc(ModuleType.MORROW,"GetEndTime")
	--剩余时间
	local remian_time = endTime - server_time
	if remian_time <= 0 then
		if self.__morrow_countdown_timer then
			GlobalTimerQuest:CancelQuest(self.__morrow_countdown_timer)
			self.__morrow_countdown_timer = nil
		end
	end
	self:__UpdateLimmitActivity(Macros.Game.SystemID.MORROW,TimeTool.TransTimeStamp("HH:MM:SS" ,endTime - server_time),remian_time <= 0)
end
