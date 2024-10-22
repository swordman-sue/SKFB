--[[
%% @module: Facebook分享入口
%% @author: ldx
%% @created: 2018-12-22
--]]

Modules = Modules or {}

Modules.FacebookShareRewardEntry = Modules.FacebookShareRewardEntry or BaseClass(Modules.BaseMainCityEntry)


function Modules.FacebookShareRewardEntry:__init()	
	self:CreateLimitActivityTimeTxt()
	--facebook分享
	if not self.__fivestar_highpraise_countdown_timer then
		self.__fivestar_highpraise_countdown_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()
			self:FaceBookShareCountDownTimer()
		end), 1, -1)				
	end	
	self:FaceBookShareCountDownTimer()
end

--好友分享倒计时
function Modules.FacebookShareRewardEntry:FaceBookShareCountDownTimer()
	--系统入口Id转换为主界面入口Id
	local entry_id = CallModuleFunc(ModuleType.MAIN_CITY, "TranslateSystemIDToEntryID",self.__system_id)
	--服务器时间
	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	--服务器时间
	local record_facebook_share_data = CallModuleFunc(ModuleType.MAIN_CITY, "GetFacebookShareData")
	if record_facebook_share_data then
		--剩余时间
		local remian_time = record_facebook_share_data.record_time - server_time
		if remian_time <= 0 then
			--时间底
			if self.__bg_time then
				self.__bg_time:SetVisible(false)
			end
			--计时器
			if self.__facebook_share_countdown_timer then
				GlobalTimerQuest:CancelQuest(self.__facebook_share_countdown_timer)
				self.__facebook_share_countdown_timer = nil
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