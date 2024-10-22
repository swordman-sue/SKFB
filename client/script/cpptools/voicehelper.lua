---声音处理模块
VoiceLogicHandle = VoiceLogicHandle or {}
VoiceLogicHandle.CurrentFileIndex = 0
VoiceLogicHandle.Record_Secord = 20
VoiceLogicHandle.Voice_Update_Interver = 0.0
VoiceLogicHandle.Last_Play_Fun_id = 0
VoiceLogicHandle.Last_Record_Time = 0.0
VoiceLogicHandle.Last_Record_Time_Interval = 1.5
VoiceLogicHandle.Is_Sending = false
VoiceLogicHandle.Is_Playing = false
VoiceLogicHandle.Is_Recording = false
VoiceLogicHandle.Record_Time = 0.0
VoiceLogicHandle.Current_Status = false
VoiceLogicHandle.Panel_Ui = false
VoiceLogicHandle.Panel_Id = false

VoiceLogicHandle.RECORD_ERROR_MAP = 
{
	[1] = CPPGameLib.GetString("record_error"),
	[2] = CPPGameLib.GetString("record_file_error"),
	[3] = CPPGameLib.GetString("record_change_cade_error"),
	[4] = CPPGameLib.GetString("record_parameter_error"),
	[5] = CPPGameLib.GetString("record_not_support"),
	[6] = CPPGameLib.GetString("record_min"),
	
}
VoiceLogicHandle.Video_ERROR_MAP = 
{
	[1] = CPPGameLib.GetString("voice_ret_error"),
	[2] = CPPGameLib.GetString("voice_wav_amr_error"),
	[3] = CPPGameLib.GetString("voice_amr_wav_error"),
	[4] = CPPGameLib.GetString("voice_save_error"),
	[5] = CPPGameLib.GetString("voice_get_error"),
	[6] = CPPGameLib.GetString("voice_rename_error"),
	[7] = CPPGameLib.GetString("voice_delete_error"),
}
VoiceLogicHandle.PLAY_ERROR_MAP = 
{
	[1] = CPPGameLib.GetString("play_error"),
	[2] = CPPGameLib.GetString("play_file_error"),
	[3] = CPPGameLib.GetString("play_change_cade_error"),
	[4] = CPPGameLib.GetString("play_parameter_error"),
	[5] = CPPGameLib.GetString("play_not_support"),
}

VoiceLogicHandle.StartRecordLogic = function()	
	--init  record
	VoiceLogicHandle.Record_Time = 0.0
	VoiceLogicHandle.Is_Recording = false
	if VoiceLogicHandle.IsOperationTooMuch() then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("record_too_much"))
		return false
	end
	VoiceLogicHandle.ResetPlayBkMusic(false,VoiceLogicHandle.Current_Status)
	VoiceLogicHandle.Is_Recording = true
	local ret_code = CPPVoiceManager:StartRecord(VoiceLogicHandle.Record_Secord)
	if (ret_code~= 0) then
		VoiceLogicHandle.ResetPlayBkMusic(true,VoiceLogicHandle.Current_Status)
		VoiceLogicHandle.Is_Recording = false
		GlobalTipMsg.GetInstance():Show(VoiceLogicHandle.RECORD_ERROR_MAP[ret_code])
		return false 
	end
	--begin  record
	VoiceLogicHandle.Record_Time = 0.0
	VoiceLogicHandle.Is_Recording = true
	VoiceLogicHandle.Is_Playing = false
	if VoiceLogicHandle.CheckPanelExist() and VoiceLogicHandle.Panel_Ui.OnRecordBegin then
		VoiceLogicHandle.Panel_Ui:OnRecordBegin()
	end
	return true
end

VoiceLogicHandle.StopRecordByLOutOfRang = function()
	--stop record
	if VoiceLogicHandle.Is_Recording then
		VoiceLogicHandle.ReleaseVoice()
	end
	VoiceLogicHandle.ResetPlayBkMusic(true,VoiceLogicHandle.Current_Status)
	VoiceLogicHandle.Record_Time = 0.0
	VoiceLogicHandle.Is_Recording = false
	VoiceLogicHandle.Is_Playing = false
	return true
