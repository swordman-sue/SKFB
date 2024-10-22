--[[
%% @module: 五星好评入口
%% @author: ldx
%% @created: 2018-12-22
--]]

Modules = Modules or {}

Modules.FivestarHighPraiseEntry = Modules.FivestarHighPraiseEntry or BaseClass(Modules.BaseMainCityEntry)


function Modules.FivestarHighPraiseEntry:__init()	
	self:CreateLimitActivityTimeTxt()
	--facebook分享倒计时
	if not self.__fivestar_highpraise_countdown_timer then
		self.__fivestar_highpraise_countdown_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()
			self:FiveStarHighPraiseCountDownTimer()
		end), 1, -1)				
	end	
	self:FiveStarHighPraiseCountDownTimer()
end

--五星好评
function Modules.FivestarHighPraiseEntry:FiveStarHighPraiseCountDownTimer()
	--系统入口Id转换为主界面入口Id
	local entry_id = CallModuleFunc(ModuleType.MAIN_CITY, "TranslateSystemIDToEntryID",Macros.Game.SystemID.FIVESTAR_HIGHPRAISE)  
	--服务器时间
	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	--服务器时间
	local record_fivestar_highpraise_data = CallModuleFunc(ModuleType.MAIN_CITY, "GetFiveStarHighPraiseData")
	if record_fivestar_highpraise_data then
		--剩余时间
		local remian_time = record_fivestar_highpraise_data.record_time - server_time
		if remian_time <= 0 then
			--时间底
			if self.__bg_time then
				self.__bg_time:SetVisible(false)
			end
			--计时器
			if self.__fivestar_highpraise_countdown_timer then
				GlobalTimerQuest:CancelQuest(self.__fivestar_highpraise_countdown_timer)
				self.__fivestar_highpraise_countdown_timer = nil
			end

			--更新主界面
			CallModuleFunc(ModuleType.MAIN_CITY, "ResetRegionEntrysBySystemID", self.__system_id)

		else
			if self.__activity_txt then
				local show_txt = TimeTool.TransTimeStamp("DD:HH:MM:SS" ,remian_time)
				local lab = GUI.RichText.GetColorWord(show_txt, Macros.Color.green_hex)
				self.__activity_txt:SetText(lab)
			end
		end		
	end
end