end


VoiceLogicHandle.StopRecordAndSendMsgLogic = function(temp_data,panel_id)
	if VoiceLogicHandle.Is_Recording == false then
		return false
	end
	if VoiceLogicHandle.CheckPanelExist() and VoiceLogicHandle.Panel_Ui.OnRecordEnd then
		VoiceLogicHandle.Panel_Ui:OnRecordEnd()
	end
	local data = CPPGameLib.CopyTbl(temp_data)
	--end sending
	local OnFileToMsgFinish = function(lua_fun_id,code,voice_id,voice_length,men_obj)
		if (code~= 0) then
			LogTool.LogInfo("OnFileToMsgFinish return code is:"..code)
			GlobalTipMsg.GetInstance():Show(VoiceLogicHandle.Video_ERROR_MAP[code])
			return
		end
		if voice_length <= 0 then
			LogTool.LogInfo("OnFileToMsgFinish return voice_length is:"..voice_length)
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("record_min"))
			return
		end
		local role_module =  module_manager:GetModule(Modules.ModulesMgr.MODULE_ROLE)
		local chat_module = module_manager:GetModule(Modules.ModulesMgr.MODULE_CHAT)
		
		if panel_id == GameConfig.UI_PRIVATECHAT then
			chat_module:InsertPrivateChatMsg(role_module:GetRolename(), tostring(voice_id),role_module:GetUserid(),data.user_id,0,0,role_module:GetRolelevel(),voice_length,role_module:GetArmyname(),GameConfig.Msg_Type.Private_Voice,NetWorkHandleMsg.CHAT_MSG_STATUS.NOTREAD)
			-- NetWorkHandleMsg.__notifyChatUpdate()
			-- NetWorkHandleMsg.RequestPrivateVoiceChat(0, 0, data.user_id, data.user_name,men_obj,voice_length)
		elseif panel_id == GameConfig.UI_CHAT then
			-- NetWorkHandleMsg.__insertMsg(data.channel_id, role_module:GetRolename(), tostring(voice_id), role_module:GetUserid(), 0, 0, role_module:GetRolelevel(),voice_length,role_module:GetArmyname(),GameConfig.Msg_Type.Channel_Voice,NetWorkHandleMsg.CHAT_MSG_STATUS.NOTREAD)
			-- NetWorkHandleMsg.__notifyChatUpdate()
			-- NetWorkHandleMsg.doFactionChannelChat(0, nil, data.channel_id, men_obj,voice_length)
		end
		if VoiceLogicHandle.CheckPanelExist() and VoiceLogicHandle.Panel_Ui.OnSendEnd then
			VoiceLogicHandle.Panel_Ui:OnSendEnd()
		end
	end
	local finish_callback = GlobalCallbackMgr:AddCallBackFunc(OnFileToMsgFinish)

	if VoiceLogicHandle.Is_Playing == false then
		VoiceLogicHandle.ResetPlayBkMusic(true,VoiceLogicHandle.Current_Status)
	end
	VoiceLogicHandle.Record_Time = 0.0
	VoiceLogicHandle.Is_Recording = false
	VoiceLogicHandle.CurrentFileIndex = VoiceLogicHandle.CurrentFileIndex + 1
	local voice_index = VoiceLogicHandle.CurrentFileIndex
	VoiceLogicHandle.SetLastVideoId(voice_index)
	local ret_val =  CPPVoiceManager:StopRecord(voice_index,finish_callback)
	if(ret_val ~= 0) then
		LogTool.LogInfo("StopRecord return ret_val is:"..ret_val)
		GlobalTipMsg.GetInstance():Show(VoiceLogicHandle.RECORD_ERROR_MAP[ret_val])
		return
	end
	--sending
	VoiceLogicHandle.Is_Sending = true
	if VoiceLogicHandle.CheckPanelExist() and VoiceLogicHandle.Panel_Ui.OnSendBegin then
		VoiceLogicHandle.Panel_Ui:OnSendBegin()
	end
end



VoiceLogicHandle.StartPlayLogic = function(temp_voice_id)
	local funcC = function(lua_func_id,code,voice_id)
		if VoiceLogicHandle.CheckPanelExist() then
			if(lua_func_id == VoiceLogicHandle.Last_Play_Fun_id) and (VoiceLogicHandle.Is_Recording == false) then
				VoiceLogicHandle.ResetPlayBkMusic(true,VoiceLogicHandle.Current_Status)
				if VoiceLogicHandle.Panel_Ui.OnPlayEnd then
					VoiceLogicHandle.Panel_Ui:OnPlayEnd()
				end
			end
			if (code~= 0) then
				GlobalTipMsg.GetInstance():Show(VoiceLogicHandle.PLAY_ERROR_MAP[code])
			end
		end
	end
	VoiceLogicHandle.ResetPlayBkMusic(false,VoiceLogicHandle.Current_Status)
	local pfuncC = GlobalCallbackMgr:AddCallBackFunc(funcC)
	VoiceLogicHandle.Last_Play_Fun_id = pfuncC
	local ret_val = CPPVoiceManager:StartPlay(temp_voice_id,pfuncC)
	if(ret_val ~= 0) then
		VoiceLogicHandle.ResetPlayBkMusic(true,VoiceLogicHandle.Current_Status)
		GlobalTipMsg.GetInstance():Show(VoiceLogicHandle.PLAY_ERROR_MAP[ret_val])
		return false
	end
	VoiceLogicHandle.Is_Playing = true
	if VoiceLogicHandle.Panel_Ui.OnPlayBegin then
		VoiceLogicHandle.Panel_Ui:OnPlayBegin()
	end
	return true
end

VoiceLogicHandle.StopPlayLogic = function(voice_id)
	VoiceLogicHandle.Is_Playing = false
	local ret_val = CPPVoiceManager:StopPlay(voice_id)
	if VoiceLogicHandle.Panel_Ui.OnPlayEnd then
		VoiceLogicHandle.Panel_Ui:OnPlayEnd()
	end
	return ret_val
end

VoiceLogicHandle.ReleaseVoice = function()
	CPPVoiceManager:ReleaseVoice()
end

VoiceLogicHandle.SetLastVideoId = function(video_id)
	CPPResManager:SetSetting(Macros.Setting.MaxVideoId, tostring(video_id))
end

VoiceLogicHandle.SaveMsgAsFileLogic = function(msg_obj,voice_id,voice_length,lua_fun_id)
	return CPPVoiceManager:SaveMsgAsFile(msg_obj,voice_id,voice_length,lua_fun_id)
end

VoiceLogicHandle.CheckPanelExist = function()
	if VoiceLogicHandle.Panel_Ui and VoiceLogicHandle.Panel_Id then
		return BaseViewMgr.GetRunningUILayer(VoiceLogicHandle.Panel_Id)
	end
	return false
end


VoiceLogicHandle.ResetPlayBkMusic = function(isplay,current_status)
	if not current_status then
		return
	end
	local property_module = module_manager:GetModule(Modules.ModulesMgr.MODULE_GAME_PROPERTY)
	local play_bgmusic = property_module:GetPlaybgmusic()
	if isplay then
		if not play_bgmusic then
			CPPGameLib.SetBgMusicPlay(isplay)
		end
	else
		if play_bgmusic then
			CPPGameLib.SetBgMusicPlay(isplay)
		end
	end
end


VoiceLogicHandle.GetVoicePlayFactor = function(voice_length)
	local IMG_WIDTH_FACTOR = 1
	local BeginWith = 5
	if voice_length > BeginWith then
		IMG_WIDTH_FACTOR = IMG_WIDTH_FACTOR + (voice_length - BeginWith)*0.05
	end
	return IMG_WIDTH_FACTOR
end

		
VoiceLogicHandle.DeleteFileLogic = function()
	local OnDelete = function(callback_id,code,voice_id)
	end
	local del_call_back = GlobalCallbackMgr:AddCallBackFunc(OnDelete)
	local last_video_id_str = CPPResManager:GetSetting(Macros.Setting.MaxVideoId)
	if last_video_id_str ~= "" and tonumber(last_video_id_str) then
		local last_video_id_num = tonumber(last_video_id_str)
		for i = 1, last_video_id_num do
			CPPVoiceManager:DeleteFileByVoiceId(i,del_call_back)
		end
	end
	VoiceLogicHandle.CurrentFileIndex = 0
	CPPResManager:SetSetting(Macros.Setting.MaxVideoId, "0")
end


VoiceLogicHandle.DeleteFileLogic()



VoiceLogicHandle.Delete = function()
	VoiceLogicHandle.ResetPlayBkMusic(true,VoiceLogicHandle.Current_Status)
	VoiceLogicHandle.ReleaseVoice()
	VoiceLogicHandle.Last_Play_Fun_id = 0
	VoiceLogicHandle.Last_Record_Time = 0.0
	VoiceLogicHandle.Is_Sending = false
	VoiceLogicHandle.Is_Playing = false
	VoiceLogicHandle.Is_Recording = false
	VoiceLogicHandle.Record_Time = 0.0
	VoiceLogicHandle.Current_Status = false
	VoiceLogicHandle.Panel_Ui = false
	VoiceLogicHandle.Panel_Id = false
end


VoiceLogicHandle.Init = function(self,ui_id)
	local property_module = module_manager:GetModule(Modules.ModulesMgr.MODULE_GAME_PROPERTY)
	VoiceLogicHandle.Last_Play_Fun_id = 0
	VoiceLogicHandle.Last_Record_Time = 0.0
	VoiceLogicHandle.Is_Sending = false
	VoiceLogicHandle.Is_Playing = false
	VoiceLogicHandle.Is_Recording = false
	VoiceLogicHandle.Record_Time = 0.0
	VoiceLogicHandle.Current_Status = property_module:GetPlaybgmusic()
	VoiceLogicHandle.Panel_Ui = self
	VoiceLogicHandle.Panel_Id = ui_id
end

VoiceLogicHandle.Update = function(interval)
	--record update
	VoiceLogicHandle.Voice_Update_Interver = VoiceLogicHandle.Voice_Update_Interver + interval
	if VoiceLogicHandle.Voice_Update_Interver > 0.3 then
		if VoiceLogicHandle.CheckPanelExist() and VoiceLogicHandle.Is_Recording and VoiceLogicHandle.Panel_Ui.voice_layer then	
			VoiceLogicHandle.Panel_Ui.voice_layer:AddCount()
		end
		if VoiceLogicHandle.CheckPanelExist() and VoiceLogicHandle.Is_Playing and VoiceLogicHandle.Panel_Ui.play_layer then	
			VoiceLogicHandle.Panel_Ui.play_layer:AddCount()
		end
		VoiceLogicHandle.Voice_Update_Interver = 0.0
	end
	VoiceLogicHandle.Record_Time = VoiceLogicHandle.Record_Time + interval
	if(VoiceLogicHandle.Record_Time > VoiceLogicHandle.Record_Secord and VoiceLogicHandle.Is_Recording) then
		VoiceLogicHandle.ResetPlayBkMusic(true,VoiceLogicHandle.Current_Status)
		VoiceLogicHandle.Record_Time = 0.0
		GlobalTipMsg.GetInstance():Show(string.format(CPPGameLib.GetString("voice_record_max_time"), VoiceLogicHandle.Record_Secord))
		if VoiceLogicHandle.Panel_Ui.voice_layer then
			VoiceLogicHandle.Panel_Ui.voice_layer:GetNode():SetVisible(false)
		end
	end
end

VoiceLogicHandle.IsOperationTooMuch = function()
	local current_time = game_app.game_server_time
	if(current_time - VoiceLogicHandle.Last_Record_Time < VoiceLogicHandle.Last_Record_Time_Interval) then
		return true
	end
	VoiceLogicHandle.Last_Record_Time = current_time
	return false
end

VoiceLogicHandle.IsposInRect = function(x,y,rect)
	if x >= rect.startx and x <= (rect.startx + rect.width) and y <= rect.starty and y >= (rect.starty - rect.height) then
		return true
	end
	return false
end